#pragma once

#include "CinderGLSketch.h"

#include "cinder/gl/GlslProg.h"
#include "Vbo.h"

#include "cinder/Vector.h"
#include "cinder/Camera.h"
#include "cinder/Perlin.h"

#include "GLSLRender.h"

using namespace ci;
using namespace std;
using namespace as;

class CinderGLKitTemplateSketch : public CinderGLSketch {
public:
    
    void setup();
    void update();
    void draw(const Area &area);
    
    void	touchesBegan( app::TouchEvent event );
	void	touchesMoved( app::TouchEvent event );
	void	touchesEnded( app::TouchEvent event );
    bool touchExists(app::TouchEvent::Touch _target, std::vector<app::TouchEvent::Touch>& _touches);
    app::TouchEvent::Touch getTouchWithId(app::TouchEvent::Touch _target, std::vector<app::TouchEvent::Touch>& _touches);
    app::TouchEvent::Touch orgTouch;
    
    fs::path getFSPathForResource(string& _name, string& _type);
    
    gl::VboRef m_vbo;
    gl::VboRef m_fullscreen_quad_vbo;
    
    gl::GlslProg m_color_shader;
    gl::GlslProg m_test_shader;
    
    CameraOrtho m_camera;
    
    Perlin m_perlin;
    
    GLSLRender* render;
    
};