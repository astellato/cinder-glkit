#import <UIKit/UIKit.h>
#import <GLKit/GLKit.h>

#include <map>
#include "CinderGLSketch.h"

using namespace ci;

@interface CinderGLViewController : GLKViewController {
    CinderGLSketch *mSketch;
    std::map<UITouch*,uint32_t> mTouchIdMap;
    std::vector<cinder::app::TouchEvent::Touch> mActiveTouches;
    app::WindowRef window;
}

@property (strong, nonatomic) EAGLContext *context;

- (void)setSketch:(CinderGLSketch*)sketch;

- (void)touchesBegan:(NSSet*)touches withEvent:(UIEvent*)event;
- (void)touchesMoved:(NSSet*)touches withEvent:(UIEvent*)event;
- (void)touchesEnded:(NSSet*)touches withEvent:(UIEvent*)event;
- (void)touchesCancelled:(NSSet*)touches withEvent:(UIEvent*)event;

- (uint32_t)addTouchToMap:(UITouch*)touch;
- (void)removeTouchFromMap:(UITouch*)touch;
- (uint32_t)findTouchInMap:(UITouch*)touch;
- (void)updateActiveTouches;
- (const std::vector<cinder::app::TouchEvent::Touch>&)getActiveTouches;

/*
 - (uint32_t)addTouchToMap:(UITouch*)touch;
 - (void)removeTouchFromMap:(UITouch*)touch;
 - (uint32_t)findTouchInMap:(UITouch*)touch;
 - (void)updateActiveTouches;
 - (void)touchesBegan:(NSSet*)touches withEvent:(UIEvent*)event;
 */

@end
