//
//  CCHierarchiesSpriteMesh.h
//  CCHierarchiesSprite
//
//  Created by bman <zx123xz321hm3@hotmail.com>.
//  Copyright (c) 2013. All rights reserved.
//
//

#ifndef __SZSG__CCHierarchiesSpriteMesh__
#define __SZSG__CCHierarchiesSpriteMesh__

#include "cocos2d.h"
#include "ExtensionMacros.h"
#include "CCHierarchiesSpriteConfig.h"

USING_NS_CC;


NS_CC_EXT_BEGIN

/**
 * CCHierarchiesSprite vertex data structure
 */
typedef struct CC_DLL _CCHierarchiesSprite_V3F_C4B_T2F
{
    // vertices (3F)
    ccVertex3F          vertices;               // 12 bytes
    // colors multiplication (4B)
    ccColor4B           colorsMul;              // 4 bytes
    // colors addition (4B)
    ccColor4B           colorsAdd;              // 4 bytes
    // tex coords (2F)
    ccTex2F             texCoords;              // 8 bytes
//    // socket matrix index (1I)
//    GLint               socketMatrixIndex;      // 4 bytes
} CCHierarchiesSprite_V3F_C4B_T2F;

/**
 * CCHierarchiesSprite quad data structure
 */
typedef struct CC_DLL _CCHierarchiesSprite_V3F_C4B_T2F_Quad
{
    // top left
    CCHierarchiesSprite_V3F_C4B_T2F    tl;
    // bottom left
    CCHierarchiesSprite_V3F_C4B_T2F    bl;
    // top right
    CCHierarchiesSprite_V3F_C4B_T2F    tr;
    // bottom right
    CCHierarchiesSprite_V3F_C4B_T2F    br;
} CCHierarchiesSprite_V3F_C4B_T2F_Quad;

/**
 * CCHierarchiesSprite texture atlas
 */
class CC_DLL CCHierarchiesSpriteMesh : public CCObject {
    
protected:
    GLushort*           m_pIndices;
    GLuint              m_uVAOname;
    GLuint              m_pBuffersVBO[2]; // 0: vertex  1: indices
    bool                m_bDirty; // indicates whether or not the array buffer of the VBO needs to be updated
    
    
    /** quantity of quads that are going to be drawn */
    CC_PROPERTY_READONLY(unsigned int, m_uTotalQuads, TotalQuads)
    /** quantity of quads that can be stored with the current texture atlas size */
    CC_PROPERTY_READONLY(unsigned int, m_uCapacity, Capacity)
    /** Quads that are going to be rendered */
    CC_PROPERTY(CCHierarchiesSprite_V3F_C4B_T2F_Quad*, m_pQuads, Quads)
    
public:
    static CCHierarchiesSpriteMesh* create(unsigned int capacity);
    
    CCHierarchiesSpriteMesh ();
    virtual ~CCHierarchiesSpriteMesh ();
    
    const char* description();
    
    bool initWithCapacity(unsigned int capacity);
    
    /** updates a Quad (texture, vertex and color) at a certain index
     * index must be between 0 and the atlas capacity - 1
     @since v0.8
     */
    void updateQuad(CCHierarchiesSprite_V3F_C4B_T2F_Quad* quad, unsigned int index);
    
    /** Inserts a Quad (texture, vertex and color) at a certain index
     index must be between 0 and the atlas capacity - 1
     @since v0.8
     */
    void insertQuad(CCHierarchiesSprite_V3F_C4B_T2F_Quad* quad, unsigned int index);
    
    /** Inserts a c array of quads at a given index
     index must be between 0 and the atlas capacity - 1
     this method doesn't enlarge the array when amount + index > totalQuads
     @since v1.1
     */
    void insertQuads(CCHierarchiesSprite_V3F_C4B_T2F_Quad* quads, unsigned int index, unsigned int amount);
    
    /** Removes the quad that is located at a certain index and inserts it at a new index
     This operation is faster than removing and inserting in a quad in 2 different steps
     @since v0.7.2
     */
    void insertQuadFromIndex(unsigned int fromIndex, unsigned int newIndex);
    
    /** removes a quad at a given index number.
     The capacity remains the same, but the total number of quads to be drawn is reduced in 1
     @since v0.7.2
     */
    void removeQuadAtIndex(unsigned int index);
    
    /** removes a amount of quads starting from index
     @since 1.1
     */
    void removeQuadsAtIndex(unsigned int index, unsigned int amount);
    /** removes all Quads.
     The TextureAtlas capacity remains untouched. No memory is freed.
     The total number of quads to be drawn will be 0
     @since v0.7.2
     */
    void removeAllQuads();
    
    
    /** resize the capacity of the CCHierarchiesSpriteMesh.
     * The new capacity can be lower or higher than the current one
     * It returns YES if the resize was successful.
     * If it fails to resize the capacity it will return NO with a new capacity of 0.
     */
    bool resizeCapacity(unsigned int n);
    
    /**
     Used internally by CCParticleBatchNode
     don't use this unless you know what you're doing
     @since 1.1
     */
    void increaseTotalQuadsWith(unsigned int amount);
    
    /** Moves an amount of quads from oldIndex at newIndex
     @since v1.1
     */
    void moveQuadsFromIndex(unsigned int oldIndex, unsigned int amount, unsigned int newIndex);
    
    /**
     Moves quads from index till totalQuads to the newIndex
     Used internally by CCParticleBatchNode
     This method doesn't enlarge the array if newIndex + quads to be moved > capacity
     @since 1.1
     */
    void moveQuadsFromIndex(unsigned int index, unsigned int newIndex);
    
    /**
     Ensures that after a realloc quads are still empty
     Used internally by CCParticleBatchNode
     @since 1.1
     */
    void fillWithEmptyQuadsFromIndex(unsigned int index, unsigned int amount);
    
    /** draws n quads
     * n can't be greater than the capacity of the Atlas
     */
    void drawNumberOfQuads(unsigned int n);
    
    /** draws n quads from an index (offset).
     n + start can't be greater than the capacity of the atlas
     
     @since v1.0
     */
    void drawNumberOfQuads(unsigned int n, unsigned int start);
    
    /** draws all the Atlas's Quads
     */
    void drawQuads();
    /** listen the event that coming to foreground on Android
     */
    void listenBackToForeground(CCObject *obj);
    
    /** whether or not the array buffer of the VBO needs to be updated*/
    inline bool isDirty(void) { return m_bDirty; }
    /** specify if the array buffer of the VBO needs to be updated */
    inline void setDirty(bool bDirty) { m_bDirty = bDirty; }
    
    /** flush all quads data to VBO */
    void flushAllQuadsToBuffer ();
    
private:
    void setupIndices();
    void mapBuffers();
    void setupVBOandVAO();
    void setupVBO();
    
};

NS_CC_EXT_END

#endif /* defined(__SZSG__CCHierarchiesSpriteMesh__) */
