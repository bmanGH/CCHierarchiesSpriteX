//
//  CCHierarchiesSpriteShader.cpp
//  CCHierarchiesSprite
//
//  Created by bman <zx123xz321hm3@hotmail.com>.
//  Copyright (c) 2013. All rights reserved.
//

#include "CCHierarchiesSpriteShader.h"

#define STRINGIZE(x) #x
#define SHADER_STRING(x) STRINGIZE(x)

#ifdef GL_ES_VERSION_2_0
#define GL_ES
#endif

#pragma mark - Non-Filter vertex shader

const GLchar* ccShader_HierarchiesSpriteNonFilter_vert = SHADER_STRING
(
#ifdef GL_ES
precision highp float;
#endif

attribute vec4 a_position;
attribute vec4 a_colorMul;
attribute vec4 a_colorAdd;
attribute vec2 a_texCoord;

varying lowp vec4 v_fragmentColorMul;
varying lowp vec4 v_fragmentColorAdd;
varying mediump vec2 v_texCoord;

void main()
{
    gl_Position = CC_MVPMatrix * a_position;
    v_fragmentColorMul = a_colorMul.rgba;
    v_fragmentColorAdd = a_colorAdd.rgba;
    v_texCoord = a_texCoord.xy;
}
);


#pragma mark - Non-Filter fragment shader

const GLchar* ccShader_HierarchiesSpriteNonFilter_frag = SHADER_STRING
(
#ifdef GL_ES
precision lowp float;
#endif

uniform	vec4 u_color;

varying lowp vec4 v_fragmentColorMul;
varying lowp vec4 v_fragmentColorAdd;
varying mediump vec2 v_texCoord;

uniform sampler2D CC_Texture0;

void main()
{
    vec4 color = texture2D(CC_Texture0, v_texCoord);
    color *= v_fragmentColorMul;
    color += v_fragmentColorAdd * color.a;
    gl_FragColor = clamp(color, 0.0, 1.0) * u_color;
}
);

const GLchar* ccShader_HierarchiesSpriteNonFilter_PremultipliedAlpha_frag = SHADER_STRING
(
#ifdef GL_ES
precision lowp float;
#endif

uniform	vec4 u_color;

varying lowp vec4 v_fragmentColorMul;
varying lowp vec4 v_fragmentColorAdd;
varying mediump vec2 v_texCoord;

uniform sampler2D CC_Texture0;

void main()
{
    vec4 color = texture2D(CC_Texture0, v_texCoord);
    color.rgb = color.rgb / color.a;
    color *= v_fragmentColorMul;
    color += v_fragmentColorAdd;
    gl_FragColor = clamp(color, 0.0, 1.0) * u_color;
    gl_FragColor.rgb = gl_FragColor.rgb * gl_FragColor.a;
}
);


#pragma mark - Filter vertex shader

const GLchar* ccShader_HierarchiesSpriteFilter_vert = SHADER_STRING
(
#ifdef GL_ES
 precision highp float;
#endif

attribute vec4 a_position;
attribute vec2 a_texCoord;
attribute vec4 a_color;

//uniform highp vec2 u_texelSize;
//uniform highp vec2 u_filterSize;
//uniform mediump vec2 u_offset;

varying lowp vec4 v_fragmentColor;
varying mediump vec2 v_texCoord;
varying mediump vec2 v_texCoord_L;
varying mediump vec2 v_texCoord_R;
varying mediump vec2 v_texCoord_T;
varying mediump vec2 v_texCoord_B;

void main()
{
    gl_Position = CC_MVPMatrix * a_position;
    v_fragmentColor = a_color;
    
    vec2 u_texelSize = vec2(1.0 / 256.0, 1.0 / 256.0);
    vec2 u_filterSize = vec2(5.0, 5.0);
    vec2 u_offset = vec2(-10.0, 10.0);
    
    v_texCoord = a_texCoord.xy;
    v_texCoord_L = a_texCoord.xy + (u_offset.xy + vec2(-u_filterSize.x, 0.0)) * u_texelSize.xy;
    v_texCoord_R = a_texCoord.xy + (u_offset.xy + vec2(u_filterSize.x, 0.0)) * u_texelSize.xy;
    v_texCoord_T = a_texCoord.xy + (u_offset.xy + vec2(0.0, u_filterSize.y)) * u_texelSize.xy;
    v_texCoord_B = a_texCoord.xy + (u_offset.xy + vec2(0.0, -u_filterSize.y)) * u_texelSize.xy;
}
);


#pragma mark - Blur fragment shader

const GLchar* ccShader_HierarchiesSpriteFilter_Blur_frag = SHADER_STRING
(
#ifdef GL_ES
precision mediump float;
#endif

uniform lowp sampler2D CC_Texture0;
 
varying lowp vec4 v_fragmentColor;
varying mediump vec2 v_texCoord;
varying mediump vec2 v_texCoord_L;
varying mediump vec2 v_texCoord_R;
varying mediump vec2 v_texCoord_T;
varying mediump vec2 v_texCoord_B;

void main()
{
    lowp vec4 origColor = texture2D(CC_Texture0, v_texCoord);
    
//    // simple box filter
//    vec4 colorH = vec4(0.0);
//    vec4 colorV = vec4(0.0);
//    colorH += texture2D(CC_Texture0, v_texCoord_L);
//    colorH += origColor;
//    colorH += texture2D(CC_Texture0, v_texCoord_R);
//    colorH /= 3.0;
//    colorV += texture2D(CC_Texture0, v_texCoord_T);
//    colorV += origColor;
//    colorV += texture2D(CC_Texture0, v_texCoord_B);
//    colorV /= 3.0;
//    vec4 fcolor = colorH * 0.5 + colorV * 0.5;
    
    mediump vec2 u_texelSize = vec2(1.0 / 256.0, 1.0 / 256.0);
    // box filter
    ivec2 fsize = ivec2(5, 5);
    vec4 fcolor = vec4(0.0, 0.0, 0.0, 0.0);
    for (int x = -fsize.x; x <= fsize.x; x++)
    {
        for (int y = -fsize.y; y <= fsize.y; y++)
        {
            fcolor += texture2D(CC_Texture0,  v_texCoord + vec2(float(x), float(y)) * u_texelSize.xy);
        }
    }
    fcolor /= float(fsize.x * 2 * fsize.y * 2) + 1.0;
    
    gl_FragColor = fcolor;
}
);


