//
//  CCHierarchiesSpriteBase.cpp
//  CCHierarchiesSprite
//
//  Created by bman <zx123xz321hm3@hotmail.com>.
//  Copyright (c) 2013. All rights reserved.
//

#include "CCHierarchiesSpriteBase.h"
#include "CCHierarchiesSpriteSheetCache.h"
#include "CCHierarchiesSpriteAnimationCache.h"


NS_CC_EXT_BEGIN

#pragma mark - Math

// void hierarchiesCalcMatrixSocket (CCHierarchiesSpriteAnimation::Element* element, CCAffineTransform* matrix) {
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

void hierarchiesCalcMatrix (const CCHierarchiesSpriteAnimation::Symbol* symbol,
                                          const CCHierarchiesSpriteSheet::Spr* spr,
                                          const CCHierarchiesSpriteAnimation::Element* element,
                                          CCAffineTransform* matrix) {
    FMatrix2D m0;
    FMatrix2D m1;
    
    m0.setAnchorX(element->anchorX - symbol->left);
    m0.setAnchorY(element->anchorY - symbol->bottom);
    
    m1.setScaleX(element->scaleX);
    m1.setScaleY(element->scaleY);
    m1.setSkewX(element->skewX);
    m1.setSkewY(element->skewY);
    m1.setTransformX(element->x);
    m1.setTransformY(element->y);
    
    FMatrix2D m = m0.concat(m1);
    
    *matrix = CCAffineTransformMake(m.a, -m.b, -m.c, m.d, m.tx, -m.ty);
}

void hierarchiesCalcMatrixSocket (const CCHierarchiesSpriteAnimation::Element* element, CCAffineTransform* matrix) {
    FMatrix2D m0;
    FMatrix2D m1;
    
    m0.setAnchorX(element->anchorX);
    m0.setAnchorY(element->anchorY);
    
    m1.setScaleX(element->scaleX);
    m1.setScaleY(element->scaleY);
    m1.setSkewX(element->skewX);
    m1.setSkewY(element->skewY);
    m1.setTransformX(element->x);
    m1.setTransformY(element->y);
    
    FMatrix2D m = m0.concat(m1);
    
    *matrix = CCAffineTransformMake(m.a, -m.b, -m.c, m.d, m.tx, -m.ty);
}

void hierarchiesUpdateQuadVertices (CCSize size, const CCAffineTransform* matrix, CCHierarchiesSprite_V3F_C4B_T2F_Quad* quad, const float z, const bool isRotation) {
    if (isRotation) {
        float tmp = size.width;
        size.width = size.height;
        size.height = tmp;
    }
    
    float x1 = 0;
    float y1 = 0;
    
    float x2 = x1 + size.width;
    float y2 = y1 + size.height;
    float x = matrix->tx;
    float y = matrix->ty;
    
    float cr = matrix->a;
    float sr = matrix->b;
    float cr2 = matrix->d;
    float sr2 = -matrix->c;
    float ax = x1 * cr - y1 * sr2 + x;
    float ay = x1 * sr + y1 * cr2 + y;
    
    float bx = x2 * cr - y1 * sr2 + x;
    float by = x2 * sr + y1 * cr2 + y;
    
    float cx = x2 * cr - y2 * sr2 + x;
    float cy = x2 * sr + y2 * cr2 + y;
    
    float dx = x1 * cr - y2 * sr2 + x;
    float dy = x1 * sr + y2 * cr2 + y;
    
	quad->bl.vertices = (ccVertex3F) { ax, ay, z };
    quad->br.vertices = (ccVertex3F) { bx, by, z };
    quad->tl.vertices = (ccVertex3F) { dx, dy, z };
    quad->tr.vertices = (ccVertex3F) { cx, cy, z };
}

