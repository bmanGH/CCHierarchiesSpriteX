//
//  CCHierarchiesSpriteBase.h
//  CCHierarchiesSprite
//
//  version 0.1   12/22/2011
//  version 0.2   12/29/2011
//  version 0.3   12/31/2011
//  version 0.4   1/13/2012   support color transform
//  version 0.5   1/16/2012   support alpha transform and cook guide layer animation to key frames
//  version 0.6   2/3/2012    use item index to identification element instead of name; add library items' left and bottom properties
//  version 0.7   2/10/2012   add avatar, dyeing, event functions
//  version 0.8   2/27/2012   add single texture batch mode function
//  version 0.9   2/28/2012   add CCHierarchiesSpriteSheetCache, CCHierarchiesAnimationCache
//  version 0.9.2 3/22/2012   fix copy string without '\0' bug
//  version 0.10  3/23/2012   add multi-thread animation in batch mode
//  version 0.10.1   3/26/2012   export .sprites file and items image automatic
//  version 0.10.2   3/26/2012   chinese localization
//  version 0.10.3   3/26/2012   support set .sprites, .anims file, set batch after init
//  version 0.11     3/28/2012   support multi texture packer in batch mode
//  version 0.11.1   4/17/2012   support retina sprites file export
//  version 0.11.2   5/21/2012   hack export .sprites precision bug
//  version 0.12     6/5/2012    replace libxml2 with rapidxml
//  version 0.12.1   6/5/2012    add setAvatarTag and setAvatarTags to CCHierarchiesSprite class
//  version 0.12.2   6/6/2012    fix no-display bug in release build configure
//  version 0.13     11/6/2012   separate version between .hanims file format and runtime code
//  version 0.14   11/6/2012   add an config option that only support one element in keyframe for simplify and performance, so the depth property in element is also removed
//  version 0.14.1 21/6/2012   add batch z order scale and clear depth buffer between draw
//  version 0.14.5 25/6/2012   add set all texture params used by CCHierarchiesSpriteBatchNode
//  version 0.15   25/6/2012   draw sprites twice in CCHierarchiesBatchNode (1:draw opaque sprites by enable alpha test and enable depth write 2:draw transparent sprite by enable blend and disable depth write)
//  version 0.16   26/6/2012   simplify batch mode convert just by add child to batch node
//  version 0.16.1 27/6/2012   add z order include sprite in batch mode
//  version 0.16.5 27/6/2012   add hierarchies sprite's PositionTextureColorAlphaTest fragment shader
//  version 0.17   28/6/2012   add global runtime animation cache instead of runtime animation cache only in batch
//  version 0.17.1 9/7/2012    lerp frames only there are one element in both frames
//  ======================================================================================
//  version 0.18x   30/8/2012   port from cocos2d-iphone to cocos2d-x without batch mode
//  version 0.20.1x   7/6/2013   upgrade to cocos2d-x v2.1.4, support rotation sprites
//  version 0.20.2x 7/19/2013   fix wrong anchor point with flip bug
//  version 0.20.3x 8/1/2013    add 'getEventContentCount' function
//  version 0.30x   8/13/2013   add support nesting hierarchies sprite
//  version 0.40x   8/19/2013   add support filter
//  version 0.41x   9/27/2013   merge sub sprites to one animation file, move item's left and bottom property to animation file, rename .sprites to .hsheet
//  version 0.42x   9/30/2013   use abbreviation xml tag
//  version 0.43x   12/3/2013   remove setSpriteFile functions
//  version 0.50x   12/3/2013   add dynamic sprite type to calculate quads in runtime, add avatar support
//  version 0.51x   12/6/2013   add reverse animation play, flip, avatar actions
//
//  Created by bman <zx123xz321hm3@hotmail.com>.
//  Copyright (c) 2013. All rights reserved.
//

#ifndef __CCHierarchiesSpriteXDemo__CCHierarchiesSpriteBase__
#define __CCHierarchiesSpriteXDemo__CCHierarchiesSpriteBase__

