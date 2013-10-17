//
//  SGHierarchiesSprite.h
//  SZSG
//
//  Created by xuxiaocheng on 9/3/13.
//
//

#ifndef __SZSG__SGHierarchiesSprite__
#define __SZSG__SGHierarchiesSprite__

#include "cocos2d.h"
#include "cocos-ext.h"

USING_NS_CC;
USING_NS_CC_EXT;


// Shader
#define kCCShader_SGHierarchiesSprite_GBufferProgram_Frag "kCCShader_SGHierarchiesSprite_GBufferProgram"
extern const GLchar* ccShader_SGHierarchiesSprite_GBufferProgram_frag;


class SGHierarchiesSprite : public CCHierarchiesSprite {
    
//    CC_SYNTHESIZE(ccColor3B, _glowColor, GlowColor);
//    CC_SYNTHESIZE(float, _depth, Depth);
//    CC_SYNTHESIZE(CCSize, _blurSize, BlurSize);
    
protected:
    CCGLProgram* _gBufferProgram;
    GLint _depthUniformLocation;
    
    virtual void updateSocketElements ();
    
public:
    static SGHierarchiesSprite* create (const char* sheetFileName,
                                        const char* animationFileName,
                                        CCHierarchiesSpriteEventDelegate* delegate = NULL);
    static SGHierarchiesSprite* create (const char* sheetFileName,
                                        const char* animationFileNameBase,
                                        const char* animationFileNameSub,
                                        CCHierarchiesSpriteEventDelegate* delegate = NULL);
    
    SGHierarchiesSprite ();
    virtual ~SGHierarchiesSprite ();
	
	virtual bool initWithFile (const char* sheetFileName,
                       const char* animationFileName,
                       CCHierarchiesSpriteEventDelegate* delegate = NULL);
    virtual bool initWithFile (const char* sheetFileName,
                       const char* animationFileNameBase,
                       const char* animationFileNameSub,
                       CCHierarchiesSpriteEventDelegate* delegate = NULL);
    
    void gBufferVisit (float depth);
    void gBufferDraw (float depth);
    
};

#endif /* defined(__SZSG__SGHierarchiesSprite__) */
