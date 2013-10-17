//
//  SGPostImage.h
//  SZSG
//
//  Created by xuxiaocheng on 8/30/13.
//
//

#ifndef __SZSG__SGPostImage__
#define __SZSG__SGPostImage__

#include "cocos2d.h"

USING_NS_CC;


// Shader

#define kCCShader_SGPostImage_EdgeDetection "kCCShader_SGPostImage_EdgeDetection"
extern const GLchar* ccShader_SGPostImage_EdgeDetection_vert;
extern const GLchar* ccShader_SGPostImage_EdgeDetection_frag;


// Class

class SGPostImage : public CCLayer {
    /** The CCSprite being used.
     The sprite, by default, will use the following blending function: GL_ONE, GL_ONE_MINUS_SRC_ALPHA.
     The blending function can be changed in runtime by calling:
     - [[renderTexture sprite] setBlendFunc:(ccBlendFunc){GL_ONE, GL_ONE_MINUS_SRC_ALPHA}];
     */
    CC_PROPERTY(CCSprite*, m_pSprite, Sprite)
public:
    SGPostImage();
    virtual ~SGPostImage();
    
    virtual void visit();
    virtual void draw();
    
    /** initializes a RenderTexture object with width and height in Points and a pixel format( only RGB and RGBA formats are valid ) and depthStencil format*/
    static SGPostImage * create(int w ,int h, CCTexture2DPixelFormat eFormat, GLuint uDepthStencilFormat);
    
    /** creates a RenderTexture object with width and height in Points and a pixel format, only RGB and RGBA formats are valid */
    static SGPostImage * create(int w, int h, CCTexture2DPixelFormat eFormat);
    
    /** creates a RenderTexture object with width and height in Points, pixel format is RGBA8888 */
    static SGPostImage * create(int w, int h);
    
    /** initializes a RenderTexture object with width and height in Points and a pixel format, only RGB and RGBA formats are valid */
    bool initWithWidthAndHeight(int w, int h, CCTexture2DPixelFormat eFormat);
    
    /** initializes a RenderTexture object with width and height in Points and a pixel format( only RGB and RGBA formats are valid ) and depthStencil format*/
    bool initWithWidthAndHeight(int w, int h, CCTexture2DPixelFormat eFormat, GLuint uDepthStencilFormat);
    
    /** starts grabbing */
    void begin();
    
    /** starts rendering to the texture while clearing the texture first.
     This is more efficient then calling -clear first and then -begin */
    void beginWithClear(float r, float g, float b, float a);
    
    /** starts rendering to the texture while clearing the texture first.
     This is more efficient then calling -clear first and then -begin */
    void beginWithClear(float r, float g, float b, float a, float depthValue);
    
    /** starts rendering to the texture while clearing the texture first.
     This is more efficient then calling -clear first and then -begin */
    void beginWithClear(float r, float g, float b, float a, float depthValue, int stencilValue);
    
    /** ends grabbing*/
    void end();
    
    /** clears the texture with a color */
    void clear(float r, float g, float b, float a);
    
    /** clears the texture with a specified depth value */
    void clearDepth(float depthValue);
    
    /** clears the texture with a specified stencil value */
    void clearStencil(int stencilValue);
    
    /** Listen "come to background" message, and save render texture.
     It only has effect on Android.
     */
    void listenToBackground(CCObject *obj);
    
    /** Listen "come to foreground" message and restore the frame buffer object
     It only has effect on Android.
     */
    void listenToForeground(CCObject *obj);
    
    /** Valid flags: GL_COLOR_BUFFER_BIT, GL_DEPTH_BUFFER_BIT, GL_STENCIL_BUFFER_BIT. They can be OR'ed. Valid when "autoDraw is YES. */
    unsigned int getClearFlags() const;
    void setClearFlags(unsigned int uClearFlags);
    
    /** Clear color value. Valid only when "autoDraw" is true. */
    const ccColor4F& getClearColor() const;
    void setClearColor(const ccColor4F &clearColor);
    
    /** Value for clearDepth. Valid only when autoDraw is true. */
    float getClearDepth() const;
    void setClearDepth(float fClearDepth);
    
    /** Value for clear Stencil. Valid only when autoDraw is true */
    int getClearStencil() const;
    void setClearStencil(float fClearStencil);
    
    /** When enabled, it will render its children into the texture automatically. Disabled by default for compatiblity reasons.
     Will be enabled in the future.
     */
    bool isAutoDraw() const;
    void setAutoDraw(bool bAutoDraw);
    
private:
    void beginWithClear(float r, float g, float b, float a, float depthValue, int stencilValue, GLbitfield flags);
    
protected:
    GLuint       m_uFBO[2]; // second use for g-buffer
    GLuint       m_uDepthRenderBufffer;
    GLint        m_nOldFBO;
    CCTexture2D* m_pTexture;
    CCTexture2D* m_pTextureCopy;    // a copy of m_pTexture
    CCImage*     m_pUITextureImage;
    GLenum       m_ePixelFormat;
    
    // code for "auto" update
    GLbitfield   m_uClearFlags;
    ccColor4F    m_sClearColor;
    GLclampf     m_fClearDepth;
    GLint        m_nClearStencil;
    bool         m_bAutoDraw;

    CCTexture2D* _gBuffer;
    GLint _gBufferUniformLocation;
    
    void beginGBuffer ();
    void endGBuffer ();
};

#endif /* defined(__SZSG__SGPostImage__) */
