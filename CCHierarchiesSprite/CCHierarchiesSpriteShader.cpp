//
//  CCHierarchiesSpriteShader.cpp
//  HierarchiesSpriteDemo
//
//  Created by xuxiaocheng on 10/23/14.
//
//

#include "CCHierarchiesSpriteShader.h"


const char* kHierarchiesSprite_GLProgram_Uniform_Name_ColorMul = "u_colorMul";
const char* kHierarchiesSprite_GLProgram_Uniform_Name_ColorAdd = "u_colorAdd";
const char* kHierarchiesSprite_GLProgram_Attribute_Name_ColorMul = "a_colorMul";
const char* kHierarchiesSprite_GLProgram_Attribute_Name_ColorAdd = "a_colorAdd";


#define STRINGIFY(A)  #A

#pragma mark - Non-Filter vertex shader

const GLchar* ccShader_HierarchiesSprite_vert = STRINGIFY
(
\n#ifdef GL_ES\n
precision highp float;
\n#endif\n
 
attribute vec4 a_position;
attribute vec4 a_colorMul;
attribute vec4 a_colorAdd;
attribute vec2 a_texCoord;

\n#ifdef GL_ES\n
varying lowp vec4 v_fragmentColorMul;
varying lowp vec4 v_fragmentColorAdd;
varying mediump vec2 v_texCoord;
\n#else\n
varying vec4 v_fragmentColorMul;
varying vec4 v_fragmentColorAdd;
varying vec2 v_texCoord;
\n#endif\n
 
void main()
{
    gl_Position = CC_MVPMatrix * a_position;
    v_fragmentColorMul = a_colorMul.rgba;
    v_fragmentColorAdd = a_colorAdd.rgba;
    v_texCoord = a_texCoord.xy;
}
);


#pragma mark - Non-Filter fragment shader

const GLchar* ccShader_HierarchiesSprite_frag = STRINGIFY
(
\n#ifdef GL_ES\n
precision lowp float;
\n#endif\n
 
uniform vec4 u_colorMul;
uniform	vec4 u_colorAdd;

\n#ifdef GL_ES\n
varying lowp vec4 v_fragmentColorMul;
varying lowp vec4 v_fragmentColorAdd;
varying mediump vec2 v_texCoord;
\n#else\n
varying vec4 v_fragmentColorMul;
varying vec4 v_fragmentColorAdd;
varying vec2 v_texCoord;
\n#endif\n
 
void main()
{
    vec4 color = texture2D(CC_Texture0, v_texCoord);
    
    color *= v_fragmentColorMul;
    color += v_fragmentColorAdd;
    
    gl_FragColor = clamp(clamp(color, 0.0, 1.0) * u_colorMul + u_colorAdd, 0.0, 1.0);
}
);

const GLchar* ccShader_HierarchiesSprite_PremultipliedAlpha_frag = STRINGIFY
(
\n#ifdef GL_ES\n
precision lowp float;
\n#endif\n
 
uniform vec4 u_colorMul;
uniform	vec4 u_colorAdd;

\n#ifdef GL_ES\n
varying lowp vec4 v_fragmentColorMul;
varying lowp vec4 v_fragmentColorAdd;
varying mediump vec2 v_texCoord;
\n#else\n
varying vec4 v_fragmentColorMul;
varying vec4 v_fragmentColorAdd;
varying vec2 v_texCoord;
\n#endif\n
 
void main()
{
    vec4 color = texture2D(CC_Texture0, v_texCoord);
    color.rgb = color.rgb / color.a;
    
    color *= v_fragmentColorMul;
    color += v_fragmentColorAdd;
    
    gl_FragColor = clamp(clamp(color, 0.0, 1.0) * u_colorMul + u_colorAdd, 0.0, 1.0);
    gl_FragColor.rgb = gl_FragColor.rgb * gl_FragColor.a;
}
);

