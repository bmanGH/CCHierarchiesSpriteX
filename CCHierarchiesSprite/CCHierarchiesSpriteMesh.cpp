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
:_indices(NULL)
,_isDirty(false)
,_quads(NULL)
{}

CCHierarchiesSpriteMesh::~CCHierarchiesSpriteMesh()
{
    CCLOGINFO("cocos2d: CCHierarchiesSpriteMesh deallocing %p.", this);
    
    CC_SAFE_FREE(_quads);
    CC_SAFE_FREE(_indices);
    
    glDeleteBuffers(2, _buffersVBO);
    
#if CC_TEXTURE_ATLAS_USE_VAO
    if (CCConfiguration::sharedConfiguration()->supportsShareableVAO()) {
        glDeleteVertexArrays(1, &_vaoName);
    }
#endif
    
#if CC_ENABLE_CACHE_TEXTURE_DATA
    CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, EVENT_COME_TO_FOREGROUND);
#endif
}

unsigned int CCHierarchiesSpriteMesh::getTotalQuads()
{
    return _totalQuads;
}

unsigned int CCHierarchiesSpriteMesh::getCapacity()
{
    return _capacity;
}

CCHierarchiesSprite_V3F_C4B_T2F_Quad* CCHierarchiesSpriteMesh::getQuads()
{
    //if someone accesses the quads directly, presume that changes will be made
    _isDirty = true;
    return _quads;
}

void CCHierarchiesSpriteMesh::setQuads(CCHierarchiesSprite_V3F_C4B_T2F_Quad *var)
{
    _quads = var;
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
    _capacity = capacity;
    _totalQuads = 0;
    
    // Re-initialization is not allowed
    CCAssert(_quads == NULL && _indices == NULL, "");
    
    _quads = (CCHierarchiesSprite_V3F_C4B_T2F_Quad*)malloc( _capacity * sizeof(CCHierarchiesSprite_V3F_C4B_T2F_Quad) );
    _indices = (GLushort *)malloc( _capacity * 6 * sizeof(GLushort) );
    
    if( ! ( _quads && _indices) && _capacity > 0)
    {
        //CCLOG("cocos2d: CCHierarchiesSpriteMesh: not enough memory");
        CC_SAFE_FREE(_quads);
        CC_SAFE_FREE(_indices);
        return false;
    }
    
    memset( _quads, 0, _capacity * sizeof(CCHierarchiesSprite_V3F_C4B_T2F_Quad) );
    memset( _indices, 0, _capacity * 6 * sizeof(GLushort) );
    
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
    
    _isDirty = true;
    
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
    
    // set _isDirty to true to force it rebinding buffer
    _isDirty = true;
    flushAllQuadsToBuffer();
}

const char* CCHierarchiesSpriteMesh::description()
{
    return CCString::createWithFormat("<CCHierarchiesSpriteMesh | totalQuads = %u>", _totalQuads)->getCString();
}


void CCHierarchiesSpriteMesh::setupIndices()
{
    if (_capacity == 0)
        return;
    
    for( unsigned int i=0; i < _capacity; i++)
    {
#if HIERARCHIES_USE_TRIANGLE_STRIP
        _indices[i*6+0] = i*4+0;
        _indices[i*6+1] = i*4+0;
        _indices[i*6+2] = i*4+2;
        _indices[i*6+3] = i*4+1;
        _indices[i*6+4] = i*4+3;
        _indices[i*6+5] = i*4+3;
#else
        _indices[i*6+0] = i*4+0;
        _indices[i*6+1] = i*4+1;
        _indices[i*6+2] = i*4+2;
        
        // inverted index. issue #179
        _indices[i*6+3] = i*4+3;
        _indices[i*6+4] = i*4+2;
        _indices[i*6+5] = i*4+1;
#endif
    }
}

