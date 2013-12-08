//
//  CCHierarchiesSpriteDynamic.cpp
//  CCHierarchiesSprite
//
//  Created by bman <zx123xz321hm3@hotmail.com>.
//  Copyright (c) 2013. All rights reserved.
//

#include "CCHierarchiesSpriteDynamic.h"
#include "CCHierarchiesSpriteSheetCache.h"
#include "CCHierarchiesSpriteAnimationCache.h"


NS_CC_EXT_BEGIN

CCHierarchiesSpriteDynamic* CCHierarchiesSpriteDynamic::create (const char* sheetFileName,
                                    const char* animationFileName,
                                    CCHierarchiesSpriteEventDelegate* delegate) {
    std::string animationFileNameBase;
    std::string animationFileNameSub;
    std::string animationFilePath(animationFileName);
    size_t found = animationFilePath.find_last_of(HIERARCHIES_SPRITE_PATH_SEPARATOR);
    if (found != std::string::npos) {
        animationFileNameBase = animationFilePath.substr(0, found + 1);
        animationFileNameSub = animationFilePath.substr(found + 1);
    }
    else {
        animationFileNameSub = animationFilePath;
    }
    
	return CCHierarchiesSpriteDynamic::create(sheetFileName, animationFileNameBase.c_str(), animationFileNameSub.c_str(), delegate);
}

CCHierarchiesSpriteDynamic* CCHierarchiesSpriteDynamic::create (const char* sheetFileName,
                                    const char* animationFileNameBase,
                                    const char* animationFileNameSub,
                                    CCHierarchiesSpriteEventDelegate* delegate) {
    CCHierarchiesSpriteDynamic* ret = new CCHierarchiesSpriteDynamic();
	if (ret->initWithFile(sheetFileName, animationFileNameBase, animationFileNameSub, delegate))
	{
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);
	return NULL;
}

CCHierarchiesSpriteDynamic* CCHierarchiesSpriteDynamic::create (const char* sheetFileName,
                                           const char* animationFileName,
                                           CCHierarchiesSpriteEventDelegate* delegate,
                                           const AvatarMapType& avatarMap) {
    std::string animationFileNameBase;
    std::string animationFileNameSub;
    std::string animationFilePath(animationFileName);
    size_t found = animationFilePath.find_last_of(HIERARCHIES_SPRITE_PATH_SEPARATOR);
    if (found != std::string::npos) {
        animationFileNameBase = animationFilePath.substr(0, found + 1);
        animationFileNameSub = animationFilePath.substr(found + 1);
    }
    else {
        animationFileNameSub = animationFilePath;
    }
    
    return CCHierarchiesSpriteDynamic::create(sheetFileName,
                                       animationFileNameBase.c_str(),
                                       animationFileNameSub.c_str(),
                                       delegate,
                                       avatarMap);
}

CCHierarchiesSpriteDynamic* CCHierarchiesSpriteDynamic::create (const char* sheetFileName,
                                           const char* animationFileNameBase,
                                           const char* animationFileNameSub,
                                           CCHierarchiesSpriteEventDelegate* delegate,
                                           const AvatarMapType& avatarMap) {
    CCHierarchiesSpriteDynamic* ret = new CCHierarchiesSpriteDynamic();
	if (ret->initWithFileAndAvatar(sheetFileName,
                                   animationFileNameBase,
                                   animationFileNameSub,
                                   delegate,
                                   avatarMap))
	{
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);
	return NULL;
}

CCHierarchiesSpriteDynamic::CCHierarchiesSpriteDynamic ()
: _quads(20), _indices(120) {
}

CCHierarchiesSpriteDynamic::~CCHierarchiesSpriteDynamic () {
}

bool CCHierarchiesSpriteDynamic::initWithFileAndAvatar (const char* sheetFileName,
                                                        const char* animationFileNameBase,
                                                        const char* animationFileNameSub,
                                                        CCHierarchiesSpriteEventDelegate* delegate,
                                                        const AvatarMapType& avatarMap) {
    if (CCHierarchiesSpriteBase::initWithFileAndAvatar(sheetFileName,
                                                   animationFileNameBase,
                                                   animationFileNameSub,
                                                   delegate,
                                                   avatarMap)) {
        _avatarMap = avatarMap;
        
        this->displayFrameAtIndex(0);
        
        return true;
    }
    return false;
}


#pragma mark - Avatar

bool CCHierarchiesSpriteDynamic::setAvatarMap (const char* name, const char* mapName) {
	_avatarMap[name] = mapName;
    this->freshCurrentFrame();
    return true;
}

bool CCHierarchiesSpriteDynamic::getAvatarMap (const char* name, std::string& mapName) {
    AvatarMapType::iterator symbolIter = _avatarMap.find(name);
    if (symbolIter == _avatarMap.end()) {
        return false;
    }
    
    mapName = _avatarMap[name];
    return true;
}

