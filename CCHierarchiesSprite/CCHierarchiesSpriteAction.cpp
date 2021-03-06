//
//  CCHierarchiesSpriteAction.cpp
//  CCHierarchiesSprite
//
//  Created by bman <zx123xz321hm3@hotmail.com>.
//  Copyright (c) 2013. All rights reserved.
//

#include "CCHierarchiesSpriteAction.h"
#include "CCHierarchiesSprite.h"
#include "CCHierarchiesSpriteDynamic.h"
#include "CCHierarchiesSpriteAnimationCache.h"


NS_CC_EXT_BEGIN

#pragma mark - CCHierarchiesAnimate

CCHierarchiesAnimate* CCHierarchiesAnimate::create (const char* animationName,
                                                    const char* spriteAnimationName)
{
    CCHierarchiesAnimate* ret = new CCHierarchiesAnimate();
	if (ret->initWithName(animationName, spriteAnimationName))
	{
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);
	return NULL;
}

CCHierarchiesAnimate* CCHierarchiesAnimate::createWholeAnimation (const char* spriteAnimationName)
{
    CCHierarchiesAnimate* ret = new CCHierarchiesAnimate();
	if (ret->initWholeAnimation(spriteAnimationName))
	{
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);
	return NULL;
}

CCHierarchiesAnimate::CCHierarchiesAnimate ()
: _spriteAnimation(NULL), _curFrameIndex(0)
{
}

CCHierarchiesAnimate::~CCHierarchiesAnimate ()
{
    CCHierarchiesSpriteAnimationCache::sharedHierarchiesSpriteAnimationCache()->removeAnimation(_spriteAnimationName.c_str());
}

bool CCHierarchiesAnimate::initWithName (const char* animationName,
                                         const char* spriteAnimationName)
{
    CCAssert(animationName != NULL && spriteAnimationName != NULL, "CCHierarchiesAnimate: arguments must be non-nil");
    
    _spriteAnimation = CCHierarchiesSpriteAnimationCache::sharedHierarchiesSpriteAnimationCache()->addAnimation(spriteAnimationName);
    
    if (!_spriteAnimation->getAnimationByName(animationName, _currentAnimation)) {
		CCLOG("no animation (%s) in (%s) while create CCHierarchiesAnimate", animationName, spriteAnimationName);
        CCHierarchiesSpriteAnimationCache::sharedHierarchiesSpriteAnimationCache()->removeAnimation(spriteAnimationName);
        CC_SAFE_RELEASE(this);
        return false;
	}
    
    float duration = (float)(_currentAnimation.endFrameIndex - _currentAnimation.startFrameIndex + 1) / _spriteAnimation->getFrameRate();
    
    if (CCActionInterval::initWithDuration(duration)) {
        _animationName = animationName;
        _spriteAnimationName = spriteAnimationName;
        return true;
    }
    else {
        CC_SAFE_RELEASE(this);
        return false;
    }
}

bool CCHierarchiesAnimate::initWholeAnimation (const char* spriteAnimationName)
{
    CCAssert(spriteAnimationName != NULL, "CCHierarchiesAnimate: arguments must be non-nil");
    
    _spriteAnimation = CCHierarchiesSpriteAnimationCache::sharedHierarchiesSpriteAnimationCache()->addAnimation(spriteAnimationName);
    
    _currentAnimation.name = HIERARCHIES_WHOLE_ANIMATION_NAME;
    _currentAnimation.startFrameIndex = 0;
    _currentAnimation.endFrameIndex = _spriteAnimation->getFrameCount() - 1;
    
    float duration = (float)(_currentAnimation.endFrameIndex - _currentAnimation.startFrameIndex + 1) / _spriteAnimation->getFrameRate();
    
    if (CCActionInterval::initWithDuration(duration)) {
        _animationName = HIERARCHIES_WHOLE_ANIMATION_NAME;
        _spriteAnimationName = spriteAnimationName;
        return true;
    }
    else {
        CC_SAFE_RELEASE(this);
        return false;
    }
}

const char* CCHierarchiesAnimate::getAnimationName ()
{
    return _animationName.c_str();
}

const char* CCHierarchiesAnimate::getSpriteAnimationName ()
{
    return _spriteAnimationName.c_str();
}

void CCHierarchiesAnimate::startWithTarget (CCNode* pTarget)
{
    CCHierarchiesSpriteBase* target = dynamic_cast<CCHierarchiesSpriteBase*>(pTarget);
    CCAssert(target != NULL, "CCHierarchiesAnimate can only valid on CCHierarchiesSpriteBase class");
    
    CCAssert(_spriteAnimationName.compare(target->getAnimationName()) == 0, "CCHierarchiesAnimate effect on a invalid CCHierarchiesSpriteBase instance");
    
    CCActionInterval::startWithTarget(pTarget);
    
    _curFrameIndex = _currentAnimation.startFrameIndex;
    target->displayFrameAtIndex(_curFrameIndex);
}

