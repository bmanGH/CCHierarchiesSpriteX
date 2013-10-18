//
//  CCHierarchiesSpriteMesh.cpp
//  CCHierarchiesSprite
//
//  Created by bman <zx123xz321hm3@hotmail.com>.
//  Copyright (c) 2013. All rights reserved.
//
//

#include "CCHierarchiesSpriteMesh.h"
#include "CCHierarchiesSpriteShader.h"


NS_CC_EXT_BEGIN

CCHierarchiesSpriteMesh::CCHierarchiesSpriteMesh()
:m_pIndices(NULL)
,m_bDirty(false)
,m_pQuads(NULL)
{}

CCHierarchiesSpriteMesh::~CCHierarchiesSpriteMesh()
{
    CCLOGINFO("cocos2d: CCHierarchiesSpriteMesh deallocing %p.", this);
    
    CC_SAFE_FREE(m_pQuads);
    CC_SAFE_FREE(m_pIndices);
    
    glDeleteBuffers(2, m_pBuffersVBO);
    
#if CC_TEXTURE_ATLAS_USE_VAO
    if (CCConfiguration::sharedConfiguration()->supportsShareableVAO()) {
        glDeleteVertexArrays(1, &m_uVAOname);
    }
#endif
    
#if CC_ENABLE_CACHE_TEXTURE_DATA
    CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, EVENT_COME_TO_FOREGROUND);
#endif
}

unsigned int CCHierarchiesSpriteMesh::getTotalQuads()
{
    return m_uTotalQuads;
}

unsigned int CCHierarchiesSpriteMesh::getCapacity()
{
    return m_uCapacity;
}

CCHierarchiesSprite_V3F_C4B_T2F_Quad* CCHierarchiesSpriteMesh::getQuads()
{
    //if someone accesses the quads directly, presume that changes will be made
    m_bDirty = true;
    return m_pQuads;
}

void CCHierarchiesSpriteMesh::setQuads(CCHierarchiesSprite_V3F_C4B_T2F_Quad *var)
{
    m_pQuads = var;
}

// TextureAtlas - alloc & init

CCHierarchiesSpriteMesh* CCHierarchiesSpriteMesh::create(unsigned int capacity)
{
    CCHierarchiesSpriteMesh* pTextureAtlas = new CCHierarchiesSpriteMesh();
    if (pTextureAtlas && pTextureAtlas->initWithCapacity(capacity))
    {
        pTextureAtlas->autorelease();
        return pTextureAtlas;
    }
    CC_SAFE_DELETE(pTextureAtlas);
    return NULL;
}

bool CCHierarchiesSpriteMesh::initWithCapacity(unsigned int capacity)
{
    m_uCapacity = capacity;
    m_uTotalQuads = 0;
    
    // Re-initialization is not allowed
    CCAssert(m_pQuads == NULL && m_pIndices == NULL, "");
    
    m_pQuads = (CCHierarchiesSprite_V3F_C4B_T2F_Quad*)malloc( m_uCapacity * sizeof(CCHierarchiesSprite_V3F_C4B_T2F_Quad) );
    m_pIndices = (GLushort *)malloc( m_uCapacity * 6 * sizeof(GLushort) );
    
    if( ! ( m_pQuads && m_pIndices) && m_uCapacity > 0)
    {
        //CCLOG("cocos2d: CCHierarchiesSpriteMesh: not enough memory");
        CC_SAFE_FREE(m_pQuads);
        CC_SAFE_FREE(m_pIndices);
        return false;
    }
    
    memset( m_pQuads, 0, m_uCapacity * sizeof(CCHierarchiesSprite_V3F_C4B_T2F_Quad) );
    memset( m_pIndices, 0, m_uCapacity * 6 * sizeof(GLushort) );
    
#if CC_ENABLE_CACHE_TEXTURE_DATA
    // listen the event when app go to background
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this,
                                                                  callfuncO_selector(CCHierarchiesSpriteMesh::listenBackToForeground),
                                                                  EVNET_COME_TO_FOREGROUND,
                                                                  NULL);
#endif
    
#if CC_TEXTURE_ATLAS_USE_VAO
    if (CCConfiguration::sharedConfiguration()->supportsShareableVAO()) {
        setupVBOandVAO();
    }
    else {
        setupVBO();
    }
#else
    setupVBO();
#endif
    
    m_bDirty = true;
    
    return true;
}

