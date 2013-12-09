//
//  MainMenu
//  CCHierarchiesSpriteXDemo
//
//  Created by bman <zx123xz321hm3@hotmail.com>.
//  Copyright (c) 2013. All rights reserved.
//
//

#include "MainMenu.h"
#include "CCHierarchiesSpriteTestScene.h"
#include "CCHierarchiesSpriteDynamicTestScene.h"
#include "CCHierarchiesSpriteActionTestScene.h"
#include "CCHierarchiesSpriteAvatarTestScene.h"

CCScene* MainMenu::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    MainMenu *layer = MainMenu::create();
    
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}

bool MainMenu::init()
{
    if ( !CCLayer::init() )
    {
        return false;
    }
    
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    
    // create menu
    CCLabelTTF* menuLabel1 = CCLabelTTF::create("CCHierarchiesSprite Animation Test", "Arial", 24);
    CCMenuItemLabel* menuItem1 = CCMenuItemLabel::create(menuLabel1, this, menu_selector(MainMenu::onMenuItemClick));
    menuItem1->setTag(101);
    
    CCLabelTTF* menuLabel2 = CCLabelTTF::create("CCHierarchiesSpriteDynamic Animation Test", "Arial", 24);
    CCMenuItemLabel* menuItem2 = CCMenuItemLabel::create(menuLabel2, this, menu_selector(MainMenu::onMenuItemClick));
    menuItem2->setTag(102);
    
    CCLabelTTF* menuLabel3 = CCLabelTTF::create("CCHierarchiesSprite Avatar Test", "Arial", 24);
    CCMenuItemLabel* menuItem3 = CCMenuItemLabel::create(menuLabel3, this, menu_selector(MainMenu::onMenuItemClick));
    menuItem3->setTag(103);
    
    CCLabelTTF* menuLabel4 = CCLabelTTF::create("CCHierarchiesSprite Action Test", "Arial", 24);
    CCMenuItemLabel* menuItem4 = CCMenuItemLabel::create(menuLabel4, this, menu_selector(MainMenu::onMenuItemClick));
    menuItem4->setTag(104);
    
    CCMenu* menu = CCMenu::create(menuItem1, menuItem2, menuItem3, menuItem4, NULL);
    menu->alignItemsVerticallyWithPadding(40);
    menu->setPosition(ccp(winSize.width / 2, winSize.height / 2));
    this->addChild(menu);
    
    return true;
}

void MainMenu::onMenuItemClick (CCObject* sender) {
    CCMenuItem* menuItem = (CCMenuItem*)sender;
    switch (menuItem->getTag()) {
        case 101: {
            CCScene* scene = CCHierarchiesSpriteTestScene::scene();
            CCDirector::sharedDirector()->pushScene(scene);
            break;
        }
        case 102: {
            CCScene* scene = CCHierarchiesSpriteDynamicTestScene::scene();
            CCDirector::sharedDirector()->pushScene(scene);
            break;
        }
        case 103: {
            CCScene* scene = CCHierarchiesSpriteAvatarTestScene::scene();
            CCDirector::sharedDirector()->pushScene(scene);
            break;
        }
        case 104: {
            CCScene* scene = CCHierarchiesSpriteActionTestScene::scene();
            CCDirector::sharedDirector()->pushScene(scene);
            break;
        }
        default:
            break;
    }
}