#include "cocos2d.h"
#include "ExtensionMacros.h"
#include "CCHierarchiesSpriteConfig.h"
#include <unordered_map>
#include <string>
#include <limits.h>
#include "CCHierarchiesSpriteSheet.h"
#include "CCHierarchiesSpriteAnimation.h"
#include "CCHierarchiesSpriteEventDelegate.h"
#include "CCHierarchiesSpriteShader.h"
#include "CCHierarchiesSpriteFilter.h"
#include "CCHierarchiesSpriteMesh.h"


NS_CC_EXT_BEGIN

#define HIERARCHIES_SPRITE_SUPPORT_HANIMS_FILE_FORMAT_VERSION "0.29"
#define HIERARCHIES_SPRITE_RUNTIME_VERSION "0.51x"


// Math
extern void hierarchiesCalcMatrixSocket (CCHierarchiesSpriteAnimation::Element* element, CCAffineTransform* matrix);
extern void hierarchiesCalcMatrix (const CCHierarchiesSpriteAnimation::Symbol* symbol,
                                   const CCHierarchiesSpriteSheet::Spr* spr,
                                   const CCHierarchiesSpriteAnimation::Element* element,
                                   CCAffineTransform* matrix);
extern void hierarchiesCalcMatrixSocket (const CCHierarchiesSpriteAnimation::Element* element,
                                         CCAffineTransform* matrix);
extern void hierarchiesUpdateQuadVertices (CCSize size,
                                           const CCAffineTransform* matrix,
                                           CCHierarchiesSprite_V3F_C4B_T2F_Quad* quad,
                                           const float z,
                                           const bool isRotation);
extern void hierarchiesUpdateQuadTextureCoords (const CCRect rect,
                                                const float texWidth,
                                                const float texHeight,
                                                CCHierarchiesSprite_V3F_C4B_T2F_Quad* quad,
                                                const bool isRotation);
extern void hierarchiesUpdateQuadTextureColorFromAnimation (const float alpha_percent, const int alpha_amount,
                                                            const float red_percent, const int red_amount,
                                                            const float green_percent, const int green_amount,
                                                            const float blue_percent, const int blue_amount,
                                                            CCHierarchiesSprite_V3F_C4B_T2F_Quad* quad);
extern void hierarchiesUpdateQuadTextureColor (const bool opacityModifyRGB,
                                               const int opacity,
                                               const int color_r,
                                               const int color_g,
                                               const int color_b,
                                               CCHierarchiesSprite_V3F_C4B_T2F_Quad* quad);
extern void hierarchiesExpandRectByPoint (float* minX,
                                         float* maxX,
                                         float* minY,
                                         float* maxY,
                                         float* pX,
                                         float* pY);
extern float float_round (float r);


// CCHierarchiesSpriteBase

typedef std::unordered_map<std::string, std::string> AvatarMapType;

class CCHierarchiesSpriteBase : public CCNodeRGBA, public CCTextureProtocol {
    
public:
    
    struct SubSpriteSocket {
        // if this value is not same as the frame index in displayFrameAtIndex then remove the sub sprite
        unsigned int lastDisplayFrameIndex;
        CCHierarchiesSpriteBase* subSprite;
        std::vector<CCHierarchiesSpriteFilter*> filters;
    };
    
    struct DisplayElement {
        CCHierarchiesSpriteAnimation::Element socketElement;
        unsigned int quadsIndex;
        unsigned int quadsCount;// 0 means it's a sub sprite element
        
        DisplayElement ()
        : quadsIndex(0), quadsCount(0) {
        }
        
        DisplayElement (const DisplayElement& copy) {
            this->socketElement = copy.socketElement;
            this->quadsIndex = copy.quadsIndex;
            this->quadsCount = copy.quadsCount;
        }
        
        DisplayElement& operator= (const DisplayElement& rhs) {
            this->socketElement = rhs.socketElement;
            this->quadsIndex = rhs.quadsIndex;
            this->quadsCount = rhs.quadsCount;
            return *this;
        }
    };
	
