//
// CCHierarchiesSprite功能性单元测试场景
//

#ifndef __CCHierarchiesSpriteFunctionalTestScene_H__
#define __CCHierarchiesSpriteFunctionalTestScene_H__

#include "cocos2d.h"
#include "cocos-ext.h"
#include "CCHierarchiesSprite.h"

USING_NS_CC;
USING_NS_CC_EXT;

class CCHierarchiesSpriteFunctionalTestScene : public CCLayer, public CCHierarchiesSpriteEventDelegate
{
private:
    CCArray* _hSprites;
    
public:
    virtual ~CCHierarchiesSpriteFunctionalTestScene ();
    
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();  

    // there's no 'id' in cpp, so we recommand to return the exactly class pointer
    static CCScene* scene();

    // implement the "static node()" method manually
    CREATE_FUNC(CCHierarchiesSpriteFunctionalTestScene);
    
    // update
    virtual void update (float delta);
    
    // draw
    virtual void draw ();
    
    // touch callback
    virtual void ccTouchesBegan (CCSet *pTouches, CCEvent *pEvent);
    virtual void ccTouchesMoved (CCSet *pTouches, CCEvent *pEvent);
    virtual void ccTouchesEnded (CCSet *pTouches, CCEvent *pEvent);
    virtual void ccTouchesCancelled (CCSet *pTouches, CCEvent *pEvent);
    
    // CCHierarchiesSpriteEventDelegate
    virtual void onEventContent (CCHierarchiesSprite* sprite, const char* eventContent);
    
    
    void loadSprites ();
};

#endif // __CCHierarchiesSpriteFunctionalTestScene_H__
