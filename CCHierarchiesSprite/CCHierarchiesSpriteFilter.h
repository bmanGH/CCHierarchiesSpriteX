//
//  CCHierarchiesSpriteFilter.h
//  CCHierarchiesSprite
//
//  Created by bman <zx123xz321hm3@hotmail.com>.
//  Copyright (c) 2013. All rights reserved.
//

#ifndef __SZSG__CCHierarchiesSpriteFilter__
#define __SZSG__CCHierarchiesSpriteFilter__

#include "cocos2d.h"
#include "ExtensionMacros.h"
#include "CCHierarchiesSpriteConfig.h"
#include "CCHierarchiesSpriteAnimation.h"
#include "CCHierarchiesSpriteGrabber.h"

USING_NS_CC;


NS_CC_EXT_BEGIN

/**
 * Filter base class
 */
class CC_DLL CCHierarchiesSpriteFilter : public CCObject {
    CC_SYNTHESIZE_READONLY(CCRect, _frame, Frame);
    CC_SYNTHESIZE_READONLY(CCGLProgram*, _shaderProgram, ShaderProgram);
    CC_SYNTHESIZE_READONLY(CCTexture2D*, _texture, Texture);
    CC_SYNTHESIZE_READONLY(CCHierarchiesSpriteGrabber*, _grabber, Grabber);
    
protected:
    struct ViewPortRect {
        GLint x;
        GLint y;
        GLint w;
        GLint h;
    };
    
    bool _isTextureFlipped;
    GLfloat _texCoordinates[12];
    GLfloat _vertices[12];
    GLushort _indices[6];
    CCPoint _anchorPoint;
    ViewPortRect _oldViewPort;
    
    virtual CCGLProgram* setupShaderProgram () = 0;
    void setupVertexPoints ();
    virtual void blit ();
    
public:
    CCHierarchiesSpriteFilter ();
    virtual ~CCHierarchiesSpriteFilter ();
    
    bool init(bool flipped);
    
    void updateFrame (const CCRect& frame);
    void updateVertexPointsWithTramsform (const CCAffineTransform& matrix, const CCPoint& anchorPoint);
    
    void beforeDraw ();
    void afterDraw ();
    
    virtual void updateFilterParams (CCHierarchiesSpriteAnimation::Filter& filterParams) = 0;
};


/**
 * Blur filter
 */
class CC_DLL CCHierarchiesSpriteBlurFilter : public CCHierarchiesSpriteFilter {
protected:
    virtual CCGLProgram* setupShaderProgram ();
    
public:
    CCHierarchiesSpriteBlurFilter ();
    virtual ~CCHierarchiesSpriteBlurFilter ();
    
    virtual void updateFilterParams (CCHierarchiesSpriteAnimation::Filter& filterParams);
};


/**
 * Glow filter
 */
class CC_DLL CCHierarchiesSpriteGlowFilter : public CCHierarchiesSpriteFilter {
protected:
    virtual CCGLProgram* setupShaderProgram ();
    
public:
    CCHierarchiesSpriteGlowFilter ();
    virtual ~CCHierarchiesSpriteGlowFilter ();
    
    virtual void updateFilterParams (CCHierarchiesSpriteAnimation::Filter& filterParams);
};


/**
 * Shadow filter
 */
class CC_DLL CCHierarchiesSpriteShadowFilter : public CCHierarchiesSpriteFilter {
protected:
    virtual CCGLProgram* setupShaderProgram ();
    
public:
    
    CCHierarchiesSpriteShadowFilter ();
    virtual ~CCHierarchiesSpriteShadowFilter ();
    
    virtual void updateFilterParams (CCHierarchiesSpriteAnimation::Filter& filterParams);
};


/**
 * Bevel filter
 */
class CC_DLL CCHierarchiesSpriteBevelFilter : public CCHierarchiesSpriteFilter {
protected:
    virtual CCGLProgram* setupShaderProgram ();
    
public:
    CCHierarchiesSpriteBevelFilter ();
    ~CCHierarchiesSpriteBevelFilter ();
    
    virtual void updateFilterParams (CCHierarchiesSpriteAnimation::Filter& filterParams);
};

NS_CC_EXT_END


#endif /* defined(__SZSG__CCHierarchiesSpriteFilter__) */
