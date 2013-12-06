//
//  MainMenu
//  CCHierarchiesSpriteXDemo
//
//  Created by bman <zx123xz321hm3@hotmail.com>.
//  Copyright (c) 2013. All rights reserved.
//
//

#ifndef __CCHierarchiesSpriteXDemo__MainMenu__
#define __CCHierarchiesSpriteXDemo__MainMenu__

#include "cocos2d.h"

USING_NS_CC;

class MainMenu : public CCLayer
{
    
private:
    CCArray* _hSprites;
    
public:
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();
    
    // there's no 'id' in cpp, so we recommand to return the exactly class pointer
    static CCScene* scene();
    
    // implement the "static node()" method manually
    CREATE_FUNC(MainMenu);
    
    void onMenuItemClick (CCObject* sender);
    
};

#endif /* defined(__CCHierarchiesSpriteXDemo__MainMenu__) */