	CC_SYNTHESIZE_READONLY(CCHierarchiesSpriteSheet*, _sheet, Sheet)
	CC_SYNTHESIZE_READONLY(CCHierarchiesSpriteAnimation*, _animation, Animation)
	
	CC_SYNTHESIZE_READONLY(CCHierarchiesSpriteMesh*, _mesh, Mesh)
	
	CC_SYNTHESIZE(CCHierarchiesSpriteEventDelegate*, _delegate, Delegate) // weak ref
	
protected:
    std::string _sheetName;
    std::string _animationName;
    std::string _animationNameBase;
    std::string _animationNameSub;
    
    CCTexture2D* _texture;
    
//    std::unordered_map<int, SubSpriteSocket> _subSprites; // key is element EUID
    
    std::vector<DisplayElement> _displayList;
    
    unsigned int _frameStartQuadIndex;
    
    unsigned int _curFrameIndex;
	bool _needFresh;
    
	ccBlendFunc _blendFunc;
	bool _opacityModifyRGB;
    
    bool _flipX;
    bool _flipY;
    CCRect _bbox;
    
    GLint _uniformColorLocation;
	
protected:
	virtual void updateOpacityModifyRGB ();
	virtual void updateBlendFunc ();
//    virtual void updateSocketElements ();
//    virtual void updateSocketFilters ();
    virtual void setup ();
    virtual void shutdown ();
    
    CCHierarchiesSpriteBase ();
    virtual ~CCHierarchiesSpriteBase ();
	
public:
	virtual bool initWithFile (const char* sheetFileName,
                               const char* animationFileName,
                               CCHierarchiesSpriteEventDelegate* delegate);
    virtual bool initWithFile (const char* sheetFileName,
                               const char* animationFileNameBase,
                               const char* animationFileNameSub,
                               CCHierarchiesSpriteEventDelegate* delegate);
    virtual bool initWithFileAndAvatar (const char* sheetFileName,
                                        const char* animationFileName,
                                        CCHierarchiesSpriteEventDelegate* delegate,
                                        const AvatarMapType& avatarMap);
    virtual bool initWithFileAndAvatar (const char* sheetFileName,
                                        const char* animationFileNameBase,
                                        const char* animationFileNameSub,
                                        CCHierarchiesSpriteEventDelegate* delegate,
                                        const AvatarMapType& avatarMap);
    
    virtual void onEnter ();
    virtual void onExit ();
	
	const char* getSheetName ();
	const char* getAnimationName ();
    const char* getAnimationNameBase ();
    const char* getAnimationNameSub ();
    unsigned int getEventContentCount (std::string eventContent);
    
    bool getFlipX ();
    void setFlipX (bool value);
    bool getFlipY ();
    void setFlipY (bool value);
	
	virtual bool displayFrameAtIndex (unsigned int frameIndex);
	bool freshCurrentFrame ();
    unsigned int getCurrentFrameIndex ();
    
    // CCNode
    virtual CCAffineTransform nodeToParentTransform ();
//    CCRect nestingBoundingBox ();
//    CCRect nestingBoundingBoxInner ();
	
	// CCBlendProtocol
	virtual void setBlendFunc (ccBlendFunc blendFunc);
	virtual ccBlendFunc getBlendFunc (void);
	
	// CCTextureProtocol
	virtual CCTexture2D* getTexture (void);
    virtual void setTexture (CCTexture2D *texture);
	
	// CCRGBAProtocol
	virtual void setColor (const ccColor3B& color);
    virtual void setOpacity (GLubyte opacity);
    virtual void updateDisplayedColor(const ccColor3B& color);
    virtual void updateDisplayedOpacity(GLubyte opacity);
    virtual void setOpacityModifyRGB (bool bValue);
    virtual bool isOpacityModifyRGB (void);
    
};

NS_CC_EXT_END

#endif /* defined(__CCHierarchiesSpriteXDemo__CCHierarchiesSpriteBase__) */
