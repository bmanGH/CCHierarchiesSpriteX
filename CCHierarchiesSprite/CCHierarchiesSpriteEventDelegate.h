//
//  CCHierarchiesSpriteEventDelegate.h
//  CCHierarchiesSprite
//
//  Created by bman <zx123xz321hm3@hotmail.com>.
//  Copyright (c) 2013. All rights reserved.
//

#ifndef _CCHierarchiesSpriteEventDelegatee_H_
#define _CCHierarchiesSpriteEventDelegatee_H_

#include "platform/CCPlatformMacros.h"
#include "ExtensionMacros.h"


NS_CC_EXT_BEGIN

class CCHierarchiesSpriteBase;

class CC_DLL CCHierarchiesSpriteEventDelegate {
	
public:
    virtual ~CCHierarchiesSpriteEventDelegate () {};
    
	virtual void onEventContent (CCHierarchiesSpriteBase* sprite, const char* eventContent) = 0;
	
};

NS_CC_EXT_END

#endif
