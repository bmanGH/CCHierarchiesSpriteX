//
//  CCHierarchiesSpriteDynamic.h
//  CCHierarchiesSprite
//
//  Created by bman <zx123xz321hm3@hotmail.com>.
//  Copyright (c) 2013. All rights reserved.
//

#ifndef __CCHierarchiesSpriteXDemo__CCHierarchiesSpriteDynamic__
#define __CCHierarchiesSpriteXDemo__CCHierarchiesSpriteDynamic__

#include "CCHierarchiesSpriteBase.h"


NS_CC_EXT_BEGIN

class CC_DLL CCHierarchiesSpriteDynamic : public CCHierarchiesSpriteBase {
    
protected:
    AvatarMapType _avatarMap;
    std::vector<CCHierarchiesSprite_V3F_C4B_T2F_Quad> _quads;
    std::vector<GLushort> _indices;
    
    void buildDynamicAnimationData (CCHierarchiesSpriteAnimation::ElementLoopMode loopMode,
                                   int frameOffset,
                                   unsigned int frameIndex,
                                   const CCAffineTransform& parentMatrix,
                                   const CCHierarchiesSpriteAnimation* animation,
                                   float& min_X, float& max_X, float& min_Y, float& max_Y,
                                   const float parent_alpha_percent, const int parent_alpha_amount,
                                   const float parent_red_percent, const int parent_red_amount,
                                   const float parent_green_percent, const int parent_green_amount,
                                   const float parent_blue_percent, const int parent_blue_amount);
    
public:
    CCHierarchiesSpriteDynamic ();
    virtual ~CCHierarchiesSpriteDynamic ();
    
	static CCHierarchiesSpriteDynamic* create (const char* sheetFileName,
                                        const char* animationFileName,
                                        CCHierarchiesSpriteEventDelegate* delegate);
    static CCHierarchiesSpriteDynamic* create (const char* sheetFileName,
                                        const char* animationFileNameBase,
                                        const char* animationFileNameSub,
                                        CCHierarchiesSpriteEventDelegate* delegate);
    static CCHierarchiesSpriteDynamic* create (const char* sheetFileName,
                                        const char* animationFileName,
                                        CCHierarchiesSpriteEventDelegate* delegate,
                                        const AvatarMapType& avatarMap);
    static CCHierarchiesSpriteDynamic* create (const char* sheetFileName,
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
	virtual bool displayFrameAtIndex (unsigned int frameIndex);
    
    // Draw
    virtual void draw ();
    
    // Avatar
	bool setAvatarMap (const char* name, const char* mapName);
    bool getAvatarMap (const char* name, std::string& mapName);
    void resetAvatarMap ();
    
};

NS_CC_EXT_END


#endif /* defined(__CCHierarchiesSpriteXDemo__CCHierarchiesSpriteDynamic__) */
