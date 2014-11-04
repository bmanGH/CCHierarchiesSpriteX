//
//  MainMenu.h
//  HierarchiesSpriteDemo
//
//  Created by xuxiaocheng on 11/3/14.
//
//

#ifndef __HierarchiesSpriteDemo__MainMenu__
#define __HierarchiesSpriteDemo__MainMenu__

#include "cocos2d.h"

USING_NS_CC;

class MainMenu : public Layer
{
    
public:
    virtual bool init();
    
    static Scene* scene();
    
    CREATE_FUNC(MainMenu);
    
    void onMenuItemClick (Ref* sender);
    
};

#endif /* defined(__HierarchiesSpriteDemo__MainMenu__) */
