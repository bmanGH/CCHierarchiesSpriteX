//
//  MainMenu.cpp
//  HierarchiesSpriteDemo
//
//  Created by xuxiaocheng on 11/3/14.
//
//

#include "MainMenu.h"
#include "CCHierarchiesSpriteTestScene.h"

Scene* MainMenu::scene()
{
    Scene *scene = Scene::create();
    MainMenu *layer = MainMenu::create();
    scene->addChild(layer);
    return scene;
}

bool MainMenu::init()
{
    if (!Layer::init())
    {
        return false;
    }
    
    Size winSize = Director::getInstance()->getWinSize();
    
    // create menu
    Label* menuLabel1 = Label::create("CCHierarchiesSprite Animation Test", "Arial", 24);
    MenuItemLabel* menuItem1 = MenuItemLabel::create(menuLabel1, this, menu_selector(MainMenu::onMenuItemClick));
    menuItem1->setTag(101);
    
    Label* menuLabel2 = Label::create("CCHierarchiesSpriteDynamic Animation Test", "Arial", 24);
    MenuItemLabel* menuItem2 = MenuItemLabel::create(menuLabel2, this, menu_selector(MainMenu::onMenuItemClick));
    menuItem2->setTag(102);
    
    Label* menuLabel3 = Label::create("CCHierarchiesSprite Avatar Test", "Arial", 24);
    MenuItemLabel* menuItem3 = MenuItemLabel::create(menuLabel3, this, menu_selector(MainMenu::onMenuItemClick));
    menuItem3->setTag(103);
    
    Label* menuLabel4 = Label::create("CCHierarchiesSprite Action Test", "Arial", 24);
    MenuItemLabel* menuItem4 = MenuItemLabel::create(menuLabel4, this, menu_selector(MainMenu::onMenuItemClick));
    menuItem4->setTag(104);
    
    Menu* menu = Menu::create(menuItem1, menuItem2, menuItem3, menuItem4, NULL);
    menu->alignItemsVerticallyWithPadding(40);
    menu->setPosition(ccp(winSize.width / 2, winSize.height / 2));
    this->addChild(menu);
    
    return true;
}

void MainMenu::onMenuItemClick (Ref* sender) {
    MenuItem* menuItem = (MenuItem*)sender;
    switch (menuItem->getTag()) {
        case 101: {
            Scene* scene = CCHierarchiesSpriteTestScene::scene();
            Director::getInstance()->pushScene(scene);
            break;
        }
        case 102: {
//            CCScene* scene = CCHierarchiesSpriteDynamicTestScene::scene();
//            CCDirector::sharedDirector()->pushScene(scene);
            break;
        }
        case 103: {
//            CCScene* scene = CCHierarchiesSpriteAvatarTestScene::scene();
//            CCDirector::sharedDirector()->pushScene(scene);
            break;
        }
        case 104: {
//            CCScene* scene = CCHierarchiesSpriteActionTestScene::scene();
//            CCDirector::sharedDirector()->pushScene(scene);
            break;
        }
        default:
            break;
    }
}
