//
//  CCHierarchiesSprite换装单元测试场景
//  CCHierarchiesSpriteXDemo
//
//  Created by bman <zx123xz321hm3@hotmail.com>.
//  Copyright (c) 2013. All rights reserved.
//
//

#include "CCHierarchiesSpriteAvatarTestScene.h"
#include "CCHierarchiesSpriteAction.h"
#include "CCHierarchiesSpriteRuntime.h"
#include "json/json.h"
#include <fstream>

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#   include <mach/mach.h>
#endif

USING_NS_CC;
USING_NS_CC_EXT;

static void report_memory (void) {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    struct task_basic_info info;
    mach_msg_type_number_t size = sizeof(info);
    kern_return_t kerr = task_info(mach_task_self(),
                                   TASK_BASIC_INFO,
                                   (task_info_t)&info,
                                   &size);
    if( kerr == KERN_SUCCESS ) {
        CCLOG("Memory in use %u : %uKB : %uMB", info.resident_size, info.resident_size / 1024, info.resident_size / 1024 / 1024);
        CCLOG("VM in use %u : %uKB : %uMB", info.virtual_size, info.virtual_size / 1024, info.virtual_size / 1024 / 1024);
    } else {
        CCLOG("Error with task_info(): %s", mach_error_string(kerr));
    }
#endif
}

CCScene* CCHierarchiesSpriteAvatarTestScene::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    CCHierarchiesSpriteAvatarTestScene *layer = CCHierarchiesSpriteAvatarTestScene::create();
    
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}

CCHierarchiesSpriteAvatarTestScene::~CCHierarchiesSpriteAvatarTestScene () {
    CC_SAFE_RELEASE(_hSprites);
}

bool CCHierarchiesSpriteAvatarTestScene::init()
{
    if ( !CCLayer::init() )
    {
        return false;
    }
    
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    
    this->setTouchEnabled(true);
    
    this->scheduleUpdate();
    
    _hSprites = new CCArray();
    _hSprites->init();
    
//    CCDirector::sharedDirector()->getScheduler()->setTimeScale(0.5);
    
    CCLayerColor* bgLayer = CCLayerColor::create(ccc4(64, 64, 64, 255));
    this->addChild(bgLayer, -1000);
    
    // load avatar map config data
    Json::Reader reader;
    std::ifstream jsonFile;
    Json::Value data;
    
    jsonFile.open(CCFileUtils::sharedFileUtils()->fullPathForFilename("avatar_data/guanyu-avatar.json"));
    if (!jsonFile.is_open() ||
        !reader.parse(jsonFile, data, false))
    {
        CCAssert(false, "load avatar map config data error");
    }
    jsonFile.close();
    
    // JSON:
    //    {
    //        "avatar name" : {
    //            "item name" : "maped item name",
    //            ...
    //        },
    //        ...
    //    }
    Json::Value::Members::const_iterator avatarNameIter;
    Json::Value::Members avatarNames = data.getMemberNames();
    for (avatarNameIter = avatarNames.cbegin(); avatarNameIter != avatarNames.cend(); avatarNameIter++) {
        
        Json::Value::Members::const_iterator itemNameIter;
        Json::Value::Members itemNames = data[*avatarNameIter].getMemberNames();
        AvatarMapType avatarMap;
        for (itemNameIter = itemNames.cbegin(); itemNameIter != itemNames.cend(); itemNameIter++) {
            avatarMap[*itemNameIter] = data[*avatarNameIter][*itemNameIter].asString();
        }
        
        _avatarMap[*avatarNameIter] = avatarMap;
    }
    
    this->loadSprites();
    
    return true;
}


#pragma mark - Update

void CCHierarchiesSpriteAvatarTestScene::update (float delta) {
    
}


#pragma mark - Draw

void CCHierarchiesSpriteAvatarTestScene::draw () {
    
}


#pragma mark - Touch callback

