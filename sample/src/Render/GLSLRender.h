//
//  GLSLRender.h
//  IOSShaders
//
//  Created by Anthony Stellato on 6/25/13.
//
//

#ifndef IOSShaders_GLSLRender_h
#define IOSShaders_GLSLRender_h

#include "cinder/gl/gl.h"
#include "cinder/gl/Fbo.h"
#include "cinder/gl/GlslProg.h"

#include "GLSLRenderShaders.h"
#include "FBOPingPong.h"

namespace as {
    
    using namespace ci;
    using namespace ci::gl;
    using namespace std;
    
    /*
     
     uniform vec3      iResolution;     // viewport resolution (in pixels)
     uniform float     iGlobalTime;     // shader playback time (in seconds)
     uniform float     iChannelTime[4]; // channel playback time (in seconds)
     uniform vec4      iMouse;          // mouse pixel coords. xy: current (if MLB down), zw: click
     uniform samplerXX iChannel0..3;    // input channel. XX = 2D/Cube
     uniform vec4      iDate;           // (year, month, day, time in seconds)
     
     */
    
    class GLSLRender {
    public:
        GLSLRender(){
            GLSLRender(800, 600);
        }
        GLSLRender(int width, int height, ci::gl::Fbo::Format format = ci::gl::Fbo::Format()){
            //setupFbos(width, height, format);
            resolution = Vec2f(width, height);
            //setup shaders
            try { shaderLoadingBox = GlslProg( shaderVertPassThru.c_str(), shaderFragDebug.c_str() ); }
            catch( gl::GlslProgCompileExc exc ) { app::console() << exc.what() << endl; }
            
        }
        
        void resize(int width, int height, Fbo::Format format = Fbo::Format()){
            if(mFbos != NULL){
                delete mFbos;
            }
            //setupFbos(width, height, format);
        }
        
        void loadingBox(float _time){
            //SaveFramebufferBinding bindingSaver;
            //mFbos->getFront().bindFramebuffer();
            //source.bind(0);
            shaderLoadingBox.bind();
            shaderLoadingBox.uniform("iResolution", Vec3f(resolution.x, resolution.y, 0.f));
            shaderLoadingBox.uniform("iGlobalTime", _time);

            gl::drawSolidRect( ci::Rectf(0, 0, resolution.x, resolution.y) );
            shaderLoadingBox.unbind();
            
            
            //return mFbos->getFront().getTexture();
        }
        
        ci::gl::GlslProg& getLoadingBoxShader(){
            return shaderLoadingBox;
        }
        
    private:
        void setupFbos(int width, int height, gl::Fbo::Format format = Fbo::Format()){
            //setup FBOs
            mFbos = new FBOPingPong();
            mFbos->init(width, height, format);
        }
        
        FBOPingPong *mFbos;
        Vec2f resolution;
        
        GlslProg shaderLoadingBox;
    };
}

#endif
