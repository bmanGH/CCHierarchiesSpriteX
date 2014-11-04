//
//  CCHierarchiesSpriteTestScene.cpp
//  HierarchiesSpriteDemo
//
//  Created by xuxiaocheng on 11/3/14.
//
//

#include "CCHierarchiesSpriteTestScene.h"
#include "Utils.h"

Scene* CCHierarchiesSpriteTestScene::scene()
{
    Scene *scene = Scene::create();
    CCHierarchiesSpriteTestScene *layer = CCHierarchiesSpriteTestScene::create();
    scene->addChild(layer);
    return scene;
}

bool CCHierarchiesSpriteTestScene::init()
{
    if ( !CCLayer::init() )
    {
        return false;
    }
    
    this->loadSprites();
    
    return true;
}

void CCHierarchiesSpriteTestScene::loadSprites () {
    Size winSize = Director::getInstance()->getWinSize();
    
    CCLOG("load sprites BEGIN");
    report_memory();
    
//    Director::getInstance()->getScheduler()->setTimeScale(0.5);
    
#define SPRITE_ARRAY_COL_COUNT (1)
#define SPRITE_ARRAY_ROW_COUNT (1)
    for (int i = 0; i < SPRITE_ARRAY_ROW_COUNT; i++) {
        for (int j = 1; j <= SPRITE_ARRAY_COL_COUNT; j++) {
            HierarchiesSprite* spr = nullptr;
            
            switch (j % 4) {
                case 0: {
                    spr = HierarchiesSprite::create("test_new_anim/hanim_test_lvmeng/lumengquan.hsheet",
                                                      "test_new_anim/hanim_test_lvmeng/lumengquan.hanims");
                    break;
                }
                case 1: {
                    spr = HierarchiesSprite::create("test_new_anim/hanim_test_taishici/taishiciquan.hsheet",
                                                      "test_new_anim/hanim_test_taishici/taishiciquan.hanims");
                    spr->setOpacity(128);
                    break;
                }
                case 2: {
                    spr = HierarchiesSprite::create("test_new_anim/hanim_test_xiahouyuan/xiahouyuanquan.hsheet",
                                                      "test_new_anim/hanim_test_xiahouyuan/xiahouyuanquan.hanims");
                    spr->setFlippedX(true);
                    break;
                }
                case 3: {
                    spr = HierarchiesSprite::create("test_new_anim/hanim_test_zhangjiao/zhangjiaoquan.hsheet",
                                                      "test_new_anim/hanim_test_zhangjiao/zhangjiaoquan.hanims");
                    spr->setColor(Color3B::RED);
                    break;
                }
            }
            
            spr->setPosition(Point(winSize.width / (SPRITE_ARRAY_COL_COUNT + 1) * j, winSize.height / (SPRITE_ARRAY_ROW_COUNT + 1) * i));
            spr->setPosition(Point(winSize.width / 2, winSize.height / 2));
            spr->setScale(2);
            spr->setFlippedX(true);
            
            spr->setEventHandle([] (int frameIndex, const std::string& eventName) {
                CCLOG("event emit:%s", eventName.c_str());
            });
            
            Action* action = nullptr;
            action = CCRepeatForever::create( HierarchiesAnimate::create("atk", spr->getAnimationName()));
            spr->runAction(action);
            
            spr->setZOrder((SPRITE_ARRAY_ROW_COUNT - i) * SPRITE_ARRAY_COL_COUNT + j);
            this->addChild(spr);
            
            _hSprites.pushBack(spr);
        }
    }
    
    CCLOG("load sprites END");
    report_memory();
}