void CCHierarchiesSpriteAvatarTestScene::ccTouchesBegan(CCSet *pTouches, CCEvent *pEvent) {
//    // 资源管理测试
//    if (_hSprites->count() > 0) {
//        CCLOG("release unused resource BEGIN");
//        report_memory();
//        
//        CCObject* iter = NULL;
//        CCARRAY_FOREACH(_hSprites, iter) {
//            CCHierarchiesSpriteBase* spr = dynamic_cast<CCHierarchiesSpriteBase*>(iter);
//            spr->removeFromParent();
//        }
//        _hSprites->removeAllObjects();
//        
//        CCHierarchiesSpriteRuntime::sharedHierarchiesSpriteRuntime()->releaseUnusedResource();
//        CCTextureCache::sharedTextureCache()->removeUnusedTextures();
//        
//        report_memory();
//        CCLOG("release unused resource END");
//    }
//    else {
//        this->loadSprites();
//    }
//    
//    report_memory();
    
    // 运行时换装测试
    CCObject* iter = NULL;
    CCARRAY_FOREACH(_hSprites, iter) {
        CCHierarchiesSpriteDynamic* spr = dynamic_cast<CCHierarchiesSpriteDynamic*>(iter);
        if (spr) {
            switch (_clickNumber) {
                case 0: {
                    spr->resetAvatarMap();
                    break;
                }
                case 1: {
                    spr->setAvatarMap("laodedadao/dadao-1", "xindedadao/caidao-1");
                    std::string avatarMapName;
                    spr->getAvatarMap("laodedadao/dadao-1", avatarMapName);
                    CCLOG("avatar map name: laodedadao/dadao-1 -> %s", avatarMapName.c_str());
                    break;
                }
                case 2: {
                    spr->setAvatarMap("laodedadao/dadao-2", "xindedadao/caidao-2");
                    std::string avatarMapName;
                    spr->getAvatarMap("laodedadao/dadao-2", avatarMapName);
                    CCLOG("avatar map name: laodedadao/dadao-2 -> %s", avatarMapName.c_str());
                    break;
                }
                case 3: {
                    spr->setAvatarMap("laodedadao/daoduang-3", "xindedadao/caidao-3");
                    std::string avatarMapName;
                    spr->getAvatarMap("laodedadao/daoduang-3", avatarMapName);
                    CCLOG("avatar map name: laodedadao/daoduang-3 -> %s", avatarMapName.c_str());
                    _clickNumber = -1;
                    break;
                }
                default:
                    break;
            }
        }
    }
    _clickNumber++;
}

void CCHierarchiesSpriteAvatarTestScene::ccTouchesMoved(CCSet *pTouches, CCEvent *pEvent) {
}

void CCHierarchiesSpriteAvatarTestScene::ccTouchesEnded(CCSet *pTouches, CCEvent *pEvent) {
}

void CCHierarchiesSpriteAvatarTestScene::ccTouchesCancelled(CCSet *pTouches, CCEvent *pEvent) {
}


void CCHierarchiesSpriteAvatarTestScene::loadSprites () {
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    
    CCLOG("load avatar sprites BEGIN");
    report_memory();
    
#define SPRITE_ARRAY_COL_COUNT (2)
    for (int j = 1; j <= SPRITE_ARRAY_COL_COUNT; j++) {
        CCHierarchiesSpriteBase* spr = NULL;
        
        switch (j % 2) {
            case 0: {
                spr = CCHierarchiesSprite::create("test_new_anim/hanim_test_guanyu-avatar/guanyuquan-avatar.hsheet",
                                                  "test_new_anim/hanim_test_guanyu-avatar/guanyuquan-avatar.hanims",
                                                  NULL,
                                                  _avatarMap["caidao_avatar"]);
                break;
            }
            case 1: {
                spr = CCHierarchiesSpriteDynamic::create("test_new_anim/hanim_test_guanyu-avatar/guanyuquan-avatar.hsheet",
                                                         "test_new_anim/hanim_test_guanyu-avatar/guanyuquan-avatar.hanims",
                                                         NULL,
                                                         _avatarMap["caidao_avatar"]);
                
                break;
            }
        }
        
        spr->setPosition(ccp(winSize.width / (SPRITE_ARRAY_COL_COUNT + 1) * j, winSize.height / 2));
        
        CCAction* action = NULL;
        CCHierarchiesAnimate* animate = CCHierarchiesAnimate::createWholeAnimation(spr->getAnimationName());
        action = CCRepeatForever::create(CCSequence::create(animate, animate->reverse(), NULL));
        spr->runAction(action);
        
        spr->setZOrder(SPRITE_ARRAY_COL_COUNT + j);
        this->addChild(spr);
        
        _hSprites->addObject(spr);
    }
    
    CCLOG("load avatar sprites END");
    report_memory();
}