#pragma mark - Glow fragment shader

const GLchar* ccShader_HierarchiesSpriteFilter_Glow_frag = SHADER_STRING
(
#ifdef GL_ES
 precision mediump float;
#endif
 
 uniform lowp sampler2D CC_Texture0;
 // uniform mediump ivec2 u_glowSize;
 // uniform mediump vec2 u_texelSize;
 // uniform lowp vec3 u_glowColor;
 
 varying lowp vec4 v_fragmentColor;
 varying mediump vec2 v_texCoord;
 varying mediump vec2 v_texCoord_L;
 varying mediump vec2 v_texCoord_R;
 varying mediump vec2 v_texCoord_T;
 varying mediump vec2 v_texCoord_B;
 
 void main()
{
    lowp vec4 glowColor = vec4(1.0, 0.0, 0.0, 1.0);
    mediump float u_strength = 1.0;
    lowp vec4 origColor = texture2D(CC_Texture0, v_texCoord);
    
//    // simple box filter
//    lowp vec4 colorH = vec4(0.0);
//    lowp vec4 colorV = vec4(0.0);
//    colorH += texture2D(CC_Texture0, v_texCoord_L);
//    colorH += origColor;
//    colorH += texture2D(CC_Texture0, v_texCoord_R);
//    colorH /= 3.0;
//    colorV += texture2D(CC_Texture0, v_texCoord_T);
//    colorV += origColor;
//    colorV += texture2D(CC_Texture0, v_texCoord_B);
//    colorV /= 3.0;
//    lowp vec4 fcolor = colorH * 0.5 + colorV * 0.5;
    
    mediump vec2 u_texelSize = vec2(1.0 / 256.0, 1.0 / 256.0);
    // box filter
    ivec2 fsize = ivec2(5, 5);
    vec4 fcolor = vec4(0.0, 0.0, 0.0, 0.0);
    for (int x = -fsize.x; x <= fsize.x; x++)
    {
        for (int y = -fsize.y; y <= fsize.y; y++)
        {
            fcolor += texture2D(CC_Texture0,  v_texCoord + vec2(float(x), float(y)) * u_texelSize.xy);
        }
    }
    fcolor /= float(fsize.x * 2 * fsize.y * 2) + 1.0;
    
    gl_FragColor = glowColor * clamp(fcolor.a * (1.0 - origColor.a) * u_strength, 0.0, 1.0) + origColor;
}
);

const GLchar* ccShader_HierarchiesSpriteFilter_Glow_Inner_frag = SHADER_STRING
(
#ifdef GL_ES
 precision mediump float;
#endif
 
 uniform lowp sampler2D CC_Texture0;
 //uniform lowp vec3 u_glowColor;
 //uniform mediump float u_strength;
 
 varying lowp vec4 v_fragmentColor;
 varying mediump vec2 v_texCoord;
 varying mediump vec2 v_texCoord_L;
 varying mediump vec2 v_texCoord_R;
 varying mediump vec2 v_texCoord_T;
 varying mediump vec2 v_texCoord_B;
 
 void main()
{
    lowp vec4 glowColor = vec4(1.0, 0.0, 0.0, 1.0);
    mediump float u_strength = 1.0;
    lowp vec4 origColor = texture2D(CC_Texture0, v_texCoord);
    
//    // simple box filter
//    mediump vec4 colorH = vec4(0.0);
//    mediump vec4 colorV = vec4(0.0);
//    mediump vec4 texColor = vec4(0.0);
//    texColor = texture2D(CC_Texture0, v_texCoord_L); colorH += vec4(texColor.rgb, 1.0 - texColor.a);
//    colorH += vec4(origColor.rgb, 1.0 - origColor.a);
//    texColor = texture2D(CC_Texture0, v_texCoord_R); colorH += vec4(texColor.rgb, 1.0 - texColor.a);
//    colorH /= 3.0;
//    texColor = texture2D(CC_Texture0, v_texCoord_T); colorV += vec4(texColor.rgb, 1.0 - texColor.a);
//    colorV += vec4(origColor.rgb, 1.0 - origColor.a);
//    texColor = texture2D(CC_Texture0, v_texCoord_B); colorV += vec4(texColor.rgb, 1.0 - texColor.a);
//    colorV /= 3.0;
//    lowp vec4 fcolor = colorH * 0.5 + colorV * 0.5;
    
    mediump vec2 u_texelSize = vec2(1.0 / 256.0, 1.0 / 256.0);
    // box filter
    ivec2 fsize = ivec2(5, 5);
    vec4 fcolor = vec4(0.0, 0.0, 0.0, 0.0);
    for (int x = -fsize.x; x <= fsize.x; x++)
    {
        for (int y = -fsize.y; y <= fsize.y; y++)
        {
            vec4 texColor = texture2D(CC_Texture0,  v_texCoord + vec2(float(x), float(y)) * u_texelSize.xy);
            fcolor += vec4(texColor.rgb, 1.0 - texColor.a);
        }
    }
    fcolor /= float(fsize.x * 2 * fsize.y * 2) + 1.0;
    
    float lerpval = clamp((origColor.a - fcolor.a), 0.0, 1.0);
    gl_FragColor = mix(origColor, glowColor, (1.0 - lerpval) * u_strength) * origColor.a;
}
);

