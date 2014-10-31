//
//  CCHierarchiesSpriteAction.cpp
//  HierarchiesSpriteDemo
//
//  Created by xuxiaocheng on 10/23/14.
//
//

#include "CCHierarchiesSpriteAction.h"
#include "CCHierarchiesSpriteRuntime.h"

NS_CC_EXT_BEGIN


#pragma mark - HierarchiesAnimate

HierarchiesAnimate* HierarchiesAnimate::create (const std::string& animationName,
                                                const std::string& spriteAnimationName)
{
    HierarchiesAnimate* ret = new (std::nothrow) HierarchiesAnimate();
    if (ret && ret->init(animationName, spriteAnimationName))
    {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

HierarchiesAnimate::HierarchiesAnimate ()
: _spriteAnimation(nullptr), _curFrameIndex(0)
{
}

HierarchiesAnimate::~HierarchiesAnimate ()
{
    HierarchiesSpriteRuntime::getInstance()->removeAnimation(_spriteAnimationName);
}

bool HierarchiesAnimate::init (const std::string& animationName,
                               const std::string& spriteAnimationName)
{
    CCASSERT(!animationName.empty() && !spriteAnimationName.empty(), "HierarchiesAnimate: arguments must be non-nil");
    
    _spriteAnimation = HierarchiesSpriteRuntime::getInstance()->addAnimation(spriteAnimationName);
    
    if (!_spriteAnimation->getAnimationByName(animationName, _currentAnimation)) {
        CCLOG("no animation (%s) in (%s) while create HierarchiesAnimate", animationName.c_str(), spriteAnimationName.c_str());
        HierarchiesSpriteRuntime::getInstance()->removeAnimation(spriteAnimationName);
        CC_SAFE_RELEASE(this);
        return false;
    }
    
    float duration = (float)(_currentAnimation.endFrameIndex - _currentAnimation.startFrameIndex + 1) / _spriteAnimation->getFrameRate();
    
    if (ActionInterval::initWithDuration(duration)) {
        _animationName = animationName;
        _spriteAnimationName = spriteAnimationName;
        return true;
    }
    else {
        CC_SAFE_RELEASE(this);
        return false;
    }
}

void HierarchiesAnimate::startWithTarget (Node* pTarget)
{
    HierarchiesSprite* target = dynamic_cast<HierarchiesSprite*>(pTarget);
    CCASSERT(target != nullptr, "HierarchiesAnimate can only valid on HierarchiesSprite class");
    
    CCASSERT(_spriteAnimationName.compare(target->getAnimationName()) == 0, "HierarchiesAnimate effect on a invalid HierarchiesSpriteBase instance");
    
    ActionInterval::startWithTarget(pTarget);
    
    _curFrameIndex = _currentAnimation.startFrameIndex;
    target->displayFrameAtIndex(_curFrameIndex);
}

ActionInterval* HierarchiesAnimate::reverse(void) const
{
    HierarchiesAnimate* animate = animate = HierarchiesAnimate::create(_animationName, _spriteAnimationName);
    unsigned int tmp = animate->_currentAnimation.endFrameIndex;
    animate->_currentAnimation.endFrameIndex = animate->_currentAnimation.startFrameIndex;
    animate->_currentAnimation.startFrameIndex = tmp;
    return animate;
}

ActionInterval* HierarchiesAnimate::clone() const
{
    return HierarchiesAnimate::create(_animationName, _spriteAnimationName);
}

void HierarchiesAnimate::update (float time)
{
    HierarchiesSprite* target = dynamic_cast<HierarchiesSprite*>(_target);
    if (_currentAnimation.endFrameIndex >= _currentAnimation.startFrameIndex) {
        unsigned int curFrameIndex = _currentAnimation.startFrameIndex + time * (_currentAnimation.endFrameIndex - _currentAnimation.startFrameIndex);
        
        while (_curFrameIndex != curFrameIndex) {
            _curFrameIndex++;
            target->displayFrameAtIndex(_curFrameIndex);
        }
    }
    else { // animation reverse play
        unsigned int curFrameIndex = _currentAnimation.endFrameIndex + (1 - time) * (_currentAnimation.startFrameIndex - _currentAnimation.endFrameIndex);
        
        while (_curFrameIndex != curFrameIndex) {
            _curFrameIndex--;
            target->displayFrameAtIndex(_curFrameIndex);
        }
    }
}


#pragma mark - HierarchiesFlipX

HierarchiesFlipX* HierarchiesFlipX::create (bool x)
{
    HierarchiesFlipX* ret = new (std::nothrow) HierarchiesFlipX();
    if (ret && ret->initWithFlipX(x)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool HierarchiesFlipX::initWithFlipX (bool x)
{
    _flipX = x;
    return true;
}

void HierarchiesFlipX::update(float time)
{
    CC_UNUSED_PARAM(time);
    
    HierarchiesSprite* target = dynamic_cast<HierarchiesSprite*>(_target);
    CCASSERT(target, "HierarchiesFlipX only valid on HierarchiesSprite class");
    target->setFlippedX(_flipX);
}

ActionInstant* HierarchiesFlipX::reverse() const
{
    return HierarchiesFlipX::create(!_flipX);
}

ActionInstant* HierarchiesFlipX::clone() const
{
    return HierarchiesFlipX::create(_flipX);
}


#pragma mark - HierarchiesFlipY

HierarchiesFlipY* HierarchiesFlipY::create (bool y)
{
    HierarchiesFlipY* ret = new (std::nothrow) HierarchiesFlipY();
    if (ret && ret->initWithFlipY(y)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool HierarchiesFlipY::initWithFlipY (bool y)
{
    _flipY = y;
    return true;
}

void HierarchiesFlipY::update(float time)
{
    CC_UNUSED_PARAM(time);
    
    HierarchiesSprite* target = dynamic_cast<HierarchiesSprite*>(_target);
    CCASSERT(target, "HierarchiesFlipY only valid on HierarchiesSprite class");
    target->setFlippedY(_flipY);
}

ActionInstant* HierarchiesFlipY::reverse() const
{
    return HierarchiesFlipY::create(!_flipY);
}

ActionInstant* HierarchiesFlipY::clone() const
{
    return HierarchiesFlipY::create(_flipY);
}


#pragma mark - HierarchiesAvatarMapInsert

HierarchiesAvatarMapInsert* HierarchiesAvatarMapInsert::create (const std::string& avatarMapFrom, const std::string& avatarMapTo)
{
    HierarchiesAvatarMapInsert* ret = new (std::nothrow) HierarchiesAvatarMapInsert();
    if (ret && ret->init(avatarMapFrom, avatarMapTo)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool HierarchiesAvatarMapInsert::init (const std::string& avatarMapFrom, const std::string& avatarMapTo)
{
    _avatarMapFrom = avatarMapFrom;
    _avatarMapTo = avatarMapTo;
    return true;
}

void HierarchiesAvatarMapInsert::update (float time)
{
    CC_UNUSED_PARAM(time);
    
    HierarchiesSprite* target = dynamic_cast<HierarchiesSprite*>(_target);
    CCASSERT(target, "HierarchiesAvatarMapInsert only valid on HierarchiesSprite class");
    target->setAvatarMap(_avatarMapFrom, _avatarMapTo);
}

ActionInstant* HierarchiesAvatarMapInsert::clone() const
{
    return HierarchiesAvatarMapInsert::create(_avatarMapFrom, _avatarMapTo);
}


#pragma mark - HierarchiesAvatarMapReset

HierarchiesAvatarMapReset* HierarchiesAvatarMapReset::create ()
{
    HierarchiesAvatarMapReset* ret = new (std::nothrow) HierarchiesAvatarMapReset();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool HierarchiesAvatarMapReset::init ()
{
    return true;
}

void HierarchiesAvatarMapReset::update (float time)
{
    CC_UNUSED_PARAM(time);
    
    HierarchiesSprite* target = dynamic_cast<HierarchiesSprite*>(_target);
    CCASSERT(target, "HierarchiesAvatarMapReset only valid on HierarchiesSprite class");
    target->resetAvatarMap();
}

ActionInstant* HierarchiesAvatarMapReset::clone() const
{
    return HierarchiesAvatarMapReset::create();
}


#pragma mark - HierarchiesAvatarMapSet

HierarchiesAvatarMapSet* HierarchiesAvatarMapSet::create (const HierarchiesSprite::AvatarMapType& avatarMap)
{
    HierarchiesAvatarMapSet* ret = new (std::nothrow) HierarchiesAvatarMapSet();
    if (ret && ret->init(avatarMap)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool HierarchiesAvatarMapSet::init (const HierarchiesSprite::AvatarMapType& avatarMap)
{
    _avatarMap = avatarMap;
    return true;
}

void HierarchiesAvatarMapSet::update (float time)
{
    CC_UNUSED_PARAM(time);
    
    HierarchiesSprite* target = dynamic_cast<HierarchiesSprite*>(_target);
    CCASSERT(target, "HierarchiesAvatarMapSet only valid on HierarchiesSprite class");
    target->resetAvatarMap();
    HierarchiesSprite::AvatarMapType::const_iterator avatarItemIter;
    for (avatarItemIter = _avatarMap.cbegin(); avatarItemIter != _avatarMap.cend(); avatarItemIter++) {
        target->setAvatarMap(avatarItemIter->first, avatarItemIter->second);
    }
}

ActionInstant* HierarchiesAvatarMapSet::clone() const
{
    return HierarchiesAvatarMapSet::create(_avatarMap);
}


NS_CC_EXT_END
