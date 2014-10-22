//
//  CCHierarchiesSpriteShader.h
//  CCHierarchiesSprite
//
//  Created by bman <zx123xz321hm3@hotmail.com>.
//  Copyright (c) 2013. All rights reserved.
//

#ifndef __SZSG__CCHierarchiesSpriteShader__
#define __SZSG__CCHierarchiesSpriteShader__

#include "cocos2d.h"
#include "ExtensionMacros.h"
#include "CCHierarchiesSpriteConfig.h"


// shader program cache key
#define kCCShader_HierarchiesSpriteNonFilter "HierarchiesSpriteNonFilter"
#define kCCShader_HierarchiesSpriteNonFilter_Premultiplied "HierarchiesSpriteNonFilter_Premultiplied"


// shader vertex attribute index
enum {
    kCCHierarchiesSprite_VertexAttrib_Color_Mul = cocos2d::kCCVertexAttrib_Color,
    kCCHierarchiesSprite_VertexAttrib_Color_Add = cocos2d::kCCVertexAttrib_MAX,
};


// non-filter vertex shader
#define kCCShader_HierarchiesSpriteNonFilter_Vert "kCCShader_HierarchiesSpriteNonFilter_Vert"
extern CC_DLL const GLchar* ccShader_HierarchiesSpriteNonFilter_vert;


// non-filter fragment shader (only color transform)
#define kCCShader_HierarchiesSpriteNonFilter_Frag "kCCShader_HierarchiesSpriteNonFilter_Frag"
extern CC_DLL const GLchar* ccShader_HierarchiesSpriteNonFilter_frag;


// non-filter fragment shader with pre-multiplied alpha texture (only color transform)
#define kCCShader_HierarchiesSpriteNonFilter_PremultipliedAlpha_Frag "kCCShader_HierarchiesSpriteNonFilter_PremultipliedAlpha_Frag"
extern CC_DLL const GLchar* ccShader_HierarchiesSpriteNonFilter_PremultipliedAlpha_frag;


// filter vertex shader
#define kCCShader_HierarchiesSpriteFilter_Vert "kCCShader_HierarchiesSpriteFilter_Vert"
extern CC_DLL const GLchar* ccShader_HierarchiesSpriteFilter_vert;


// blur fragment shader
#define kCCShader_HierarchiesSpriteFilter_Blur "kCCShader_HierarchiesSpriteFilter_Blur"
extern CC_DLL const GLchar* ccShader_HierarchiesSpriteFilter_Blur_frag;


// glow fragment shader
#define kCCShader_HierarchiesSpriteFilter_Glow "kCCShader_HierarchiesSpriteFilter_Glow"
extern CC_DLL const GLchar* ccShader_HierarchiesSpriteFilter_Glow_frag;

#define kCCShader_HierarchiesSpriteFilter_Glow_Inner "kCCShader_HierarchiesSpriteFilter_Glow_Inner"
extern CC_DLL const GLchar* ccShader_HierarchiesSpriteFilter_Glow_Inner_frag;

#define kCCShader_HierarchiesSpriteFilter_Glow_Knockout "kCCShader_HierarchiesSpriteFilter_Glow_Knockout"
extern CC_DLL const GLchar* ccShader_HierarchiesSpriteFilter_Glow_Knockout_frag;

#define kCCShader_HierarchiesSpriteFilter_Glow_InnerAndKnockout "kCCShader_HierarchiesSpriteFilter_Glow_InnerAndKnockout"
extern CC_DLL const GLchar* ccShader_HierarchiesSpriteFilter_Glow_InnerAndKnockout_frag;


// shadow fragment shader
#define kCCShader_HierarchiesSpriteFilter_Shadow "kCCShader_HierarchiesSpriteFilter_Shadow"
extern CC_DLL const GLchar* ccShader_HierarchiesSpriteFilter_Shadow_frag;

#define kCCShader_HierarchiesSpriteFilter_Shadow_Inner "kCCShader_HierarchiesSpriteFilter_Shadow_Inner"
extern CC_DLL const GLchar* ccShader_HierarchiesSpriteFilter_Shadow_Inner_frag;

#define kCCShader_HierarchiesSpriteFilter_Shadow_Knockout "kCCShader_HierarchiesSpriteFilter_Shadow_Knockout"
extern CC_DLL const GLchar* ccShader_HierarchiesSpriteFilter_Shadow_Knockout_frag;

#define kCCShader_HierarchiesSpriteFilter_Shadow_HideObject "kCCShader_HierarchiesSpriteFilter_Shadow_HideObject"
extern CC_DLL const GLchar* ccShader_HierarchiesSpriteFilter_Shadow_HideObject_frag;

#define kCCShader_HierarchiesSpriteFilter_Shadow_InnerAndKnockout "kCCShader_HierarchiesSpriteFilter_Shadow_InnerAndKnockout"
extern CC_DLL const GLchar* ccShader_HierarchiesSpriteFilter_Shadow_InnerAndKnockout_frag;

// Knockout = Knockout + HideObject

// Knockout + Inner = Inner + HideObject = Knockout + Inner + HideObject


// bevel fragment shader
#define kCCShader_HierarchiesSpriteFilter_Bevel_Outer "kCCShader_HierarchiesSpriteFilter_Bevel_Outer"
extern CC_DLL const GLchar* ccShader_HierarchiesSpriteFilter_Bevel_Outer_frag;

#define kCCShader_HierarchiesSpriteFilter_Bevel_Knockout_Outer "kCCShader_HierarchiesSpriteFilter_Bevel_Knockout_Outer"
extern CC_DLL const GLchar* ccShader_HierarchiesSpriteFilter_Bevel_Knockout_Outer_frag;

#define kCCShader_HierarchiesSpriteFilter_Bevel_Inner "kCCShader_HierarchiesSpriteFilter_Bevel_Inner"
extern CC_DLL const GLchar* ccShader_HierarchiesSpriteFilter_Bevel_Inner_frag;

#define kCCShader_HierarchiesSpriteFilter_Bevel_Knockout_Inner "kCCShader_HierarchiesSpriteFilter_Bevel_Knockout_Inner"
extern CC_DLL const GLchar* ccShader_HierarchiesSpriteFilter_Bevel_Knockout_Inner_frag;

#define kCCShader_HierarchiesSpriteFilter_Bevel_Full "kCCShader_HierarchiesSpriteFilter_Bevel_Full"
extern CC_DLL const GLchar* ccShader_HierarchiesSpriteFilter_Bevel_Full_frag;

#define kCCShader_HierarchiesSpriteFilter_Bevel_Knockout_Full "kCCShader_HierarchiesSpriteFilter_Bevel_Knockout_Full"
extern CC_DLL const GLchar* ccShader_HierarchiesSpriteFilter_Bevel_Knockout_Full_frag;


#endif /* defined(__SZSG__CCHierarchiesSpriteShader__) */