const GLchar* ccShader_HierarchiesSpriteFilter_Glow_Knockout_frag = SHADER_STRING
(
#ifdef GL_ES
 precision mediump float;
#endif
 
 uniform lowp sampler2D CC_Texture0;
 //uniform lowp vec3 u_glowColor;
 //uniform mediump float u_strength;
 
 varying lowp vec4 v_fragmentColor;
 varying mediump vec2 v_texCoord;
 varying mediump vec2 v_texCoord_L;
 varying mediump vec2 v_texCoord_R;
 varying mediump vec2 v_texCoord_T;
 varying mediump vec2 v_texCoord_B;
 
 void main()
{
    lowp vec4 glowColor = vec4(1.0, 0.0, 0.0, 1.0);
    mediump float u_strength = 1.0;
    lowp vec4 origColor = texture2D(CC_Texture0, v_texCoord);
    
//    // simple box filter
//    lowp vec4 colorH = vec4(0.0);
//    lowp vec4 colorV = vec4(0.0);
//    colorH += texture2D(CC_Texture0, v_texCoord_L);
//    colorH += origColor;
//    colorH += texture2D(CC_Texture0, v_texCoord_R);
//    colorH /= 3.0;
//    colorV += texture2D(CC_Texture0, v_texCoord_T);
//    colorV += origColor;
//    colorV += texture2D(CC_Texture0, v_texCoord_B);
//    colorV /= 3.0;
//    lowp vec4 fcolor = colorH * 0.5 + colorV * 0.5;
    
    mediump vec2 u_texelSize = vec2(1.0 / 256.0, 1.0 / 256.0);
    // box filter
    ivec2 fsize = ivec2(5, 5);
    vec4 fcolor = vec4(0.0, 0.0, 0.0, 0.0);
    for (int x = -fsize.x; x <= fsize.x; x++)
    {
        for (int y = -fsize.y; y <= fsize.y; y++)
        {
            fcolor += texture2D(CC_Texture0,  v_texCoord + vec2(float(x), float(y)) * u_texelSize.xy);
        }
    }
    fcolor /= float(fsize.x * 2 * fsize.y * 2) + 1.0;
    
    gl_FragColor = glowColor * clamp(fcolor.a * (1.0 - origColor.a) * u_strength, 0.0, 1.0) + origColor;
    gl_FragColor -= origColor;
}
);

const GLchar* ccShader_HierarchiesSpriteFilter_Glow_InnerAndKnockout_frag = SHADER_STRING
( 
#ifdef GL_ES
 precision mediump float;
#endif
 
 uniform lowp sampler2D CC_Texture0;
 //uniform lowp vec3 u_glowColor;
 //uniform mediump float u_strength;
 
 varying lowp vec4 v_fragmentColor;
 varying mediump vec2 v_texCoord;
 varying mediump vec2 v_texCoord_L;
 varying mediump vec2 v_texCoord_R;
 varying mediump vec2 v_texCoord_T;
 varying mediump vec2 v_texCoord_B;
 
 void main()
{
    lowp vec4 glowColor = vec4(1.0, 0.0, 0.0, 1.0);
    mediump float u_strength = 1.0;
    lowp vec4 origColor = texture2D(CC_Texture0, v_texCoord);
    
//    // simple box filter
//    mediump vec4 colorH = vec4(0.0);
//    mediump vec4 colorV = vec4(0.0);
//    mediump vec4 texColor = vec4(0.0);
//    texColor = texture2D(CC_Texture0, v_texCoord_L); colorH += vec4(texColor.rgb, 1.0 - texColor.a);
//    colorH += vec4(origColor.rgb, 1.0 - origColor.a);
//    texColor = texture2D(CC_Texture0, v_texCoord_R); colorH += vec4(texColor.rgb, 1.0 - texColor.a);
//    colorH /= 3.0;
//    texColor = texture2D(CC_Texture0, v_texCoord_T); colorV += vec4(texColor.rgb, 1.0 - texColor.a);
//    colorV += vec4(origColor.rgb, 1.0 - origColor.a);
//    texColor = texture2D(CC_Texture0, v_texCoord_B); colorV += vec4(texColor.rgb, 1.0 - texColor.a);
//    colorV /= 3.0;
//    lowp vec4 fcolor = colorH * 0.5 + colorV * 0.5;
    
    mediump vec2 u_texelSize = vec2(1.0 / 256.0, 1.0 / 256.0);
    // box filter
    ivec2 fsize = ivec2(5, 5);
    vec4 fcolor = vec4(0.0, 0.0, 0.0, 0.0);
    for (int x = -fsize.x; x <= fsize.x; x++)
    {
        for (int y = -fsize.y; y <= fsize.y; y++)
        {
            vec4 texColor = texture2D(CC_Texture0,  v_texCoord + vec2(float(x), float(y)) * u_texelSize.xy);
            fcolor += vec4(texColor.rgb, 1.0 - texColor.a);
        }
    }
    fcolor /= float(fsize.x * 2 * fsize.y * 2) + 1.0;
    
    float lerpval = clamp((origColor.a - fcolor.a), 0.0, 1.0);
    gl_FragColor = mix(vec4(0.0), glowColor, (1.0 - lerpval) * u_strength) * origColor.a;
}
);


