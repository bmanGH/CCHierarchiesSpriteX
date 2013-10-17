//
//  CCHierarchiesSpriteRuntime.m
//  HierarchiesSpriteViewer
//
//  Created by bman <zx123xz321hm3@hotmail.com>.
//  Copyright (c) 2013. All rights reserved.
//

#include "CCHierarchiesSpriteRuntime.h"
#include "CCHierarchiesSpriteAnimationCache.h"
#include <sstream>
#include "CCHierarchiesSpriteAnimationCache.h"
#include "CCHierarchiesSpriteSheetCache.h"
#include "CCHierarchiesSpriteShader.h"

NS_CC_EXT_BEGIN


inline static void hierarchiesCalcMatrix (const CCHierarchiesSpriteAnimation::Symbol* symbol,
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

inline static void hierarchiesCalcMatrixSocket (const CCHierarchiesSpriteAnimation::Element* element, CCAffineTransform* matrix) {
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

inline static void hierarchiesUpdateQuadVertices (CCSize size, const CCAffineTransform* matrix, CCHierarchiesSprite_V3F_C4B_T2F_Quad* quad, const float z, const bool isRotation) {
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

inline static void hierarchiesUpdateQuadTextureCoords (const CCRect rect, const float texWidth, const float texHeight, CCHierarchiesSprite_V3F_C4B_T2F_Quad* quad, const bool isRotation) {
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

inline static void hierarchiesUpdateQuadTextureColorFromAnimation (const float alpha_percent, const int alpha_amount,
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

//inline static void hierarchiesUpdateQuadTextureColor (const bool opacityModifyRGB, const int opacity, const int color_r, const int color_g, const int color_b, CCHierarchiesSprite_V3F_C4B_T2F_Quad* quad) {
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

inline static void hierarchiesExpandRectByPoint (float* minX, float* maxX, float* minY, float* maxY, float* pX, float* pY) {
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

inline static float float_round (float r) {
    return (r > 0.0f) ? floorf(r + 0.5f) : ceilf(r - 0.5f);
}


static CCHierarchiesSpriteRuntime* g_sharedHierarchiesSpriteRuntime = NULL;

CCHierarchiesSpriteRuntime* CCHierarchiesSpriteRuntime::sharedHierarchiesSpriteRuntime () {
	if (!g_sharedHierarchiesSpriteRuntime) {
        g_sharedHierarchiesSpriteRuntime = new CCHierarchiesSpriteRuntime();
        if (!g_sharedHierarchiesSpriteRuntime->init())
        {
            CC_SAFE_DELETE(g_sharedHierarchiesSpriteRuntime);
        }
    }
    return g_sharedHierarchiesSpriteRuntime;
}

void CCHierarchiesSpriteRuntime::purgeHierarchiesSpriteRuntime () {
	CC_SAFE_RELEASE_NULL(g_sharedHierarchiesSpriteRuntime);
}

CCHierarchiesSpriteRuntime::~CCHierarchiesSpriteRuntime () {
	AnimationCacheHashItem* hashItem, *tmp;
    HASH_ITER(hh, _animationCache, hashItem, tmp) {
        HASH_DEL(_animationCache, hashItem);
        delete hashItem;
    }
}

bool CCHierarchiesSpriteRuntime::init () {
	_animationCache = NULL;
	return true;
}

void CCHierarchiesSpriteRuntime::insertHierarchiesSprite (CCHierarchiesSprite* sprite) {
	AnimationCacheHashItem* hashItem = NULL;
	HASH_FIND_STR(_animationCache, sprite->getAnimationName(), hashItem);
	if (!hashItem) {
		AnimationCacheHashItem* newCache = new AnimationCacheHashItem(sprite->getAnimation()->getFrameCount());
		int keyLen = strlen(sprite->getAnimationName());
		newCache->name = (char*)calloc(keyLen + 1, sizeof(char)); // add 1 for char '\0'
		strcpy(newCache->name, sprite->getAnimationName());
		newCache->retainCount = 2; // + 1 for cache
		HASH_ADD_KEYPTR(hh, _animationCache, newCache->name, keyLen, newCache);
        
        for (unsigned int frameIndex = 0; frameIndex < sprite->getAnimation()->getFrameCount(); frameIndex++) {
            float min_X = 10000, max_X = -10000, min_Y = 10000, max_Y = -10000;
            this->buildRuntimeAnimationData(true,
                                            CCHierarchiesSpriteAnimation::kNoneLoopMode,
                                            0,
                                            frameIndex,
                                            CCAffineTransformIdentity,
                                            newCache,
                                            sprite->getSheet(),
                                            sprite->getAnimation(),
                                            sprite->getAnimationNameBase(),
                                            min_X, max_X, min_Y, max_Y,
                                            1, 0,
                                            1, 0,
                                            1, 0,
                                            1, 0);
        }
        newCache->sharedMesh->flushAllQuadsToBuffer();
	}
	else {
		hashItem->retainCount++;
	}
}

void CCHierarchiesSpriteRuntime::removeHierarchiesSprite (CCHierarchiesSprite* sprite) {
	AnimationCacheHashItem* hashItem = NULL;
    HASH_FIND_STR(_animationCache, sprite->getAnimationName(), hashItem);
    if (hashItem) {
        hashItem->retainCount--;
        if (hashItem->retainCount <= 0) {
            HASH_DEL(_animationCache, hashItem);
            delete hashItem;
        }
    }
}

void CCHierarchiesSpriteRuntime::removeUnusedHierarchiesSprite() {
    AnimationCacheHashItem* hashItem, *tmp;
    HASH_ITER(hh, _animationCache, hashItem, tmp) {
        if (hashItem->retainCount == 1) {
            HASH_DEL(_animationCache, hashItem);
            delete hashItem;
        }
    }
}

CCHierarchiesSpriteRuntime::AnimationCacheHashItem* CCHierarchiesSpriteRuntime::getCacheItem (const char* itemName) {
    CCHierarchiesSpriteRuntime::AnimationCacheHashItem* ret = NULL;
	HASH_FIND_STR(_animationCache, itemName, ret);
    return ret;
}

const char* CCHierarchiesSpriteRuntime::description () {
	std::stringstream info;
    info << "CCHierarchiesSpriteRuntime:\n";
    info << "{\n";
	AnimationCacheHashItem* hashItem, *tmp;
    HASH_ITER(hh, _animationCache, hashItem, tmp) {
		info << "\t[" << hashItem->name << " : " << hashItem->frameCache.size() << " : " << hashItem->retainCount << "]\n";
    }
    info << "}\n";
	
	return CCString::create(info.str().c_str())->getCString();
}

void CCHierarchiesSpriteRuntime::buildRuntimeAnimationData (bool isRoot,
                                                                     CCHierarchiesSpriteAnimation::ElementLoopMode loopMode,
                                                                     int frameOffset,
                                                                     unsigned int frameIndex,
                                                                     const CCAffineTransform& parentMatrix,
                                                                     AnimationCacheHashItem* cacheItem,
                                                                     const CCHierarchiesSpriteSheet* sheet,
                                                                     const CCHierarchiesSpriteAnimation* animation,
                                                                     const char* animationFileNameBase,
                                                                     float& min_X, float& max_X, float& min_Y, float& max_Y,
                                                                     const float parent_alpha_percent, const int parent_alpha_amount,
                                                                     const float parent_red_percent, const int parent_red_amount,
                                                                     const float parent_green_percent, const int parent_green_amount,
                                                                     const float parent_blue_percent, const int parent_blue_amount) {
    CCHierarchiesSpriteAnimation::FrameElements frameElements;
    int eNum = animation->getFrameElementsAtFrameIndex(loopMode, frameOffset, frameIndex, frameElements); //!!! frameIndex will update with loop mode
    
    CCHierarchiesSpriteMesh* mesh = cacheItem->sharedMesh;
    size_t frameStartQuadIndex = mesh->getTotalQuads();
    
    std::vector<CCHierarchiesSprite_V3F_C4B_T2F_Quad> quads(eNum);
    quads.clear();
    std::vector<CCHierarchiesSprite::DisplayElement> displayList;
    
//    float min_X = 10000, max_X = -10000, min_Y = 10000, max_Y = -10000;
    
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
            
//                AvatarMapItem avatarMapItem;
//                avatarMapItem = _avatarMap[symbol.name];
//
//                CCHierarchiesSpriteSheet::Spr spr;
//                result = _sheet->getSpr(avatarMapItem.itemName, spr);
//                assert(result);
            
            CCHierarchiesSpriteSheet::Spr spr;
            result = sheet->getSpr(symbol.name, spr);
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
            //NEEDFIX: update quad color while each sprite OpacityModifyRGB property change
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
                                               sheet->getImageWidth(),
                                               sheet->getImageHeight(),
                                               &quad,
                                               spr.isRotation);
            
            quads.push_back(quad);
            
//                quadsCount++;
        } // simple element END
        else { // nesting sprite element BEGIN
            
//                // element display list
//                if (quadsCount > 0) {
//                    CCHierarchiesSprite::DisplayElement displayElement;
//                    displayElement.quadsIndex = quadsIndex;
//                    displayElement.quadsCount = quadsCount;
//                    displayList.push_back(displayElement);
//                    
//                    quadsIndex += quadsCount;
//                    quadsCount = 0;
//                }
//                
//                // socket element display list
//                CCHierarchiesSprite::DisplayElement displayElement;
//                displayElement.socketElement = *elementIter;
//                displayElement.quadsIndex = 0;
//                displayElement.quadsCount = 0;
//                displayList.push_back(displayElement);
            
////                float px = elementIter->x;
////                float py = elementIter->y;
////                hierarchiesExpandRectByPoint(&min_X, &max_X, &min_Y, &max_Y, &px, &py);
////                px = elementIter->x + 1;
////                py = elementIter->y;
////                hierarchiesExpandRectByPoint(&min_X, &max_X, &min_Y, &max_Y, &px, &py);
////                px = elementIter->x + 1;
////                py = elementIter->y + 1;
////                hierarchiesExpandRectByPoint(&min_X, &max_X, &min_Y, &max_Y, &px, &py);
////                px = elementIter->x;
////                py = elementIter->y + 1;
////                hierarchiesExpandRectByPoint(&min_X, &max_X, &min_Y, &max_Y, &px, &py);
            
            size_t quadsToDraw = quads.size();
            size_t oldTotalQuads = mesh->getTotalQuads();
            if ((oldTotalQuads + quadsToDraw) > mesh->getCapacity())
                mesh->resizeCapacity(oldTotalQuads + quadsToDraw);
            if (quadsToDraw > 0)
                mesh->insertQuads(&quads[0], oldTotalQuads, quadsToDraw);
            quads.clear();
            
            std::string subSpriteAnimationName(animationFileNameBase);
            subSpriteAnimationName += symbol.name + ".hanims";
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
            this->buildRuntimeAnimationData(false,
                                            elementIter->loopMode,
                                            elementIter->frameOffset,
                                            frameIndex - elementIter->startDisplayFrameIndex,
                                            matrix,
                                            cacheItem,
                                            sheet,
                                            subAnimation,
                                            animationFileNameBase,
                                            min_X, max_X, min_Y, max_Y,
                                            alpha_percent, alpha_amount,
                                            red_percent, red_amount,
                                            green_percent, green_amount,
                                            blue_percent, blue_amount);
        } // nesting sprite element END
        
        layerZOrder++;
    } // element layers END
//        if (quadsCount > 0) { // element display list
//            CCHierarchiesSprite::DisplayElement displayElement;
//            displayElement.quadsIndex = quadsIndex;
//            displayElement.quadsCount = quadsCount;
//            displayList.push_back(displayElement);
//            
//            quadsIndex += quadsCount;
//            quadsCount = 0;
//        }
    
    // update bounding box, content size and anchor point
    CCRect bbox;
    if (min_X == 10000 ||
        max_X == -10000 ||
        min_Y == 10000 ||
        max_Y == -10000) {
        // this frame is whole empty
        bbox = CCRectZero;
    }
    else {
        bbox = CCRectMake(min_X, min_Y, max_X - min_X, max_Y - min_Y);
    }
    
    // insert animation frame data to cache
    size_t quadsToDraw = quads.size();
    size_t oldTotalQuads = mesh->getTotalQuads();
    if ((oldTotalQuads + quadsToDraw) > mesh->getCapacity())
        mesh->resizeCapacity(oldTotalQuads + quadsToDraw);
    if (quadsToDraw > 0)
        mesh->insertQuads(&quads[0], oldTotalQuads, quadsToDraw);
    
    if (isRoot) {
        CCHierarchiesSprite::DisplayElement displayElement;
        displayElement.quadsIndex = 0;
        displayElement.quadsCount = mesh->getTotalQuads() - frameStartQuadIndex;
        displayList.push_back(displayElement);
        
        cacheItem->frameCache[frameIndex].frameStartQuadIndex = frameStartQuadIndex;
        cacheItem->frameCache[frameIndex].displayList = displayList;
        cacheItem->frameCache[frameIndex].bbox = bbox;
    }
}


#pragma mark - resource manage

void CCHierarchiesSpriteRuntime::initializationRuntime () {
    // proload shader program
    CCGLProgram* glProgram = CCShaderCache::sharedShaderCache()->programForKey(kCCShader_HierarchiesSpriteNonFilter);
    if (glProgram == NULL) {
        glProgram = new CCGLProgram();
        glProgram->initWithVertexShaderByteArray(ccShader_HierarchiesSpriteNonFilter_vert,
                                                 ccShader_HierarchiesSpriteNonFilter_frag);
        
        glProgram->addAttribute(kCCAttributeNamePosition, kCCVertexAttrib_Position);
        glProgram->addAttribute("a_colorMul", kCCHierarchiesSprite_VertexAttrib_Color_Mul);
        glProgram->addAttribute("a_colorAdd", kCCHierarchiesSprite_VertexAttrib_Color_Add);
        glProgram->addAttribute(kCCAttributeNameTexCoord, kCCVertexAttrib_TexCoords);
        
        glProgram->link();
        glProgram->updateUniforms();
        
        CHECK_GL_ERROR_DEBUG();
        
        CCShaderCache::sharedShaderCache()->addProgram(glProgram, kCCShader_HierarchiesSpriteNonFilter);
        CC_SAFE_RELEASE_NULL(glProgram);
    }
    
    glProgram = CCShaderCache::sharedShaderCache()->programForKey(kCCShader_HierarchiesSpriteNonFilter_Premultiplied);
    if (glProgram == NULL) {
        glProgram = new CCGLProgram();
        glProgram->initWithVertexShaderByteArray(ccShader_HierarchiesSpriteNonFilter_vert,
                                                 ccShader_HierarchiesSpriteNonFilter_PremultipliedAlpha_frag);
        
        glProgram->addAttribute(kCCAttributeNamePosition, kCCVertexAttrib_Position);
        glProgram->addAttribute("a_colorMul", kCCHierarchiesSprite_VertexAttrib_Color_Mul);
        glProgram->addAttribute("a_colorAdd", kCCHierarchiesSprite_VertexAttrib_Color_Add);
        glProgram->addAttribute(kCCAttributeNameTexCoord, kCCVertexAttrib_TexCoords);
        
        glProgram->link();
        glProgram->updateUniforms();
        
        CHECK_GL_ERROR_DEBUG();
        
        CCShaderCache::sharedShaderCache()->addProgram(glProgram, kCCShader_HierarchiesSpriteNonFilter_Premultiplied);
        CC_SAFE_RELEASE_NULL(glProgram);
    }
}

void CCHierarchiesSpriteRuntime::releaseUnusedResource () {
    this->removeUnusedHierarchiesSprite();
    CCHierarchiesSpriteAnimationCache::sharedHierarchiesSpriteAnimationCache()->removeUnusedAnimation();
    CCHierarchiesSpriteSheetCache::sharedHierarchiesSpriteSheetCache()->removeUnusedSpriteSheet();
}

NS_CC_EXT_END