CCActionInterval* CCHierarchiesAnimate::reverse(void)
{
    CCHierarchiesAnimate* animate = NULL;
    if (_animationName == HIERARCHIES_WHOLE_ANIMATION_NAME) {
        animate = CCHierarchiesAnimate::createWholeAnimation(_spriteAnimationName.c_str());
    }
    else {
        animate = CCHierarchiesAnimate::create(_animationName.c_str(), _spriteAnimationName.c_str());
    }
    unsigned int tmp = animate->_currentAnimation.endFrameIndex;
    animate->_currentAnimation.endFrameIndex = animate->_currentAnimation.startFrameIndex;
    animate->_currentAnimation.startFrameIndex = tmp;
    return animate;
}

CCObject* CCHierarchiesAnimate::copyWithZone (CCZone* pZone)
{
    CCZone* pNewZone = NULL;
    CCHierarchiesAnimate* pCopy = NULL;
    if(pZone && pZone->m_pCopyObject)
    {
        //in case of being called at sub class
        pCopy = dynamic_cast<CCHierarchiesAnimate*>(pZone->m_pCopyObject);
    }
    else
    {
        pCopy = new CCHierarchiesAnimate();
        pZone = pNewZone = new CCZone(pCopy);
    }
    
    CCActionInterval::copyWithZone(pZone); // call super class init once
    pCopy->initWithName(_animationName.c_str(), _spriteAnimationName.c_str()); // call super class init twice ??
    CC_SAFE_DELETE(pNewZone);
    return pCopy;
    
//    if (pCopy->initWithName(_animationName.c_str(), _spriteAnimationName.c_str())) {
//        CC_SAFE_DELETE(pNewZone);
//        return pCopy;
//    }
//    else {
//        CC_SAFE_DELETE(pNewZone);
//        return NULL;
//    }
}

