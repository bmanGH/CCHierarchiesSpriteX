//
//  CCHierarchiesSpriteGrabber.cpp
//  CCHierarchiesSprite
//
//  Created by bman <zx123xz321hm3@hotmail.com>.
//  Copyright (c) 2013. All rights reserved.
//
//

#include "CCHierarchiesSpriteGrabber.h"


NS_CC_EXT_BEGIN

CCHierarchiesSpriteGrabber::CCHierarchiesSpriteGrabber ()
: _FBO(0), _oldFBO(0) {
    memset(_oldClearColor, 0, sizeof(_oldClearColor));
    
    // generate FBO
    glGenFramebuffers(1, &_FBO);
}

CCHierarchiesSpriteGrabber::~CCHierarchiesSpriteGrabber () {
    glDeleteFramebuffers(1, &_FBO);
}

void CCHierarchiesSpriteGrabber::beforeRender (GLuint textureName) {
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &_oldFBO);
    glGetFloatv(GL_COLOR_CLEAR_VALUE, _oldClearColor);
    
    glBindFramebuffer(GL_FRAMEBUFFER, _FBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureName, 0);
    
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);
}

void CCHierarchiesSpriteGrabber::afterRender () {
    glBindFramebuffer(GL_FRAMEBUFFER, _oldFBO);
    
    // Restore clear color
    glClearColor(_oldClearColor[0], _oldClearColor[1], _oldClearColor[2], _oldClearColor[3]);
}

NS_CC_EXT_END