void CCHierarchiesSpriteMesh::listenBackToForeground(CCObject *obj)
{
#if CC_TEXTURE_ATLAS_USE_VAO
    if (CCConfiguration::sharedConfiguration()->supportsShareableVAO()) {
        setupVBOandVAO();
    }
    else {
        setupVBO();
    }
#else
    setupVBO();
#endif
    
    // set m_bDirty to true to force it rebinding buffer
    m_bDirty = true;
    flushAllQuadsToBuffer();
}

const char* CCHierarchiesSpriteMesh::description()
{
    return CCString::createWithFormat("<CCHierarchiesSpriteMesh | totalQuads = %u>", m_uTotalQuads)->getCString();
}


void CCHierarchiesSpriteMesh::setupIndices()
{
    if (m_uCapacity == 0)
        return;
    
    for( unsigned int i=0; i < m_uCapacity; i++)
    {
#if HIERARCHIES_USE_TRIANGLE_STRIP
        m_pIndices[i*6+0] = i*4+0;
        m_pIndices[i*6+1] = i*4+0;
        m_pIndices[i*6+2] = i*4+2;
        m_pIndices[i*6+3] = i*4+1;
        m_pIndices[i*6+4] = i*4+3;
        m_pIndices[i*6+5] = i*4+3;
#else
        m_pIndices[i*6+0] = i*4+0;
        m_pIndices[i*6+1] = i*4+1;
        m_pIndices[i*6+2] = i*4+2;
        
        // inverted index. issue #179
        m_pIndices[i*6+3] = i*4+3;
        m_pIndices[i*6+4] = i*4+2;
        m_pIndices[i*6+5] = i*4+1;
#endif
    }
}

//TextureAtlas - VAO / VBO specific
#if CC_TEXTURE_ATLAS_USE_VAO
void CCHierarchiesSpriteMesh::setupVBOandVAO()
{
    glGenVertexArrays(1, &m_uVAOname);
    ccGLBindVAO(m_uVAOname);
    
#define kQuadSize sizeof(m_pQuads[0].bl)
    
    glGenBuffers(2, &m_pBuffersVBO[0]);
    
    glBindBuffer(GL_ARRAY_BUFFER, m_pBuffersVBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_pQuads[0]) * m_uCapacity, m_pQuads, GL_DYNAMIC_DRAW);
    
    // vertices
    glEnableVertexAttribArray(kCCVertexAttrib_Position);
    glVertexAttribPointer(kCCVertexAttrib_Position, 3, GL_FLOAT, GL_FALSE, kQuadSize, (GLvoid*) offsetof( CCHierarchiesSprite_V3F_C4B_T2F, vertices));
    
    // colors
    glEnableVertexAttribArray(kCCHierarchiesSprite_VertexAttrib_Color_Mul);
    glVertexAttribPointer(kCCHierarchiesSprite_VertexAttrib_Color_Mul, 4, GL_UNSIGNED_BYTE, GL_TRUE, kQuadSize, (GLvoid*) offsetof( CCHierarchiesSprite_V3F_C4B_T2F, colorsMul));
    glEnableVertexAttribArray(kCCHierarchiesSprite_VertexAttrib_Color_Add);
    glVertexAttribPointer(kCCHierarchiesSprite_VertexAttrib_Color_Add, 4, GL_UNSIGNED_BYTE, GL_TRUE, kQuadSize, (GLvoid*) offsetof( CCHierarchiesSprite_V3F_C4B_T2F, colorsAdd));
    
    // tex coords
    glEnableVertexAttribArray(kCCVertexAttrib_TexCoords);
    glVertexAttribPointer(kCCVertexAttrib_TexCoords, 2, GL_FLOAT, GL_FALSE, kQuadSize, (GLvoid*) offsetof( CCHierarchiesSprite_V3F_C4B_T2F, texCoords));
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_pBuffersVBO[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_pIndices[0]) * m_uCapacity * 6, m_pIndices, GL_STATIC_DRAW);
    
    // Must unbind the VAO before changing the element buffer.
    ccGLBindVAO(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    CHECK_GL_ERROR_DEBUG();
}
#endif

void CCHierarchiesSpriteMesh::setupVBO()
{
    glGenBuffers(2, &m_pBuffersVBO[0]);
}