#pragma mark - Shadow fragment shader

const GLchar* ccShader_HierarchiesSpriteFilter_Shadow_frag = SHADER_STRING
(
#ifdef GL_ES
 precision mediump float;
#endif
 
 uniform lowp sampler2D CC_Texture0;
 //uniform lowp vec3 u_shadowColor;
 //uniform mediump float u_strength;
 //uniform mediump vec2 u_offset;
 
 varying lowp vec4 v_fragmentColor;
 varying mediump vec2 v_texCoord;
 varying mediump vec2 v_texCoord_L;
 varying mediump vec2 v_texCoord_R;
 varying mediump vec2 v_texCoord_T;
 varying mediump vec2 v_texCoord_B;
 
 void main()
{
    lowp vec4 shadowColor = vec4(1.0, 0.0, 0.0, 1.0);
    mediump float u_strength = 1.0;
    
    lowp vec4 origColor = texture2D(CC_Texture0, v_texCoord);
    
//    // simple box filter
//    mediump vec4 colorH = vec4(0.0);
//    mediump vec4 colorV = vec4(0.0);
//    colorH += texture2D(CC_Texture0, v_texCoord_L);
//    colorH += origColor;
//    colorH += texture2D(CC_Texture0, v_texCoord_R);
//    colorH /= 3.0;
//    colorV += texture2D(CC_Texture0, v_texCoord_T);
//    colorV += origColor;
//    colorV += texture2D(CC_Texture0, v_texCoord_B);
//    colorV /= 3.0;
//    lowp vec4 fcolor = colorH * 0.5 + colorV * 0.5;
    
    mediump vec2 u_texelSize = vec2(1.0 / 256.0, 1.0 / 256.0);
    mediump vec2 u_offset = vec2(-10.0, 10.0);
    // box filter
    ivec2 fsize = ivec2(5, 5);
    vec4 fcolor = vec4(0.0, 0.0, 0.0, 0.0);
    for (int x = -fsize.x; x <= fsize.x; x++)
    {
        for (int y = -fsize.y; y <= fsize.y; y++)
        {
            fcolor += texture2D(CC_Texture0,  v_texCoord + (u_offset.xy + vec2(float(x), float(y))) * u_texelSize.xy);
        }
    }
    fcolor /= float(fsize.x * 2 * fsize.y * 2) + 1.0;
    
    gl_FragColor = shadowColor * clamp(fcolor.a * (1.0 - origColor.a) * u_strength, 0.0, 1.0) + origColor * origColor.a;
}
);

const GLchar* ccShader_HierarchiesSpriteFilter_Shadow_Inner_frag = SHADER_STRING
(
#ifdef GL_ES
 precision mediump float;
#endif
 
 uniform lowp sampler2D CC_Texture0;
 //uniform lowp vec3 u_shadowColor;
 //uniform mediump float u_strength;
 //uniform mediump vec2 u_offset;
 
 varying lowp vec4 v_fragmentColor;
 varying mediump vec2 v_texCoord;
 varying mediump vec2 v_texCoord_L;
 varying mediump vec2 v_texCoord_R;
 varying mediump vec2 v_texCoord_T;
 varying mediump vec2 v_texCoord_B;
 
 void main()
{
    lowp vec4 shadowColor = vec4(1.0, 0.0, 0.0, 1.0);
    mediump float u_strength = 1.0;
    
    lowp vec4 origColor = texture2D(CC_Texture0, v_texCoord);
    
//    // simple box filter
//    mediump vec4 colorH = vec4(0.0);
//    mediump vec4 colorV = vec4(0.0);
//    mediump vec4 texColor = vec4(0.0);
//    texColor = texture2D(CC_Texture0, v_texCoord_L); colorH += vec4(texColor.rgb, 1.0 - texColor.a);
//    colorH += vec4(origColor.rgb, 1.0 - origColor.a);
//    texColor = texture2D(CC_Texture0, v_texCoord_R); colorH += vec4(texColor.rgb, 1.0 - texColor.a);
//    colorH /= 3.0;
//    texColor = texture2D(CC_Texture0, v_texCoord_T); colorV += vec4(texColor.rgb, 1.0 - texColor.a);
//    colorV += vec4(origColor.rgb, 1.0 - origColor.a);
//    texColor = texture2D(CC_Texture0, v_texCoord_B); colorV += vec4(texColor.rgb, 1.0 - texColor.a);
//    colorV /= 3.0;
//    lowp vec4 fcolor = colorH * 0.5 + colorV * 0.5;
    
    mediump vec2 u_texelSize = vec2(1.0 / 256.0, 1.0 / 256.0);
    mediump vec2 u_offset = vec2(-10.0, 10.0);
    // box filter
    ivec2 fsize = ivec2(5, 5);
    vec4 fcolor = vec4(0.0, 0.0, 0.0, 0.0);
    for (int x = -fsize.x; x <= fsize.x; x++)
    {
        for (int y = -fsize.y; y <= fsize.y; y++)
        {
            vec4 texColor = texture2D(CC_Texture0,  v_texCoord + (u_offset.xy + vec2(float(x), float(y))) * u_texelSize.xy);
            fcolor += vec4(texColor.rgb, 1.0 - texColor.a);
        }
    }
    fcolor /= float(fsize.x * 2 * fsize.y * 2) + 1.0;
    
    float lerpval = clamp((origColor.a - fcolor.a), 0.0, 1.0);
    gl_FragColor = mix(origColor, shadowColor, (1.0 - lerpval) * u_strength) * origColor.a;
}
);

