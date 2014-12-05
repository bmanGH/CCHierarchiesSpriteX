//
//  CCHierarchiesSpriteRuntime.cpp
//  HierarchiesSpriteDemo
//
//  Created by xuxiaocheng on 10/23/14.
//
//

#include "CCHierarchiesSpriteRuntime.h"
#include <sstream>
#include <algorithm>
#include "CCHierarchiesSpriteReader.h"
#include "CCHierarchiesSpriteShader.h"

NS_CC_EXT_BEGIN


static HierarchiesSpriteRuntime* g_hierarchiesSpriteRuntime = nullptr;

HierarchiesSpriteRuntime* HierarchiesSpriteRuntime::getInstance ()
{
    if (!g_hierarchiesSpriteRuntime) {
        g_hierarchiesSpriteRuntime = new (std::nothrow) HierarchiesSpriteRuntime();
        if (!g_hierarchiesSpriteRuntime->init())
        {
            CC_SAFE_DELETE(g_hierarchiesSpriteRuntime);
            g_hierarchiesSpriteRuntime = nullptr;
        }
    }
    return g_hierarchiesSpriteRuntime;
}

void HierarchiesSpriteRuntime::purge ()
{
	CC_SAFE_RELEASE_NULL(g_hierarchiesSpriteRuntime);
}

HierarchiesSpriteRuntime::HierarchiesSpriteRuntime ()
: _sheetCache(nullptr)
, _animationCache(nullptr)
, _rendererRecreatedListener(nullptr)
{
}

HierarchiesSpriteRuntime::~HierarchiesSpriteRuntime ()
{
    // remove sheet cache
    {
        SheetCacheHashItem* hashItem, *tmp;
        HASH_ITER(hh, _sheetCache, hashItem, tmp) {
            HASH_DEL(_sheetCache, hashItem);
            delete hashItem;
        }
    }
    
    // remove animation cache
    {
        AnimationCacheHashItem* hashItem, *tmp;
        HASH_ITER(hh, _animationCache, hashItem, tmp) {
            HASH_DEL(_animationCache, hashItem);
            delete hashItem;
        }
    }
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_WP8 || CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
    if (_rendererRecreatedListener)
        Director::getInstance()->getEventDispatcher()->removeEventListener(_rendererRecreatedListener);
#endif
}

bool HierarchiesSpriteRuntime::init () {
	// proload shader program
    GLProgram* glProgram = GLProgramCache::getInstance()->getGLProgram(kShader_Name_HierarchiesSprite);
    if (glProgram == nullptr) {
        glProgram = new (std::nothrow) GLProgram();
        glProgram->initWithByteArrays(ccShader_HierarchiesSprite_vert,
                                      ccShader_HierarchiesSprite_frag);
        glProgram->bindAttribLocation(kHierarchiesSprite_GLProgram_Attribute_Name_ColorMul, kHierarchiesSprite_GLProgram_Attribute_ColorMul);
        glProgram->bindAttribLocation(kHierarchiesSprite_GLProgram_Attribute_Name_ColorAdd, kHierarchiesSprite_GLProgram_Attribute_ColorAdd);
        
        glProgram->link();
        glProgram->updateUniforms();
        
        CHECK_GL_ERROR_DEBUG();
        
        GLProgramCache::getInstance()->addGLProgram(glProgram, kShader_Name_HierarchiesSprite);
        CC_SAFE_RELEASE_NULL(glProgram);
    }
    
    glProgram = GLProgramCache::getInstance()->getGLProgram(kShader_Name_HierarchiesSprite_Premultiplied);
    if (glProgram == nullptr) {
        glProgram = new (std::nothrow) GLProgram();
        glProgram->initWithByteArrays(ccShader_HierarchiesSprite_vert,
                                      ccShader_HierarchiesSprite_PremultipliedAlpha_frag);
        glProgram->bindAttribLocation(kHierarchiesSprite_GLProgram_Attribute_Name_ColorMul, kHierarchiesSprite_GLProgram_Attribute_ColorMul);
        glProgram->bindAttribLocation(kHierarchiesSprite_GLProgram_Attribute_Name_ColorAdd, kHierarchiesSprite_GLProgram_Attribute_ColorAdd);
        
        glProgram->link();
        glProgram->updateUniforms();
        
        CHECK_GL_ERROR_DEBUG();
        
        GLProgramCache::getInstance()->addGLProgram(glProgram, kShader_Name_HierarchiesSprite_Premultiplied);
        CC_SAFE_RELEASE_NULL(glProgram);
    }
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_WP8 || CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
    /** listen the event that renderer was recreated on Android/WP8 */
    if (_rendererRecreatedListener == nullptr)
    {
        _rendererRecreatedListener = EventListenerCustom::create(EVENT_RENDERER_RECREATED, [] (EventCustom* e) {
            GLProgram* glProgram = GLProgramCache::getInstance()->getGLProgram(kShader_Name_HierarchiesSprite);
            glProgram->reset();
            glProgram->initWithByteArrays(ccShader_HierarchiesSprite_vert,
                                          ccShader_HierarchiesSprite_frag);
            glProgram->bindAttribLocation(kHierarchiesSprite_GLProgram_Attribute_Name_ColorMul, kHierarchiesSprite_GLProgram_Attribute_ColorMul);
            glProgram->bindAttribLocation(kHierarchiesSprite_GLProgram_Attribute_Name_ColorAdd, kHierarchiesSprite_GLProgram_Attribute_ColorAdd);
            glProgram->link();
            glProgram->updateUniforms();
            
            glProgram = GLProgramCache::getInstance()->getGLProgram(kShader_Name_HierarchiesSprite_Premultiplied);
            glProgram->reset();
            glProgram->initWithByteArrays(ccShader_HierarchiesSprite_vert,
                                          ccShader_HierarchiesSprite_PremultipliedAlpha_frag);
            glProgram->bindAttribLocation(kHierarchiesSprite_GLProgram_Attribute_Name_ColorMul, kHierarchiesSprite_GLProgram_Attribute_ColorMul);
            glProgram->bindAttribLocation(kHierarchiesSprite_GLProgram_Attribute_Name_ColorAdd, kHierarchiesSprite_GLProgram_Attribute_ColorAdd);
            glProgram->link();
            glProgram->updateUniforms();
        });
        Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(_rendererRecreatedListener, -1);
    }
#endif
    
    return true;
}

