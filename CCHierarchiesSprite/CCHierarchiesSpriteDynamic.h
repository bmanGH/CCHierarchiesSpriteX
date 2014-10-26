//
//  CCHierarchiesSpriteDynamic.h
//  HierarchiesSpriteDemo
//
//  Created by xuxiaocheng on 10/23/14.
//
//

#ifndef __HierarchiesSpriteDemo__CCHierarchiesSpriteDynamic__
#define __HierarchiesSpriteDemo__CCHierarchiesSpriteDynamic__

#include "cocos2d.h"
#include "ExtensionMacros.h"
#include <unordered_map>
#include <string>
#include <limits.h>
#include "CCHierarchiesSpriteSheet.h"
#include "CCHierarchiesSpriteAnimation.h"
#include "CCHierarchiesSpriteShader.h"
#include "CCHierarchiesSpriteMesh.h"


NS_CC_EXT_BEGIN


// Math
extern void hierarchiesCalcMatrixSocket (HierarchiesSpriteAnimation::Element* element, AffineTransform* matrix);
extern void hierarchiesCalcMatrix (const HierarchiesSpriteAnimation::Symbol* symbol,
                                   const HierarchiesSpriteSheet::Spr* spr,
                                   const HierarchiesSpriteAnimation::Element* element,
                                   AffineTransform* matrix);
extern void hierarchiesCalcMatrixSocket (const HierarchiesSpriteAnimation::Element* element,
                                         AffineTransform* matrix);
extern void hierarchiesUpdateQuadVertices (CCSize size,
                                           const CCAffineTransform* matrix,
                                           HierarchiesSprite_V3F_C4B_T2F_Quad* quad,
                                           const float z,
                                           const bool isRotation);
extern void hierarchiesUpdateQuadTextureCoords (const Rect rect,
                                                const float texWidth,
                                                const float texHeight,
                                                HierarchiesSprite_V3F_C4B_T2F_Quad* quad,
                                                const bool isRotation);
extern void hierarchiesUpdateQuadTextureColorFromAnimation (const float alpha_percent, const int alpha_amount,
                                                            const float red_percent, const int red_amount,
                                                            const float green_percent, const int green_amount,
                                                            const float blue_percent, const int blue_amount,
                                                            HierarchiesSprite_V3F_C4B_T2F_Quad* quad);
extern void hierarchiesUpdateQuadTextureColor (const bool opacityModifyRGB,
                                               const int opacity,
                                               const int color_r,
                                               const int color_g,
                                               const int color_b,
                                               HierarchiesSprite_V3F_C4B_T2F_Quad* quad);
extern void hierarchiesExpandRectByPoint (float* minX,
                                          float* maxX,
                                          float* minY,
                                          float* maxY,
                                          float* pX,
                                          float* pY);
extern float float_round (float r);


// CCHierarchiesSprite
class CC_DLL CCHierarchiesSprite : public Node, public TextureProtocol {
    
public:
    typedef std::unordered_map<std::string, std::string> AvatarMapType;
    
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
    
    std::string _cacheKey; // key for mesh data cache (use in CCHierarchiesSpriteRuntime)
    
public:
    CCHierarchiesSprite ();
    virtual ~CCHierarchiesSprite ();
    
    static CCHierarchiesSprite* create (const char* sheetFileName,
                                        const char* animationFileName);
    static CCHierarchiesSprite* create (const char* sheetFileName,
                                        const char* animationFileNameBase,
                                        const char* animationFileNameSub);
    static CCHierarchiesSprite* create (const char* sheetFileName,
                                        const char* animationFileName,
                                        const AvatarMapType& avatarMap);
    static CCHierarchiesSprite* create (const char* sheetFileName,
                                        const char* animationFileNameBase,
                                        const char* animationFileNameSub,
                                        const AvatarMapType& avatarMap);
    
    virtual bool initWithFileAndAvatar (const char* sheetFileName,
                                        const char* animationFileNameBase,
                                        const char* animationFileNameSub,
                                        const AvatarMapType& avatarMap);
    
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
    
    // Animation
    bool displayFrameAtIndex (unsigned int frameIndex);
    
    // Draw
    virtual void draw ();
    
};

NS_CC_EXT_END

#endif /* defined(__HierarchiesSpriteDemo__CCHierarchiesSpriteDynamic__) */