const GLchar* ccShader_HierarchiesSpriteFilter_Shadow_Knockout_frag = SHADER_STRING
(
#ifdef GL_ES
 precision mediump float;
#endif
 
 uniform lowp sampler2D CC_Texture0;
 //uniform lowp vec3 u_shadowColor;
 //uniform mediump float u_strength;
 //uniform mediump vec2 u_offset;
 
 varying lowp vec4 v_fragmentColor;
 varying mediump vec2 v_texCoord;
 varying mediump vec2 v_texCoord_L;
 varying mediump vec2 v_texCoord_R;
 varying mediump vec2 v_texCoord_T;
 varying mediump vec2 v_texCoord_B;
 
 void main()
{
    lowp vec4 shadowColor = vec4(1.0, 0.0, 0.0, 1.0);
    mediump float u_strength = 1.0;
    
    lowp vec4 origColor = texture2D(CC_Texture0, v_texCoord);
    
//    // simple box filter
//    mediump vec4 colorH = vec4(0.0);
//    mediump vec4 colorV = vec4(0.0);
//    colorH += texture2D(CC_Texture0, v_texCoord_L);
//    colorH += origColor;
//    colorH += texture2D(CC_Texture0, v_texCoord_R);
//    colorH /= 3.0;
//    colorV += texture2D(CC_Texture0, v_texCoord_T);
//    colorV += origColor;
//    colorV += texture2D(CC_Texture0, v_texCoord_B);
//    colorV /= 3.0;
//    lowp vec4 fcolor = colorH * 0.5 + colorV * 0.5;
    
    mediump vec2 u_texelSize = vec2(1.0 / 256.0, 1.0 / 256.0);
    mediump vec2 u_offset = vec2(-10.0, 10.0);
    // box filter
    ivec2 fsize = ivec2(5, 5);
    vec4 fcolor = vec4(0.0, 0.0, 0.0, 0.0);
    for (int x = -fsize.x; x <= fsize.x; x++)
    {
        for (int y = -fsize.y; y <= fsize.y; y++)
        {
            fcolor += texture2D(CC_Texture0,  v_texCoord + (u_offset.xy + vec2(float(x), float(y))) * u_texelSize.xy);
        }
    }
    fcolor /= float(fsize.x * 2 * fsize.y * 2) + 1.0;
    
    gl_FragColor = shadowColor * clamp(fcolor.a * (1.0 - origColor.a) * u_strength, 0.0, 1.0) + origColor;
    gl_FragColor -= origColor;
}
);

const GLchar* ccShader_HierarchiesSpriteFilter_Shadow_HideObject_frag = SHADER_STRING
(
#ifdef GL_ES
 precision mediump float;
#endif
 
 uniform lowp sampler2D CC_Texture0;
 //uniform lowp vec3 u_shadowColor;
 //uniform mediump float u_strength;
 //uniform mediump vec2 u_offset;
 
 varying lowp vec4 v_fragmentColor;
 varying mediump vec2 v_texCoord;
 varying mediump vec2 v_texCoord_L;
 varying mediump vec2 v_texCoord_R;
 varying mediump vec2 v_texCoord_T;
 varying mediump vec2 v_texCoord_B;
 
 void main()
{
    lowp vec4 shadowColor = vec4(1.0, 0.0, 0.0, 1.0);
    mediump float u_strength = 1.0;
    
    lowp vec4 origColor = texture2D(CC_Texture0, v_texCoord);
    
//    // simple box filter
//    mediump vec4 colorH = vec4(0.0);
//    mediump vec4 colorV = vec4(0.0);
//    colorH += texture2D(CC_Texture0, v_texCoord_L);
//    colorH += origColor;
//    colorH += texture2D(CC_Texture0, v_texCoord_R);
//    colorH /= 3.0;
//    colorV += texture2D(CC_Texture0, v_texCoord_T);
//    colorV += origColor;
//    colorV += texture2D(CC_Texture0, v_texCoord_B);
//    colorV /= 3.0;
//    lowp vec4 fcolor = colorH * 0.5 + colorV * 0.5;
    
    mediump vec2 u_texelSize = vec2(1.0 / 256.0, 1.0 / 256.0);
    mediump vec2 u_offset = vec2(-10.0, 10.0);
    // box filter
    ivec2 fsize = ivec2(5, 5);
    vec4 fcolor = vec4(0.0, 0.0, 0.0, 0.0);
    for (int x = -fsize.x; x <= fsize.x; x++)
    {
        for (int y = -fsize.y; y <= fsize.y; y++)
        {
            fcolor += texture2D(CC_Texture0,  v_texCoord + (u_offset.xy + vec2(float(x), float(y))) * u_texelSize.xy);
        }
    }
    fcolor /= float(fsize.x * 2 * fsize.y * 2) + 1.0;
    
    gl_FragColor = vec4(shadowColor.rgb, fcolor.a);
}
);