std::string HierarchiesSpriteRuntime::description () {
	std::stringstream info;
    info << "HierarchiesSpriteRuntime:\n";
    
    info << "sheet: {\n";
    {
        SheetCacheHashItem* hashItem, *tmp;
        HASH_ITER(hh, _sheetCache, hashItem, tmp) {
            info << "    [" << hashItem->name << " : " << hashItem->retainCount << "]\n";
        }
    }
    info << "}\n";
    
    info << "animation: {\n";
    {
        AnimationCacheHashItem* hashItem, *tmp;
        HASH_ITER(hh, _animationCache, hashItem, tmp) {
            info << "    [" << hashItem->name << " : " << hashItem->retainCount << "]\n";
        }
    }
    info << "}\n";
	
	return info.str();
}


#pragma mark - resource manage

HierarchiesSpriteSheet* HierarchiesSpriteRuntime::addSheet (const std::string& name)
{
    SheetCacheHashItem* hashItem = nullptr;
    HASH_FIND_STR(_sheetCache, name.c_str(), hashItem);
    if (!hashItem) {
        HierarchiesSpriteSheet* ret = new (std::nothrow) HierarchiesSpriteSheet();
        if (ret)
        {
            HierarchiesSpriteSheetReader reader(name);
            if (reader.parse(*ret))
            {
                SheetCacheHashItem* newCache = new SheetCacheHashItem();
                newCache->name = name;
                newCache->retainCount = 2; // + 1 for cache
                newCache->spriteSheet = ret;
                HASH_ADD_KEYPTR(hh, _sheetCache, newCache->name.c_str(), newCache->name.length(), newCache);
            }
            else
            {
                delete ret;
                ret = nullptr;
            }
        }
        return ret;
    }
    else {
        hashItem->retainCount++;
        return hashItem->spriteSheet;
    }
}

HierarchiesSpriteSheet* HierarchiesSpriteRuntime::getSheet (const std::string& name)
{
    SheetCacheHashItem* hashItem = nullptr;
    HASH_FIND_STR(_sheetCache, name.c_str(), hashItem);
    if (hashItem) {
        return hashItem->spriteSheet;
    }
    return nullptr;
}

void HierarchiesSpriteRuntime::removeSheet (const std::string& name)
{
    SheetCacheHashItem* hashItem = nullptr;
    HASH_FIND_STR(_sheetCache, name.c_str(), hashItem);
    if (hashItem) {
        hashItem->retainCount--;
        if (hashItem->retainCount <= 0) {
            HASH_DEL(_sheetCache, hashItem);
            delete hashItem;
        }
    }
}

HierarchiesSpriteAnimation* HierarchiesSpriteRuntime::addAnimation (const std::string& name)
{
    AnimationCacheHashItem* hashItem = nullptr;
    HASH_FIND_STR(_animationCache, name.c_str(), hashItem);
    if (!hashItem) {
        HierarchiesSpriteAnimation* ret = new (std::nothrow) HierarchiesSpriteAnimation();
        if (ret)
        {
            HierarchiesSpriteAnimationReader reader(name);
            if (reader.parse(*ret))
            {
                AnimationCacheHashItem* newCache = new AnimationCacheHashItem();
                newCache->name = name;
                newCache->retainCount = 2; // + 1 for cache
                newCache->animation = ret;
                HASH_ADD_KEYPTR(hh, _animationCache, newCache->name.c_str(), newCache->name.length(), newCache);
            }
            else
            {
                delete ret;
                ret = nullptr;
            }
        }
        return ret;
    }
    else {
        hashItem->retainCount++;
        return hashItem->animation;
    }
}

HierarchiesSpriteAnimation* HierarchiesSpriteRuntime::getAnimation (const std::string& name)
{
    AnimationCacheHashItem* hashItem = nullptr;
    HASH_FIND_STR(_animationCache, name.c_str(), hashItem);
    if (hashItem) {
        return hashItem->animation;
    }
    return nullptr;
}

void HierarchiesSpriteRuntime::removeAnimation (const std::string& name)
{
    AnimationCacheHashItem* hashItem = nullptr;
    HASH_FIND_STR(_animationCache, name.c_str(), hashItem);
    if (hashItem) {
        hashItem->retainCount--;
        if (hashItem->retainCount <= 0) {
            HASH_DEL(_animationCache, hashItem);
            delete hashItem;
        }
    }
}

void HierarchiesSpriteRuntime::releaseUnusedResource ()
{
    // remove unused sheet
    {
        SheetCacheHashItem* hashItem, *tmp;
        HASH_ITER(hh, _sheetCache, hashItem, tmp) {
            if (hashItem->retainCount == 1) {
                HASH_DEL(_sheetCache, hashItem);
                delete hashItem;
            }
        }
    }
    
    // remove unused animation
    {
        AnimationCacheHashItem* hashItem, *tmp;
        HASH_ITER(hh, _animationCache, hashItem, tmp) {
            if (hashItem->retainCount == 1) {
                HASH_DEL(_animationCache, hashItem);
                delete hashItem;
            }
        }
    }
}


NS_CC_EXT_END