void CCHierarchiesSpriteMesh::mapBuffers()
{
    // Avoid changing the element buffer for whatever VAO might be bound.
	ccGLBindVAO(0);
    
    glBindBuffer(GL_ARRAY_BUFFER, m_pBuffersVBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_pQuads[0]) * m_uCapacity, m_pQuads, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_pBuffersVBO[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_pIndices[0]) * m_uCapacity * 6, m_pIndices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    CHECK_GL_ERROR_DEBUG();
}

// TextureAtlas - Update, Insert, Move & Remove

void CCHierarchiesSpriteMesh::updateQuad(CCHierarchiesSprite_V3F_C4B_T2F_Quad *quad, unsigned int index)
{
    CCAssert( index >= 0 && index < m_uCapacity, "updateQuadWithTexture: Invalid index");
    
    m_uTotalQuads = MAX( index+1, m_uTotalQuads);
    
    m_pQuads[index] = *quad;
    
    m_bDirty = true;
}

void CCHierarchiesSpriteMesh::insertQuad(CCHierarchiesSprite_V3F_C4B_T2F_Quad *quad, unsigned int index)
{
    CCAssert( index < m_uCapacity, "insertQuadWithTexture: Invalid index");
    
    m_uTotalQuads++;
    CCAssert( m_uTotalQuads <= m_uCapacity, "invalid totalQuads");
    
    // issue #575. index can be > totalQuads
    unsigned int remaining = (m_uTotalQuads-1) - index;
    
    // last object doesn't need to be moved
    if( remaining > 0)
    {
        // texture coordinates
        memmove( &m_pQuads[index+1],&m_pQuads[index], sizeof(m_pQuads[0]) * remaining );
    }
    
    m_pQuads[index] = *quad;
    
    m_bDirty = true;
}

void CCHierarchiesSpriteMesh::insertQuads(CCHierarchiesSprite_V3F_C4B_T2F_Quad* quads, unsigned int index, unsigned int amount)
{
    CCAssert(index + amount <= m_uCapacity, "insertQuadWithTexture: Invalid index + amount");
    
    m_uTotalQuads += amount;
    
    CCAssert( m_uTotalQuads <= m_uCapacity, "invalid totalQuads");
    
    // issue #575. index can be > totalQuads
    int remaining = (m_uTotalQuads-1) - index - amount;
    
    // last object doesn't need to be moved
    if( remaining > 0)
    {
        // tex coordinates
        memmove( &m_pQuads[index+amount],&m_pQuads[index], sizeof(m_pQuads[0]) * remaining );
    }
    
    unsigned int max = index + amount;
    unsigned int j = 0;
    for (unsigned int i = index; i < max ; i++)
    {
        m_pQuads[index] = quads[j];
        index++;
        j++;
    }
    
    m_bDirty = true;
}

void CCHierarchiesSpriteMesh::insertQuadFromIndex(unsigned int oldIndex, unsigned int newIndex)
{
    CCAssert( newIndex >= 0 && newIndex < m_uTotalQuads, "insertQuadFromIndex:atIndex: Invalid index");
    CCAssert( oldIndex >= 0 && oldIndex < m_uTotalQuads, "insertQuadFromIndex:atIndex: Invalid index");
    
    if( oldIndex == newIndex )
    {
        return;
    }
    // because it is ambiguous in iphone, so we implement abs ourselves
    // unsigned int howMany = abs( oldIndex - newIndex);
    unsigned int howMany = (oldIndex - newIndex) > 0 ? (oldIndex - newIndex) :  (newIndex - oldIndex);
    unsigned int dst = oldIndex;
    unsigned int src = oldIndex + 1;
    if( oldIndex > newIndex)
    {
        dst = newIndex+1;
        src = newIndex;
    }
    
    // texture coordinates
    CCHierarchiesSprite_V3F_C4B_T2F_Quad quadsBackup = m_pQuads[oldIndex];
    memmove( &m_pQuads[dst],&m_pQuads[src], sizeof(m_pQuads[0]) * howMany );
    m_pQuads[newIndex] = quadsBackup;
    
    m_bDirty = true;
}

void CCHierarchiesSpriteMesh::removeQuadAtIndex(unsigned int index)
{
    CCAssert( index < m_uTotalQuads, "removeQuadAtIndex: Invalid index");
    
    unsigned int remaining = (m_uTotalQuads-1) - index;
    
    
    // last object doesn't need to be moved
    if( remaining )
    {
        // texture coordinates
        memmove( &m_pQuads[index],&m_pQuads[index+1], sizeof(m_pQuads[0]) * remaining );
    }
    
    m_uTotalQuads--;
    
    m_bDirty = true;
}

