#include "CinderGLKitTemplateSketch.h"

#include "cinder/ImageIO.h"
#include "cinder/app/App.h"

using namespace std;
using namespace ci;
using namespace ci::app;

void CinderGLKitTemplateSketch::setup()
{
    m_vbo = gl::Vbo::create(GL_LINE_STRIP);
    m_vbo->set(gl::Vbo::Attribute::create("position", 3));
    
    try{
        m_color_shader = gl::GlslProg(app::loadResource("color.vsh"), app::loadResource("color.fsh"));
    }catch(Exception &e){
        cout << "Shader Compile Failed: " << e.what();
    }
    
    m_camera.setOrtho(0, 1, -1, 1, -1, 1);
}

void CinderGLKitTemplateSketch::update()
{
    float time = getElapsedSeconds() * 0.2f;
    
    vector<Vec3f> positions;
    for(int n = 100, i = 0; i < n; ++i){
        float t = i / (n - 1.0f);
        positions.push_back(Vec3f(t, m_perlin.fBm(t, time), 0));
    }
    
    m_vbo->get("position")->setData(positions);
}

void CinderGLKitTemplateSketch::draw(const Area &area)
{
    gl::clear(ColorA(1, 1, 0, 1));
    
    glLineWidth(4);
    
    m_color_shader.bind();
    m_color_shader.uniform("u_mvp_matrix", m_camera.getProjectionMatrix() * m_camera.getModelViewMatrix());
    m_color_shader.uniform("u_color", Vec4f(0, 0, 0, 1));
    m_vbo->draw(m_color_shader);
}

void CinderGLKitTemplateSketch::touchesBegan( TouchEvent event )
{
    TouchEvent::Touch thisTouch = event.getTouches().at(0);
    console()<<"Touches began: "<<thisTouch<<"\n";
    orgTouch = thisTouch;

}

void CinderGLKitTemplateSketch::touchesMoved( TouchEvent event )
{
    if(touchExists(orgTouch, event.getTouches())){
        TouchEvent::Touch thisTouch = getTouchWithId(orgTouch, event.getTouches());
        //input.touchMoved(thisTouch.getPos());
        console()<<"Touches moved: "<<thisTouch<<"\n";
    }
}

void CinderGLKitTemplateSketch::touchesEnded( TouchEvent event )
{
    if(touchExists(orgTouch, event.getTouches())){
        TouchEvent::Touch thisTouch = getTouchWithId(orgTouch, event.getTouches());
        //input.touchEnded(thisTouch.getPos());
        console()<<"Touches ended: "<<thisTouch<<"\n";
    }
}

bool CinderGLKitTemplateSketch::touchExists(TouchEvent::Touch _target, std::vector<TouchEvent::Touch>& _touches){
    bool bExists = false;
    for(TouchEvent::Touch t : _touches){
        if(t.getId() == _target.getId()){
            bExists = true;
            break;
        }
    }
    return bExists;
}

TouchEvent::Touch CinderGLKitTemplateSketch::getTouchWithId(TouchEvent::Touch _target, std::vector<TouchEvent::Touch>& _touches){
    for(TouchEvent::Touch t : _touches){
        if(t.getId() == _target.getId()){
            return t;
        }
    }
    
    return TouchEvent::Touch();
}

fs::path CinderGLKitTemplateSketch:: getFSPathForResource(string& _name, string& _type){
    NSString* name = [[NSString alloc] initWithUTF8String:_name.c_str()];
    NSString* type = [[NSString alloc] initWithUTF8String:_type.c_str()];
    NSString* path = [[NSBundle mainBundle] pathForResource:name ofType:type];
    NSLog(@"%@", path);
    return fs::path(cocoa::convertNsString(path));
}