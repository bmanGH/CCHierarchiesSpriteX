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
#include <string>
#include <vector>
#include "CCHierarchiesSpriteSheet.h"
#include "CCHierarchiesSpriteAnimation.h"

NS_CC_EXT_BEGIN


#define HIERARCHIES_SPRITE_RUNTIME_VERSION "2.0"

class CC_DLL HierarchiesSpriteRuntime : Ref {
    
public:
    struct SheetCacheHashItem {
        std::string name;
        int retainCount;
        HierarchiesSpriteSheet* spriteSheet;
        UT_hash_handle hh;
        
        SheetCacheHashItem ()
        : retainCount(0), spriteSheet(nullptr) {}
        ~SheetCacheHashItem () {
            if (spriteSheet != nullptr)
                delete spriteSheet;
        }
    };
    
    struct AnimationCacheHashItem {
        std::string name;
        int retainCount;
        HierarchiesSpriteAnimation* animation;
        UT_hash_handle hh;
        
        AnimationCacheHashItem ()
        : retainCount(0), animation(nullptr) {}
        ~AnimationCacheHashItem () {
            if (animation != nullptr)
                delete animation;
        }
    };
	
protected:
    // data cache
    SheetCacheHashItem* _sheetCache;
    AnimationCacheHashItem* _animationCache;
    
CC_CONSTRUCTOR_ACCESS:
    HierarchiesSpriteRuntime ();
    virtual ~HierarchiesSpriteRuntime ();
    
	bool init ();
	
public:
    static HierarchiesSpriteRuntime* getInstance ();
	static void purge ();
	
    std::string description ();
    
    // resource manage
    HierarchiesSpriteSheet* addSheet (const std::string& name);
    HierarchiesSpriteSheet* getSheet (const std::string& name);
    void removeSheet (const std::string& name);
    
    HierarchiesSpriteAnimation* addAnimation (const std::string& name);
	HierarchiesSpriteAnimation* getAnimation (const std::string& name);
	void removeAnimation (const std::string& name);
    
    void releaseUnusedResource ();
	
};


NS_CC_EXT_END

#endif /* defined(__HierarchiesSpriteDemo__CCHierarchiesSpriteRuntime__) */
