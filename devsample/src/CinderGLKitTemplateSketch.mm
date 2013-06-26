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
    
    m_fullscreen_quad_vbo = gl::Vbo::createPlane(Vec2f(0, 0), Vec2f(1024, 768));//Vec2f(getWindowWidth(), getWindowHeight()));
    //m_fullscreen_quad_vbo->set(gl::Vbo::Attribute::create("position", 3));

    string name = "color";
    string othername = "loadingbox";
    string vsh = "vsh";
    string fsh = "fsh";
    fs::path fsVSH = getFSPathForResource(name, vsh);
    fs::path fsFSH = getFSPathForResource(name, fsh);
    
    fs::path fsLBV = getFSPathForResource(othername, vsh);
    fs::path fsLBF = getFSPathForResource(othername, fsh);
    
    try{
        m_color_shader = gl::GlslProg(DataSourcePath::create(fsVSH), DataSourcePath::create(fsFSH));
    }catch(...){
        cout << "Color Shader Compile Failed.\n";
    }
    
    try{
        m_test_shader = gl::GlslProg(DataSourcePath::create(fsLBV), DataSourcePath::create(fsLBF));
    }catch(...){
        cout << "Test Shader Compile Failed.\n";
    }

    //m_camera.setOrtho(0, 1, -1, 1, -1, 1);
    m_camera.setOrtho(0, 1024, 768, 0, -100, 100);
    //render = new GLSLRender(getWindowWidth(), getWindowHeight());
}

void CinderGLKitTemplateSketch::update()
{
    float time = getElapsedSeconds() * 0.2f;

    vector<Vec3f> positions;
    for(int n = 100, i = 0; i < n; ++i){
        float t = i / (n - 1.0f);
        Vec3f pos = Vec3f(t*getWindowWidth(), m_perlin.fBm(t, time)*getWindowHeight(), 0);
        //console()<<i<<" pos: "<<pos<<"\n";
        positions.push_back(pos);
    }
    
    m_vbo->get("position")->setData(positions);
}

void CinderGLKitTemplateSketch::draw(const Area &area)
{
    gl::clear(ColorA(0, 1, 0, 1));
    
    //render->getLoadingBoxShader().bind();
    //render->getLoadingBoxShader().uniform("iResolution", Vec3f(getWindowWidth(), getWindowHeight(), 0.f));
    //render->getLoadingBoxShader().uniform("iGlobalTime", (float)getElapsedSeconds());
//    console()<<"MADE IT TO DRAW.\n";
    m_test_shader.bind();
    m_fullscreen_quad_vbo->draw(m_test_shader);
    
    //glLineWidth(4);
    
    m_color_shader.bind();
    m_color_shader.uniform("u_mvp_matrix", m_camera.getProjectionMatrix() * m_camera.getModelViewMatrix());
    m_color_shader.uniform("u_color", Vec4f(0, 0, 0, 1));
    m_vbo->draw(m_color_shader);
    
//    m_test_shader.bind();
//    m_test_shader.uniform("u_mvp_matrix", m_camera.getProjectionMatrix() * m_camera.getModelViewMatrix());
//    m_test_shader.uniform("u_color", Vec4f(0, 0, 0, 1));
//    m_vbo->draw(m_test_shader);
    
    
    
    
    //render->loadingBox(getElapsedSeconds());
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