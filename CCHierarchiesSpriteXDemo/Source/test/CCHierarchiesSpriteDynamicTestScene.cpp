//
//  CCHierarchiesSpriteDynamic动画单元测试场景
//  CCHierarchiesSpriteXDemo
//
//  Created by bman <zx123xz321hm3@hotmail.com>.
//  Copyright (c) 2013. All rights reserved.
//
//

#include "CCHierarchiesSpriteDynamicTestScene.h"
#include "CCHierarchiesSpriteAction.h"
#include "CCHierarchiesSpriteRuntime.h"
#include "Utils.h"


CCScene* CCHierarchiesSpriteDynamicTestScene::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    CCHierarchiesSpriteDynamicTestScene *layer = CCHierarchiesSpriteDynamicTestScene::create();
    
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}

CCHierarchiesSpriteDynamicTestScene::~CCHierarchiesSpriteDynamicTestScene () {
    CC_SAFE_RELEASE(_hSprites);
}

bool CCHierarchiesSpriteDynamicTestScene::init()
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
    
    this->loadSprites();
    
    return true;
}


#pragma mark - Update

void CCHierarchiesSpriteDynamicTestScene::update (float delta) {
    
}


#pragma mark - Draw

void CCHierarchiesSpriteDynamicTestScene::draw () {
    
}


#pragma mark - Touch callback

void CCHierarchiesSpriteDynamicTestScene::ccTouchesBegan(CCSet *pTouches, CCEvent *pEvent) {
    // 点击测试
    CCTouch* touch = (CCTouch*)pTouches->anyObject();
    CCPoint touchPoint = touch->getLocation();
    CCLOG("hit at %f, %f", touchPoint.x, touchPoint.y);

    CCObject* iter = NULL;
    CCARRAY_FOREACH(_hSprites, iter) {
        CCHierarchiesSpriteDynamic* spr = dynamic_cast<CCHierarchiesSpriteDynamic*>(iter);
        if (spr && spr->boundingBox().containsPoint(touchPoint)) {
            CCLOG("hit %s with %u * fxattack", spr->getAnimationName(), spr->getEventContentCount("fxattack"));
        }
    }
    
    // 资源管理测试
    if (_hSprites->count() > 0) {
        CCLOG("release unused resource BEGIN");
        report_memory();
        
        CCObject* iter = NULL;
        CCARRAY_FOREACH(_hSprites, iter) {
            CCHierarchiesSpriteDynamic* spr = dynamic_cast<CCHierarchiesSpriteDynamic*>(iter);
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

void CCHierarchiesSpriteDynamicTestScene::ccTouchesMoved(CCSet *pTouches, CCEvent *pEvent) {
}

void CCHierarchiesSpriteDynamicTestScene::ccTouchesEnded(CCSet *pTouches, CCEvent *pEvent) {
}

void CCHierarchiesSpriteDynamicTestScene::ccTouchesCancelled(CCSet *pTouches, CCEvent *pEvent) {
}

#pragma mark - CCHierarchiesSpriteEventDelegate

void CCHierarchiesSpriteDynamicTestScene::onEventContent (CCHierarchiesSpriteBase* sprite, const char* eventContent) {
    // 动画事件测试
    CCLOG("on event content [%s] [%s]", sprite->getAnimationName(), eventContent);
}


void CCHierarchiesSpriteDynamicTestScene::loadSprites () {
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    
    CCLOG("load sprites BEGIN");
    report_memory();
    
#define SPRITE_ARRAY_COL_COUNT (4)
#define SPRITE_ARRAY_ROW_COUNT (4)
    for (int i = 0; i < SPRITE_ARRAY_ROW_COUNT; i++) {
        for (int j = 1; j <= SPRITE_ARRAY_COL_COUNT; j++) {
            CCHierarchiesSpriteDynamic* spr = NULL;
            
            switch (j % 4) {
                case 0: {
                    spr = CCHierarchiesSpriteDynamic::create("test_new_anim/hanim_test_lvmeng/lumengquan.hsheet",
                                                      "test_new_anim/hanim_test_lvmeng/lumengquan.hanims",
                                                      this);
                    break;
                }
                case 1: {
                    spr = CCHierarchiesSpriteDynamic::create("test_new_anim/hanim_test_taishici/taishiciquan.hsheet",
                                                      "test_new_anim/hanim_test_taishici/taishiciquan.hanims",
                                                      this);
                    spr->setOpacity(128);
                    break;
                }
                case 2: {
                    spr = CCHierarchiesSpriteDynamic::create("test_new_anim/hanim_test_xiahouyuan/xiahouyuanquan.hsheet",
                                                      "test_new_anim/hanim_test_xiahouyuan/xiahouyuanquan.hanims",
                                                      this);
                    spr->setFlipX(true);
                    break;
                }
                case 3: {
                    spr = CCHierarchiesSpriteDynamic::create("test_new_anim/hanim_test_zhangjiao/zhangjiaoquan.hsheet",
                                                      "test_new_anim/hanim_test_zhangjiao/zhangjiaoquan.hanims",
                                                      this);
                    spr->setColor(ccRED);
                    break;
                }
            }
            
            spr->setPosition(ccp(winSize.width / (SPRITE_ARRAY_COL_COUNT + 1) * j, winSize.height / (SPRITE_ARRAY_ROW_COUNT + 1) * i));
            
            CCAction* action = NULL;
            action = CCRepeatForever::create(CCHierarchiesAnimate::createWholeAnimation(spr->getAnimationName()));
            spr->runAction(action);
            
            spr->setZOrder((SPRITE_ARRAY_ROW_COUNT - i) * SPRITE_ARRAY_COL_COUNT + j);
            this->addChild(spr);
            
            _hSprites->addObject(spr);
        }
    }
    
    CCLOG("load sprites END");
    report_memory();
}
