//
//  CCHierarchiesSprite.h
//  HierarchiesSpriteDemo
//
//  Created by bman on 10/26/14.
//
//

#ifndef __HierarchiesSpriteDemo__CCHierarchiesSprite__
#define __HierarchiesSpriteDemo__CCHierarchiesSprite__

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
extern void hierarchiesCalcMatrix (const HierarchiesSpriteAnimation::Symbol* symbol,
                                   const HierarchiesSpriteSheet::Spr* spr,
                                   const HierarchiesSpriteAnimation::Element* element,
                                   AffineTransform* matrix);
extern void hierarchiesCalcMatrixSocket (const HierarchiesSpriteAnimation::Element* element,
                                         AffineTransform* matrix);
extern void hierarchiesUpdateQuadVertices (Size size,
                                           const AffineTransform* matrix,
                                           CCHierarchiesSprite_V3F_C4B_T2F_Quad* quad,
                                           const float z,
                                           const bool isRotation);
extern void hierarchiesUpdateQuadTextureCoords (const Rect rect,
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


// HierarchiesSprite
class HierarchiesSprite : public Node, public TextureProtocol {
    
public:
    struct DisplayElement {
        HierarchiesSpriteAnimation::Element socketElement;
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
    
    typedef std::unordered_map<std::string, std::string> AvatarMapType;
	
protected:
    std::string _sheetName;
    std::string _animationName;
    std::string _animationNameBase;
    std::string _animationNameSub;
    
    HierarchiesSpriteSheet* _sheet;
    HierarchiesSpriteAnimation* _animation;
    
    std::vector<DisplayElement> _displayList;
    
    unsigned int _frameStartQuadIndex;
    
    unsigned int _curFrameIndex;
	bool _needFresh;
    
	BlendFunc _blendFunc;
	bool _opacityModifyRGB;
    
    bool _flipX;
    bool _flipY;
    Rect _bbox;
    Color3B _colorAmount;
    
    GLint _uniformColorLocation;
    
	virtual void updateOpacityModifyRGB ();
	virtual void updateBlendFunc ();
	
CC_CONSTRUCTOR_ACCESS:
    HierarchiesSprite ();
    virtual ~HierarchiesSprite ();
	virtual bool initWithFile (const char* sheetFileName,
                               const char* animationFileName);
    virtual bool initWithFile (const char* sheetFileName,
                               const char* animationFileNameBase,
                               const char* animationFileNameSub);
    virtual bool initWithFileAndAvatar (const char* sheetFileName,
                                        const char* animationFileName,
                                        const AvatarMapType& avatarMap);
    virtual bool initWithFileAndAvatar (const char* sheetFileName,
                                        const char* animationFileNameBase,
                                        const char* animationFileNameSub,
                                        const AvatarMapType& avatarMap);
    
public:
	static HierarchiesSprite* create (const char* sheetFileName,
                                               const char* animationFileName);
    static HierarchiesSprite* create (const char* sheetFileName,
                                               const char* animationFileNameBase,
                                               const char* animationFileNameSub);
    static HierarchiesSprite* create (const char* sheetFileName,
                                               const char* animationFileName,
                                               const AvatarMapType& avatarMap);
    static HierarchiesSprite* create (const char* sheetFileName,
                                               const char* animationFileNameBase,
                                               const char* animationFileNameSub,
                                               const AvatarMapType& avatarMap);
    
    std::string getSheetName () {
        return _sheetName;
    }
    std::string getAnimationName () {
        return _animationName;
    }
    std::string getAnimationNameBase () {
        return _animationNameBase;
    }
    std::string getAnimationNameSub () {
        return _animationNameSub;
    }
    
    HierarchiesSpriteSheet* getSheet () {
        return _sheet;
    }
    void setSheet (HierarchiesSpriteSheet* value) {
        CC_SAFE_RETAIN(value);
        CC_SAFE_RELEASE(_sheet);
        _sheet = value;
    }
    HierarchiesSpriteAnimation* getAnimation () {
        return _animation;
    }
    void setAnimation (HierarchiesSpriteAnimation* value) {
        CC_SAFE_RETAIN(value);
        CC_SAFE_RELEASE(_animation);
        _animation = value;
    }
    
    unsigned int getEventCount (std::string eventName);
    
    bool getFlipX ();
    void setFlipX (bool value);
    bool getFlipY ();
    void setFlipY (bool value);
    
    Color3B getColorAmount ();
    void setColorAmount (const Color3B& color);
	
    // Animation
	virtual bool displayFrameAtIndex (unsigned int frameIndex);
	bool freshCurrentFrame ();
    unsigned int getCurrentFrameIndex ();
    
    // Avatar
	bool setAvatarMap (const char* name, const char* mapName);
    bool getAvatarMap (const char* name, std::string& mapName);
    void resetAvatarMap ();
    
    // Node
    virtual AffineTransform nodeToParentTransform ();
//    CCRect nestingBoundingBox ();
//    CCRect nestingBoundingBoxInner ();
	
	// BlendProtocol
	virtual void setBlendFunc (BlendFunc blendFunc);
	virtual BlendFunc getBlendFunc (void);
	
	// TextureProtocol
	virtual Texture2D* getTexture (void);
    virtual void setTexture (Texture2D *texture);
	
	// RGBAProtocol
	virtual void setColor (const Color3B& color);
    virtual void setOpacity (GLubyte opacity);
    virtual void updateDisplayedColor(const Color3B& color);
    virtual void updateDisplayedOpacity(GLubyte opacity);
    virtual void setOpacityModifyRGB (bool bValue);
    virtual bool isOpacityModifyRGB (void);
    
};


NS_CC_EXT_END

#endif /* defined(__HierarchiesSpriteDemo__CCHierarchiesSprite__) */
