//
//  CCHierarchiesSpriteBase.h
//  HierarchiesSpriteDemo
//
//  Created by xuxiaocheng on 10/23/14.
//
//

#ifndef __HierarchiesSpriteDemo__CCHierarchiesSpriteBase__
#define __HierarchiesSpriteDemo__CCHierarchiesSpriteBase__

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

#define HIERARCHIES_SPRITE_FILE_FORMAT_VERSION "2.0"
#define HIERARCHIES_SPRITE_RUNTIME_VERSION "2.0"


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

#endif /* defined(__HierarchiesSpriteDemo__CCHierarchiesSpriteBase__) */
