//
//  PostProduction.h
//  InstaSpace
//
//  Created by Anthony Stellato on 2/11/13.
//
//

#ifndef InstaSpace_PostProduction_h
#define InstaSpace_PostProduction_h

#include "cinder/gl/gl.h"
#include "cinder/gl/Fbo.h"
#include "cinder/gl/GlslProg.h"

#include "PostProductionShaders.h"
#include "FBOPingPong.h"

namespace as {
    
    using namespace ci;
    using namespace ci::gl;
    using namespace std;
    
    class PostProduction {
    public:
        PostProduction(){
            PostProduction(800, 600);
        }
        PostProduction(int width, int height, ci::gl::Fbo::Format format = ci::gl::Fbo::Format()){
            setupFbos(width, height, format);
            
            //setup shaders
            try { shaderQuickBlur = GlslProg( shaderVertPassthru.c_str(), shaderFragQuickBlur.c_str() ); }
            catch( gl::GlslProgCompileExc exc ) { app::console() << exc.what() << endl; }
            
            try { shaderVertBlur = GlslProg( shaderVertPassthruBlur.c_str(), shaderFragBlurVert.c_str() ); }
            catch( gl::GlslProgCompileExc exc ) { app::console() << exc.what() << endl; }
            
            try { shaderHorzBlur = GlslProg( shaderVertPassthruBlur.c_str(), shaderFragBlurHorz.c_str() ); }
            catch( gl::GlslProgCompileExc exc ) { app::console() << exc.what() << endl; }
            
            try { shaderInvert = GlslProg( shaderVertPassthru.c_str(), shaderFragInvert.c_str() ); }
            catch( gl::GlslProgCompileExc exc ) { app::console() << exc.what() << endl; }
        
            try { shaderPrewitt = GlslProg( shaderVertPassthru.c_str(), shaderFragPrewitt.c_str() ); }
            catch( gl::GlslProgCompileExc exc ) { app::console() << exc.what() << endl; }
            
            try { shaderPixelate = GlslProg( shaderVertPassthru.c_str(), shaderFragPixelate.c_str() ); }
            catch( gl::GlslProgCompileExc exc ) { app::console() << exc.what() << endl; }
        }
        
        void resize(int width, int height, Fbo::Format format = Fbo::Format()){
            if(mFbos != NULL){
                delete mFbos;
            }
            setupFbos(width, height, format);
        }
        
        Texture& quickBlur( Texture& source, Vec2f sampleOffset = Vec2f::one() ){
            SaveFramebufferBinding bindingSaver;
            mFbos->getFront().bindFramebuffer();
            source.bind(0);
            shaderQuickBlur.bind();
            shaderQuickBlur.uniform("tex0", 0);
            shaderQuickBlur.uniform("sampleOffset", sampleOffset);
            gl::color(ColorA::white());
            gl::drawSolidRect( source.getBounds() );
            shaderQuickBlur.unbind();
            source.unbind();
            
            return mFbos->getFront().getTexture();
        }
        
        Texture& invert( Texture& source ){
            SaveFramebufferBinding bindingSaver;
            mFbos->getFront().bindFramebuffer();
            gl::clear();
            gl::setViewport( mFbos->getFront().getBounds() );
            gl::setMatricesWindow( mFbos->getFront().getSize() );
            source.bind(0);
            shaderInvert.bind();
            shaderInvert.uniform("tex0", 0);
            gl::color(ColorA::white());
            gl::drawSolidRect( source.getBounds() );
            shaderInvert.unbind();
            source.unbind();
            
            return mFbos->getFront().getTexture();
        }
        
        Texture& blur( Texture& source, Vec2f strength ){
            {
                SaveFramebufferBinding bindingSaver;
                mFbos->getFront().bindFramebuffer();
                gl::clear();
                gl::setViewport( mFbos->getFront().getBounds() );
                gl::setMatricesWindow( mFbos->getFront().getSize() );
                source.bind(0);
                shaderVertBlur.bind();
                shaderVertBlur.uniform("tex0", 0);
                shaderVertBlur.uniform("strength", strength);
                gl::color(ColorA::white());
                gl::drawSolidRect( source.getBounds() );
                shaderVertBlur.unbind();
                source.unbind();
            }
            
//            mFbos->swap();
//            
//            {
//                SaveFramebufferBinding bindingSaver;
//                mFbos->getFront().bindFramebuffer();
//                gl::clear();
//                gl::setViewport( mFbos->getFront().getBounds() );
//                gl::setMatricesWindow( mFbos->getFront().getSize() );
//                mFbos->getBack().getTexture().bind(0);
//                shaderHorzBlur.bind();
//                shaderHorzBlur.uniform("tex0", 0);
//                shaderHorzBlur.uniform("strength", strength);
//                gl::color(ColorA::white());
//                gl::drawSolidRect( source.getBounds() );
//                shaderHorzBlur.unbind();
//                mFbos->getBack().getTexture().unbind();
//            }
            
            return mFbos->getFront().getTexture();
        }
        
        Texture& prewitt( Texture& source ){
            SaveFramebufferBinding bindingSaver;
            mFbos->getFront().bindFramebuffer();
            gl::clear();
            gl::setViewport( mFbos->getFront().getBounds() );
            gl::setMatricesWindow( mFbos->getFront().getSize() );
            source.bind(0);
            shaderPrewitt.bind();
            shaderPrewitt.uniform("tex0", 0);
            shaderPrewitt.uniform("size", Vec2f(source.getWidth(), source.getHeight()));
            gl::color(ColorA::white());
            gl::drawSolidRect( source.getBounds() );
            shaderInvert.unbind();
            source.unbind();
            return mFbos->getFront().getTexture();
        }
        
        Texture& pixelate( Texture& source, Vec2f sampleDivisor ){
            SaveFramebufferBinding bindingSaver;
            mFbos->getFront().bindFramebuffer();
            gl::clear();
            gl::setViewport( mFbos->getFront().getBounds() );
            gl::setMatricesWindow( mFbos->getFront().getSize() );
            source.bind(0);
            shaderPixelate.bind();
            shaderPixelate.uniform("tex0", 0);
            shaderPixelate.uniform("sampleDivisor", sampleDivisor);
            gl::color(ColorA::white());
            gl::drawSolidRect( source.getBounds() );
            shaderPixelate.unbind();
            source.unbind();
            return mFbos->getFront().getTexture();
        }
        
        FBOPingPong* getFbos(){
            return mFbos;
        }
        
    private:
        void setupFbos(int width, int height, gl::Fbo::Format format = Fbo::Format()){
            //setup FBOs
            mFbos = new FBOPingPong();
            mFbos->init(width, height, format);
        }
        
        FBOPingPong *mFbos;
        
        GlslProg shaderQuickBlur;
        GlslProg shaderVertBlur;
        GlslProg shaderHorzBlur;
        GlslProg shaderInvert;
        GlslProg shaderPrewitt;
        GlslProg shaderPixelate;
    
    };
    
}

#endif
