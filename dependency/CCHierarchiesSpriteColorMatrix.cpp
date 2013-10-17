//
//  CCHierarchiesSpriteColorMatrix.cpp
//  CCHierarchiesSprite
//
//  Created by bman <zx123xz321hm3@hotmail.com>.
//  Copyright (c) 2013. All rights reserved.
//

#include "CCHierarchiesSpriteColorMatrix.h"
#include "shaders/CCShaderCache.h"
#include "shaders/ccGLStateCache.h"
#include "shaders/CCGLProgram.h"
#include "CCColorMatrixShader.h"

#define HIERARCHIES_SPRITE_COLORMATRIX_PROFILER                         1
#define HIERARCHIES_COLORMATRIX_DEBUG_DRAW                              0


CCHierarchiesSpriteColorMatrix* CCHierarchiesSpriteColorMatrix::create (const char* sheetFileName, const char* animationFileName, CCHierarchiesSpriteEventDelegate* delegate) {
    CCHierarchiesSpriteColorMatrix* ret = new CCHierarchiesSpriteColorMatrix();
	if (ret->initWithFile(sheetFileName, animationFileName, delegate))
	{
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);
	return NULL;
}

CCHierarchiesSpriteColorMatrix::CCHierarchiesSpriteColorMatrix ()
: _colorMatrixUniformLocation(-1), _needResetColorMatrix(false) {
    _colorMatrix = CCColorMatrixMakeIdentity();
}

CCHierarchiesSpriteColorMatrix::~CCHierarchiesSpriteColorMatrix () {
}


#pragma mark - Private

void CCHierarchiesSpriteColorMatrix::setup () {
    // image should load from the same directory with spritesheet file
    std::string imageName(_sheet->getImageName());
    size_t found = _sheetName.find_last_of(HIERARCHIES_SPRITE_PATH_SEPARATOR);
    if (found != std::string::npos)
        imageName = _sheetName.substr(0, found) + HIERARCHIES_SPRITE_PATH_SEPARATOR + imageName;
    
    _texture = CCTextureCache::sharedTextureCache()->addImage(imageName.c_str());
    CC_SAFE_RETAIN(_texture);
    
    _mesh = new CCHierarchiesSpriteMesh();
    _mesh->initWithCapacity(40);
    
    this->_updateShader();
	
	this->updateBlendFunc();
	this->updateOpacityModifyRGB();
}

void CCHierarchiesSpriteColorMatrix::_updateShader () {
    // use position texture color color_matrix shader program premultiplied alpha
    // or
    // use position texture color color_matrix shader program
    if (_texture->hasPremultipliedAlpha()) {
        CCGLProgram* glProgram = CCShaderCache::sharedShaderCache()->programForKey(kCCShader_PositionTextureColorColorMatrixPremultipliedAlpha);
        if (glProgram == NULL) {
            glProgram = new CCGLProgram();
            glProgram->initWithVertexShaderByteArray(ccPositionTextureColor_vert, ccShader_PositionTextureColorColorMatrixPremultipliedAlpha_frag);
            
            glProgram->addAttribute(kCCAttributeNamePosition, kCCVertexAttrib_Position);
            glProgram->addAttribute(kCCAttributeNameColor, kCCVertexAttrib_Color);
            glProgram->addAttribute(kCCAttributeNameTexCoord, kCCVertexAttrib_TexCoords);
            
            glProgram->link();
            glProgram->updateUniforms();
            
            CHECK_GL_ERROR_DEBUG();
            
            CCShaderCache::sharedShaderCache()->addProgram(glProgram, kCCShader_PositionTextureColorColorMatrixPremultipliedAlpha);
        }
        else {
            glProgram->retain();
        }
        this->setShaderProgram(glProgram);
        
        _colorMatrixUniformLocation = glProgram->getUniformLocationForName("u_colorMatrix");
        
        glProgram->release();
    }
    else {
        CCGLProgram* glProgram = CCShaderCache::sharedShaderCache()->programForKey(kCCShader_PositionTextureColorColorMatrix);
        if (glProgram == NULL) {
            glProgram = new CCGLProgram();
            glProgram->initWithVertexShaderByteArray(ccPositionTextureColor_vert, ccShader_PositionTextureColorColorMatrix_frag);
            
            glProgram->addAttribute(kCCAttributeNamePosition, kCCVertexAttrib_Position);
            glProgram->addAttribute(kCCAttributeNameColor, kCCVertexAttrib_Color);
            glProgram->addAttribute(kCCAttributeNameTexCoord, kCCVertexAttrib_TexCoords);
            
            glProgram->link();
            glProgram->updateUniforms();
            
            CHECK_GL_ERROR_DEBUG();
            
            CCShaderCache::sharedShaderCache()->addProgram(glProgram, kCCShader_PositionTextureColorColorMatrix);
        }
        else {
            glProgram->retain();
        }
        this->setShaderProgram(glProgram);
        
        _colorMatrixUniformLocation = glProgram->getUniformLocationForName("u_colorMatrix");
        
        glProgram->release();
    }
}


