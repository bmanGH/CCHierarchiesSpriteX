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
#include "CCHierarchiesSpriteRuntime.h"
#include "CCHierarchiesSpriteSheet.h"
#include "CCHierarchiesSpriteAnimation.h"
#include "CCHierarchiesSpriteMesh.h"
#include "CCHierarchiesSpriteAction.h"

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
                                           HierarchiesSprite_V3F_C4B_T2F* quad,
                                           const float z,
                                           const bool isRotation);
extern void hierarchiesUpdateQuadTextureCoords (const Rect rect,
                                                const float texWidth,
                                                const float texHeight,
                                                HierarchiesSprite_V3F_C4B_T2F* quad,
                                                const bool isRotation);
extern void hierarchiesUpdateQuadTextureColorFromAnimation (const float alpha_percent, const int alpha_amount,
                                                            const float red_percent, const int red_amount,
                                                            const float green_percent, const int green_amount,
                                                            const float blue_percent, const int blue_amount,
                                                            HierarchiesSprite_V3F_C4B_T2F* quad);
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
//    struct DisplayElement {
//        HierarchiesSpriteAnimation::Element socketElement;
//        unsigned int quadsIndex;
//        unsigned int quadsCount;// 0 means it's a sub sprite element
//        
//        DisplayElement ()
//        : quadsIndex(0), quadsCount(0) {
//        }
//        
//        DisplayElement (const DisplayElement& copy) {
//            this->socketElement = copy.socketElement;
//            this->quadsIndex = copy.quadsIndex;
//            this->quadsCount = copy.quadsCount;
//        }
//        
//        DisplayElement& operator= (const DisplayElement& rhs) {
//            this->socketElement = rhs.socketElement;
//            this->quadsIndex = rhs.quadsIndex;
//            this->quadsCount = rhs.quadsCount;
//            return *this;
//        }
//    };
    
    typedef std::unordered_map<std::string, std::string> AvatarMapType;
	
protected:
    std::string _sheetName;
    std::string _animationName;
    std::string _animationNameBase;
    std::string _animationNameSub;
    
    HierarchiesSpriteSheet* _sheet;
    HierarchiesSpriteAnimation* _animation;
    
    AvatarMapType _avatarMap;
    
//    std::vector<DisplayElement> _displayList;
    
    unsigned int _frameStartQuadIndex;
    
    unsigned int _curFrameIndex;
    
	BlendFunc _blendFunc;
	bool _opacityModifyRGB;
    
    bool _flipX;
    bool _flipY;
    
    Color3B _displayColorAmount;
    
    Rect _bbox;
    bool _insideBounds;
    
    Texture2D* _texture;
    
    std::vector<HierarchiesSprite_V3F_C4B_T2F_Quad> _quads;
    std::vector<GLushort> _indices;
    Primitive* _primitive;
    PrimitiveCommand _renderCommand;
    
#if CC_SPRITE_DEBUG_DRAW
    DrawNode *_debugDrawNode;
#endif //CC_SPRITE_DEBUG_DRAW
	
CC_CONSTRUCTOR_ACCESS:
    HierarchiesSprite ();
    virtual ~HierarchiesSprite ();
    virtual bool init (const std::string& sheetFileName,
                       const std::string& animationFileNameBase,
                       const std::string& animationFileNameSub,
                       const AvatarMapType& avatarMap);
    
public:
	static HierarchiesSprite* create (const std::string& sheetFileName,
                                      const std::string& animationFileName);
    static HierarchiesSprite* create (const std::string& sheetFileName,
                                      const std::string& animationFileNameBase,
                                      const std::string& animationFileNameSub);
    static HierarchiesSprite* create (const std::string& sheetFileName,
                                      const std::string& animationFileName,
                                      const AvatarMapType& avatarMap);
    static HierarchiesSprite* create (const std::string& sheetFileName,
                                      const std::string& animationFileNameBase,
                                      const std::string& animationFileNameSub,
                                      const AvatarMapType& avatarMap);
    
    // property
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
    HierarchiesSpriteAnimation* getAnimation () {
        return _animation;
    }
    
    bool getFlippedX () {
        return _flipX;
    }
    void setFlippedX (bool value);
    bool getFlippedY () {
        return _flipY;
    }
    void setFlippedY (bool value);
    
    Color3B getColorAmount () {
        return _displayColorAmount;
    }
    void setColorAmount (const Color3B& value);
    
    unsigned int getEventCount (const std::string& eventName);
	
    // Animation
	virtual bool displayFrameAtIndex (unsigned int frameIndex);
	bool freshCurrentFrame ();
    unsigned int getCurrentFrameIndex ();
    
    // Avatar
	void setAvatarMap (const std::string& name, const std::string& mapName);
    void setAvatarMap (const AvatarMapType& map);
    std::string getAvatarMap (const std::string& name);
    void resetAvatarMap ();
    
    // Draw
    virtual void draw(Renderer *renderer, const Mat4& transform, uint32_t flags) override;
    
    // Override
    virtual const Mat4& getNodeToParentTransform() const override;
    
    virtual bool isOpacityModifyRGB() const override {
        return _opacityModifyRGB;
    }
    virtual void setOpacityModifyRGB(bool value) override;

    virtual const BlendFunc& getBlendFunc() const override {
        return _blendFunc;
    }
    virtual void setBlendFunc(const BlendFunc &blendFunc) override;
    
    virtual Texture2D* getTexture() const override {
        return _texture;
    }
    virtual void setTexture(Texture2D *texture) override;

protected:
	virtual void updateShader ();
	virtual void updateBlendFunc ();
    
    void buildAnimationData (HierarchiesSpriteAnimation::ElementLoopMode loopMode,
                             int frameOffset,
                             unsigned int frameIndex,
                             const AffineTransform& parentMatrix,
                             const HierarchiesSpriteAnimation* animation,
                             float& min_X, float& max_X, float& min_Y, float& max_Y,
                             const float parent_alpha_percent, const int parent_alpha_amount,
                             const float parent_red_percent, const int parent_red_amount,
                             const float parent_green_percent, const int parent_green_amount,
                             const float parent_blue_percent, const int parent_blue_amount);

    void buildVertexData ();

};


NS_CC_EXT_END

#endif /* defined(__HierarchiesSpriteDemo__CCHierarchiesSprite__) */
