//
//  CCHierarchiesSprite Action单元测试场景
//  CCHierarchiesSpriteXDemo
//
//  Created by bman <zx123xz321hm3@hotmail.com>.
//  Copyright (c) 2013. All rights reserved.
//
//

#include "CCHierarchiesSpriteActionTestScene.h"
#include "CCHierarchiesSpriteAction.h"
#include "CCHierarchiesSpriteRuntime.h"
#include "json/json.h"
#include <fstream>
#include "Utils.h"


CCScene* CCHierarchiesSpriteActionTestScene::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    CCHierarchiesSpriteActionTestScene *layer = CCHierarchiesSpriteActionTestScene::create();
    
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}

CCHierarchiesSpriteActionTestScene::~CCHierarchiesSpriteActionTestScene () {
    CC_SAFE_RELEASE(_hSprites);
}

bool CCHierarchiesSpriteActionTestScene::init()
{
    if ( !CCLayer::init() )
    {
        return false;
    }
    
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

void CCHierarchiesSpriteActionTestScene::update (float delta) {
    
}


#pragma mark - Draw

void CCHierarchiesSpriteActionTestScene::draw () {
    
}


#pragma mark - Touch callback

void CCHierarchiesSpriteActionTestScene::ccTouchesBegan(CCSet *pTouches, CCEvent *pEvent) {
    // 资源管理测试
    if (_hSprites->count() > 0) {
        CCLOG("release unused resource BEGIN");
        report_memory();
        
        CCObject* iter = NULL;
        CCARRAY_FOREACH(_hSprites, iter) {
            CCHierarchiesSpriteBase* spr = dynamic_cast<CCHierarchiesSpriteBase*>(iter);
            spr->removeFromParent();
        }
        _hSprites->removeAllObjects();
        
        CCHierarchiesSpriteRuntime::sharedHierarchiesSpriteRuntime()->releaseUnusedResource();
        CCTextureCache::sharedTextureCache()->removeUnusedTextures();
        
        report_memory();
        CCLOG("release unused resource END");
    }
    else {
        this->loadSprites();
    }
    
    report_memory();
}

void CCHierarchiesSpriteActionTestScene::ccTouchesMoved(CCSet *pTouches, CCEvent *pEvent) {
}

void CCHierarchiesSpriteActionTestScene::ccTouchesEnded(CCSet *pTouches, CCEvent *pEvent) {
}

void CCHierarchiesSpriteActionTestScene::ccTouchesCancelled(CCSet *pTouches, CCEvent *pEvent) {
}


void CCHierarchiesSpriteActionTestScene::loadSprites () {
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    
    CCLOG("load sprites BEGIN");
    report_memory();
    
#define SPRITE_ARRAY_COL_COUNT (2)
    for (int j = 1; j <= SPRITE_ARRAY_COL_COUNT; j++) {
        CCHierarchiesSpriteBase* spr = NULL;
        
        switch (j % 2) {
            case 0: {
                spr = CCHierarchiesSprite::create("test_new_anim/hanim_test_zhangfei/zhangfeiquan.hsheet",
                                                  "test_new_anim/hanim_test_zhangfei/zhangfeiquan.hanims",
                                                  NULL);
                
                CCAction* action = NULL;
                CCSequence* seq = CCSequence::create(CCHierarchiesFlipX::create(true),
                                                     CCDelayTime::create(1),
                                                     CCHierarchiesFlipY::create(true),
                                                     CCDelayTime::create(1),
                                                     CCHierarchiesFlipX::create(true)->reverse(),
                                                     CCDelayTime::create(1),
                                                     CCHierarchiesFlipY::create(true)->reverse(),
                                                     CCDelayTime::create(1),
                                                     NULL);
                action = CCRepeatForever::create(CCSequence::create(seq, seq->reverse(), NULL));
                spr->runAction(action);
                
                break;
            }
            case 1: {
                spr = CCHierarchiesSpriteDynamic::create("test_new_anim/hanim_test_guanyu-avatar/guanyuquan-avatar.hsheet",
                                                         "test_new_anim/hanim_test_guanyu-avatar/guanyuquan-avatar.hanims",
                                                         NULL);
                
                CCAction* action = NULL;
                CCSequence* seq = CCSequence::create(CCHierarchiesAvatarMapReset::create(),
                                                     CCDelayTime::create(1),
                                                     CCHierarchiesAvatarMapInsert::create("laodedadao/dadao-1", "xindedadao/caidao-1"),
                                                     CCDelayTime::create(1),
                                                     CCHierarchiesAvatarMapInsert::create("laodedadao/dadao-2", "xindedadao/caidao-2"),
                                                     CCDelayTime::create(1),
                                                     CCHierarchiesAvatarMapInsert::create("laodedadao/daoduang-3", "xindedadao/caidao-3"),
                                                     CCDelayTime::create(1),
                                                     CCHierarchiesAvatarMapSet::create(_avatarMap["caidao_avatar"]),
                                                     CCDelayTime::create(1),
                                                     NULL);
                action = CCRepeatForever::create(seq);
                spr->runAction(action);
                
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
    
    CCLOG("load sprites END");
    report_memory();
}