const GLchar* ccShader_HierarchiesSpriteFilter_Shadow_InnerAndKnockout_frag = SHADER_STRING
(
#ifdef GL_ES
 precision mediump float;
#endif
 
 uniform lowp sampler2D CC_Texture0;
 //uniform lowp vec3 u_shadowColor;
 //uniform mediump float u_strength;
 //uniform mediump vec2 u_offset;
 
 varying lowp vec4 v_fragmentColor;
 varying mediump vec2 v_texCoord;
 varying mediump vec2 v_texCoord_L;
 varying mediump vec2 v_texCoord_R;
 varying mediump vec2 v_texCoord_T;
 varying mediump vec2 v_texCoord_B;
 
 void main()
{
    lowp vec4 shadowColor = vec4(1.0, 0.0, 0.0, 1.0);
    mediump float u_strength = 1.0;
    
    lowp vec4 origColor = texture2D(CC_Texture0, v_texCoord);
    
//    // simple box filter
//    mediump vec4 colorH = vec4(0.0);
//    mediump vec4 colorV = vec4(0.0);
//    mediump vec4 texColor = vec4(0.0);
//    texColor = texture2D(CC_Texture0, v_texCoord_L); colorH += vec4(texColor.rgb, 1.0 - texColor.a);
//    colorH += vec4(origColor.rgb, 1.0 - origColor.a);
//    texColor = texture2D(CC_Texture0, v_texCoord_R); colorH += vec4(texColor.rgb, 1.0 - texColor.a);
//    colorH /= 3.0;
//    texColor = texture2D(CC_Texture0, v_texCoord_T); colorV += vec4(texColor.rgb, 1.0 - texColor.a);
//    colorV += vec4(origColor.rgb, 1.0 - origColor.a);
//    texColor = texture2D(CC_Texture0, v_texCoord_B); colorV += vec4(texColor.rgb, 1.0 - texColor.a);
//    colorV /= 3.0;
//    lowp vec4 fcolor = colorH * 0.5 + colorV * 0.5;
    
    mediump vec2 u_texelSize = vec2(1.0 / 256.0, 1.0 / 256.0);
    mediump vec2 u_offset = vec2(-10.0, 10.0);
    // box filter
    ivec2 fsize = ivec2(5, 5);
    vec4 fcolor = vec4(0.0, 0.0, 0.0, 0.0);
    for (int x = -fsize.x; x <= fsize.x; x++)
    {
        for (int y = -fsize.y; y <= fsize.y; y++)
        {
            vec4 texColor = texture2D(CC_Texture0,  v_texCoord + (u_offset.xy + vec2(float(x), float(y))) * u_texelSize.xy);
            fcolor += vec4(texColor.rgb, 1.0 - texColor.a);
        }
    }
    fcolor /= float(fsize.x * 2 * fsize.y * 2) + 1.0;
    
    float lerpval = clamp((origColor.a - fcolor.a), 0.0, 1.0);
    gl_FragColor = mix(vec4(0.0), shadowColor, (1.0 - lerpval) * u_strength) * origColor.a;
}
);


#pragma mark - Bevel fragment shader

const GLchar* ccShader_HierarchiesSpriteFilter_Bevel_Outer_frag = SHADER_STRING
(
#ifdef GL_ES
 precision mediump float;
#endif
 
 uniform lowp sampler2D CC_Texture0;
 //uniform lowp vec3 u_shadowColor;
 //uniform lowp vec3 u_highlightColor;
 //uniform mediump float u_strength;
 //uniform mediump vec2 u_offset;
 
 varying lowp vec4 v_fragmentColor;
 varying mediump vec2 v_texCoord;
 varying mediump vec2 v_texCoord_L;
 varying mediump vec2 v_texCoord_R;
 varying mediump vec2 v_texCoord_T;
 varying mediump vec2 v_texCoord_B;
 
 void main()
{
    lowp vec4 shadowColor = vec4(1.0, 0.0, 0.0, 1.0);
    lowp vec4 highlightColor = vec4(0.0, 1.0, 0.0, 1.0);
    mediump float u_strength = 1.0;
    lowp vec4 origColor = texture2D(CC_Texture0, v_texCoord);
    
    mediump vec2 u_texelSize = vec2(1.0 / 256.0, 1.0 / 256.0);
    mediump vec2 u_offset = vec2(-10.0, 10.0);
    // box filter
    ivec2 fsize = ivec2(5, 5);
    vec4 fcolor = vec4(0.0, 0.0, 0.0, 0.0);
    for (int x = -fsize.x; x <= fsize.x; x++)
    {
        for (int y = -fsize.y; y <= fsize.y; y++)
        {
            fcolor.a += texture2D(CC_Texture0,  v_texCoord + (u_offset.xy + vec2(float(x), float(y))) * u_texelSize.xy).a;
            fcolor.r += texture2D(CC_Texture0,  v_texCoord - (u_offset.xy + vec2(float(x), float(y))) * u_texelSize.xy).a;
        }
    }
    fcolor /= float(fsize.x * 2 * fsize.y * 2) + 1.0;
    
//    fcolor.ra = clamp((1.0 - fcolor.ar * u_strength) - (1.0 - fcolor.ra * u_strength), 0.0, 1.0);
    fcolor.ar = clamp((fcolor.ar - fcolor.ra) * u_strength, 0.0, 1.0);
    gl_FragColor = (shadowColor * fcolor.a + highlightColor * fcolor.r) * (1.0 - origColor.a) + origColor;
}
);