void CCHierarchiesSpriteMesh::removeQuadsAtIndex(unsigned int index, unsigned int amount)
{
    CCAssert(index + amount <= m_uTotalQuads, "removeQuadAtIndex: index + amount out of bounds");
    
    unsigned int remaining = (m_uTotalQuads) - (index + amount);
    
    m_uTotalQuads -= amount;
    
    if ( remaining )
    {
        memmove( &m_pQuads[index], &m_pQuads[index+amount], sizeof(m_pQuads[0]) * remaining );
    }
    
    m_bDirty = true;
}

void CCHierarchiesSpriteMesh::removeAllQuads()
{
    m_uTotalQuads = 0;
}

bool CCHierarchiesSpriteMesh::resizeCapacity(unsigned int newCapacity)
{
    if( newCapacity == m_uCapacity )
    {
        return true;
    }
    unsigned int uOldCapactiy = m_uCapacity;
    // update capacity and totolQuads
    m_uTotalQuads = MIN(m_uTotalQuads, newCapacity);
    m_uCapacity = newCapacity;
    
    CCHierarchiesSprite_V3F_C4B_T2F_Quad* tmpQuads = NULL;
    GLushort* tmpIndices = NULL;
    
    // when calling initWithTexture(fileName, 0) on bada device, calloc(0, 1) will fail and return NULL,
    // so here must judge whether m_pQuads and m_pIndices is NULL.
    if (m_pQuads == NULL)
    {
        tmpQuads = (CCHierarchiesSprite_V3F_C4B_T2F_Quad*)malloc( m_uCapacity * sizeof(m_pQuads[0]) );
        if (tmpQuads != NULL)
        {
            memset(tmpQuads, 0, m_uCapacity * sizeof(m_pQuads[0]) );
        }
    }
    else
    {
        tmpQuads = (CCHierarchiesSprite_V3F_C4B_T2F_Quad*)realloc( m_pQuads, sizeof(m_pQuads[0]) * m_uCapacity );
        if (tmpQuads != NULL && m_uCapacity > uOldCapactiy)
        {
            memset(tmpQuads+uOldCapactiy, 0, (m_uCapacity - uOldCapactiy)*sizeof(m_pQuads[0]) );
        }
    }
    
    if (m_pIndices == NULL)
    {
        tmpIndices = (GLushort*)malloc( m_uCapacity * 6 * sizeof(m_pIndices[0]) );
        if (tmpIndices != NULL)
        {
            memset( tmpIndices, 0, m_uCapacity * 6 * sizeof(m_pIndices[0]) );
        }
        
    }
    else
    {
        tmpIndices = (GLushort*)realloc( m_pIndices, sizeof(m_pIndices[0]) * m_uCapacity * 6 );
        if (tmpIndices != NULL && m_uCapacity > uOldCapactiy)
        {
            memset( tmpIndices+uOldCapactiy, 0, (m_uCapacity-uOldCapactiy) * 6 * sizeof(m_pIndices[0]) );
        }
    }
    
    if( ! ( tmpQuads && tmpIndices) ) {
        CCLOG("cocos2d: CCHierarchiesSpriteMesh: not enough memory");
        CC_SAFE_FREE(tmpQuads);
        CC_SAFE_FREE(tmpIndices);
        CC_SAFE_FREE(m_pQuads);
        CC_SAFE_FREE(m_pIndices);
        m_uCapacity = m_uTotalQuads = 0;
        return false;
    }
    
    m_pQuads = tmpQuads;
    m_pIndices = tmpIndices;
    
    m_bDirty = true;
    
    return true;
}

void CCHierarchiesSpriteMesh::increaseTotalQuadsWith(unsigned int amount)
{
    m_uTotalQuads += amount;
}

