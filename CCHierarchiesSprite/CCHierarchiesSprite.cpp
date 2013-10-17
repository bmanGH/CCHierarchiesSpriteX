//
//  CCHierarchiesSprite.m
//  CCHierarchiesSprite
//
//  Created by bman <zx123xz321hm3@hotmail.com>.
//  Copyright (c) 2013. All rights reserved.
//

#include "CCHierarchiesSprite.h"
#include "CCHierarchiesSpriteSheetCache.h"
#include "CCHierarchiesSpriteAnimationCache.h"
#include "CCHierarchiesSpriteRuntime.h"


USING_NS_CC;
USING_NS_CC_EXT;


NS_CC_EXT_BEGIN

#pragma mark - CCHierarchiesSprite

//inline static void hierarchiesCalcMatrixSocket (CCHierarchiesSpriteAnimation::Element* element, CCAffineTransform* matrix) {
//    FMatrix2D m0;
//    FMatrix2D m1;
//    
//    m0.setAnchorX(element->anchorX);
//    m0.setAnchorY(element->anchorY);
//    
//    m1.setScaleX(element->scaleX);
//    m1.setScaleY(element->scaleY);
//    m1.setSkewX(element->skewX);
//    m1.setSkewY(element->skewY);
//    m1.setTransformX(element->x);
//    m1.setTransformY(element->y);
//    
//    FMatrix2D m = m0.concat(m1);
//    
//    *matrix = CCAffineTransformMake(m.a, -m.b, -m.c, m.d, m.tx, -m.ty);
//}


CCHierarchiesSprite* CCHierarchiesSprite::create (const char* sheetFileName,
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
    
	return CCHierarchiesSprite::create(sheetFileName, animationFileNameBase.c_str(), animationFileNameSub.c_str(), delegate);
}


CCHierarchiesSprite* CCHierarchiesSprite::create (const char* sheetFileName,
                                                  const char* animationFileNameBase,
                                                  const char* animationFileNameSub,
                                                  CCHierarchiesSpriteEventDelegate* delegate) {
    CCHierarchiesSprite* ret = new CCHierarchiesSprite();
	if (ret->initWithFile(sheetFileName, animationFileNameBase, animationFileNameSub, delegate))
	{
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);
	return NULL;
}

CCHierarchiesSprite::CCHierarchiesSprite ()
: _sheet(NULL), _animation(NULL), _mesh(NULL), _texture(NULL), _delegate(NULL), _curFrameIndex(0),
_needFresh(false), _blendFunc(kCCBlendFuncDisable) , _opacityModifyRGB (false),
_flipX(false), _flipY(false), _bbox(CCRectZero), _enabledAvatar(false), _uniformColorLocation(0) {
}

CCHierarchiesSprite::~CCHierarchiesSprite () {
    CCHierarchiesSpriteRuntime::sharedHierarchiesSpriteRuntime()->removeHierarchiesSprite(this);
    CCHierarchiesSpriteSheetCache::sharedHierarchiesSpriteSheetCache()->removeSpriteSheet(_sheetName.c_str());
    CCHierarchiesSpriteAnimationCache::sharedHierarchiesSpriteAnimationCache()->removeAnimation(_animationName.c_str());
}

bool CCHierarchiesSprite::initWithFile (const char* sheetFileName,
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
    
    return this->initWithFile(sheetFileName, animationFileNameBase.c_str(), animationFileNameSub.c_str(), delegate);
}

bool CCHierarchiesSprite::initWithFile (const char* sheetFileName,
                                        const char* animationFileNameBase,
                                        const char* animationFileNameSub,
                                        CCHierarchiesSpriteEventDelegate* delegate) {
    if (CCNodeRGBA::init()) {
        _sheetName = sheetFileName;
        _animationNameBase = animationFileNameBase;
        _animationNameSub = animationFileNameSub;
        _animationName = _animationNameBase + _animationNameSub;
        
        _blendFunc.src = GL_SRC_ALPHA;
        _blendFunc.dst = GL_ONE_MINUS_SRC_ALPHA;
        
        _flipX = false;
        _flipY = false;
        
        _delegate = delegate;
        _curFrameIndex = UINT_MAX;
        
        _sheet = CCHierarchiesSpriteSheetCache::sharedHierarchiesSpriteSheetCache()->addSpriteSheet(sheetFileName);
        _animation = CCHierarchiesSpriteAnimationCache::sharedHierarchiesSpriteAnimationCache()->addAnimation(_animationName.c_str());
        
//        for (int i = 0; i < _animation->getSymbolCount(); i++) {
//            CCHierarchiesSpriteAnimation::Symbol symbol;
//            _animation->getSymbolByIndex(i, symbol);
//            _avatarMap[symbol.name].itemName = symbol.itemName;
//        }
        
        // !!! order is important, should after sheet, animation, avatar map data init
        CCHierarchiesSpriteRuntime::sharedHierarchiesSpriteRuntime()->insertHierarchiesSprite(this);
        
        this->displayFrameAtIndex(0);
        
        return true;
    }
    return false;
}