void CCHierarchiesSpriteDynamic::resetAvatarMap () {
    _avatarMap.clear();
    this->freshCurrentFrame();
}


#pragma mark - Animation

bool CCHierarchiesSpriteDynamic::displayFrameAtIndex (unsigned int frameIndex) {
    CCHierarchiesSpriteBase::displayFrameAtIndex(frameIndex);
    
    // calc quads
    CC_PROFILER_START_CATEGORY(HIERARCHIES_SPRITE_PROFILER, "CCHierarchiesSpriteDynamic - animation");
    
    _quads.clear();
    float min_X = 10000, max_X = -10000, min_Y = 10000, max_Y = -10000;
    this->buildDynamicAnimationData(CCHierarchiesSpriteAnimation::kNoneLoopMode,
                                    0,
                                    _curFrameIndex,
                                    CCAffineTransformIdentity,
                                    _animation,
                                    min_X, max_X, min_Y, max_Y,
                                    1, 0,
                                    1, 0,
                                    1, 0,
                                    1, 0);
    
    if (min_X == 10000 ||
        max_X == -10000 ||
        min_Y == 10000 ||
        max_Y == -10000) {
        // this frame is whole empty
        _bbox = CCRectZero;
        
        this->setAnchorPoint(CCPointZero);
        this->setContentSize(CCSizeZero);
    }
    else {
        _bbox = CCRectMake(min_X, min_Y, max_X - min_X, max_Y - min_Y);
        
        CCPoint anchorPoint = ccp((0 - _bbox.origin.x) / _bbox.size.width,
                                  (0 - _bbox.origin.y) / _bbox.size.height);
        this->setAnchorPoint(anchorPoint);
        this->setContentSize(_bbox.size);
    }
    
#if HIERARCHIES_ENABLE_FILTER == 1
        this->updateSocketFilters();
#endif
    
    CC_PROFILER_STOP_CATEGORY(HIERARCHIES_SPRITE_PROFILER, "CCHierarchiesSpriteDynamic - animation");
    
    return true;
}