void hierarchiesUpdateQuadTextureCoords (const CCRect rect, const float texWidth, const float texHeight, CCHierarchiesSprite_V3F_C4B_T2F_Quad* quad, const bool isRotation) {
    float left, right, top, bottom;
    
#if CC_FIX_ARTIFACTS_BY_STRECHING_TEXEL
    left	= (2 * rect.origin.x + 1) / (2 * texWidth);
    right	= left + (rect.size.width * 2 - 2) / (2 * texWidth);
    top		= (2 * rect.origin.y + 1) / (2 * texHeight);
    bottom	= top + (rect.size.height * 2 - 2) / (2 * texHeight);
#else
    left	= rect.origin.x / texWidth;
    right	= left + rect.size.width / texWidth;
    top		= rect.origin.y / texHeight;
    bottom	= top + rect.size.height / texHeight;
#endif
    
    if (!isRotation) {
        quad->bl.texCoords.u = left;
        quad->bl.texCoords.v = bottom;
        quad->br.texCoords.u = right;
        quad->br.texCoords.v = bottom;
        quad->tl.texCoords.u = left;
        quad->tl.texCoords.v = top;
        quad->tr.texCoords.u = right;
        quad->tr.texCoords.v = top;
    }
    else {
        quad->bl.texCoords.u = left; // tl
        quad->bl.texCoords.v = top; // tl
        quad->br.texCoords.u = left; // bl
        quad->br.texCoords.v = bottom; // bl
        quad->tl.texCoords.u = right; // tr
        quad->tl.texCoords.v = top; // tr
        quad->tr.texCoords.u = right; // br
        quad->tr.texCoords.v = bottom; // br
    }
}

void hierarchiesUpdateQuadTextureColorFromAnimation (const float alpha_percent, const int alpha_amount,
                                                                   const float red_percent, const int red_amount,
                                                                   const float green_percent, const int green_amount,
                                                                   const float blue_percent, const int blue_amount,
                                                                   CCHierarchiesSprite_V3F_C4B_T2F_Quad* quad) {
    int value;
    
    value = 255 * alpha_percent;
    quad->bl.colorsMul.a = value;
    quad->br.colorsMul.a = value;
    quad->tl.colorsMul.a = value;
    quad->tr.colorsMul.a = value;
    quad->bl.colorsAdd.a = alpha_amount;
    quad->br.colorsAdd.a = alpha_amount;
    quad->tl.colorsAdd.a = alpha_amount;
    quad->tr.colorsAdd.a = alpha_amount;
    
    value = 255 * red_percent;
    quad->bl.colorsMul.r = value;
    quad->br.colorsMul.r = value;
    quad->tl.colorsMul.r = value;
    quad->tr.colorsMul.r = value;
    quad->bl.colorsAdd.r = red_amount;
    quad->br.colorsAdd.r = red_amount;
    quad->tl.colorsAdd.r = red_amount;
    quad->tr.colorsAdd.r = red_amount;
    
    value = 255 * green_percent;
    quad->bl.colorsMul.g = value;
    quad->br.colorsMul.g = value;
    quad->tl.colorsMul.g = value;
    quad->tr.colorsMul.g = value;
    quad->bl.colorsAdd.g = green_amount;
    quad->br.colorsAdd.g = green_amount;
    quad->tl.colorsAdd.g = green_amount;
    quad->tr.colorsAdd.g = green_amount;
    
    value = 255 * blue_percent;
    quad->bl.colorsMul.b = value;
    quad->br.colorsMul.b = value;
    quad->tl.colorsMul.b = value;
    quad->tr.colorsMul.b = value;
    quad->bl.colorsAdd.b = blue_amount;
    quad->br.colorsAdd.b = blue_amount;
    quad->tl.colorsAdd.b = blue_amount;
    quad->tr.colorsAdd.b = blue_amount;
}