bool CCHierarchiesSprite::setSpriteFile (const char* sheetFileName, const char* animationFileName) {
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
    
    return this->setSpriteFile(sheetFileName, animationFileNameBase.c_str(), animationFileNameSub.c_str());
}

bool CCHierarchiesSprite::setSpriteFile (const char* sheetFileName,
                                         const char* animationFileNameBase,
                                         const char* animationFileNameSub) {
    // cleanup
    if (this->isRunning()) {
        this->shutdown();
    }
    
	CCHierarchiesSpriteRuntime::sharedHierarchiesSpriteRuntime()->removeHierarchiesSprite(this);
    CCHierarchiesSpriteSheetCache::sharedHierarchiesSpriteSheetCache()->removeSpriteSheet(_sheetName.c_str());
	_sheet = NULL;
	CCHierarchiesSpriteAnimationCache::sharedHierarchiesSpriteAnimationCache()->removeAnimation(_animationName.c_str());
	_animation = NULL;
    
//    _avatarMap.clear();
    
	_sheetName = sheetFileName;
    _animationNameBase = animationFileNameBase;
    _animationNameSub = animationFileNameSub;
	_animationName = _animationNameBase + _animationNameSub;
	
	_sheet = CCHierarchiesSpriteSheetCache::sharedHierarchiesSpriteSheetCache()->addSpriteSheet(sheetFileName);
    _animation = CCHierarchiesSpriteAnimationCache::sharedHierarchiesSpriteAnimationCache()->addAnimation(_animationName.c_str());
    
//    for (int i = 0; i < _animation->getSymbolCount(); i++) {
//		CCHierarchiesSpriteAnimation::Symbol symbol;
//		_animation->getSymbolByIndex(i, symbol);
//		_avatarMap[symbol.name].itemName = symbol.itemName;
//	}
    
    // !!! order is important, should after sheet, animation, avatar map data init
    CCHierarchiesSpriteRuntime::sharedHierarchiesSpriteRuntime()->insertHierarchiesSprite(this);
    
    _curFrameIndex = UINT_MAX;
    this->displayFrameAtIndex(0);
    
    if (this->isRunning()) {
        this->setup();
    }
	
	return true;
}

const char* CCHierarchiesSprite::getSheetName () {
	return _sheetName.c_str();
}

const char* CCHierarchiesSprite::getAnimationName () {
	return _animationName.c_str();
}

const char* CCHierarchiesSprite::getAnimationNameBase () {
    return _animationNameBase.c_str();
}

const char* CCHierarchiesSprite::getAnimationNameSub () {
    return _animationNameSub.c_str();
}

unsigned int CCHierarchiesSprite::getEventContentCount (std::string eventContent) {
    const std::vector<CCHierarchiesSpriteAnimation::Event>& events = _animation->getEvents();
    unsigned int ret = 0;
    std::vector<CCHierarchiesSpriteAnimation::Event>::const_iterator iter;
    for (iter = events.begin(); iter != events.end(); iter++) {
        if (iter->content.compare(eventContent) == 0)
            ret++;
    }
    return ret;
}

bool CCHierarchiesSprite::getFlipX () {
    return _flipX;
}

void CCHierarchiesSprite::setFlipX (bool value) {
    _flipX = value;
    m_bTransformDirty = m_bInverseDirty = true;
}

bool CCHierarchiesSprite::getFlipY () {
    return _flipY;
}

void CCHierarchiesSprite::setFlipY (bool value) {
    _flipY = value;
    m_bTransformDirty = m_bInverseDirty = true;
}


#pragma mark - Setup and shutdown

void CCHierarchiesSprite::onEnter () {
    CCNode::onEnter();
    this->setup();
    this->freshCurrentFrame();
}

void CCHierarchiesSprite::onExit () {
    CCNode::onExit();
    this->shutdown();
}

