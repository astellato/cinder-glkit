/*
 *  FBOPingPong.h
 *  eschercam
 *
 *  Created by astellato on 3/14/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

#include "cinder/gl/Fbo.h"

class FBOPingPong {
public:
	
	FBOPingPong() {
		currentIndex = 0;
		bSetup = false;
	}
	
	void init(int width, int height, ci::gl::Fbo::Format format){
		objects[0] = new ci::gl::Fbo( width, height, format );
		objects[1] = new ci::gl::Fbo( width, height, format );
		
		bSetup = true;
	}
	
	ci::gl::Fbo& getFront() {
		if(!bSetup){
			init(800, 600, ci::gl::Fbo::Format());
		}
		return (*objects[currentIndex]);
		//else
			//return 0;
	}
	
	ci::gl::Fbo& getBack() {
		if(!bSetup){
			init(800, 600, ci::gl::Fbo::Format());
		}
		return (*objects[1-currentIndex]);
		//else
			//return 0;
	}
	
	void swap() {
		currentIndex = 1 - currentIndex;
	}
	
protected:
	ci::gl::Fbo*		objects[2];
	int		currentIndex;
	bool bSetup;
};