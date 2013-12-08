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
#include <algorithm>
#include "CCHierarchiesSpriteAnimationCache.h"
#include "CCHierarchiesSpriteSheetCache.h"
#include "CCHierarchiesSpriteShader.h"
#include "CCHierarchiesSprite.h"

NS_CC_EXT_BEGIN


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

void CCHierarchiesSpriteRuntime::cacheStaticAnimationData (CCHierarchiesSprite* sprite,
                                                           const AvatarMapType& avatarMap) {
    std::string cacheKey = sprite->getAnimationName();
    
    // calc cache key by avatar map string hash code
    // !!! avatar map string different order, surplus key-value will spawn duplicate cache
    AvatarMapType::const_iterator avatarMapIter;
    std::string avatarMapString;
    for (avatarMapIter = avatarMap.cbegin(); avatarMapIter != avatarMap.cend(); avatarMapIter++) {
        avatarMapString += avatarMapIter->second + ";";
    }
    cacheKey += "+" + std::to_string(std::hash<std::string>()(avatarMapString));
    
	AnimationCacheHashItem* hashItem = NULL;
	HASH_FIND_STR(_animationCache, cacheKey.c_str(), hashItem);
	if (!hashItem) {
		AnimationCacheHashItem* newCache = new AnimationCacheHashItem(sprite->getAnimation()->getFrameCount() * 20);
		int keyLen = strlen(cacheKey.c_str());
		newCache->name = (char*)calloc(keyLen + 1, sizeof(char)); // add 1 for char '\0'
		strcpy(newCache->name, cacheKey.c_str());
		newCache->retainCount = 2; // + 1 for cache
		HASH_ADD_KEYPTR(hh, _animationCache, newCache->name, keyLen, newCache);
        
        for (unsigned int frameIndex = 0; frameIndex < sprite->getAnimation()->getFrameCount(); frameIndex++) {
            float min_X = 10000, max_X = -10000, min_Y = 10000, max_Y = -10000;
            this->buildStaticAnimationData(true,
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
                                            1, 0,
                                            avatarMap);
        }
        newCache->sharedMesh->flushAllQuadsToBuffer();
	}
	else {
		hashItem->retainCount++;
	}
    
    sprite->_cacheKey = cacheKey;
}

void CCHierarchiesSpriteRuntime::removeStaticAnimationData (CCHierarchiesSprite* sprite) {
	AnimationCacheHashItem* hashItem = NULL;
    HASH_FIND_STR(_animationCache, sprite->_cacheKey.c_str(), hashItem);
    if (hashItem) {
        hashItem->retainCount--;
        if (hashItem->retainCount <= 0) {
            HASH_DEL(_animationCache, hashItem);
            delete hashItem;
        }
    }
}

void CCHierarchiesSpriteRuntime::removeUnusedStaticAnimationData() {
    AnimationCacheHashItem* hashItem, *tmp;
    HASH_ITER(hh, _animationCache, hashItem, tmp) {
        if (hashItem->retainCount == 1) {
            HASH_DEL(_animationCache, hashItem);
            delete hashItem;
        }
    }
}

CCHierarchiesSpriteRuntime::AnimationCacheHashItem* CCHierarchiesSpriteRuntime::getCacheItem (CCHierarchiesSprite* sprite) {
    CCHierarchiesSpriteRuntime::AnimationCacheHashItem* ret = NULL;
	HASH_FIND_STR(_animationCache, sprite->_cacheKey.c_str(), ret);
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

void CCHierarchiesSpriteRuntime::buildStaticAnimationData (bool isRoot,
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
                                                            const float parent_blue_percent, const int parent_blue_amount,
                                                            const AvatarMapType& avatarMap) {
    CCHierarchiesSpriteAnimation::FrameElements frameElements;
    int eNum = animation->getFrameElementsAtFrameIndex(loopMode, frameOffset, frameIndex, frameElements); //!!! frameIndex will update with loop mode
    
    CCHierarchiesSpriteMesh* mesh = cacheItem->sharedMesh;
    size_t frameStartQuadIndex = mesh->getTotalQuads();
    
    std::vector<CCHierarchiesSprite_V3F_C4B_T2F_Quad> quads(eNum);
    quads.clear();
    std::vector<CCHierarchiesSprite::DisplayElement> displayList;
    
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
            result = sheet->getSpr(symbol.name.c_str(), spr);
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
            if (avatarMap.count(symbol.name) > 0) { // with avatar support
                subSpriteAnimationName += avatarMap.at(symbol.name) + ".hanims";
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
            this->buildStaticAnimationData(false,
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
                                            blue_percent, blue_amount,
                                            avatarMap);
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
    this->removeUnusedStaticAnimationData();
    CCHierarchiesSpriteAnimationCache::sharedHierarchiesSpriteAnimationCache()->removeUnusedAnimation();
    CCHierarchiesSpriteSheetCache::sharedHierarchiesSpriteSheetCache()->removeUnusedSpriteSheet();
}

NS_CC_EXT_END