void CCHierarchiesSpriteMesh::moveQuadsFromIndex(unsigned int oldIndex, unsigned int amount, unsigned int newIndex)
{
    CCAssert(newIndex + amount <= m_uTotalQuads, "insertQuadFromIndex:atIndex: Invalid index");
    CCAssert(oldIndex < m_uTotalQuads, "insertQuadFromIndex:atIndex: Invalid index");
    
    if( oldIndex == newIndex )
    {
        return;
    }
    //create buffer
    size_t quadSize = sizeof(CCHierarchiesSprite_V3F_C4B_T2F_Quad);
    CCHierarchiesSprite_V3F_C4B_T2F_Quad* tempQuads = (CCHierarchiesSprite_V3F_C4B_T2F_Quad*)malloc( quadSize * amount);
    memcpy( tempQuads, &m_pQuads[oldIndex], quadSize * amount );
    
    if (newIndex < oldIndex)
    {
        // move quads from newIndex to newIndex + amount to make room for buffer
        memmove( &m_pQuads[newIndex], &m_pQuads[newIndex+amount], (oldIndex-newIndex)*quadSize);
    }
    else
    {
        // move quads above back
        memmove( &m_pQuads[oldIndex], &m_pQuads[oldIndex+amount], (newIndex-oldIndex)*quadSize);
    }
    memcpy( &m_pQuads[newIndex], tempQuads, amount*quadSize);
    
    free(tempQuads);
    
    m_bDirty = true;
}

void CCHierarchiesSpriteMesh::moveQuadsFromIndex(unsigned int index, unsigned int newIndex)
{
    CCAssert(newIndex + (m_uTotalQuads - index) <= m_uCapacity, "moveQuadsFromIndex move is out of bounds");
    
    memmove(m_pQuads + newIndex,m_pQuads + index, (m_uTotalQuads - index) * sizeof(m_pQuads[0]));
}

void CCHierarchiesSpriteMesh::fillWithEmptyQuadsFromIndex(unsigned int index, unsigned int amount)
{
    CCHierarchiesSprite_V3F_C4B_T2F_Quad quad;
    memset(&quad, 0, sizeof(quad));
    
    unsigned int to = index + amount;
    for (unsigned int i = index ; i < to ; i++)
    {
        m_pQuads[i] = quad;
    }
}


#pragma mark - Draw

void CCHierarchiesSpriteMesh::drawQuads()
{
    this->drawNumberOfQuads(m_uTotalQuads, 0);
}

void CCHierarchiesSpriteMesh::drawNumberOfQuads(unsigned int n)
{
    this->drawNumberOfQuads(n, 0);
}

void CCHierarchiesSpriteMesh::drawNumberOfQuads(unsigned int n, unsigned int start)
{
    if (0 == n)
    {
        return;
    }
    
#if CC_TEXTURE_ATLAS_USE_VAO
    if (CCConfiguration::sharedConfiguration()->supportsShareableVAO()) {
        //
        // Using VBO and VAO
        //
        ccGLBindVAO(m_uVAOname);
        
#   if CC_REBIND_INDICES_BUFFER
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_pBuffersVBO[1]);
#   endif
        
#   if HIERARCHIES_USE_TRIANGLE_STRIP
        glDrawElements(GL_TRIANGLE_STRIP, (GLsizei) n*6, GL_UNSIGNED_SHORT, (GLvoid*) (start*6*sizeof(m_pIndices[0])) );
#   else
        glDrawElements(GL_TRIANGLES, (GLsizei) n*6, GL_UNSIGNED_SHORT, (GLvoid*) (start*6*sizeof(m_pIndices[0])) );
#   endif // HIERARCHIES_USE_TRIANGLE_STRIP
        
#   if CC_REBIND_INDICES_BUFFER
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
#   endif
        
        //    glBindVertexArray(0);
    }
    else
    {
        //
        // Using VBO without VAO
        //
        
#   define kQuadSize sizeof(m_pQuads[0].bl)
        glBindBuffer(GL_ARRAY_BUFFER, m_pBuffersVBO[0]);
        
        ccGLEnableVertexAttribs(kCCVertexAttribFlag_Position | kCCVertexAttribFlag_TexCoords);
        glEnableVertexAttribArray(kCCHierarchiesSprite_VertexAttrib_Color_Mul);
        glEnableVertexAttribArray(kCCHierarchiesSprite_VertexAttrib_Color_Add);
        
        // vertices
        glVertexAttribPointer(kCCVertexAttrib_Position, 3, GL_FLOAT, GL_FALSE, kQuadSize, (GLvoid*) offsetof(CCHierarchiesSprite_V3F_C4B_T2F, vertices));
        
        // colors
        glVertexAttribPointer(kCCHierarchiesSprite_VertexAttrib_Color_Mul, 4, GL_UNSIGNED_BYTE, GL_TRUE, kQuadSize, (GLvoid*) offsetof(CCHierarchiesSprite_V3F_C4B_T2F, colorsMul));
        glVertexAttribPointer(kCCHierarchiesSprite_VertexAttrib_Color_Add, 4, GL_UNSIGNED_BYTE, GL_TRUE, kQuadSize, (GLvoid*) offsetof(CCHierarchiesSprite_V3F_C4B_T2F, colorsAdd));
        
        // tex coords
        glVertexAttribPointer(kCCVertexAttrib_TexCoords, 2, GL_FLOAT, GL_FALSE, kQuadSize, (GLvoid*) offsetof(CCHierarchiesSprite_V3F_C4B_T2F, texCoords));
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_pBuffersVBO[1]);
        
