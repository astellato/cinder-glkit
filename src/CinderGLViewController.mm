#import "CinderGLViewController.h"

#include <vector>

#include "cinder/Area.h"
#include "cinder/Vector.h"
#include "cinder/app/TouchEvent.h"

using namespace ci;
using namespace ci::app;

@implementation CinderGLViewController

@synthesize context = _context;


- (id)init
{
    self = [super init];
    if(self){
        self->mSketch = NULL;
    }
    return self;
}


- (void)viewDidLoad
{
    [super viewDidLoad];
    
    self.context = [[EAGLContext alloc] initWithAPI: kEAGLRenderingAPIOpenGLES2];
    self.preferredFramesPerSecond = 60;
    
    if(!self.context){
        NSLog(@"Failed to create ES2 context");
    }
    
    GLKView *view = (GLKView *)self.view;
    view.context = self.context;
    view.drawableDepthFormat = GLKViewDrawableDepthFormat24;
}

- (void)viewDidUnload
{    
    [super viewDidUnload];
    
    if(mSketch)
        mSketch->tearDown();
    
    if([EAGLContext currentContext] == self.context){
        [EAGLContext setCurrentContext: nil];
    }
    self.context = nil;
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    return mSketch->supportsInterfaceOrientation((CinderGLSketch::UIInterfaceOrientation)interfaceOrientation);
}


- (void)setSketch:(CinderGLSketch *)sketch
{
    mSketch = sketch;
}


#pragma mark - GLKView and GLKViewController delegate methods

- (void)update
{
    [EAGLContext setCurrentContext: self.context];
    if(mSketch){
        mSketch->privateUpdate__();
    }
}

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
    [EAGLContext setCurrentContext: self.context];
    
    if(mSketch){
        GLKView *view = (GLKView *)self.view;
        
        Vec2i size(view.drawableWidth, view.drawableHeight);
        if(size != mSketch->getSize()){
            mSketch->setSize(size);
        }
        
        if(mSketch->mNeedsSetup){
            mSketch->privateSetup__();
            mSketch->mNeedsSetup = false;
        }
        
        mSketch->draw(Area(rect.origin.x, rect.origin.y, rect.size.width, rect.size.height));
    }
}


#pragma mark - Touch Handling

- (uint32_t)addTouchToMap:(UITouch*)touch
{
	uint32_t candidateId = 0;
	bool found = true;
	while( found ) {
		candidateId++;
		found = false;
		for( const auto &touchId : mTouchIdMap ) {
			if( touchId.second == candidateId ) {
				found = true;
				break;
			}
		}
	}
	
	mTouchIdMap.insert( std::make_pair( touch, candidateId ) );
	
	return candidateId;
}

- (void)removeTouchFromMap:(UITouch*)touch
{
	auto found( mTouchIdMap.find( touch ) );
	if( found == mTouchIdMap.end() )
		;//std::cout << "Couldn' find touch in map?" << std::endl;
	else
		mTouchIdMap.erase( found );
}

- (uint32_t)findTouchInMap:(UITouch*)touch
{
	const auto found( mTouchIdMap.find( touch ) );
	if( found == mTouchIdMap.end() ) {
		;//std::cout << "Couldn' find touch in map?" << std::endl;
		return 0;
	}
	else
		return found->second;
}

- (void)updateActiveTouches
{
	// const float contentScale = [self contentScaleFactor];
    
	mActiveTouches.clear();
	for( const auto &touch : mTouchIdMap ) {
		CGPoint pt = [touch.first locationInView:self.view];
		CGPoint prevPt = [touch.first previousLocationInView:self.view];
		mActiveTouches.push_back( ci::app::TouchEvent::Touch( ci::Vec2f( pt.x, pt.y ), ci::Vec2f( prevPt.x, prevPt.y ), touch.second, [touch.first timestamp], (__bridge_retained void*)touch.first ) );
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////
// Event handlers
- (void)touchesBegan:(NSSet*)touches withEvent:(UIEvent*)event
{
    std::vector<ci::app::TouchEvent::Touch> touchList;
    for( UITouch *touch in touches ) {
        CGPoint pt = [touch locationInView:self.view];
        CGPoint prevPt = [touch previousLocationInView:self.view];
        touchList.push_back( ci::app::TouchEvent::Touch( ci::Vec2f( pt.x, pt.y ), ci::Vec2f( prevPt.x, prevPt.y ), [self addTouchToMap:touch], [touch timestamp], (__bridge_retained void*)touch ) );
    }
    [self updateActiveTouches];
    if( ! touchList.empty() ) {
        mSketch->touchesBegan(TouchEvent(window,touchList));
    }
}

- (void)touchesMoved:(NSSet*)touches withEvent:(UIEvent*)event
{
	// const float contentScale = [self contentScaleFactor];

		std::vector<ci::app::TouchEvent::Touch> touchList;
		for( UITouch *touch in touches ) {
			CGPoint pt = [touch locationInView:self.view];
			CGPoint prevPt = [touch previousLocationInView:self.view];
			touchList.push_back( ci::app::TouchEvent::Touch( ci::Vec2f( pt.x, pt.y ), ci::Vec2f( prevPt.x, prevPt.y ), [self findTouchInMap:touch], [touch timestamp], (__bridge_retained void*)touch ) );
		}
		[self updateActiveTouches];
		if( ! touchList.empty() ) {
			mSketch->touchesMoved(TouchEvent(window,touchList));
		}

}

- (void)touchesEnded:(NSSet*)touches withEvent:(UIEvent*)event
{
	// const float contentScale = [self contentScaleFactor];
    
		std::vector<ci::app::TouchEvent::Touch> touchList;
		for( UITouch *touch in touches ) {
			CGPoint pt = [touch locationInView:self.view];
			CGPoint prevPt = [touch previousLocationInView:self.view];
			touchList.push_back( ci::app::TouchEvent::Touch( ci::Vec2f( pt.x, pt.y ), ci::Vec2f( prevPt.x, prevPt.y ), [self findTouchInMap:touch], [touch timestamp], (__bridge_retained void*)touch ) );
			[self removeTouchFromMap:touch];
		}
		[self updateActiveTouches];
		if( ! touchList.empty() ) {
			mSketch->touchesEnded(TouchEvent(window,touchList));
		}

}

- (void)touchesCancelled:(NSSet*)touches withEvent:(UIEvent*)event
{
	[self touchesEnded:touches withEvent:event];
}

- (const std::vector<cinder::app::TouchEvent::Touch>&)getActiveTouches
{
	return mActiveTouches;
}



@end