void CCHierarchiesSprite::setup () {
    // image should load from the same directory with spritesheet file
    std::string imageName(_sheet->getImageName());
    size_t found = _sheetName.find_last_of(HIERARCHIES_SPRITE_PATH_SEPARATOR);
    if (found != std::string::npos)
        imageName = _sheetName.substr(0, found + 1) + imageName;
    
    _texture = CCTextureCache::sharedTextureCache()->addImage(imageName.c_str());
    CC_SAFE_RETAIN(_texture);
    
    CCHierarchiesSpriteRuntime::AnimationCacheHashItem* cacheItem = CCHierarchiesSpriteRuntime::sharedHierarchiesSpriteRuntime()->getCacheItem(_animationName.c_str());
    _mesh = cacheItem->sharedMesh;
    CC_SAFE_RETAIN(_mesh);
    
	this->updateBlendFunc();
	this->updateOpacityModifyRGB();
}

void CCHierarchiesSprite::shutdown() {
    CC_SAFE_RELEASE_NULL(_texture);
    CC_SAFE_RELEASE_NULL(_mesh);
    
//    // clear sub sprites
//    std::unordered_map<int, SubSpriteSocket>::iterator subSpritesIter;
//    for (subSpritesIter = _subSprites.begin(); subSpritesIter != _subSprites.end(); subSpritesIter++) {
//        subSpritesIter->second.subSprite->shutdown();
//        subSpritesIter->second.subSprite->release();
//        subSpritesIter->second.subSprite = NULL;
//    }
//    _subSprites.clear();
}


#pragma mark - Animation

bool CCHierarchiesSprite::displayFrameAtIndex (unsigned int frameIndex) {
    if (frameIndex > _animation->getFrameCount() - 1)
        return false;
    
	if (_curFrameIndex == frameIndex)
        return true;
	
	if (_curFrameIndex != frameIndex) {
		// event dispatch
		if (_delegate) {
			CCHierarchiesSpriteAnimation::Event event;
			if (_animation->getEventByFrameId(frameIndex, event)) {
				_delegate->onEventContent(this, event.content.c_str());
			}
		}
	}
    
    _needFresh = true;
	
    // if animation data is cached than read cached data
    CCHierarchiesSpriteRuntime::AnimationCacheHashItem* cacheItem = CCHierarchiesSpriteRuntime::sharedHierarchiesSpriteRuntime()->getCacheItem(_animationName.c_str());
	if (cacheItem) {
        _curFrameIndex = frameIndex;
        
        CCHierarchiesSpriteRuntime::FrameCacheItem& frameCacheItem = cacheItem->frameCache.at(frameIndex);
        
        // update bounding box, content size and anchor point
        _bbox = frameCacheItem.bbox;
        _frameStartQuadIndex = frameCacheItem.frameStartQuadIndex;
        
        if (_bbox.equals(CCRectZero)) {
            // this frame is whole empty
            this->setAnchorPoint(CCPointZero);
            this->setContentSize(CCSizeZero);
        }
        else {
            CCPoint anchorPoint = ccp((0 - _bbox.origin.x) / _bbox.size.width,
                                      (0 - _bbox.origin.y) / _bbox.size.height);
            this->setAnchorPoint(anchorPoint);
            
            this->setContentSize(_bbox.size);
        }
        
        // update display list
        _displayList = frameCacheItem.displayList;
//        this->updateSocketElements();
        
        return true;
	}
    else {
        CCLOG("<CCHierarchiesSprite> no animation cache with [%s]", _animationName.c_str());
        return false;
    }
}