//TextureAtlas - VAO / VBO specific
#if CC_TEXTURE_ATLAS_USE_VAO
void CCHierarchiesSpriteMesh::setupVBOandVAO()
{
    glGenVertexArrays(1, &_vaoName);
    ccGLBindVAO(_vaoName);
    
#define kQuadSize sizeof(_quads[0].bl)
    
    glGenBuffers(2, &_buffersVBO[0]);
    
    glBindBuffer(GL_ARRAY_BUFFER, _buffersVBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(_quads[0]) * _capacity, _quads, GL_DYNAMIC_DRAW);
    
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
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _buffersVBO[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(_indices[0]) * _capacity * 6, _indices, GL_STATIC_DRAW);
    
    // Must unbind the VAO before changing the element buffer.
    ccGLBindVAO(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    CHECK_GL_ERROR_DEBUG();
}
#endif

void CCHierarchiesSpriteMesh::setupVBO()
{
    glGenBuffers(2, &_buffersVBO[0]);
}

void CCHierarchiesSpriteMesh::mapBuffers()
{
    // Avoid changing the element buffer for whatever VAO might be bound.
	ccGLBindVAO(0);
    
    glBindBuffer(GL_ARRAY_BUFFER, _buffersVBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(_quads[0]) * _capacity, _quads, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _buffersVBO[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(_indices[0]) * _capacity * 6, _indices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    CHECK_GL_ERROR_DEBUG();
}

// TextureAtlas - Update, Insert, Move & Remove

void CCHierarchiesSpriteMesh::updateQuad(CCHierarchiesSprite_V3F_C4B_T2F_Quad *quad, unsigned int index)
{
    CCAssert( index >= 0 && index < _capacity, "updateQuadWithTexture: Invalid index");
    
    _totalQuads = MAX( index+1, _totalQuads);
    
    _quads[index] = *quad;
    
    _isDirty = true;
}

void CCHierarchiesSpriteMesh::insertQuad(CCHierarchiesSprite_V3F_C4B_T2F_Quad *quad, unsigned int index)
{
    CCAssert( index < _capacity, "insertQuadWithTexture: Invalid index");
    
    _totalQuads++;
    CCAssert( _totalQuads <= _capacity, "invalid totalQuads");
    
    // issue #575. index can be > totalQuads
    unsigned int remaining = (_totalQuads-1) - index;
    
    // last object doesn't need to be moved
    if( remaining > 0)
    {
        // texture coordinates
        memmove( &_quads[index+1],&_quads[index], sizeof(_quads[0]) * remaining );
    }
    
    _quads[index] = *quad;
    
    _isDirty = true;
}

void CCHierarchiesSpriteMesh::insertQuads(CCHierarchiesSprite_V3F_C4B_T2F_Quad* quads, unsigned int index, unsigned int amount)
{
    CCAssert(index + amount <= _capacity, "insertQuadWithTexture: Invalid index + amount");
    
    _totalQuads += amount;
    
    CCAssert( _totalQuads <= _capacity, "invalid totalQuads");
    
    // issue #575. index can be > totalQuads
    int remaining = (_totalQuads-1) - index - amount;
    
    // last object doesn't need to be moved
    if( remaining > 0)
    {
        // tex coordinates
        memmove( &_quads[index+amount],&_quads[index], sizeof(_quads[0]) * remaining );
    }
    
    unsigned int max = index + amount;
    unsigned int j = 0;
    for (unsigned int i = index; i < max ; i++)
    {
        _quads[index] = quads[j];
        index++;
        j++;
    }
    
    _isDirty = true;
}

void CCHierarchiesSpriteMesh::insertQuadFromIndex(unsigned int oldIndex, unsigned int newIndex)
{
    CCAssert( newIndex >= 0 && newIndex < _totalQuads, "insertQuadFromIndex:atIndex: Invalid index");
    CCAssert( oldIndex >= 0 && oldIndex < _totalQuads, "insertQuadFromIndex:atIndex: Invalid index");
    
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
    CCHierarchiesSprite_V3F_C4B_T2F_Quad quadsBackup = _quads[oldIndex];
    memmove( &_quads[dst],&_quads[src], sizeof(_quads[0]) * howMany );
    _quads[newIndex] = quadsBackup;
    
    _isDirty = true;
}

void CCHierarchiesSpriteMesh::removeQuadAtIndex(unsigned int index)
{
    CCAssert( index < _totalQuads, "removeQuadAtIndex: Invalid index");
    
    unsigned int remaining = (_totalQuads-1) - index;
    
    
    // last object doesn't need to be moved
    if( remaining )
    {
        // texture coordinates
        memmove( &_quads[index],&_quads[index+1], sizeof(_quads[0]) * remaining );
    }
    
    _totalQuads--;
    
    _isDirty = true;
}

void CCHierarchiesSpriteMesh::removeQuadsAtIndex(unsigned int index, unsigned int amount)
{
    CCAssert(index + amount <= _totalQuads, "removeQuadAtIndex: index + amount out of bounds");
    
    unsigned int remaining = (_totalQuads) - (index + amount);
    
    _totalQuads -= amount;
    
    if ( remaining )
    {
        memmove( &_quads[index], &_quads[index+amount], sizeof(_quads[0]) * remaining );
    }
    
    _isDirty = true;
}

void CCHierarchiesSpriteMesh::removeAllQuads()
{
    _totalQuads = 0;
}

bool CCHierarchiesSpriteMesh::resizeCapacity(unsigned int newCapacity)
{
    if( newCapacity == _capacity )
    {
        return true;
    }
    unsigned int uOldCapactiy = _capacity;
    // update capacity and totolQuads
    _totalQuads = MIN(_totalQuads, newCapacity);
    _capacity = newCapacity;
    
    CCHierarchiesSprite_V3F_C4B_T2F_Quad* tmpQuads = NULL;
    GLushort* tmpIndices = NULL;
    
    // when calling initWithTexture(fileName, 0) on bada device, calloc(0, 1) will fail and return NULL,
    // so here must judge whether _quads and _indices is NULL.
    if (_quads == NULL)
    {
        tmpQuads = (CCHierarchiesSprite_V3F_C4B_T2F_Quad*)malloc( _capacity * sizeof(_quads[0]) );
        if (tmpQuads != NULL)
        {
            memset(tmpQuads, 0, _capacity * sizeof(_quads[0]) );
        }
    }
    else
    {
        tmpQuads = (CCHierarchiesSprite_V3F_C4B_T2F_Quad*)realloc( _quads, sizeof(_quads[0]) * _capacity );
        if (tmpQuads != NULL && _capacity > uOldCapactiy)
        {
            memset(tmpQuads+uOldCapactiy, 0, (_capacity - uOldCapactiy)*sizeof(_quads[0]) );
        }
    }
    
    if (_indices == NULL)
    {
        tmpIndices = (GLushort*)malloc( _capacity * 6 * sizeof(_indices[0]) );
        if (tmpIndices != NULL)
        {
            memset( tmpIndices, 0, _capacity * 6 * sizeof(_indices[0]) );
        }
        
    }
    else
    {
        tmpIndices = (GLushort*)realloc( _indices, sizeof(_indices[0]) * _capacity * 6 );
        if (tmpIndices != NULL && _capacity > uOldCapactiy)
        {
            memset( tmpIndices+uOldCapactiy, 0, (_capacity-uOldCapactiy) * 6 * sizeof(_indices[0]) );
        }
    }
    
    if( ! ( tmpQuads && tmpIndices) ) {
        CCLOG("cocos2d: CCHierarchiesSpriteMesh: not enough memory");
        CC_SAFE_FREE(tmpQuads);
        CC_SAFE_FREE(tmpIndices);
        CC_SAFE_FREE(_quads);
        CC_SAFE_FREE(_indices);
        _capacity = _totalQuads = 0;
        return false;
    }
    
    _quads = tmpQuads;
    _indices = tmpIndices;
    
    _isDirty = true;
    
    return true;
}

void CCHierarchiesSpriteMesh::increaseTotalQuadsWith(unsigned int amount)
{
    _totalQuads += amount;
}

void CCHierarchiesSpriteMesh::moveQuadsFromIndex(unsigned int oldIndex, unsigned int amount, unsigned int newIndex)
{
    CCAssert(newIndex + amount <= _totalQuads, "insertQuadFromIndex:atIndex: Invalid index");
    CCAssert(oldIndex < _totalQuads, "insertQuadFromIndex:atIndex: Invalid index");
    
    if( oldIndex == newIndex )
    {
        return;
    }
    //create buffer
    size_t quadSize = sizeof(CCHierarchiesSprite_V3F_C4B_T2F_Quad);
    CCHierarchiesSprite_V3F_C4B_T2F_Quad* tempQuads = (CCHierarchiesSprite_V3F_C4B_T2F_Quad*)malloc( quadSize * amount);
    memcpy( tempQuads, &_quads[oldIndex], quadSize * amount );
    
    if (newIndex < oldIndex)
    {
        // move quads from newIndex to newIndex + amount to make room for buffer
        memmove( &_quads[newIndex], &_quads[newIndex+amount], (oldIndex-newIndex)*quadSize);
    }
    else
    {
        // move quads above back
        memmove( &_quads[oldIndex], &_quads[oldIndex+amount], (newIndex-oldIndex)*quadSize);
    }
    memcpy( &_quads[newIndex], tempQuads, amount*quadSize);
    
    free(tempQuads);
    
    _isDirty = true;
}

void CCHierarchiesSpriteMesh::moveQuadsFromIndex(unsigned int index, unsigned int newIndex)
{
    CCAssert(newIndex + (_totalQuads - index) <= _capacity, "moveQuadsFromIndex move is out of bounds");
    
    memmove(_quads + newIndex,_quads + index, (_totalQuads - index) * sizeof(_quads[0]));
}

void CCHierarchiesSpriteMesh::fillWithEmptyQuadsFromIndex(unsigned int index, unsigned int amount)
{
    CCHierarchiesSprite_V3F_C4B_T2F_Quad quad;
    memset(&quad, 0, sizeof(quad));
    
    unsigned int to = index + amount;
    for (unsigned int i = index ; i < to ; i++)
    {
        _quads[i] = quad;
    }
}


#pragma mark - Draw

void CCHierarchiesSpriteMesh::drawQuads()
{
    this->drawNumberOfQuads(_totalQuads, 0);
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
        ccGLBindVAO(_vaoName);
        
#   if CC_REBIND_INDICES_BUFFER
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _buffersVBO[1]);
#   endif
        
#   if HIERARCHIES_USE_TRIANGLE_STRIP
        glDrawElements(GL_TRIANGLE_STRIP, (GLsizei) n*6, GL_UNSIGNED_SHORT, (GLvoid*) (start*6*sizeof(_indices[0])) );
#   else
        glDrawElements(GL_TRIANGLES, (GLsizei) n*6, GL_UNSIGNED_SHORT, (GLvoid*) (start*6*sizeof(_indices[0])) );
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
        
#   define kQuadSize sizeof(_quads[0].bl)
        glBindBuffer(GL_ARRAY_BUFFER, _buffersVBO[0]);
        
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
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _buffersVBO[1]);
        
#   if HIERARCHIES_USE_TRIANGLE_STRIP
        glDrawElements(GL_TRIANGLE_STRIP, (GLsizei)n*6, GL_UNSIGNED_SHORT, (GLvoid*) (start*6*sizeof(_indices[0])));
#   else
        glDrawElements(GL_TRIANGLES, (GLsizei)n*6, GL_UNSIGNED_SHORT, (GLvoid*) (start*6*sizeof(_indices[0])));
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
    
#   define kQuadSize sizeof(_quads[0].bl)
    glBindBuffer(GL_ARRAY_BUFFER, _buffersVBO[0]);
    
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
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _buffersVBO[1]);
    
#   if HIERARCHIES_USE_TRIANGLE_STRIP
    glDrawElements(GL_TRIANGLE_STRIP, (GLsizei)n*6, GL_UNSIGNED_SHORT, (GLvoid*) (start*6*sizeof(_indices[0])));
#   else
    glDrawElements(GL_TRIANGLES, (GLsizei)n*6, GL_UNSIGNED_SHORT, (GLvoid*) (start*6*sizeof(_indices[0])));
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
    if (_isDirty)
    {
        setupIndices();
        mapBuffers();
        
        _isDirty = false;
    }
}

NS_CC_EXT_END