//void hierarchiesUpdateQuadTextureColor (const bool opacityModifyRGB, const int opacity, const int color_r, const int color_g, const int color_b, CCHierarchiesSprite_V3F_C4B_T2F_Quad* quad) {
//	// If opacityModifyRGB is NO then opacity will be applied as: glColor(R,G,B,opacity);
//	// If opacityModifyRGB is YES then oapcity will be applied as: glColor(opacity, opacity, opacity, opacity );
//	if (opacityModifyRGB) {
//		quad->bl.colorsMul.a = quad->bl.colorsMul.a * (opacity / 255.0f);
//		quad->br.colorsMul.a = quad->br.colorsMul.a * (opacity / 255.0f);
//		quad->tl.colorsMul.a = quad->tl.colorsMul.a * (opacity / 255.0f);
//		quad->tr.colorsMul.a = quad->tr.colorsMul.a * (opacity / 255.0f);
//
//		quad->bl.colorsMul.r = quad->bl.colorsMul.r * (color_r / 255.0f) * (opacity / 255.0f);
//		quad->br.colorsMul.r = quad->br.colorsMul.r * (color_r / 255.0f) * (opacity / 255.0f);
//		quad->tl.colorsMul.r = quad->tl.colorsMul.r * (color_r / 255.0f) * (opacity / 255.0f);
//		quad->tr.colorsMul.r = quad->tr.colorsMul.r * (color_r / 255.0f) * (opacity / 255.0f);
//
//		quad->bl.colorsMul.g = quad->bl.colorsMul.g * (color_g / 255.0f) * (opacity / 255.0f);
//		quad->br.colorsMul.g = quad->br.colorsMul.g * (color_g / 255.0f) * (opacity / 255.0f);
//		quad->tl.colorsMul.g = quad->tl.colorsMul.g * (color_g / 255.0f) * (opacity / 255.0f);
//		quad->tr.colorsMul.g = quad->tr.colorsMul.g * (color_g / 255.0f) * (opacity / 255.0f);
//
//		quad->bl.colorsMul.b = quad->bl.colorsMul.b * (color_b / 255.0f) * (opacity / 255.0f);
//		quad->br.colorsMul.b = quad->br.colorsMul.b * (color_b / 255.0f) * (opacity / 255.0f);
//		quad->tl.colorsMul.b = quad->tl.colorsMul.b * (color_b / 255.0f) * (opacity / 255.0f);
//		quad->tr.colorsMul.b = quad->tr.colorsMul.b * (color_b / 255.0f) * (opacity / 255.0f);
//	}
//	else {
//		quad->bl.colorsMul.a = quad->bl.colorsMul.a * (opacity / 255.0f);
//		quad->br.colorsMul.a = quad->br.colorsMul.a * (opacity / 255.0f);
//		quad->tl.colorsMul.a = quad->tl.colorsMul.a * (opacity / 255.0f);
//		quad->tr.colorsMul.a = quad->tr.colorsMul.a * (opacity / 255.0f);
//
//		quad->bl.colorsMul.r = quad->bl.colorsMul.r * (color_r / 255.0f);
//		quad->br.colorsMul.r = quad->br.colorsMul.r * (color_r / 255.0f);
//		quad->tl.colorsMul.r = quad->tl.colorsMul.r * (color_r / 255.0f);
//		quad->tr.colorsMul.r = quad->tr.colorsMul.r * (color_r / 255.0f);
//
//		quad->bl.colorsMul.g = quad->bl.colorsMul.g * (color_g / 255.0f);
//		quad->br.colorsMul.g = quad->br.colorsMul.g * (color_g / 255.0f);
//		quad->tl.colorsMul.g = quad->tl.colorsMul.g * (color_g / 255.0f);
//		quad->tr.colorsMul.g = quad->tr.colorsMul.g * (color_g / 255.0f);
//
//		quad->bl.colorsMul.b = quad->bl.colorsMul.b * (color_b / 255.0f);
//		quad->br.colorsMul.b = quad->br.colorsMul.b * (color_b / 255.0f);
//		quad->tl.colorsMul.b = quad->tl.colorsMul.b * (color_b / 255.0f);
//		quad->tr.colorsMul.b = quad->tr.colorsMul.b * (color_b / 255.0f);
//	}
//}

