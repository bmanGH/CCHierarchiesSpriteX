//
//  CCHierarchiesSpriteAnimationCache.h
//  CCHierarchiesSprite
//
//  Created by bman <zx123xz321hm3@hotmail.com>.
//  Copyright (c) 2013. All rights reserved.
//

#ifndef _CCHierarchiesSpriteAnimationCache_H_
#define _CCHierarchiesSpriteAnimationCache_H_

#include "cocos2d.h"
#include "ExtensionMacros.h"
#include "CCHierarchiesSpriteConfig.h"
#include "CCHierarchiesSpriteAnimation.h"


NS_CC_EXT_BEGIN

class CC_DLL CCHierarchiesSpriteAnimationCache : public CCObject {
	
public:
	struct SpriteAnimationCacheHashItem {
        char* name;
        int retainCount;
        CCHierarchiesSpriteAnimation* animation;
        UT_hash_handle hh;
        
        SpriteAnimationCacheHashItem () {}
        ~SpriteAnimationCacheHashItem () {
            if (name != NULL)
                free(name);
            if (animation != NULL)
                delete animation;
        }
    };
	
protected:
    SpriteAnimationCacheHashItem* _animationCache;
	
public:
	static CCHierarchiesSpriteAnimationCache* sharedHierarchiesSpriteAnimationCache ();
	static void purgeHierarchiesSpriteAnimationCache ();
	
	virtual ~CCHierarchiesSpriteAnimationCache ();
	bool init ();
	
	CCHierarchiesSpriteAnimation* addAnimation (const char* name);
	CCHierarchiesSpriteAnimation* getAnimation (const char* name);
	void removeAnimation (const char* name);
    void removeUnusedAnimation ();
	
	const char* description ();
	
};

NS_CC_EXT_END

#endif