void CCHierarchiesSpriteDynamic::buildDynamicAnimationData (CCHierarchiesSpriteAnimation::ElementLoopMode loopMode,
                                                            int frameOffset,
                                                            unsigned int frameIndex,
                                                            const CCAffineTransform& parentMatrix,
                                                            const CCHierarchiesSpriteAnimation* animation,
                                                            float& min_X, float& max_X, float& min_Y, float& max_Y,
                                                            const float parent_alpha_percent, const int parent_alpha_amount,
                                                            const float parent_red_percent, const int parent_red_amount,
                                                            const float parent_green_percent, const int parent_green_amount,
                                                            const float parent_blue_percent, const int parent_blue_amount) {
    CCHierarchiesSpriteAnimation::FrameElements frameElements;
    int eNum = animation->getFrameElementsAtFrameIndex(loopMode, frameOffset, frameIndex, frameElements); //!!! frameIndex will update with loop mode
    
    _quads.reserve(_quads.size() + eNum);
    
    CCAffineTransform matrix;
    CCHierarchiesSprite_V3F_C4B_T2F_Quad quad;
    CCHierarchiesSpriteAnimation::FrameElements::const_iterator layerIter;
    int layerZOrder = 0;
//        unsigned int quadsIndex = 0;
//        unsigned int quadsCount = 0;
    //!!! reverse order to display
    for (layerIter = frameElements.cbegin(); layerIter != frameElements.cend(); layerIter++) { // element layers BEGIN
        const CCHierarchiesSpriteAnimation::Element* elementIter = &(*layerIter);
        
        CCHierarchiesSpriteAnimation::Symbol symbol;
        bool result = animation->getSymbolByIndex(elementIter->symbolIndex, symbol);
        assert(result);
        
        if (symbol.isSocket == false) { // simple element BEGIN
            CCHierarchiesSpriteSheet::Spr spr;
            result = _sheet->getSpr(symbol.name.c_str(), spr);
            assert(result);
            
            // calc matrix
            hierarchiesCalcMatrix(&symbol, &spr, &(*elementIter), &matrix);
            matrix = CCAffineTransformConcat(matrix, parentMatrix);
            
            // update vertices
            hierarchiesUpdateQuadVertices(CCSizeMake(spr.w, spr.h),
                                          &matrix,
                                          &quad,
                                          layerZOrder * HIERARCHIES_SPRITE_LAYER_Z_ORDER_SCALE,
                                          spr.isRotation);
            
            // update color from animation
            float alpha_percent = parent_alpha_percent * elementIter->color_alpha_percent;
            int alpha_amount = parent_alpha_percent * elementIter->color_alpha_amount + parent_alpha_amount;
            float red_percent = parent_red_percent * elementIter->color_red_percent;
            int red_amount = parent_red_percent * elementIter->color_red_amount + parent_red_amount;
            float green_percent = parent_green_percent * elementIter->color_green_percent;
            int green_amount = parent_green_percent * elementIter->color_green_amount + parent_green_amount;
            float blue_percent = parent_blue_percent * elementIter->color_blue_percent;
            int blue_amount = parent_blue_percent * elementIter->color_blue_amount + parent_blue_amount;
            hierarchiesUpdateQuadTextureColorFromAnimation (alpha_percent, alpha_amount,
                                                            red_percent, red_amount,
                                                            green_percent, green_amount,
                                                            blue_percent, blue_amount,
                                                            &quad);
            
            hierarchiesExpandRectByPoint(&min_X, &max_X, &min_Y, &max_Y, &quad.bl.vertices.x, &quad.bl.vertices.y);
            hierarchiesExpandRectByPoint(&min_X, &max_X, &min_Y, &max_Y, &quad.br.vertices.x, &quad.br.vertices.y);
            hierarchiesExpandRectByPoint(&min_X, &max_X, &min_Y, &max_Y, &quad.tl.vertices.x, &quad.tl.vertices.y);
            hierarchiesExpandRectByPoint(&min_X, &max_X, &min_Y, &max_Y, &quad.tr.vertices.x, &quad.tr.vertices.y);
            
            hierarchiesUpdateQuadTextureCoords(CCRectMake(spr.x, spr.y, spr.w, spr.h),
                                               _sheet->getImageWidth(),
                                               _sheet->getImageHeight(),
                                               &quad,
                                               spr.isRotation);
            
            _quads.push_back(quad);
            
//                quadsCount++;
        } // simple element END
        else { // nesting sprite element BEGIN
            std::string subSpriteAnimationName(_animationNameBase);
            if (_avatarMap.count(symbol.name) > 0) { // with avatar support
                subSpriteAnimationName += _avatarMap.at(symbol.name) + ".hanims";
            }
            else {
                subSpriteAnimationName += symbol.name + ".hanims";
            }
            CCHierarchiesSpriteAnimation* subAnimation = CCHierarchiesSpriteAnimationCache::sharedHierarchiesSpriteAnimationCache()->addAnimation(subSpriteAnimationName.c_str());
            
            hierarchiesCalcMatrixSocket(&(*elementIter), &matrix);
            matrix = CCAffineTransformConcat(matrix, parentMatrix);
            
            float alpha_percent = parent_alpha_percent * elementIter->color_alpha_percent;
            int alpha_amount = parent_alpha_percent * elementIter->color_alpha_amount + parent_alpha_amount;
            float red_percent = parent_red_percent * elementIter->color_red_percent;
            int red_amount = parent_red_percent * elementIter->color_red_amount + parent_red_amount;
            float green_percent = parent_green_percent * elementIter->color_green_percent;
            int green_amount = parent_green_percent * elementIter->color_green_amount + parent_green_amount;
            float blue_percent = parent_blue_percent * elementIter->color_blue_percent;
            int blue_amount = parent_blue_percent * elementIter->color_blue_amount + parent_blue_amount;
            this->buildDynamicAnimationData(elementIter->loopMode,
                                           elementIter->frameOffset,
                                           frameIndex - elementIter->startDisplayFrameIndex,
                                           matrix,
                                           subAnimation,
                                           min_X, max_X, min_Y, max_Y,
                                           alpha_percent, alpha_amount,
                                           red_percent, red_amount,
                                           green_percent, green_amount,
                                           blue_percent, blue_amount);
        } // nesting sprite element END
        
        layerZOrder++;
    } // element layers END
}


#pragma mark - Draw

void CCHierarchiesSpriteDynamic::draw () {
    if (0 == _quads.size())
    {
        return;
    }
    
    if (_needFresh) {
        _indices.clear();
        _indices.resize(_quads.size() * 6);
        
        for(size_t i = 0; i < _quads.size(); i++)
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
            _indices[i*6+3] = i*4+3;
            _indices[i*6+4] = i*4+2;
            _indices[i*6+5] = i*4+1;
#endif
        }
        
        _needFresh = false;
    }
    
    // ignore the anchor point while drawing
    CCPoint ap = this->getAnchorPointInPoints();
    kmGLTranslatef(ap.x, ap.y, 0);
    
    // draw with Vertex Array
    CC_PROFILER_START_CATEGORY(HIERARCHIES_SPRITE_PROFILER, "CCHierarchiesSpriteDynamic - draw");
    
    CC_NODE_DRAW_SETUP();
    
    GLfloat colors[4];
    colors[0] = _displayedColor.r / 255.0f;
    colors[1] = _displayedColor.g / 255.0f;
    colors[2] = _displayedColor.b / 255.0f;
    colors[3] = _displayedOpacity / 255.0f;
    this->getShaderProgram()->setUniformLocationWith4fv(_uniformColorLocation, colors, 1);

    ccGLBlendFunc( _blendFunc.src, _blendFunc.dst );

    ccGLBindTexture2D(_texture->getName());
    
    ccGLEnableVertexAttribs(kCCVertexAttribFlag_Position | kCCVertexAttribFlag_TexCoords);
    glEnableVertexAttribArray(kCCHierarchiesSprite_VertexAttrib_Color_Mul);
    glEnableVertexAttribArray(kCCHierarchiesSprite_VertexAttrib_Color_Add);
    
