//
//  CCHierarchiesSpriteAction.h
//  CCHierarchiesSprite
//
//  Created by bman <zx123xz321hm3@hotmail.com>.
//  Copyright (c) 2013. All rights reserved.
//

#ifndef __SZSG__CCHierarchiesSpriteAction__
#define __SZSG__CCHierarchiesSpriteAction__

#include "cocos2d.h"
#include "ExtensionMacros.h"
#include "CCHierarchiesSpriteConfig.h"
#include <string>
#include "CCHierarchiesSpriteAnimation.h"


NS_CC_EXT_BEGIN

class CC_DLL CCHierarchiesAnimate : public CCActionInterval {
	
protected:
    std::string _animationName;
    std::string _spriteAnimationName;
	CCHierarchiesSpriteAnimation* _spriteAnimation; // weak ref
    CCHierarchiesSpriteAnimation::Animation _currentAnimation;
    unsigned int _curFrameIndex;
    unsigned int _startFrameIndex;
	
public:
    static CCHierarchiesAnimate* create (const char* animationName,
                                         const char* spriteAnimationName);
    static CCHierarchiesAnimate* createWholeAnimation (const char* spriteAnimationName);
    
    CCHierarchiesAnimate ();
    ~CCHierarchiesAnimate ();
    bool initWithName (const char* animationName,
                       const char* spriteAnimationName);
    bool initWholeAnimation (const char* spriteAnimationName);
    
    const char* getAnimationName ();
    const char* getSpriteAnimationName ();
    
    virtual void startWithTarget (CCNode *pTarget);
    virtual CCObject* copyWithZone (CCZone* pZone);
    virtual void update (float time);
	
};

NS_CC_EXT_END

#endif /* defined(__SZSG__CCHierarchiesSpriteAction__) */