void CCHierarchiesAnimate::update (float time)
{
    CCHierarchiesSpriteBase* target = dynamic_cast<CCHierarchiesSpriteBase*>(m_pTarget);
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


#pragma mark - CCHierarchiesFlipX

CCHierarchiesFlipX* CCHierarchiesFlipX::create (bool x)
{
    CCHierarchiesFlipX* ret = new CCHierarchiesFlipX();
    
    if (ret && ret->initWithFlipX(x)) {
        ret->autorelease();
        return ret;
    }

    CC_SAFE_DELETE(ret);
    return NULL;
}

bool CCHierarchiesFlipX::initWithFlipX (bool x)
{
    _flipX = x;
    return true;
}

void CCHierarchiesFlipX::update(float time)
{
    CC_UNUSED_PARAM(time);
    
    CCHierarchiesSpriteBase* target = dynamic_cast<CCHierarchiesSpriteBase*>(m_pTarget);
    CCAssert(target, "CCHierarchiesFlipX only valid on CCHierarchiesSpriteBase class");
    target->setFlipX(_flipX);
}

CCFiniteTimeAction* CCHierarchiesFlipX::reverse()
{
    return CCHierarchiesFlipX::create(!_flipX);
}

CCObject * CCHierarchiesFlipX::copyWithZone(CCZone* zone)
{
    CCZone* newZone = NULL;
    CCHierarchiesFlipX* ret = NULL;
    
    if (zone && zone->m_pCopyObject) {
        ret = (CCHierarchiesFlipX*) (zone->m_pCopyObject);
    } else {
        ret = new CCHierarchiesFlipX();
        zone = newZone = new CCZone(ret);
    }
    
    CCActionInstant::copyWithZone(zone);
    ret->initWithFlipX(_flipX);
    CC_SAFE_DELETE(newZone);
    return ret;
}


#pragma mark - CCHierarchiesFlipY

CCHierarchiesFlipY* CCHierarchiesFlipY::create (bool y)
{
    CCHierarchiesFlipY* ret = new CCHierarchiesFlipY();
    
    if (ret && ret->initWithFlipY(y)) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return NULL;
}

bool CCHierarchiesFlipY::initWithFlipY (bool y)
{
    _flipY = y;
    return true;
}

void CCHierarchiesFlipY::update(float time)
{
    CC_UNUSED_PARAM(time);
    
    CCHierarchiesSpriteBase* target = dynamic_cast<CCHierarchiesSpriteBase*>(m_pTarget);
    CCAssert(target, "CCHierarchiesFlipY only valid on CCHierarchiesSpriteBase class");
    target->setFlipY(_flipY);
}

CCFiniteTimeAction* CCHierarchiesFlipY::reverse()
{
    return CCHierarchiesFlipY::create(!_flipY);
}

CCObject * CCHierarchiesFlipY::copyWithZone(CCZone* zone)
{
    CCZone* newZone = NULL;
    CCHierarchiesFlipY* ret = NULL;
    
    if (zone && zone->m_pCopyObject) {
        ret = (CCHierarchiesFlipY*) (zone->m_pCopyObject);
    } else {
        ret = new CCHierarchiesFlipY();
        zone = newZone = new CCZone(ret);
    }
    
    CCActionInstant::copyWithZone(zone);
    ret->initWithFlipY(_flipY);
    CC_SAFE_DELETE(newZone);
    return ret;
}


#pragma mark - CCHierarchiesAvatarMapInsert

CCHierarchiesAvatarMapInsert* CCHierarchiesAvatarMapInsert::create (const char* avatarMapFrom, const char* avatarMapTo)
{
    CCHierarchiesAvatarMapInsert* ret = new CCHierarchiesAvatarMapInsert();
    
    if (ret && ret->init(avatarMapFrom, avatarMapTo)) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return NULL;
}

bool CCHierarchiesAvatarMapInsert::init (const char* avatarMapFrom, const char* avatarMapTo)
{
    _avatarMapFrom = avatarMapFrom;
    _avatarMapTo = avatarMapTo;
    return true;
}

void CCHierarchiesAvatarMapInsert::update (float time)
{
    CC_UNUSED_PARAM(time);
    
    CCHierarchiesSpriteDynamic* target = dynamic_cast<CCHierarchiesSpriteDynamic*>(m_pTarget);
    CCAssert(target, "CCHierarchiesAvatarMapInsert only valid on CCHierarchiesSpriteDynamic class");
    target->setAvatarMap(_avatarMapFrom.c_str(), _avatarMapTo.c_str());
}

CCObject* CCHierarchiesAvatarMapInsert::copyWithZone (CCZone *zone)
{
    CCZone* newZone = NULL;
    CCHierarchiesAvatarMapInsert* ret = NULL;
    
    if (zone && zone->m_pCopyObject) {
        ret = (CCHierarchiesAvatarMapInsert*) (zone->m_pCopyObject);
    } else {
        ret = new CCHierarchiesAvatarMapInsert();
        zone = newZone = new CCZone(ret);
    }
    
    CCActionInstant::copyWithZone(zone);
    ret->init(_avatarMapFrom.c_str(), _avatarMapTo.c_str());
    CC_SAFE_DELETE(newZone);
    return ret;
}


#pragma mark - CCHierarchiesAvatarMapReset

CCHierarchiesAvatarMapReset* CCHierarchiesAvatarMapReset::create ()
{
    CCHierarchiesAvatarMapReset* ret = new CCHierarchiesAvatarMapReset();
    
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return NULL;
}

bool CCHierarchiesAvatarMapReset::init ()
{
    return true;
}

void CCHierarchiesAvatarMapReset::update (float time)
{
    CC_UNUSED_PARAM(time);
    
    CCHierarchiesSpriteDynamic* target = dynamic_cast<CCHierarchiesSpriteDynamic*>(m_pTarget);
    CCAssert(target, "CCHierarchiesAvatarMapReset only valid on CCHierarchiesSpriteDynamic class");
    target->resetAvatarMap();
}

CCObject* CCHierarchiesAvatarMapReset::copyWithZone (CCZone *zone)
{
    CCZone* newZone = NULL;
    CCHierarchiesAvatarMapReset* ret = NULL;
    
    if (zone && zone->m_pCopyObject) {
        ret = (CCHierarchiesAvatarMapReset*) (zone->m_pCopyObject);
    } else {
        ret = new CCHierarchiesAvatarMapReset();
        zone = newZone = new CCZone(ret);
    }
    
    CCActionInstant::copyWithZone(zone);
    ret->init();
    CC_SAFE_DELETE(newZone);
    return ret;
}


#pragma mark - CCHierarchiesAvatarMapSet

CCHierarchiesAvatarMapSet* CCHierarchiesAvatarMapSet::create (const AvatarMapType& avatarMap)
{
    CCHierarchiesAvatarMapSet* ret = new CCHierarchiesAvatarMapSet();
    
    if (ret && ret->init(avatarMap)) {
        ret->autorelease();
        return ret;
    }
    
    CC_SAFE_DELETE(ret);
    return NULL;
}

bool CCHierarchiesAvatarMapSet::init (const AvatarMapType& avatarMap)
{
    _avatarMap = avatarMap;
    return true;
}

void CCHierarchiesAvatarMapSet::update (float time)
{
    CC_UNUSED_PARAM(time);
    
    CCHierarchiesSpriteDynamic* target = dynamic_cast<CCHierarchiesSpriteDynamic*>(m_pTarget);
    CCAssert(target, "CCHierarchiesAvatarMapSet only valid on CCHierarchiesSpriteDynamic class");
    target->resetAvatarMap();
    AvatarMapType::const_iterator avatarItemIter;
    for (avatarItemIter = _avatarMap.cbegin(); avatarItemIter != _avatarMap.cend(); avatarItemIter++) {
        target->setAvatarMap(avatarItemIter->first.c_str(), avatarItemIter->second.c_str());
    }
}

CCObject* CCHierarchiesAvatarMapSet::copyWithZone (CCZone *zone)
{
    CCZone* newZone = NULL;
    CCHierarchiesAvatarMapSet* ret = NULL;
    
    if (zone && zone->m_pCopyObject) {
        ret = (CCHierarchiesAvatarMapSet*) (zone->m_pCopyObject);
    } else {
        ret = new CCHierarchiesAvatarMapSet();
        zone = newZone = new CCZone(ret);
    }
    
    CCActionInstant::copyWithZone(zone);
    ret->init(_avatarMap);
    CC_SAFE_DELETE(newZone);
    return ret;
}


NS_CC_EXT_END