#pragma mark - Draw

void CCHierarchiesSpriteColorMatrix::draw () {
	CC_PROFILER_START_CATEGORY(HIERARCHIES_SPRITE_COLORMATRIX_PROFILER, "CCHierarchiesSpriteColorMatrix - animation");
    
    if (_needFresh) {
        _needFresh = false;
    }
    
    CC_PROFILER_STOP_CATEGORY(HIERARCHIES_SPRITE_COLORMATRIX_PROFILER, "CCHierarchiesSpriteColorMatrix - animation");
    
    // draw
	CC_PROFILER_START_CATEGORY(HIERARCHIES_SPRITE_COLORMATRIX_PROFILER, "CCHierarchiesSpriteColorMatrix - draw");
	
    //	// update quads to textureAtlas
    //	int quadsToDraw = _quads.size();
    //	if ((quadsToDraw) > _texAtlas.capacity)
    //		[_texAtlas resizeCapacity:quadsToDraw];
    //	[_texAtlas removeAllQuads];
    //	if (quadsToDraw > 0)
    //		[_texAtlas insertQuads:&_quads[0] atIndex:0 amount:quadsToDraw];
	
	// ignore the anchor point while drawing
    CCPoint ap = this->getAnchorPointInPoints();
    kmGLTranslatef(ap.x, ap.y, 0);
	
	CC_NODE_DRAW_SETUP();
    // set color matrix
    m_pShaderProgram->setUniformLocationWithMatrix4fv(_colorMatrixUniformLocation,
                                                      _colorMatrix.m,
                                                      1);
	
	ccGLBlendFunc( _blendFunc.src, _blendFunc.dst );
    
    ccGLBindTexture2D(_texture->getName());
	
    _mesh->drawNumberOfQuads(_mesh->getTotalQuads(), 0);
	
	CHECK_GL_ERROR_DEBUG();
    
#if HIERARCHIES_COLORMATRIX_DEBUG_DRAW == 1
    // draw bounding box
    CCPoint vertices[4] = {
        ccp(_bbox.origin.x, _bbox.origin.y),
        ccp(_bbox.origin.x + _bbox.size.width, _bbox.origin.y),
        ccp(_bbox.origin.x + _bbox.size.width, _bbox.origin.y + _bbox.size.height),
        ccp(_bbox.origin.x, _bbox.origin.y + _bbox.size.height),
    };
    ccDrawPoly(vertices, 4, true);
#endif
	
	CC_PROFILER_STOP_CATEGORY(HIERARCHIES_SPRITE_COLORMATRIX_PROFILER, "CCHierarchiesSpriteColorMatrix - draw");
    
    if (_needResetColorMatrix)
        _colorMatrix = CCColorMatrixMakeIdentity();
}


#pragma mark - CCColorMatrixProtocol

void CCHierarchiesSpriteColorMatrix::setColorMatrix (const CCColorMatrix4x4& colorMatrix) {
    _colorMatrix = colorMatrix;
}

CCColorMatrix4x4 CCHierarchiesSpriteColorMatrix::getColorMatrix () {
    return _colorMatrix;
}

void CCHierarchiesSpriteColorMatrix::concatColorMatrix (const CCColorMatrix4x4& colorMatrix) {
    _colorMatrix = CCColorMatrixConcat(&colorMatrix, &_colorMatrix);
}

void CCHierarchiesSpriteColorMatrix::setNeedResetColorMatrixAfterDraw (bool value) {
    _needResetColorMatrix = value;
}

bool CCHierarchiesSpriteColorMatrix::isNeedResetColorMatrixAfterDraw () {
    return _needResetColorMatrix;
}