//void CCHierarchiesSprite::updateSocketElements () {
//    // update socket element
//    std::vector<DisplayElement>::iterator displayElementIter;
//    for (displayElementIter = _displayList.begin(); displayElementIter != _displayList.end(); displayElementIter++) {
//        if (displayElementIter->quadsCount > 0)
//            continue;
//        
//        CCAffineTransform matrix;
//        CCHierarchiesSpriteAnimation::Element* elementIter = &(displayElementIter->socketElement);
//        CCHierarchiesSprite* subSprite = NULL;
//        
//        if (_subSprites.count(elementIter->euid) == 0) {
//            CCHierarchiesSpriteAnimation::Symbol symbol;
//            bool result = _animation->getSymbolByIndex(elementIter->symbolIndex, symbol);
//            assert(result);
//            
//            // instance sub sprite
//            std::string subSpriteNameSub = symbol.name + ".hanims";
//            subSprite = CCHierarchiesSprite::create(_sheetName.c_str(), _animationNameBase.c_str(), subSpriteNameSub.c_str());
//            subSprite->setup();
//            
//            SubSpriteSocket socket;
//            socket.lastDisplayFrameIndex = _curFrameIndex;
//            subSprite->retain();
//            socket.subSprite = subSprite;
//            
//#if HIERARCHIES_ENABLE_FILTER == 1
//            // instance filter
//            std::vector<CCHierarchiesSpriteAnimation::Filter>::const_iterator elementFilterIter;
//            for (elementFilterIter = elementIter->filters.begin(); elementFilterIter != elementIter->filters.end(); elementFilterIter++) {
//                //TODO: add filter instance
//                
//                CCHierarchiesSpriteFilter* filter = new CCHierarchiesSpriteBevelFilter();
//                filter->init(false);
//                
//                socket.filters.push_back(filter);
//                
//                break; //TEMP
//            }
//#endif       
//            
//            std::pair<int, SubSpriteSocket> subSpritePair;
//            subSpritePair.first = elementIter->euid;
//            subSpritePair.second = socket;
//            
//            _subSprites.insert(subSpritePair);
//        }
//        else {
//            const SubSpriteSocket& socket = _subSprites[elementIter->euid];
//            subSprite = socket.subSprite;
//            
//#if HIERARCHIES_ENABLE_FILTER == 1
//            // lerp filter
//            std::vector<CCHierarchiesSpriteFilter*>::const_iterator filterIter;
//            for (filterIter = socket.filters.begin(); filterIter != socket.filters.end(); filterIter++) {
//                //TODO: lerp filter params
//            }
//#endif
//        }
//        
//        // update sub sprite
//        _subSprites[elementIter->euid].lastDisplayFrameIndex = _curFrameIndex;
//        
//        hierarchiesCalcMatrixSocket(&(*elementIter), &matrix);
//        subSprite->setAdditionalTransform(matrix);
////        subSprite->setOpacity(elementIter->color_alpha_percent * 255);
////        subSprite->setColor(ccc3(elementIter->color_red_percent * 255,
////                                 elementIter->color_green_percent * 255,
////                                 elementIter->color_blue_percent * 255));
//        
//        subSprite->displayFrameAtIndex(_curFrameIndex - elementIter->startDisplayFrameIndex);
//    }
//    
//    // remove non-displaying sub sprite
//    std::unordered_map<int, SubSpriteSocket>::iterator subSpritesIter;
//    subSpritesIter = _subSprites.begin();
//    while (subSpritesIter != _subSprites.end()) {
//        if (subSpritesIter->second.lastDisplayFrameIndex != _curFrameIndex) {
//            subSpritesIter->second.subSprite->shutdown();
//            subSpritesIter->second.subSprite->release();
//            subSpritesIter->second.subSprite = NULL;
//            
//#if HIERARCHIES_ENABLE_FILTER == 1
//            // remove filters on sub sprite
//            std::vector<CCHierarchiesSpriteFilter*>::iterator filterIter;
//            for (filterIter = subSpritesIter->second.filters.begin();
//                 filterIter != subSpritesIter->second.filters.end();
//                 filterIter++) {
//                (*filterIter)->release();
//            }
//#endif
//            
//            _subSprites.erase(subSpritesIter++);
//            continue;
//        }
//        subSpritesIter++;
//    }
//}
//
//void CCHierarchiesSprite::updateSocketFilters () {
//    std::unordered_map<int, SubSpriteSocket>::const_iterator subSpritesIter;
//    for (subSpritesIter = _subSprites.begin(); subSpritesIter != _subSprites.end(); subSpritesIter++) {
//        
//        std::vector<CCHierarchiesSpriteFilter*>::const_iterator filterIter;
//        for (filterIter = subSpritesIter->second.filters.begin();
//             filterIter != subSpritesIter->second.filters.end();
//             filterIter++) {
//            CCRect bbox = subSpritesIter->second.subSprite->nestingBoundingBoxInner();
//            if (bbox.equals(CCRectZero)) { //TEMP: !!! there is a whole empty frame in sub sprite
//                bbox = CCRectMake(0, 0, 64, 64);
//            }
//            (*filterIter)->updateFrame(bbox);
//            (*filterIter)->updateVertexPointsWithTramsform(subSpritesIter->second.subSprite->nodeToParentTransform(),
//                                                           subSpritesIter->second.subSprite->getAnchorPointInPoints());
//        }
//        
//    }
//}

bool CCHierarchiesSprite::freshCurrentFrame () {
	unsigned int frameIndex = _curFrameIndex;
    _curFrameIndex = UINT_MAX;
	return displayFrameAtIndex(frameIndex);
}

unsigned int CCHierarchiesSprite::getCurrentFrameIndex () {
    return _curFrameIndex;
}


