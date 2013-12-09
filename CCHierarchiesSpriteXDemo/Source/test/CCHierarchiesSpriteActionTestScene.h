//
//  CCHierarchiesSprite Action单元测试场景
//  CCHierarchiesSpriteXDemo
//
//  Created by bman <zx123xz321hm3@hotmail.com>.
//  Copyright (c) 2013. All rights reserved.
//
//

#ifndef __CCHierarchiesSpriteXDemo__CCHierarchiesSpriteActionTestScene__
#define __CCHierarchiesSpriteXDemo__CCHierarchiesSpriteActionTestScene__

#include "cocos2d.h"
#include "cocos-ext.h"
#include "CCHierarchiesSprite.h"
#include "CCHierarchiesSpriteDynamic.h"

USING_NS_CC;
USING_NS_CC_EXT;

class CCHierarchiesSpriteActionTestScene : public CCLayer
{
private:
    CCArray* _hSprites;
    std::map<std::string, AvatarMapType> _avatarMap;
    
public:
    virtual ~CCHierarchiesSpriteActionTestScene ();
    
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();
    
    // there's no 'id' in cpp, so we recommand to return the exactly class pointer
    static CCScene* scene();
    
    // implement the "static node()" method manually
    CREATE_FUNC(CCHierarchiesSpriteActionTestScene);
    
    // update
    virtual void update (float delta);
    
    // draw
    virtual void draw ();
    
    // touch callback
    virtual void ccTouchesBegan (CCSet *pTouches, CCEvent *pEvent);
    virtual void ccTouchesMoved (CCSet *pTouches, CCEvent *pEvent);
    virtual void ccTouchesEnded (CCSet *pTouches, CCEvent *pEvent);
    virtual void ccTouchesCancelled (CCSet *pTouches, CCEvent *pEvent);
    
    
    void loadSprites ();
};

#endif /* defined(__CCHierarchiesSpriteXDemo__CCHierarchiesSpriteActionTestScene__) */