const GLchar* ccShader_HierarchiesSpriteFilter_Bevel_Knockout_Outer_frag = SHADER_STRING
(
#ifdef GL_ES
 precision mediump float;
#endif
 
 uniform lowp sampler2D CC_Texture0;
 //uniform lowp vec3 u_shadowColor;
 //uniform lowp vec3 u_highlightColor;
 //uniform mediump float u_strength;
 //uniform mediump vec2 u_offset;
 
 varying lowp vec4 v_fragmentColor;
 varying mediump vec2 v_texCoord;
 varying mediump vec2 v_texCoord_L;
 varying mediump vec2 v_texCoord_R;
 varying mediump vec2 v_texCoord_T;
 varying mediump vec2 v_texCoord_B;
 
 void main()
{
    lowp vec4 shadowColor = vec4(1.0, 0.0, 0.0, 1.0);
    lowp vec4 highlightColor = vec4(0.0, 1.0, 0.0, 1.0);
    mediump float u_strength = 1.0;
    lowp vec4 origColor = texture2D(CC_Texture0, v_texCoord);
    
    mediump vec2 u_texelSize = vec2(1.0 / 256.0, 1.0 / 256.0);
    mediump vec2 u_offset = vec2(-10.0, 10.0);
    // box filter
    ivec2 fsize = ivec2(5, 5);
    vec4 fcolor = vec4(0.0, 0.0, 0.0, 0.0);
    for (int x = -fsize.x; x <= fsize.x; x++)
    {
        for (int y = -fsize.y; y <= fsize.y; y++)
        {
            fcolor.a += texture2D(CC_Texture0,  v_texCoord + (u_offset.xy + vec2(float(x), float(y))) * u_texelSize.xy).a;
            fcolor.r += texture2D(CC_Texture0,  v_texCoord - (u_offset.xy + vec2(float(x), float(y))) * u_texelSize.xy).a;
        }
    }
    fcolor /= float(fsize.x * 2 * fsize.y * 2) + 1.0;
    
//    fcolor.ra = clamp((1.0 - fcolor.ar * u_strength) - (1.0 - fcolor.ra * u_strength), 0.0, 1.0);
    fcolor.ar = clamp((fcolor.ar - fcolor.ra) * u_strength, 0.0, 1.0);
    gl_FragColor = (shadowColor * fcolor.a + highlightColor * fcolor.r) * (1.0 - origColor.a) + origColor;
    gl_FragColor -= origColor;
}
);

const GLchar* ccShader_HierarchiesSpriteFilter_Bevel_Inner_frag = SHADER_STRING
(
#ifdef GL_ES
 precision mediump float;
#endif
 
 uniform lowp sampler2D CC_Texture0;
 //uniform lowp vec3 u_shadowColor;
 //uniform lowp vec3 u_highlightColor;
 //uniform mediump float u_strength;
 //uniform mediump vec2 u_offset;
 
 varying lowp vec4 v_fragmentColor;
 varying mediump vec2 v_texCoord;
 varying mediump vec2 v_texCoord_L;
 varying mediump vec2 v_texCoord_R;
 varying mediump vec2 v_texCoord_T;
 varying mediump vec2 v_texCoord_B;
 
 void main()
{
    lowp vec4 shadowColor = vec4(1.0, 0.0, 0.0, 1.0);
    lowp vec4 highlightColor = vec4(0.0, 1.0, 0.0, 1.0);
    mediump float u_strength = 1.0;
    lowp vec4 origColor = texture2D(CC_Texture0, v_texCoord);
    
    mediump vec2 u_texelSize = vec2(1.0 / 256.0, 1.0 / 256.0);
    mediump vec2 u_offset = vec2(-10.0, 10.0);
    // box filter
    ivec2 fsize = ivec2(5, 5);
    vec4 fcolor = vec4(0.0, 0.0, 0.0, 0.0);
    for (int x = -fsize.x; x <= fsize.x; x++)
    {
        for (int y = -fsize.y; y <= fsize.y; y++)
        {
            fcolor.a += texture2D(CC_Texture0,  v_texCoord + (u_offset.xy + vec2(float(x), float(y))) * u_texelSize.xy).a;
            fcolor.r += texture2D(CC_Texture0,  v_texCoord - (u_offset.xy + vec2(float(x), float(y))) * u_texelSize.xy).a;
        }
    }
    fcolor /= float(fsize.x * 2 * fsize.y * 2) + 1.0;
    
//    fcolor.ra = clamp((1.0 - fcolor.ar * u_strength) - (1.0 - fcolor.ra * u_strength), 0.0, 1.0);
    fcolor.ar = clamp((fcolor.ar - fcolor.ra) * u_strength, 0.0, 1.0);
    gl_FragColor = (shadowColor * fcolor.a + highlightColor * fcolor.r) * origColor.a + origColor;
}
);

const GLchar* ccShader_HierarchiesSpriteFilter_Bevel_Knockout_Inner_frag = SHADER_STRING
(
#ifdef GL_ES
 precision mediump float;
#endif
 
 uniform lowp sampler2D CC_Texture0;
 //uniform lowp vec3 u_shadowColor;
 //uniform lowp vec3 u_highlightColor;
 //uniform mediump float u_strength;
 //uniform mediump vec2 u_offset;
 
 varying lowp vec4 v_fragmentColor;
 varying mediump vec2 v_texCoord;
 varying mediump vec2 v_texCoord_L;
 varying mediump vec2 v_texCoord_R;
 varying mediump vec2 v_texCoord_T;
 varying mediump vec2 v_texCoord_B;
 
 void main()
{
    lowp vec4 shadowColor = vec4(1.0, 0.0, 0.0, 1.0);
    lowp vec4 highlightColor = vec4(0.0, 1.0, 0.0, 1.0);
    mediump float u_strength = 1.0;
    lowp vec4 origColor = texture2D(CC_Texture0, v_texCoord);
    
    mediump vec2 u_texelSize = vec2(1.0 / 256.0, 1.0 / 256.0);
    mediump vec2 u_offset = vec2(-10.0, 10.0);
    // box filter
    ivec2 fsize = ivec2(5, 5);
    vec4 fcolor = vec4(0.0, 0.0, 0.0, 0.0);
    for (int x = -fsize.x; x <= fsize.x; x++)
    {
        for (int y = -fsize.y; y <= fsize.y; y++)
        {
            fcolor.a += texture2D(CC_Texture0,  v_texCoord + (u_offset.xy + vec2(float(x), float(y))) * u_texelSize.xy).a;
            fcolor.r += texture2D(CC_Texture0,  v_texCoord - (u_offset.xy + vec2(float(x), float(y))) * u_texelSize.xy).a;
        }
    }
    fcolor /= float(fsize.x * 2 * fsize.y * 2) + 1.0;
    
//    fcolor.ra = clamp((1.0 - fcolor.ar * u_strength) - (1.0 - fcolor.ra * u_strength), 0.0, 1.0);
    fcolor.ar = clamp((fcolor.ar - fcolor.ra) * u_strength, 0.0, 1.0);
    gl_FragColor = (shadowColor * fcolor.a + highlightColor * fcolor.r) * origColor.a + origColor;
    gl_FragColor -= origColor;
}
);

