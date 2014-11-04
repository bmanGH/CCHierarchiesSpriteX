//
//  CCHierarchiesSpriteTestScene.h
//  HierarchiesSpriteDemo
//
//  Created by xuxiaocheng on 11/3/14.
//
//

#ifndef __HierarchiesSpriteDemo__CCHierarchiesSpriteTestScene__
#define __HierarchiesSpriteDemo__CCHierarchiesSpriteTestScene__

#include "cocos2d.h"
#include "cocos-ext.h"
#include "CCHierarchiesSprite.h"
#include "CCHierarchiesSpriteAction.h"

USING_NS_CC;
USING_NS_CC_EXT;

class CCHierarchiesSpriteTestScene : public Layer
{
private:
    Vector<HierarchiesSprite*> _hSprites;
    
public:
    virtual bool init();
    
    static Scene* scene();
    
    CREATE_FUNC(CCHierarchiesSpriteTestScene);
    
    void loadSprites ();
};

#endif /* defined(__HierarchiesSpriteDemo__CCHierarchiesSpriteTestScene__) */
