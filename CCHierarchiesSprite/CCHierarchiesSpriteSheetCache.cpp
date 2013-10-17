//
//  CCHierarchiesSpriteSheetCache.m
//  CCHierarchiesSprite
//
//  Created by bman <zx123xz321hm3@hotmail.com>.
//  Copyright (c) 2013. All rights reserved.
//

#include "CCHierarchiesSpriteSheetCache.h"
#include <sstream>


NS_CC_EXT_BEGIN

static CCHierarchiesSpriteSheetCache* g_sharedHierarchiesSpriteSheetCache = NULL;

CCHierarchiesSpriteSheetCache* CCHierarchiesSpriteSheetCache::sharedHierarchiesSpriteSheetCache () {
    if (!g_sharedHierarchiesSpriteSheetCache) {
        g_sharedHierarchiesSpriteSheetCache = new CCHierarchiesSpriteSheetCache();
        if (!g_sharedHierarchiesSpriteSheetCache->init()) {
            CC_SAFE_DELETE(g_sharedHierarchiesSpriteSheetCache);
        }
    }
    return g_sharedHierarchiesSpriteSheetCache;
}

void CCHierarchiesSpriteSheetCache::purgeHierarchiesSpriteSheetCache () {
    CC_SAFE_RELEASE_NULL(g_sharedHierarchiesSpriteSheetCache);
}

CCHierarchiesSpriteSheetCache::~CCHierarchiesSpriteSheetCache () {
    SheetCacheHashItem* hashItem, *tmp;
    HASH_ITER(hh, _sheetCache, hashItem, tmp) {
        HASH_DEL(_sheetCache, hashItem);
        delete hashItem;
    }
}

bool CCHierarchiesSpriteSheetCache::init () {
    _sheetCache = NULL;
    return true;
}

CCHierarchiesSpriteSheet* CCHierarchiesSpriteSheetCache::addSpriteSheet (const char* name) {
    SheetCacheHashItem* hashItem = NULL;
    HASH_FIND_STR(_sheetCache, name, hashItem);
    if (!hashItem) {
        CCHierarchiesSpriteSheet* sheet = new CCHierarchiesSpriteSheet(name);
        SheetCacheHashItem* newCache = new SheetCacheHashItem();
        int keyLen = strlen(name);
        newCache->name = (char*)calloc(keyLen + 1, sizeof(char)); // add 1 char for '\0'
        strcpy(newCache->name, name);
        newCache->retainCount = 2; // + 1 for cache
        newCache->spriteSheet = sheet;
        HASH_ADD_KEYPTR(hh, _sheetCache, newCache->name, keyLen, newCache);
        return sheet;
    }
    else {
        hashItem->retainCount++;
        return hashItem->spriteSheet;
    }
}

CCHierarchiesSpriteSheet* CCHierarchiesSpriteSheetCache::getSpriteSheet (const char* name) {
    SheetCacheHashItem* hashItem = NULL;
    HASH_FIND_STR(_sheetCache, name, hashItem);
    if (hashItem) {
        return hashItem->spriteSheet;
    }
    return NULL;
}

void CCHierarchiesSpriteSheetCache::removeSpriteSheet (const char* name) {
    SheetCacheHashItem* hashItem = NULL;
    HASH_FIND_STR(_sheetCache, name, hashItem);
    if (hashItem) {
        hashItem->retainCount--;
        if (hashItem->retainCount <= 0) {
            HASH_DEL(_sheetCache, hashItem);
            delete hashItem;
        }
    }
}

void CCHierarchiesSpriteSheetCache::removeUnusedSpriteSheet () {
    SheetCacheHashItem* hashItem, *tmp;
    HASH_ITER(hh, _sheetCache, hashItem, tmp) {
        if (hashItem->retainCount == 1) {
            HASH_DEL(_sheetCache, hashItem);
            delete hashItem;
        }
    }
}

const char* CCHierarchiesSpriteSheetCache::description () {
    std::stringstream info;
    info << "CCHierarchiesSpriteSheetCache:\n";
    info << "{\n";
    SheetCacheHashItem* hashItem, *tmp;
    HASH_ITER(hh, _sheetCache, hashItem, tmp) {
        info << "\t[" << hashItem->name << " : " << hashItem->retainCount << "]\n";
    }
    info << "}\n";
	return CCString::create(info.str().c_str())->getCString();
}

NS_CC_EXT_END
