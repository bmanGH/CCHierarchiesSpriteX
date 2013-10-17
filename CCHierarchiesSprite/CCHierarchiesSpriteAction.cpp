//
//  CCHierarchiesSpriteAction.cpp
//  CCHierarchiesSprite
//
//  Created by bman <zx123xz321hm3@hotmail.com>.
//  Copyright (c) 2013. All rights reserved.
//

#include "CCHierarchiesSpriteAction.h"
#include "CCHierarchiesSprite.h"
#include "CCHierarchiesSpriteAnimationCache.h"


NS_CC_EXT_BEGIN

#pragma mark - CCHierarchiesAnimate

CCHierarchiesAnimate* CCHierarchiesAnimate::create (const char* animationName,
                                                    const char* spriteAnimationName) {
    CCHierarchiesAnimate* ret = new CCHierarchiesAnimate();
	if (ret->initWithName(animationName, spriteAnimationName))
	{
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);
	return NULL;
}

CCHierarchiesAnimate* CCHierarchiesAnimate::createWholeAnimation (const char* spriteAnimationName) {
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
: _spriteAnimation(NULL), _curFrameIndex(0) {
}

CCHierarchiesAnimate::~CCHierarchiesAnimate () {
    CCHierarchiesSpriteAnimationCache::sharedHierarchiesSpriteAnimationCache()->removeAnimation(_spriteAnimationName.c_str());
}

bool CCHierarchiesAnimate::initWithName (const char* animationName,
                                         const char* spriteAnimationName) {
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

bool CCHierarchiesAnimate::initWholeAnimation (const char* spriteAnimationName) {
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

const char* CCHierarchiesAnimate::getAnimationName () {
    return _animationName.c_str();
}

const char* CCHierarchiesAnimate::getSpriteAnimationName () {
    return _spriteAnimationName.c_str();
}

void CCHierarchiesAnimate::startWithTarget (CCNode* pTarget) {
    CCHierarchiesSprite* target = dynamic_cast<CCHierarchiesSprite*>(pTarget);
    CCAssert(target != NULL, "CCHierarchiesAnimate can only effect on CCHierarchiesSprite");
    
    CCAssert(_spriteAnimationName.compare(target->getAnimationName()) == 0, "CCHierarchiesAnimate effect on a invalid CCHierarchiesSprite instance");
    
    CCActionInterval::startWithTarget(pTarget);
    
    _curFrameIndex = _currentAnimation.startFrameIndex;
    target->displayFrameAtIndex(_curFrameIndex);
}

CCObject* CCHierarchiesAnimate::copyWithZone (CCZone* pZone) {
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

void CCHierarchiesAnimate::update (float time) {
    CCHierarchiesSprite* target = dynamic_cast<CCHierarchiesSprite*>(m_pTarget);
    unsigned int curFrameIndex = _currentAnimation.startFrameIndex + time * (_currentAnimation.endFrameIndex - _currentAnimation.startFrameIndex);
    while (_curFrameIndex != curFrameIndex) {
        _curFrameIndex++;
        target->displayFrameAtIndex(_curFrameIndex);
    }
}

NS_CC_EXT_END