#pragma mark - Draw

void CCHierarchiesSprite::draw () {
	CC_PROFILER_START_CATEGORY(HIERARCHIES_SPRITE_PROFILER, "CCHierarchiesSprite - animation");
    
    if (_needFresh) {
//        _mesh->flushAllQuadsToBuffer();
    
#if HIERARCHIES_ENABLE_FILTER == 1
        this->updateSocketFilters();
#endif
        
        _needFresh = false;
    }
    
    CC_PROFILER_STOP_CATEGORY(HIERARCHIES_SPRITE_PROFILER, "CCHierarchiesSprite - animation");
    
    // ignore the anchor point while drawing
    CCPoint ap = this->getAnchorPointInPoints();
    kmGLTranslatef(ap.x, ap.y, 0);
    
    // nesting draw
    std::vector<DisplayElement>::iterator displayElementIter;
    for (displayElementIter = _displayList.begin(); displayElementIter != _displayList.end(); displayElementIter++) {
        if (displayElementIter->quadsCount > 0) {
            // quads draw
            CC_PROFILER_START_CATEGORY(HIERARCHIES_SPRITE_PROFILER, "CCHierarchiesSprite - draw");
            
            CC_NODE_DRAW_SETUP();
            
            GLfloat colors[4];
            colors[0] = _displayedColor.r / 255.0f;
            colors[1] = _displayedColor.g / 255.0f;
            colors[2] = _displayedColor.b / 255.0f;
            colors[3] = _displayedOpacity / 255.0f;
            this->getShaderProgram()->setUniformLocationWith4fv(_uniformColorLocation, colors, 1);
            
            ccGLBlendFunc( _blendFunc.src, _blendFunc.dst );
            
            ccGLBindTexture2D(_texture->getName());
            
            _mesh->drawNumberOfQuads(displayElementIter->quadsCount, _frameStartQuadIndex + displayElementIter->quadsIndex);
            
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
            
#elif HIERARCHIES_DEBUG_DRAW == 2
            // draw nesting bounding box
            ccDrawColor4B(0, 255, 255, 255);
            CCRect bbox = this->nestingBoundingBoxInner();
            CCPoint vertices2[4] = {
                ccp(bbox.origin.x, bbox.origin.y),
                ccp(bbox.origin.x + bbox.size.width, bbox.origin.y),
                ccp(bbox.origin.x + bbox.size.width, bbox.origin.y + bbox.size.height),
                ccp(bbox.origin.x, bbox.origin.y + bbox.size.height),
            };
            ccDrawPoly(vertices2, 4, true);

#elif HIERARCHIES_DEBUG_DRAW == 3
            // draw quads
            ccDrawColor4B(0, 255, 0, 255);
            CCHierarchiesSprite_V3F_C4B_T2F_Quad* quad = _mesh->getQuads();
            for (int i = 0; i < displayElementIter->quadsCount; i++) {
                int verxIndex = _frameStartQuadIndex + displayElementIter->quadsIndex + i;
                CCPoint vertices3[4] = {
                    ccp(quad[verxIndex].tl.vertices.x, quad[verxIndex].tl.vertices.y),
                    ccp(quad[verxIndex].tr.vertices.x, quad[verxIndex].tr.vertices.y),
                    ccp(quad[verxIndex].br.vertices.x, quad[verxIndex].br.vertices.y),
                    ccp(quad[verxIndex].bl.vertices.x, quad[verxIndex].bl.vertices.y),
                };
                ccDrawPoly(vertices3, 4, true);
            }
            
            ccDrawColor4B(255, 255, 255, 255);
#endif
            
            CC_PROFILER_STOP_CATEGORY(HIERARCHIES_SPRITE_PROFILER, "CCHierarchiesSprite - draw");
        }
//        else {
//            // sub sprite draw
//            if (_subSprites.count(displayElementIter->socketElement.euid)) {
//                const SubSpriteSocket& socket = _subSprites[displayElementIter->socketElement.euid];
//#if HIERARCHIES_ENABLE_FILTER == 1
//                if (socket.filters.size() > 0) {
//                    std::vector<CCHierarchiesSpriteFilter*>::const_iterator filterIter;
//                    for (filterIter = socket.filters.begin(); filterIter != socket.filters.end(); filterIter++) {
//                        (*filterIter)->beforeDraw();
//                    }
//                    
//                    socket.subSprite->draw();
//                    glFlush();
//                    
//                    for (filterIter = socket.filters.begin(); filterIter != socket.filters.end(); filterIter++) {
//                        (*filterIter)->afterDraw();
//                    }
//                } // if (socket.filters.size() > 0) END
//                else {
//                    socket.subSprite->visit();
//                }
//#else
//                socket.subSprite->visit();
//#endif
//            } // // sub sprite draw END
//        }
    }
}


