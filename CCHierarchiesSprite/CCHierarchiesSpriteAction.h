//
//  CCHierarchiesSpriteAction.h
//  HierarchiesSpriteDemo
//
//  Created by xuxiaocheng on 10/23/14.
//
//

#ifndef __HierarchiesSpriteDemo__CCHierarchiesSpriteAction__
#define __HierarchiesSpriteDemo__CCHierarchiesSpriteAction__

#include "cocos2d.h"
#include "ExtensionMacros.h"
#include <string>
#include "CCHierarchiesSpriteAnimation.h"
#include "CCHierarchiesSprite.h"

NS_CC_EXT_BEGIN


// animate action
class CC_DLL HierarchiesAnimate : public ActionInterval {
    
protected:
    std::string _animationName;
    std::string _spriteAnimationName;
    HierarchiesSpriteAnimation* _spriteAnimation;
    HierarchiesSpriteAnimation::Animation _currentAnimation;
    unsigned int _curFrameIndex;
    unsigned int _startFrameIndex;
    
public:
    static HierarchiesAnimate* create (const std::string& animationName,
                                       const std::string& spriteAnimationName);
    
    HierarchiesAnimate ();
    virtual ~HierarchiesAnimate ();
    bool init (const std::string& animationName,
               const std::string& spriteAnimationName);
    
    std::string getAnimationName () {
        return _animationName;
    }
    std::string getSpriteAnimationName () {
        return _spriteAnimationName;
    }
    
    virtual void startWithTarget (Node *pTarget);
    virtual void update (float time) override;
    virtual ActionInterval* reverse() const override;
    virtual ActionInterval* clone() const override;

};


// flip action
class CC_DLL HierarchiesFlipX : public ActionInstant {
    
protected:
    bool _flipX;
    
public:
    static HierarchiesFlipX* create(bool x);
    
    HierarchiesFlipX ()
    : _flipX(false)
    {}
    virtual ~HierarchiesFlipX () {}
    
    bool initWithFlipX (bool x);
    
    virtual void update (float time) override;
    virtual ActionInstant* reverse() const override;
    virtual ActionInstant* clone() const override;
    
};


class CC_DLL HierarchiesFlipY : public ActionInstant {
    
protected:
    bool _flipY;
    
public:
    static HierarchiesFlipY* create(bool y);
    
    HierarchiesFlipY ()
    : _flipY(false)
    {}
    virtual ~HierarchiesFlipY () {}
    
    bool initWithFlipY (bool y);
    
    virtual void update (float time) override;
    virtual ActionInstant* reverse() const override;
    virtual ActionInstant* clone() const override;
    
};


// avatar action
class CC_DLL HierarchiesAvatarMapInsert : public ActionInstant {
    
protected:
    std::string _avatarMapFrom;
    std::string _avatarMapTo;
    
public:
    static HierarchiesAvatarMapInsert* create(const std::string& avatarMapFrom, const std::string& avatarMapTo);
    
    HierarchiesAvatarMapInsert () {}
    virtual ~HierarchiesAvatarMapInsert () {}
    
    bool init (const std::string& avatarMapFrom, const std::string& avatarMapTo);
    
    virtual void update (float time) override;
    virtual ActionInstant* clone() const override;
    
};


class CC_DLL HierarchiesAvatarMapReset : public ActionInstant {
    
public:
    static HierarchiesAvatarMapReset* create();
    
    HierarchiesAvatarMapReset () {}
    virtual ~HierarchiesAvatarMapReset () {}
    
    bool init ();
    
    virtual void update (float time) override;
    virtual ActionInstant* clone() const override;
    
};


class CC_DLL HierarchiesAvatarMapSet : public ActionInstant {
    
protected:
    AvatarMapType _avatarMap;
    
public:
    static HierarchiesAvatarMapSet* create(const AvatarMapType& avatarMap);
    
    HierarchiesAvatarMapSet () {}
    virtual ~HierarchiesAvatarMapSet () {}
    
    bool init (const AvatarMapType& avatarMap);
    
    virtual void update (float time) override;
    virtual ActionInstant* clone() const override;
    
};


NS_CC_EXT_END

#endif /* defined(__HierarchiesSpriteDemo__CCHierarchiesSpriteAction__) */