#   if HIERARCHIES_USE_TRIANGLE_STRIP
        glDrawElements(GL_TRIANGLE_STRIP, (GLsizei)n*6, GL_UNSIGNED_SHORT, (GLvoid*) (start*6*sizeof(m_pIndices[0])));
#   else
        glDrawElements(GL_TRIANGLES, (GLsizei)n*6, GL_UNSIGNED_SHORT, (GLvoid*) (start*6*sizeof(m_pIndices[0])));
#   endif // HIERARCHIES_USE_TRIANGLE_STRIP
        
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        
        glDisableVertexAttribArray(kCCHierarchiesSprite_VertexAttrib_Color_Mul);
        glDisableVertexAttribArray(kCCHierarchiesSprite_VertexAttrib_Color_Add);
    }
#else
    //
    // Using VBO without VAO
    //
    
#   define kQuadSize sizeof(m_pQuads[0].bl)
    glBindBuffer(GL_ARRAY_BUFFER, m_pBuffersVBO[0]);
    
    ccGLEnableVertexAttribs(kCCVertexAttribFlag_Position | kCCVertexAttribFlag_TexCoords);
    glEnableVertexAttribArray(kCCHierarchiesSprite_VertexAttrib_Color_Mul);
    glEnableVertexAttribArray(kCCHierarchiesSprite_VertexAttrib_Color_Add);
    
    // vertices
    glVertexAttribPointer(kCCVertexAttrib_Position, 3, GL_FLOAT, GL_FALSE, kQuadSize, (GLvoid*) offsetof(CCHierarchiesSprite_V3F_C4B_T2F, vertices));
    
    // colors
    glVertexAttribPointer(kCCHierarchiesSprite_VertexAttrib_Color_Mul, 4, GL_UNSIGNED_BYTE, GL_TRUE, kQuadSize, (GLvoid*) offsetof(CCHierarchiesSprite_V3F_C4B_T2F, colorsMul));
    glVertexAttribPointer(kCCHierarchiesSprite_VertexAttrib_Color_Add, 4, GL_UNSIGNED_BYTE, GL_TRUE, kQuadSize, (GLvoid*) offsetof(CCHierarchiesSprite_V3F_C4B_T2F, colorsAdd));
    
    // tex coords
    glVertexAttribPointer(kCCVertexAttrib_TexCoords, 2, GL_FLOAT, GL_FALSE, kQuadSize, (GLvoid*) offsetof(CCHierarchiesSprite_V3F_C4B_T2F, texCoords));
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_pBuffersVBO[1]);
    
#   if HIERARCHIES_USE_TRIANGLE_STRIP
    glDrawElements(GL_TRIANGLE_STRIP, (GLsizei)n*6, GL_UNSIGNED_SHORT, (GLvoid*) (start*6*sizeof(m_pIndices[0])));
#   else
    glDrawElements(GL_TRIANGLES, (GLsizei)n*6, GL_UNSIGNED_SHORT, (GLvoid*) (start*6*sizeof(m_pIndices[0])));
#   endif // HIERARCHIES_USE_TRIANGLE_STRIP
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    glDisableVertexAttribArray(kCCHierarchiesSprite_VertexAttrib_Color_Mul);
    glDisableVertexAttribArray(kCCHierarchiesSprite_VertexAttrib_Color_Add);
#endif
    
    CC_INCREMENT_GL_DRAWS(1);
    CHECK_GL_ERROR_DEBUG();
}

void CCHierarchiesSpriteMesh::flushAllQuadsToBuffer () {
    if (m_bDirty)
    {
        setupIndices();
        mapBuffers();
        
        m_bDirty = false;
    }
}

NS_CC_EXT_END