//#pragma mark - Avatar
//
//void CCHierarchiesSprite::setEnableAvatar (bool value) {
//    _enabledAvatar = value;
//    this->freshCurrentFrame();
//}
//
//bool CCHierarchiesSprite::isEnableAvatar () {
//    return _enabledAvatar;
//}
//
//bool CCHierarchiesSprite::setAvatarMap (const char* symbol, const char* item) {
//	std::string symbolStr(symbol);
//    std::string itemStr(item);
//    
//    std::unordered_map<std::string, AvatarMapItem>::iterator symbolIter;
//    symbolIter = _avatarMap.find(symbolStr);
//    if (symbolIter == _avatarMap.end()) {
//        return false;
//    }
//    
//    CCHierarchiesSpriteSheet::Spr spr;
//    bool result = _sheet->getSpr(itemStr, spr);
//    if (!result) {
//        return false;
//    }
//    
//    symbolIter->second.itemName = spr.name;
//    
//    this->freshCurrentFrame();
//    return true;
//}
//
//void CCHierarchiesSprite::setAvatarTag (const char* tagName) {
//	std::string tagNameStr(tagName);
//	
//	std::unordered_map<std::string, AvatarMapItem>::iterator symbolIter;
//	for (symbolIter = _avatarMap.begin(); symbolIter != _avatarMap.end(); symbolIter++) {
//		std::string itemName = tagNameStr + HIERARCHIES_SPRITE_PATH_SEPARATOR + symbolIter->first;
//        CCHierarchiesSpriteSheet::Spr spr;
//        bool result = _sheet->getSpr(itemName, spr);
//        if (result) {
//            symbolIter->second.itemName = spr.name;
//        }
//	}
//	
//	this->freshCurrentFrame();
//}


#pragma mark - CCNode

CCAffineTransform CCHierarchiesSprite::nodeToParentTransform () {
    if (m_bTransformDirty)
    {
        // flip anchor point in points
        CCPoint obAnchorPointInPoints = m_obAnchorPointInPoints;
        if (_flipX && _flipY) {
            obAnchorPointInPoints = ccp(-m_obAnchorPointInPoints.x, -m_obAnchorPointInPoints.y);
        }
        else if (_flipX) {
            obAnchorPointInPoints = ccp(-m_obAnchorPointInPoints.x, m_obAnchorPointInPoints.y);
        }
        else if (_flipY) {
            obAnchorPointInPoints = ccp(m_obAnchorPointInPoints.x, -m_obAnchorPointInPoints.y);
        }
        
        // Translate values
        float x = m_obPosition.x;
        float y = m_obPosition.y;
        
        if (m_bIgnoreAnchorPointForPosition)
        {
            x += obAnchorPointInPoints.x;
            y += obAnchorPointInPoints.y;
        }
        
        // Rotation values
		// Change rotation code to handle X and Y
		// If we skew with the exact same value for both x and y then we're simply just rotating
        float cx = 1, sx = 0, cy = 1, sy = 0;
        if (m_fRotationX || m_fRotationY)
        {
            float radiansX = -CC_DEGREES_TO_RADIANS(m_fRotationX);
            float radiansY = -CC_DEGREES_TO_RADIANS(m_fRotationY);
            cx = cosf(radiansX);
            sx = sinf(radiansX);
            cy = cosf(radiansY);
            sy = sinf(radiansY);
        }
        
        bool needsSkewMatrix = ( m_fSkewX || m_fSkewY );
        
        
        // optimization:
        // inline anchor point calculation if skew is not needed
        // Adjusted transform calculation for rotational skew
        if (! needsSkewMatrix && !obAnchorPointInPoints.equals(CCPointZero))
        {
            x += cy * -obAnchorPointInPoints.x * m_fScaleX + -sx * -obAnchorPointInPoints.y * m_fScaleY;
            y += sy * -obAnchorPointInPoints.x * m_fScaleX +  cx * -obAnchorPointInPoints.y * m_fScaleY;
        }
        
        
        // Build Transform Matrix
        // Adjusted transform calculation for rotational skew
        m_sTransform = CCAffineTransformMake( cy * m_fScaleX,  sy * m_fScaleX,
                                             -sx * m_fScaleY, cx * m_fScaleY,
                                             x, y );
        
        // XXX: Try to inline skew
        // If skew is needed, apply skew and then anchor point
        if (needsSkewMatrix)
        {
            CCAffineTransform skewMatrix = CCAffineTransformMake(1.0f, tanf(CC_DEGREES_TO_RADIANS(m_fSkewY)),
                                                                 tanf(CC_DEGREES_TO_RADIANS(m_fSkewX)), 1.0f,
                                                                 0.0f, 0.0f );
            m_sTransform = CCAffineTransformConcat(skewMatrix, m_sTransform);
            
            // adjust anchor point
            if (!obAnchorPointInPoints.equals(CCPointZero))
            {
                m_sTransform = CCAffineTransformTranslate(m_sTransform, -obAnchorPointInPoints.x, -obAnchorPointInPoints.y);
            }
        }
        
        if (m_bAdditionalTransformDirty)
        {
            m_sTransform = CCAffineTransformConcat(m_sTransform, m_sAdditionalTransform);
            m_bAdditionalTransformDirty = false;
        }
        
        // flip
        if (_flipX && _flipY) {
            m_sTransform = CCAffineTransformScale(m_sTransform, -1, -1);
        }
        else if (_flipX) {
            m_sTransform = CCAffineTransformScale(m_sTransform, -1, 1);
        }
        else if (_flipY) {
            m_sTransform = CCAffineTransformScale(m_sTransform, 1, -1);
        }
        
        m_bTransformDirty = false;
    }
    
    return m_sTransform;
}