void hierarchiesExpandRectByPoint (float* minX, float* maxX, float* minY, float* maxY, float* pX, float* pY) {
    if (*pX < *minX) {
        *minX = *pX;
    }
    if (*pX > *maxX) {
        *maxX = *pX;
    }
    
    if (*pY < *minY) {
        *minY = *pY;
    }
    if (*pY > *maxY) {
        *maxY = *pY;
    }
}

float float_round (float r) {
    return (r > 0.0f) ? floorf(r + 0.5f) : ceilf(r - 0.5f);
}


#pragma mark - CCHierarchiesSpriteBase

CCHierarchiesSpriteBase::CCHierarchiesSpriteBase ()
: _sheet(NULL), _animation(NULL), _mesh(NULL), _texture(NULL), _delegate(NULL), _curFrameIndex(0),
_needFresh(false), _blendFunc(kCCBlendFuncDisable) , _opacityModifyRGB(false),
_flipX(false), _flipY(false), _bbox(CCRectZero), _uniformColorLocation(0) {
}

CCHierarchiesSpriteBase::~CCHierarchiesSpriteBase () {
    CCHierarchiesSpriteSheetCache::sharedHierarchiesSpriteSheetCache()->removeSpriteSheet(_sheetName.c_str());
    CCHierarchiesSpriteAnimationCache::sharedHierarchiesSpriteAnimationCache()->removeAnimation(_animationName.c_str());
}

bool CCHierarchiesSpriteBase::initWithFile (const char* sheetFileName,
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
    
    return this->initWithFileAndAvatar(sheetFileName,
                                       animationFileNameBase.c_str(),
                                       animationFileNameSub.c_str(),
                                       delegate,
                                       AvatarMapType());
}

bool CCHierarchiesSpriteBase::initWithFile (const char* sheetFileName,
                                            const char* animationFileNameBase,
                                            const char* animationFileNameSub,
                                            CCHierarchiesSpriteEventDelegate* delegate) {
    return this->initWithFileAndAvatar(sheetFileName,
                                       animationFileNameBase,
                                       animationFileNameSub,
                                       delegate,
                                       AvatarMapType());
}

bool CCHierarchiesSpriteBase::initWithFileAndAvatar (const char* sheetFileName,
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
    
    return this->initWithFileAndAvatar(sheetFileName,
                                       animationFileNameBase.c_str(),
                                       animationFileNameSub.c_str(),
                                       delegate,
                                       avatarMap);
}

bool CCHierarchiesSpriteBase::initWithFileAndAvatar (const char* sheetFileName,
                                                 const char* animationFileNameBase,
                                                 const char* animationFileNameSub,
                                                 CCHierarchiesSpriteEventDelegate* delegate,
                                                 const AvatarMapType& avatarMap) {
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
        
        return true;
    }
    return false;
}

const char* CCHierarchiesSpriteBase::getSheetName () {
	return _sheetName.c_str();
}

const char* CCHierarchiesSpriteBase::getAnimationName () {
	return _animationName.c_str();
}

const char* CCHierarchiesSpriteBase::getAnimationNameBase () {
    return _animationNameBase.c_str();
}

const char* CCHierarchiesSpriteBase::getAnimationNameSub () {
    return _animationNameSub.c_str();
}

unsigned int CCHierarchiesSpriteBase::getEventContentCount (std::string eventContent) {
    const std::vector<CCHierarchiesSpriteAnimation::Event>& events = _animation->getEvents();
    unsigned int ret = 0;
    std::vector<CCHierarchiesSpriteAnimation::Event>::const_iterator iter;
    for (iter = events.begin(); iter != events.end(); iter++) {
        if (iter->content.compare(eventContent) == 0)
            ret++;
    }
    return ret;
}

bool CCHierarchiesSpriteBase::getFlipX () {
    return _flipX;
}

void CCHierarchiesSpriteBase::setFlipX (bool value) {
    _flipX = value;
    m_bTransformDirty = m_bInverseDirty = true;
}

bool CCHierarchiesSpriteBase::getFlipY () {
    return _flipY;
}

void CCHierarchiesSpriteBase::setFlipY (bool value) {
    _flipY = value;
    m_bTransformDirty = m_bInverseDirty = true;
}


