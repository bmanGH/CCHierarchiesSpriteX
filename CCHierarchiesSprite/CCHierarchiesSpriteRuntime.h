//
//  CCHierarchiesSpriteRuntime.h
//  HierarchiesSpriteDemo
//
//  Created by xuxiaocheng on 10/23/14.
//
//

#ifndef __HierarchiesSpriteDemo__CCHierarchiesSpriteRuntime__
#define __HierarchiesSpriteDemo__CCHierarchiesSpriteRuntime__

#include "cocos2d.h"
#include "ExtensionMacros.h"
#include <vector>
#include "CCHierarchiesSpriteBase.h"
#include "CCHierarchiesSpriteSheet.h"
#include "CCHierarchiesSpriteAnimation.h"

NS_CC_EXT_BEGIN


#define HIERARCHIES_SPRITE_RUNTIME_VERSION "2.0"

class CCHierarchiesSprite;

class CC_DLL CCHierarchiesSpriteRuntime : Ref {
	
public:
	struct FrameCacheItem {
        unsigned int frameStartQuadIndex;
        std::vector<CCHierarchiesSpriteBase::DisplayElement> displayList;
		Rect bbox;
		
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
    
    // static animation data cache
    void cacheStaticAnimationData (CCHierarchiesSprite* sprite, const AvatarMapType& avatarMap);
	void removeStaticAnimationData (CCHierarchiesSprite* sprite);
    void removeUnusedStaticAnimationData ();
    AnimationCacheHashItem* getCacheItem (CCHierarchiesSprite* sprite);
    
    // processing all frame data to static cache data
    void buildStaticAnimationData (bool isRoot,
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
                                   const AvatarMapType& avatarMap);
	
public:
	static CCHierarchiesSpriteRuntime* sharedHierarchiesSpriteRuntime ();
	static void purgeHierarchiesSpriteRuntime ();
	
	virtual ~CCHierarchiesSpriteRuntime ();
	bool init ();
    
	const char* description ();
    
    // resource manage
    void initializationRuntime ();
    void releaseUnusedResource ();
    
    friend class CCHierarchiesSprite;
	
};


NS_CC_EXT_END

#endif /* defined(__HierarchiesSpriteDemo__CCHierarchiesSpriteRuntime__) */