//CCRect CCHierarchiesSprite::nestingBoundingBox()
//{
//    std::unordered_map<int, SubSpriteSocket>::iterator subSpritesIter;
//    CCRect bbox = _bbox;
//    
//    for (subSpritesIter = _subSprites.begin(); subSpritesIter != _subSprites.end(); subSpritesIter++) {
//        CCRect subBBox = subSpritesIter->second.subSprite->nestingBoundingBox();
//        
//        // extend bounding box
//        float nx = subBBox.getMinX() < bbox.getMinX() ? subBBox.origin.x : bbox.origin.x;
//        float ny = subBBox.getMinY() < bbox.getMinY() ? subBBox.origin.y : bbox.origin.y;
//        float nw = subBBox.getMaxX() > bbox.getMaxX() ? subBBox.getMaxX() - nx : bbox.getMaxX() - nx;
//        float nh = subBBox.getMaxY() > bbox.getMaxY() ? subBBox.getMaxY() - ny : bbox.getMaxY() - ny;
//        bbox = CCRectMake(nx, ny, nw, nh);
//    }
//    
//    CCPoint anchorPoint = this->getAnchorPointInPoints();
//    bbox = CCRectMake(bbox.origin.x + anchorPoint.x,
//                      bbox.origin.y + anchorPoint.y,
//                      bbox.size.width,
//                      bbox.size.height);
//    return CCRectApplyAffineTransform(bbox, this->nodeToParentTransform());
//}
//
//CCRect CCHierarchiesSprite::nestingBoundingBoxInner()
//{
//    std::unordered_map<int, SubSpriteSocket>::iterator subSpritesIter;
//    CCRect bbox = _bbox;
//    
//    for (subSpritesIter = _subSprites.begin(); subSpritesIter != _subSprites.end(); subSpritesIter++) {
//        CCRect subBBox = subSpritesIter->second.subSprite->nestingBoundingBoxInner();
//        CCPoint anchorPoint = subSpritesIter->second.subSprite->getAnchorPointInPoints();
//        subBBox = CCRectMake(subBBox.origin.x + anchorPoint.x,
//                          subBBox.origin.y + anchorPoint.y,
//                          subBBox.size.width,
//                          subBBox.size.height);
//        subBBox = CCRectApplyAffineTransform(subBBox, subSpritesIter->second.subSprite->nodeToParentTransform());
//        
//        // extend bounding box
//        float nx = subBBox.getMinX() < bbox.getMinX() ? subBBox.origin.x : bbox.origin.x;
//        float ny = subBBox.getMinY() < bbox.getMinY() ? subBBox.origin.y : bbox.origin.y;
//        float nw = subBBox.getMaxX() > bbox.getMaxX() ? subBBox.getMaxX() - nx : bbox.getMaxX() - nx;
//        float nh = subBBox.getMaxY() > bbox.getMaxY() ? subBBox.getMaxY() - ny : bbox.getMaxY() - ny;
//        bbox = CCRectMake(nx, ny, nw, nh);
//    }
//    
//    return bbox;
//}


