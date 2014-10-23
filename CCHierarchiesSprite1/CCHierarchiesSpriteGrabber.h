//
//  CCHierarchiesSpriteGrabber.h
//  CCHierarchiesSprite
//
//  Created by bman <zx123xz321hm3@hotmail.com>.
//  Copyright (c) 2013. All rights reserved.
//
//

#ifndef __SZSG__CCHierarchiesSpriteGrabber__
#define __SZSG__CCHierarchiesSpriteGrabber__

#include "cocos2d.h"
#include "ExtensionMacros.h"
#include "CCHierarchiesSpriteConfig.h"

USING_NS_CC;


NS_CC_EXT_BEGIN

class CC_DLL CCHierarchiesSpriteGrabber : public CCObject {
    
protected:
    GLuint _FBO;
    GLint _oldFBO;
    GLfloat    _oldClearColor[4];
    
public:
    CCHierarchiesSpriteGrabber ();
    virtual ~CCHierarchiesSpriteGrabber ();
    
    void beforeRender (GLuint textureName);
    void afterRender ();

};

NS_CC_EXT_END

#endif /* defined(__SZSG__CCHierarchiesSpriteGrabber__) */