const GLchar* ccShader_HierarchiesSpriteFilter_Bevel_Full_frag = SHADER_STRING
(
#ifdef GL_ES
 precision mediump float;
#endif
 
 uniform lowp sampler2D CC_Texture0;
 //uniform lowp vec3 u_shadowColor;
 //uniform lowp vec3 u_highlightColor;
 //uniform mediump float u_strength;
 //uniform mediump vec2 u_offset;
 
 varying lowp vec4 v_fragmentColor;
 varying mediump vec2 v_texCoord;
 varying mediump vec2 v_texCoord_L;
 varying mediump vec2 v_texCoord_R;
 varying mediump vec2 v_texCoord_T;
 varying mediump vec2 v_texCoord_B;
 
 void main()
{
    lowp vec4 shadowColor = vec4(1.0, 0.0, 0.0, 1.0);
    lowp vec4 highlightColor = vec4(0.0, 1.0, 0.0, 1.0);
    mediump float u_strength = 1.0;
    lowp vec4 origColor = texture2D(CC_Texture0, v_texCoord);
    
    mediump vec2 u_texelSize = vec2(1.0 / 256.0, 1.0 / 256.0);
    mediump vec2 u_offset = vec2(-10.0, 10.0);
    // box filter
    ivec2 fsize = ivec2(5, 5);
    vec4 fcolor = vec4(0.0, 0.0, 0.0, 0.0);
    for (int x = -fsize.x; x <= fsize.x; x++)
    {
        for (int y = -fsize.y; y <= fsize.y; y++)
        {
            fcolor.a += texture2D(CC_Texture0,  v_texCoord + (u_offset.xy + vec2(float(x), float(y))) * u_texelSize.xy).a;
            fcolor.r += texture2D(CC_Texture0,  v_texCoord - (u_offset.xy + vec2(float(x), float(y))) * u_texelSize.xy).a;
        }
    }
    fcolor /= float(fsize.x * 2 * fsize.y * 2) + 1.0;
    
//    fcolor.ra = clamp((1.0 - fcolor.ar * u_strength) - (1.0 - fcolor.ra * u_strength), 0.0, 1.0);
    fcolor.ar = clamp((fcolor.ar - fcolor.ra) * u_strength, 0.0, 1.0);
    gl_FragColor = (shadowColor * fcolor.a + highlightColor * fcolor.r) + origColor;
}
);

const GLchar* ccShader_HierarchiesSpriteFilter_Bevel_Knockout_Full_frag = SHADER_STRING
(
#ifdef GL_ES
 precision mediump float;
#endif
 
 uniform lowp sampler2D CC_Texture0;
 //uniform lowp vec3 u_shadowColor;
 //uniform lowp vec3 u_highlightColor;
 //uniform mediump float u_strength;
 //uniform mediump vec2 u_offset;
 
 varying lowp vec4 v_fragmentColor;
 varying mediump vec2 v_texCoord;
 varying mediump vec2 v_texCoord_L;
 varying mediump vec2 v_texCoord_R;
 varying mediump vec2 v_texCoord_T;
 varying mediump vec2 v_texCoord_B;
 
 void main()
{
    lowp vec4 shadowColor = vec4(1.0, 0.0, 0.0, 1.0);
    lowp vec4 highlightColor = vec4(0.0, 1.0, 0.0, 1.0);
    mediump float u_strength = 1.0;
    lowp vec4 origColor = texture2D(CC_Texture0, v_texCoord);
    
    mediump vec2 u_texelSize = vec2(1.0 / 256.0, 1.0 / 256.0);
    mediump vec2 u_offset = vec2(-10.0, 10.0);
    // box filter
    ivec2 fsize = ivec2(5, 5);
    vec4 fcolor = vec4(0.0, 0.0, 0.0, 0.0);
    for (int x = -fsize.x; x <= fsize.x; x++)
    {
        for (int y = -fsize.y; y <= fsize.y; y++)
        {
            fcolor.a += texture2D(CC_Texture0,  v_texCoord + (u_offset.xy + vec2(float(x), float(y))) * u_texelSize.xy).a;
            fcolor.r += texture2D(CC_Texture0,  v_texCoord - (u_offset.xy + vec2(float(x), float(y))) * u_texelSize.xy).a;
        }
    }
    fcolor /= float(fsize.x * 2 * fsize.y * 2) + 1.0;
    
//    fcolor.ra = clamp((1.0 - fcolor.ar * u_strength) - (1.0 - fcolor.ra * u_strength), 0.0, 1.0);
    fcolor.ar = clamp((fcolor.ar - fcolor.ra) * u_strength, 0.0, 1.0);
    gl_FragColor = (shadowColor * fcolor.a + highlightColor * fcolor.r) + origColor;
    gl_FragColor -= origColor;
}
);