#pragma mark - CCBlendProtocol

void CCHierarchiesSprite::setBlendFunc (ccBlendFunc value) {
	_blendFunc = value;
}

ccBlendFunc CCHierarchiesSprite::getBlendFunc (void) {
	return _blendFunc;
}


#pragma mark - CCTextureProtocol

CCTexture2D* CCHierarchiesSprite::getTexture (void) {
    return _texture;
}

void CCHierarchiesSprite::setTexture(CCTexture2D *texture) {
    if (_texture != texture) {
        CC_SAFE_RETAIN(texture);
        CC_SAFE_RELEASE(_texture);
        _texture = texture;
        
        this->updateBlendFunc();
        this->updateOpacityModifyRGB();
        
        this->freshCurrentFrame();
    }
}


#pragma mark - CCRGBAProtocol

void CCHierarchiesSprite::setColor (const ccColor3B& color3) {
    CCNodeRGBA::setColor(color3);
    this->freshCurrentFrame();
}

void CCHierarchiesSprite::setOpacity (GLubyte opacity) {
    CCNodeRGBA::setOpacity(opacity);
    this->freshCurrentFrame();
}

void CCHierarchiesSprite::updateDisplayedColor(const ccColor3B& color) {
    CCNodeRGBA::updateDisplayedColor(color);
    this->freshCurrentFrame();
}

void CCHierarchiesSprite::updateDisplayedOpacity(GLubyte opacity) {
    CCNodeRGBA::updateDisplayedOpacity(opacity);
    this->freshCurrentFrame();
}

void CCHierarchiesSprite::setOpacityModifyRGB (bool bValue) {
    if (_opacityModifyRGB != bValue) {
        _opacityModifyRGB = bValue;
        
        // setup shader program
        if (_opacityModifyRGB) {
            CCGLProgram* glProgram = CCShaderCache::sharedShaderCache()->programForKey(kCCShader_HierarchiesSpriteNonFilter_Premultiplied);
            CCAssert(glProgram, "should call CCHierarchiesSpriteRuntime::sharedHierarchiesSpriteRuntime()->initializationRuntime() at startup");
            this->setShaderProgram(glProgram);
            
            _uniformColorLocation = glProgram->getUniformLocationForName("u_color");
            
            this->freshCurrentFrame();
        }
        else {
            CCGLProgram* glProgram = CCShaderCache::sharedShaderCache()->programForKey(kCCShader_HierarchiesSpriteNonFilter);
            CCAssert(glProgram, "should call CCHierarchiesSpriteRuntime::sharedHierarchiesSpriteRuntime()->initializationRuntime() at startup");
            this->setShaderProgram(glProgram);
            
            _uniformColorLocation = glProgram->getUniformLocationForName("u_color");
            
            this->freshCurrentFrame();
        }
    }
}

bool CCHierarchiesSprite::isOpacityModifyRGB (void) {
	return _opacityModifyRGB;
}


#pragma mark - Private

void CCHierarchiesSprite::updateOpacityModifyRGB () {
	_opacityModifyRGB = _texture->hasPremultipliedAlpha();
    
    // setup shader program
    if (_opacityModifyRGB) {
        CCGLProgram* glProgram = CCShaderCache::sharedShaderCache()->programForKey(kCCShader_HierarchiesSpriteNonFilter_Premultiplied);
        CCAssert(glProgram, "should call CCHierarchiesSpriteRuntime::sharedHierarchiesSpriteRuntime()->initializationRuntime() at startup");
        this->setShaderProgram(glProgram);
        
        _uniformColorLocation = glProgram->getUniformLocationForName("u_color");
    }
    else {
        CCGLProgram* glProgram = CCShaderCache::sharedShaderCache()->programForKey(kCCShader_HierarchiesSpriteNonFilter);
        CCAssert(glProgram, "should call CCHierarchiesSpriteRuntime::sharedHierarchiesSpriteRuntime()->initializationRuntime() at startup");
        this->setShaderProgram(glProgram);
        
        _uniformColorLocation = glProgram->getUniformLocationForName("u_color");
    }
}

void CCHierarchiesSprite::updateBlendFunc () {
	if( !_texture->hasPremultipliedAlpha() ) {
		_blendFunc.src = GL_SRC_ALPHA;
		_blendFunc.dst = GL_ONE_MINUS_SRC_ALPHA;
	}
    else {
        _blendFunc.src = GL_ONE;
		_blendFunc.dst = GL_ONE_MINUS_SRC_ALPHA;
    }
}


NS_CC_EXT_END