#   define kQuadSize sizeof(_quads[0].tl)
    // vertices
    glVertexAttribPointer(kCCVertexAttrib_Position, 3, GL_FLOAT, GL_FALSE, kQuadSize,
                          (GLvoid*) &_quads[0].tl.vertices);
    
    // colors
    glVertexAttribPointer(kCCHierarchiesSprite_VertexAttrib_Color_Mul, 4, GL_UNSIGNED_BYTE, GL_TRUE, kQuadSize,
                          (GLvoid*) &_quads[0].tl.colorsMul);
    glVertexAttribPointer(kCCHierarchiesSprite_VertexAttrib_Color_Add, 4, GL_UNSIGNED_BYTE, GL_TRUE, kQuadSize,
                          (GLvoid*) &_quads[0].tl.colorsAdd);
    
    // tex coords
    glVertexAttribPointer(kCCVertexAttrib_TexCoords, 2, GL_FLOAT, GL_FALSE, kQuadSize,
                          (GLvoid*) &_quads[0].tl.texCoords);
    
    
#if HIERARCHIES_USE_TRIANGLE_STRIP
    glDrawElements(GL_TRIANGLE_STRIP, (GLsizei)_quads.size()*6, GL_UNSIGNED_SHORT, (GLvoid*) &_indices[0]);
#else
    glDrawElements(GL_TRIANGLES, (GLsizei)_quads.size()*6, GL_UNSIGNED_SHORT, (GLvoid*) &_indices[0]);
#endif // HIERARCHIES_USE_TRIANGLE_STRIP
    
    glDisableVertexAttribArray(kCCHierarchiesSprite_VertexAttrib_Color_Mul);
    glDisableVertexAttribArray(kCCHierarchiesSprite_VertexAttrib_Color_Add);
    
    CC_INCREMENT_GL_DRAWS(1);
    CHECK_GL_ERROR_DEBUG();
    
#if HIERARCHIES_DEBUG_DRAW == 1
            // draw bounding box
            ccDrawColor4B(255, 0, 255, 255);
            CCPoint vertices[4] = {
                ccp(_bbox.origin.x, _bbox.origin.y),
                ccp(_bbox.origin.x + _bbox.size.width, _bbox.origin.y),
                ccp(_bbox.origin.x + _bbox.size.width, _bbox.origin.y + _bbox.size.height),
                ccp(_bbox.origin.x, _bbox.origin.y + _bbox.size.height),
            };
            ccDrawPoly(vertices, 4, true);
    
//#elif HIERARCHIES_DEBUG_DRAW == 2
//            // draw nesting bounding box
//            ccDrawColor4B(0, 255, 255, 255);
//            CCRect bbox = this->nestingBoundingBoxInner();
//            CCPoint vertices2[4] = {
//                ccp(bbox.origin.x, bbox.origin.y),
//                ccp(bbox.origin.x + bbox.size.width, bbox.origin.y),
//                ccp(bbox.origin.x + bbox.size.width, bbox.origin.y + bbox.size.height),
//                ccp(bbox.origin.x, bbox.origin.y + bbox.size.height),
//            };
//            ccDrawPoly(vertices2, 4, true);
            
#elif HIERARCHIES_DEBUG_DRAW == 3
            // draw quads
            ccDrawColor4B(0, 255, 0, 255);
            for (int i = 0; i < _quads.size(); i++) {
                CCPoint vertices3[4] = {
                    ccp(_quads[i].tl.vertices.x, _quads[i].tl.vertices.y),
                    ccp(_quads[i].tr.vertices.x, _quads[i].tr.vertices.y),
                    ccp(_quads[i].br.vertices.x, _quads[i].br.vertices.y),
                    ccp(_quads[i].bl.vertices.x, _quads[i].bl.vertices.y),
                };
                ccDrawPoly(vertices3, 4, true);
            }
            
            ccDrawColor4B(255, 255, 255, 255);
#endif
    
    CC_PROFILER_STOP_CATEGORY(HIERARCHIES_SPRITE_PROFILER, "CCHierarchiesSpriteDynamic - draw");
}

NS_CC_EXT_END
