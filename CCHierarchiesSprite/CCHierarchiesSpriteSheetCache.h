//
//  CCHierarchiesSpriteSheetCache.h
//  CCHierarchiesSprite
//
//  Created by bman <zx123xz321hm3@hotmail.com>.
//  Copyright (c) 2013. All rights reserved.
//

#ifndef _CCHierarchiesSpriteSheetCache_H_
#define _CCHierarchiesSpriteSheetCache_H_

#include "cocos2d.h"
#include "ExtensionMacros.h"
#include "CCHierarchiesSpriteConfig.h"
#include "CCHierarchiesSpriteSheet.h"

NS_CC_EXT_BEGIN

class CC_DLL CCHierarchiesSpriteSheetCache : CCObject {
    
public:
    struct SheetCacheHashItem {
        char* name;
        int retainCount;
        CCHierarchiesSpriteSheet* spriteSheet;
        UT_hash_handle hh;
        
        SheetCacheHashItem () {}
        ~SheetCacheHashItem () {
            if (name != NULL)
                free(name);
            if (spriteSheet != NULL)
                delete spriteSheet;
        }
    };
    
protected:
    SheetCacheHashItem* _sheetCache;
    
public:
    static CCHierarchiesSpriteSheetCache* sharedHierarchiesSpriteSheetCache ();
    static void purgeHierarchiesSpriteSheetCache ();
    
    virtual ~CCHierarchiesSpriteSheetCache ();
    bool init ();
    
    CCHierarchiesSpriteSheet* addSpriteSheet (const char* name);
    CCHierarchiesSpriteSheet* getSpriteSheet (const char* name);
    void removeSpriteSheet (const char* name);
    void removeUnusedSpriteSheet ();
    
    const char* description ();
    
};

NS_CC_EXT_END

#endif
