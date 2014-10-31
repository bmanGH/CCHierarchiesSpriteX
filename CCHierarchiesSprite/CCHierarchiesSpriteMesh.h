//
//  CCHierarchiesSpriteMesh.h
//  HierarchiesSpriteDemo
//
//  Created by bman on 10/26/14.
//
//

#ifndef __HierarchiesSpriteDemo__CCHierarchiesSpriteMesh__
#define __HierarchiesSpriteDemo__CCHierarchiesSpriteMesh__

#include "cocos2d.h"
#include "ExtensionMacros.h"

USING_NS_CC;

NS_CC_EXT_BEGIN


/**
 * HierarchiesSprite vertex data structure
 */
struct CC_DLL HierarchiesSprite_V3F_C4B_T2F
{
    // vertices (3F)
    Vec3                vertices;               // 12 bytes
    // colors multiplication (4B)
    Color4B             colorsMul;              // 4 bytes
    // colors addition (4B)
    Color4B             colorsAdd;              // 4 bytes
    // tex coords (2F)
    Tex2F                texCoords;              // 8 bytes
};

/**
 * HierarchiesSprite quad data structure
 */
struct CC_DLL HierarchiesSprite_V3F_C4B_T2F_Quad
{
    // top left
    HierarchiesSprite_V3F_C4B_T2F    tl;
    // bottom left
    HierarchiesSprite_V3F_C4B_T2F    bl;
    // top right
    HierarchiesSprite_V3F_C4B_T2F    tr;
    // bottom right
    HierarchiesSprite_V3F_C4B_T2F    br;
};


NS_CC_EXT_END

#endif /* defined(__HierarchiesSpriteDemo__CCHierarchiesSpriteMesh__) */
