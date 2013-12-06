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
#include "CCHierarchiesSpriteBase.h"


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
    virtual CCActionInterval* reverse();
    virtual void update (float time);
	
};


class CC_DLL CCHierarchiesFlipX : public CCActionInstant {
    
protected:
    bool _flipX;
	
public:
    static CCHierarchiesFlipX * create(bool x);
    
    CCHierarchiesFlipX ()
    : _flipX(false)
    {}
    virtual ~CCHierarchiesFlipX () {}
    
    bool initWithFlipX (bool x);
    
    virtual void update (float time);
    virtual CCFiniteTimeAction * reverse ();
    virtual CCObject* copyWithZone (CCZone* zone);
    
};


class CC_DLL CCHierarchiesFlipY : public CCActionInstant {
	
protected:
    bool _flipY;
	
public:
    static CCHierarchiesFlipY * create(bool y);
    
    CCHierarchiesFlipY ()
    : _flipY(false)
    {}
    virtual ~CCHierarchiesFlipY () {}
    
    bool initWithFlipY (bool y);
    
    virtual void update (float time);
    virtual CCFiniteTimeAction * reverse ();
    virtual CCObject* copyWithZone (CCZone *pZone);
	
};

class CC_DLL CCHierarchiesAvatarMapInsert : public CCActionInstant {
	
protected:
	
public:
	
};

class CC_DLL CCHierarchiesAvatarMapReset : public CCActionInstant {
	
protected:
	
public:
	
};

class CC_DLL CCHierarchiesAvatarMapSet : public CCActionInstant {
	
protected:
	
public:
	
};

NS_CC_EXT_END

#endif /* defined(__SZSG__CCHierarchiesSpriteAction__) */
