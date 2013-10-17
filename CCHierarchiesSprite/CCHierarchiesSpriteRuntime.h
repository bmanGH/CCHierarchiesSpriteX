//
//  CCHierarchiesSpriteRuntime.h
//  CCHierarchiesSprite
//
//  Created by bman <zx123xz321hm3@hotmail.com>.
//  Copyright (c) 2013. All rights reserved.
//

#ifndef _CCHierarchiesSpriteRuntime_H_
#define _CCHierarchiesSpriteRuntime_H_

#include "cocos2d.h"
#include "ExtensionMacros.h"
#include "CCHierarchiesSpriteConfig.h"
#include <vector>
#include "CCHierarchiesSprite.h"
#include "CCHierarchiesSpriteMesh.h"
#include "CCHierarchiesSpriteSheet.h"
#include "CCHierarchiesSpriteAnimation.h"

NS_CC_EXT_BEGIN


class CC_DLL CCHierarchiesSpriteRuntime : CCObject {
	
public:
	struct FrameCacheItem {
        unsigned int frameStartQuadIndex;
        std::vector<CCHierarchiesSprite::DisplayElement> displayList;
		CCRect bbox;
		
		FrameCacheItem () {}
        
        FrameCacheItem (const FrameCacheItem& copy) {
            this->displayList = copy.displayList;
            this->bbox = copy.bbox;
        }
        
        FrameCacheItem& operator= (const FrameCacheItem& rhs) {
            this->displayList = rhs.displayList;
            this->bbox = rhs.bbox;
            return *this;
        }
	};
	
	struct AnimationCacheHashItem {
		char* name;
		std::vector<FrameCacheItem> frameCache;
        CCHierarchiesSpriteMesh* sharedMesh;
		int retainCount;
		UT_hash_handle hh;
		
		AnimationCacheHashItem () : name(NULL), retainCount(0), sharedMesh(NULL) {}

		AnimationCacheHashItem (size_t n)
		: frameCache(n), name(NULL), retainCount(0) {
            sharedMesh = new CCHierarchiesSpriteMesh();
            sharedMesh->initWithCapacity(n);
		}
        
		~AnimationCacheHashItem () {
			if (name != NULL)
				free(name);
            sharedMesh->release();
		}
	};
	
protected:
	AnimationCacheHashItem* _animationCache;
	
public:
	static CCHierarchiesSpriteRuntime* sharedHierarchiesSpriteRuntime ();
	static void purgeHierarchiesSpriteRuntime ();
	
	virtual ~CCHierarchiesSpriteRuntime ();
	bool init ();
	
	void insertHierarchiesSprite (CCHierarchiesSprite* sprite);
	void removeHierarchiesSprite (CCHierarchiesSprite* sprite);
    void removeUnusedHierarchiesSprite ();
    
    AnimationCacheHashItem* getCacheItem (const char* itemName);
	
	const char* description ();
    
    // processing all frame data to runtime cache data
    void buildRuntimeAnimationData (bool isRoot,
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
                                    const float parent_blue_percent, const int parent_blue_amount);
        
    // resource manage
    void initializationRuntime ();
    void releaseUnusedResource ();
	
};

NS_CC_EXT_END

#endif
