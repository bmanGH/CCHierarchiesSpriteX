//
//  CCHierarchiesSprite.h
//  CCHierarchiesSprite
//
//  Created by bman <zx123xz321hm3@hotmail.com>.
//  Copyright (c) 2013. All rights reserved.
//

#ifndef _CCHierarchiesSprite_H_
#define _CCHierarchiesSprite_H_

#include "CCHierarchiesSpriteBase.h"


NS_CC_EXT_BEGIN

class CCHierarchiesSpriteRuntime;

class CC_DLL CCHierarchiesSprite : public CCHierarchiesSpriteBase {
    
protected:
    std::string _cacheKey; // key for mesh data cache (use in CCHierarchiesSpriteRuntime)
	
public:
    CCHierarchiesSprite ();
    virtual ~CCHierarchiesSprite ();
	
	static CCHierarchiesSprite* create (const char* sheetFileName,
                                        const char* animationFileName,
                                        CCHierarchiesSpriteEventDelegate* delegate);
    static CCHierarchiesSprite* create (const char* sheetFileName,
                                        const char* animationFileNameBase,
                                        const char* animationFileNameSub,
                                        CCHierarchiesSpriteEventDelegate* delegate);
    static CCHierarchiesSprite* create (const char* sheetFileName,
                                        const char* animationFileName,
                                        CCHierarchiesSpriteEventDelegate* delegate,
                                        const AvatarMapType& avatarMap);
    static CCHierarchiesSprite* create (const char* sheetFileName,
                                        const char* animationFileNameBase,
                                        const char* animationFileNameSub,
                                        CCHierarchiesSpriteEventDelegate* delegate,
                                        const AvatarMapType& avatarMap);
	
    virtual bool initWithFileAndAvatar (const char* sheetFileName,
                                        const char* animationFileNameBase,
                                        const char* animationFileNameSub,
                                        CCHierarchiesSpriteEventDelegate* delegate,
                                        const AvatarMapType& avatarMap);
    
    // Animation
    bool displayFrameAtIndex (unsigned int frameIndex);
    
    // Draw
	virtual void draw ();
    
    friend class CCHierarchiesSpriteRuntime;

};

NS_CC_EXT_END

#endif

