#include "AppDelegate.h"

#include "cocos2d.h"
#include "CCHierarchiesSprite.h"
#include "CCHierarchiesSpriteRuntime.h"
#include "MainMenu.h"

USING_NS_CC;
USING_NS_CC_EXT;


AppDelegate::AppDelegate()
{
}

AppDelegate::~AppDelegate()
{
}

bool AppDelegate::applicationDidFinishLaunching()
{
    // initialize director & GLView
    CCDirector *pDirector = CCDirector::sharedDirector();
    CCEGLView* glView = CCEGLView::sharedOpenGLView();
    pDirector->setOpenGLView(glView);
    pDirector->setProjection(kCCDirectorProjection2D);
    
    // set default texture pixel format to RGBA4444
//    CCTexture2D::setDefaultAlphaPixelFormat(kCCTexture2DPixelFormat_RGBA4444);
    
    // initialize HierarchiesSprite runtime
    CCHierarchiesSpriteRuntime::sharedHierarchiesSpriteRuntime()->initializationRuntime();

    // turn on display FPS
    pDirector->setDisplayStats(true);

    // set FPS. the default value is 1.0/60 if you don't call this
    pDirector->setAnimationInterval(1.0 / 60);

    // create a scene. it's an autorelease object
    CCScene *pScene = MainMenu::scene();

    // run
    pDirector->runWithScene(pScene);

    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground()
{
    CCDirector::sharedDirector()->pause();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground()
{
    CCDirector::sharedDirector()->resume();
}
