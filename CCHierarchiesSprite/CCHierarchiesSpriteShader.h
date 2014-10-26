//
//  CCHierarchiesSpriteShader.h
//  HierarchiesSpriteDemo
//
//  Created by xuxiaocheng on 10/23/14.
//
//

#ifndef __HierarchiesSpriteDemo__CCHierarchiesSpriteShader__
#define __HierarchiesSpriteDemo__CCHierarchiesSpriteShader__

#include "cocos2d.h"
#include "ExtensionMacros.h"


// shader program cache key
#define kCCShader_HierarchiesSprite "Shader_HierarchiesSprite"
#define kCCShader_HierarchiesSprite_Premultiplied "Shader_HierarchiesSprite_Premultiplied"


// shader vertex attribute index
enum {
    kCCHierarchiesSprite_VertexAttrib_Color_Mul = cocos2d::kCCVertexAttrib_Color,
    kCCHierarchiesSprite_VertexAttrib_Color_Add = cocos2d::kCCVertexAttrib_MAX,
};


// vertex shader
#define kCCShader_HierarchiesSprite_Vert "kCCShader_HierarchiesSprite_Vert"
extern CC_DLL const GLchar* ccShader_HierarchiesSprite_vert;


// fragment shader (only color transform)
#define kCCShader_HierarchiesSprite_Frag "kCCShader_HierarchiesSprite_Frag"
extern CC_DLL const GLchar* ccShader_HierarchiesSprite_frag;


// fragment shader with pre-multiplied alpha texture (only color transform)
#define kCCShader_HierarchiesSprite_PremultipliedAlpha_Frag "kCCShader_HierarchiesSprite_PremultipliedAlpha_Frag"
extern CC_DLL const GLchar* ccShader_HierarchiesSprite_PremultipliedAlpha_frag;


#endif /* defined(__HierarchiesSpriteDemo__CCHierarchiesSpriteShader__) */
