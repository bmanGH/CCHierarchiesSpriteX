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


// shader program name
#define kShader_Name_HierarchiesSprite "Shader_Name_HierarchiesSprite"
#define kShader_Name_HierarchiesSprite_Premultiplied "Shader_Name_HierarchiesSprite_Premultiplied"


// shader vertex attribute names and index
extern const char* kHierarchiesSprite_Attribute_Name_ColorMul;
extern const char* kHierarchiesSprite_Attribute_Name_ColorAdd;
enum {
    kHierarchiesSprite_Attribute_ColorMul = cocos2d::GLProgram::VERTEX_ATTRIB_COLOR,
    kHierarchiesSprite_Attribute_ColorAdd = cocos2d::GLProgram::VERTEX_ATTRIB_MAX,
};


// vertex shader
extern CC_DLL const GLchar* ccShader_HierarchiesSprite_vert;


// fragment shader (only color transform)
extern CC_DLL const GLchar* ccShader_HierarchiesSprite_frag;


// fragment shader with pre-multiplied alpha texture (only color transform)
extern CC_DLL const GLchar* ccShader_HierarchiesSprite_PremultipliedAlpha_frag;


#endif /* defined(__HierarchiesSpriteDemo__CCHierarchiesSpriteShader__) */