#pragma mark - Setup and shutdown

void CCHierarchiesSpriteBase::onEnter () {
    CCNode::onEnter();
    this->setup();
    this->freshCurrentFrame();
}

void CCHierarchiesSpriteBase::onExit () {
    CCNode::onExit();
    this->shutdown();
}

void CCHierarchiesSpriteBase::setup () {
    // image should load from the same directory with spritesheet file
    std::string imageName(_sheet->getImageName());
    size_t found = _sheetName.find_last_of(HIERARCHIES_SPRITE_PATH_SEPARATOR);
    if (found != std::string::npos)
        imageName = _sheetName.substr(0, found + 1) + imageName;
    
    _texture = CCTextureCache::sharedTextureCache()->addImage(imageName.c_str());
    CC_SAFE_RETAIN(_texture);
    
	this->updateBlendFunc();
	this->updateOpacityModifyRGB();
}

void CCHierarchiesSpriteBase::shutdown() {
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

bool CCHierarchiesSpriteBase::displayFrameAtIndex (unsigned int frameIndex) {
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
    
    return true;
}

//void CCHierarchiesSpriteBase::updateSocketElements () {
//    // update socket element
//    std::vector<DisplayElement>::iterator displayElementIter;
//    for (displayElementIter = _displayList.begin(); displayElementIter != _displayList.end(); displayElementIter++) {
//        if (displayElementIter->quadsCount > 0)
//            continue;
//
//        CCAffineTransform matrix;
//        CCHierarchiesSpriteBaseAnimation::Element* elementIter = &(displayElementIter->socketElement);
//        CCHierarchiesSpriteBase* subSprite = NULL;
//
//        if (_subSprites.count(elementIter->euid) == 0) {
//            CCHierarchiesSpriteBaseAnimation::Symbol symbol;
//            bool result = _animation->getSymbolByIndex(elementIter->symbolIndex, symbol);
//            assert(result);
//
//            // instance sub sprite
//            std::string subSpriteNameSub = symbol.name + ".hanims";
//            subSprite = CCHierarchiesSpriteBase::create(_sheetName.c_str(), _animationNameBase.c_str(), subSpriteNameSub.c_str());
//            subSprite->setup();
//
//            SubSpriteSocket socket;
//            socket.lastDisplayFrameIndex = _curFrameIndex;
//            subSprite->retain();
//            socket.subSprite = subSprite;
//
//#if HIERARCHIES_ENABLE_FILTER == 1
//            // instance filter
//            std::vector<CCHierarchiesSpriteBaseAnimation::Filter>::const_iterator elementFilterIter;
//            for (elementFilterIter = elementIter->filters.begin(); elementFilterIter != elementIter->filters.end(); elementFilterIter++) {
//                //TODO: add filter instance
//
//                CCHierarchiesSpriteBaseFilter* filter = new CCHierarchiesSpriteBaseBevelFilter();
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
//            std::vector<CCHierarchiesSpriteBaseFilter*>::const_iterator filterIter;
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
//            std::vector<CCHierarchiesSpriteBaseFilter*>::iterator filterIter;
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
//void CCHierarchiesSpriteBase::updateSocketFilters () {
//    std::unordered_map<int, SubSpriteSocket>::const_iterator subSpritesIter;
//    for (subSpritesIter = _subSprites.begin(); subSpritesIter != _subSprites.end(); subSpritesIter++) {
//
//        std::vector<CCHierarchiesSpriteBaseFilter*>::const_iterator filterIter;
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

bool CCHierarchiesSpriteBase::freshCurrentFrame () {
	unsigned int frameIndex = _curFrameIndex;
    _curFrameIndex = UINT_MAX;
	return displayFrameAtIndex(frameIndex);
}

unsigned int CCHierarchiesSpriteBase::getCurrentFrameIndex () {
    return _curFrameIndex;
}


#pragma mark - CCNode

CCAffineTransform CCHierarchiesSpriteBase::nodeToParentTransform () {
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

//CCRect CCHierarchiesSpriteBase::nestingBoundingBox()
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
//CCRect CCHierarchiesSpriteBase::nestingBoundingBoxInner()
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

void CCHierarchiesSpriteBase::setBlendFunc (ccBlendFunc value) {
	_blendFunc = value;
}

ccBlendFunc CCHierarchiesSpriteBase::getBlendFunc (void) {
	return _blendFunc;
}


#pragma mark - CCTextureProtocol

CCTexture2D* CCHierarchiesSpriteBase::getTexture (void) {
    return _texture;
}

void CCHierarchiesSpriteBase::setTexture(CCTexture2D *texture) {
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

void CCHierarchiesSpriteBase::setColor (const ccColor3B& color3) {
    CCNodeRGBA::setColor(color3);
    this->freshCurrentFrame();
}

void CCHierarchiesSpriteBase::setOpacity (GLubyte opacity) {
    CCNodeRGBA::setOpacity(opacity);
    this->freshCurrentFrame();
}

void CCHierarchiesSpriteBase::updateDisplayedColor(const ccColor3B& color) {
    CCNodeRGBA::updateDisplayedColor(color);
    this->freshCurrentFrame();
}

void CCHierarchiesSpriteBase::updateDisplayedOpacity(GLubyte opacity) {
    CCNodeRGBA::updateDisplayedOpacity(opacity);
    this->freshCurrentFrame();
}

void CCHierarchiesSpriteBase::setOpacityModifyRGB (bool bValue) {
    if (_opacityModifyRGB != bValue) {
        _opacityModifyRGB = bValue;
        
        // setup shader program
        if (_opacityModifyRGB) {
            CCGLProgram* glProgram = CCShaderCache::sharedShaderCache()->programForKey(kCCShader_HierarchiesSpriteNonFilter_Premultiplied);
            CCAssert(glProgram, "should call CCHierarchiesSpriteBaseRuntime::sharedHierarchiesSpriteRuntime()->initializationRuntime() at startup");
            this->setShaderProgram(glProgram);
            
            _uniformColorLocation = glProgram->getUniformLocationForName("u_color");
            
            this->freshCurrentFrame();
        }
        else {
            CCGLProgram* glProgram = CCShaderCache::sharedShaderCache()->programForKey(kCCShader_HierarchiesSpriteNonFilter);
            CCAssert(glProgram, "should call CCHierarchiesSpriteBaseRuntime::sharedHierarchiesSpriteRuntime()->initializationRuntime() at startup");
            this->setShaderProgram(glProgram);
            
            _uniformColorLocation = glProgram->getUniformLocationForName("u_color");
            
            this->freshCurrentFrame();
        }
    }
}

bool CCHierarchiesSpriteBase::isOpacityModifyRGB (void) {
	return _opacityModifyRGB;
}


#pragma mark - Private

void CCHierarchiesSpriteBase::updateOpacityModifyRGB () {
	_opacityModifyRGB = _texture->hasPremultipliedAlpha();
    
    // setup shader program
    if (_opacityModifyRGB) {
        CCGLProgram* glProgram = CCShaderCache::sharedShaderCache()->programForKey(kCCShader_HierarchiesSpriteNonFilter_Premultiplied);
        CCAssert(glProgram, "should call CCHierarchiesSpriteBaseRuntime::sharedHierarchiesSpriteRuntime()->initializationRuntime() at startup");
        this->setShaderProgram(glProgram);
        
        _uniformColorLocation = glProgram->getUniformLocationForName("u_color");
    }
    else {
        CCGLProgram* glProgram = CCShaderCache::sharedShaderCache()->programForKey(kCCShader_HierarchiesSpriteNonFilter);
        CCAssert(glProgram, "should call CCHierarchiesSpriteBaseRuntime::sharedHierarchiesSpriteRuntime()->initializationRuntime() at startup");
        this->setShaderProgram(glProgram);
        
        _uniformColorLocation = glProgram->getUniformLocationForName("u_color");
    }
}

void CCHierarchiesSpriteBase::updateBlendFunc () {
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

