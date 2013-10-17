//
//  SGPostImage.cpp
//  SZSG
//
//  Created by xuxiaocheng on 8/30/13.
//
//

#include "SGPostImage.h"
#include "support/ccUtils.h"
#include "SGHierarchiesSprite.h"

USING_NS_CC;


#pragma mark - Shader

const GLchar* ccShader_SGPostImage_EdgeDetection_vert =
"\n\
attribute vec4 a_position;                                                      \n\
attribute vec2 a_texCoord;                                                      \n\
attribute vec4 a_color;                                                         \n\
\n\
varying lowp vec4 v_fragmentColor;                                              \n\
\n\
varying mediump vec2 v_texCoord;                                                \n\
varying mediump vec2 v_texCoord_L;                                              \n\
varying mediump vec2 v_texCoord_R;                                              \n\
varying mediump vec2 v_texCoord_T;                                              \n\
varying mediump vec2 v_texCoord_B;                                              \n\
\n\
void main()                                                                     \n\
{                                                                               \n\
gl_Position = CC_MVPMatrix * a_position;                                      \n\
v_fragmentColor = a_color;                                                    \n\
\n\
float texelWidth = 1.0 / 1136.0;                                              \n\
float texelHeight = 1.0 / 768.0;                                              \n\
vec2 widthStep = vec2(texelWidth, 0.0);                                       \n\
vec2 heightStep = vec2(0.0, texelHeight);                                     \n\
vec2 widthHeightStep = vec2(texelWidth, texelHeight);                         \n\
vec2 widthNegativeHeightStep = vec2(texelWidth, -texelHeight);                \n\
\n\
v_texCoord = a_texCoord.xy;                                                   \n\
v_texCoord_L = a_texCoord.xy - widthStep;                                     \n\
v_texCoord_R = a_texCoord.xy + widthStep;                                     \n\
v_texCoord_T = a_texCoord.xy - heightStep;                                    \n\
v_texCoord_B = a_texCoord.xy + heightStep;                                    \n\
}                                                                               \n\
";

const GLchar* ccShader_SGPostImage_EdgeDetection_frag =
"\n\
#ifdef GL_ES                                                                                \n\
precision lowp float;                                                                       \n\
#endif                                                                          \n\
\n\
uniform lowp sampler2D CC_Texture0;                                             \n\
uniform lowp sampler2D u_texGBuffer;                                                   \n\
// uniform mediump ivec2 u_glowSize;                                            \n\
// uniform mediump vec2 u_texelSize;                                            \n\
// uniform lowp vec3 u_glowColor;                                               \n\
\n\
varying vec4 v_fragmentColor;                                                               \n\
varying vec2 v_texCoord;                                                        \n\
varying mediump vec2 v_texCoord_L;                                              \n\
varying mediump vec2 v_texCoord_R;                                              \n\
varying mediump vec2 v_texCoord_T;                                              \n\
varying mediump vec2 v_texCoord_B;                                                                  \n\
\n\
\n\
float threshold(in float thr1, in float thr2 , in float val) {                  \n\
if (val < thr1) {return 0.0;}                                                  \n\
if (val > thr2) {return 1.0;}                                                  \n\
return val;                                                                    \n\
}                                                                               \n\
\n\
// averaged pixel intensity from 3 color channels                               \n\
float avg_intensity(in vec4 pix) {                                              \n\
return (pix.r + pix.g + pix.b)/3.;                                             \n\
}                                                                               \n\
\n\
lowp vec4 get_pixel(in vec2 coords, in float dx, in float dy) {                 \n\
return texture2D(CC_Texture0, coords + vec2(dx, dy));                          \n\
}                                                                               \n\
\n\
// returns pixel color                                                          \n\
float IsEdge(in vec2 coords){                                                   \n\
// float dxtex = 1.0 / 1136.0 /*image width*/;                                \n\
// float dytex = 1.0 / 768.0 /*image height*/;                                \n\
float pix[9];                                                                 \n\
// int k = -1;                                                                \n\
float delta;                                                                  \n\
\n\
// read neighboring pixel intensities                                         \n\
// for (int i=-1; i<2; i++) {                                                 \n\
//  for(int j=-1; j<2; j++) {                                                 \n\
//   k++;                                                                     \n\
//   pix[k] = avg_intensity(get_pixel(coords,float(i)*dxtex,                  \n\
//                                         float(j)*dytex));                  \n\
//  }                                                                         \n\
// }                                                                          \n\
pix[1] = avg_intensity(texture2D(u_texGBuffer, v_texCoord_L));               \n\
pix[7] = avg_intensity(texture2D(u_texGBuffer, v_texCoord_R));               \n\
pix[5] = avg_intensity(texture2D(u_texGBuffer, v_texCoord_T));               \n\
pix[3] = avg_intensity(texture2D(u_texGBuffer, v_texCoord_B));               \n\
\n\
// average color differences around neighboring pixels                        \n\
// delta = (abs(pix[1]-pix[7])+                                               \n\
//         abs(pix[5]-pix[3]) +                                               \n\
//         abs(pix[0]-pix[8])+                                                \n\
//         abs(pix[2]-pix[6])                                                 \n\
//          )/4.;                                                             \n\
delta = (abs(pix[1]-pix[7])+                                                \n\
abs(pix[5]-pix[3])                                                    \n\
)/2.;                                                                \n\
\n\
// return threshold(0.25,0.4,clamp(1.8*delta,0.0,1.0));                       \n\
// return clamp(1.8*delta,0.0,1.0);                                           \n\
return delta;                                                                 \n\
}                                                                               \n\
\n\
void main()                                                                     \n\
{                                                                               \n\
// lowp vec4 color = vec4(0.0,0.0,0.0,1.0);                                   \n\
// color.g = IsEdge(v_texCoord.xy);                                           \n\
// gl_FragColor = color;                                                      \n\
\n\
// lowp vec3 glowColor = vec3(0.0, 1.0, 0.0);                                  \n\
//  gl_FragColor = texture2D(CC_Texture0, vec2(v_texCoord.x, v_texCoord.y));   \n\
//  float d = IsEdge(v_texCoord.xy);                                           \n\
//  gl_FragColor.rgb = glowColor.rgb * d + gl_FragColor.rgb * (1.0 - d);       \n\
\n\
lowp vec3 glowColor = vec3(0.0, 0.0, 0.0);                                    \n\
gl_FragColor = texture2D(CC_Texture0, vec2(v_texCoord.x, v_texCoord.y));      \n\
float d = IsEdge(v_texCoord.xy) > 0.005 ? 1.0 : 0.0 ;                             \n\
if (d > 0.0) \n\
gl_FragColor = vec4(glowColor, 1.0); \n\
//gl_FragColor.rgb = glowColor.rgb * d + gl_FragColor.rgb * (1.0 - d);          \n\
}                                                                               \n\
";


#pragma mark - SGPostImage

// implementation CCRenderTexture
SGPostImage::SGPostImage()
: m_pSprite(NULL)
, m_uDepthRenderBufffer(0)
, m_nOldFBO(0)
, m_pTexture(0)
, m_pTextureCopy(0)
, m_pUITextureImage(NULL)
, m_ePixelFormat(kCCTexture2DPixelFormat_RGBA8888)
, m_uClearFlags(0)
, m_sClearColor(ccc4f(0,0,0,0))
, m_fClearDepth(0.0f)
, m_nClearStencil(0)
, m_bAutoDraw(false)
{
#if CC_ENABLE_CACHE_TEXTURE_DATA
    // Listen this event to save render texture before come to background.
    // Then it can be restored after coming to foreground on Android.
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this,
                                                                  callfuncO_selector(CCRenderTexture::listenToBackground),
                                                                  EVENT_COME_TO_BACKGROUND,
                                                                  NULL);
    
    CCNotificationCenter::sharedNotificationCenter()->addObserver(this,
                                                                  callfuncO_selector(CCRenderTexture::listenToForeground),
                                                                  EVNET_COME_TO_FOREGROUND, // this is misspelt
                                                                  NULL);
#endif
}

SGPostImage::~SGPostImage()
{
    CC_SAFE_RELEASE(m_pSprite);
    CC_SAFE_RELEASE(m_pTextureCopy);
    CC_SAFE_RELEASE(_gBuffer);
    
    glDeleteFramebuffers(2, m_uFBO);
    if (m_uDepthRenderBufffer)
    {
        glDeleteRenderbuffers(1, &m_uDepthRenderBufffer);
    }
    CC_SAFE_DELETE(m_pUITextureImage);
    
#if CC_ENABLE_CACHE_TEXTURE_DATA
    CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, EVENT_COME_TO_BACKGROUND);
    CCNotificationCenter::sharedNotificationCenter()->removeObserver(this, EVNET_COME_TO_FOREGROUND);
#endif
}

void SGPostImage::listenToBackground(cocos2d::CCObject *obj)
{
#if CC_ENABLE_CACHE_TEXTURE_DATA
    CC_SAFE_DELETE(m_pUITextureImage);
    
    // to get the rendered texture data
    m_pUITextureImage = newCCImage(false);
    
    if (m_pUITextureImage)
    {
        const CCSize& s = m_pTexture->getContentSizeInPixels();
        VolatileTexture::addDataTexture(m_pTexture, m_pUITextureImage->getData(), kTexture2DPixelFormat_RGBA8888, s);
        
        if ( m_pTextureCopy )
        {
            VolatileTexture::addDataTexture(m_pTextureCopy, m_pUITextureImage->getData(), kTexture2DPixelFormat_RGBA8888, s);
        }
    }
    else
    {
        CCLOG("Cache rendertexture failed!");
    }
    
    glDeleteFramebuffers(1, &m_uFBO);
    m_uFBO = 0;
#endif
}

void SGPostImage::listenToForeground(cocos2d::CCObject *obj)
{
#if CC_ENABLE_CACHE_TEXTURE_DATA
    // -- regenerate frame buffer object and attach the texture
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &m_nOldFBO);
    
    glGenFramebuffers(1, &m_uFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_uFBO);
    
    m_pTexture->setAliasTexParameters();
    
    if ( m_pTextureCopy )
    {
        m_pTextureCopy->setAliasTexParameters();
    }
    
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_pTexture->getName(), 0);
    glBindFramebuffer(GL_FRAMEBUFFER, m_nOldFBO);
#endif
}

CCSprite * SGPostImage::getSprite()
{
    return m_pSprite;
}

void SGPostImage::setSprite(CCSprite* var)
{
    CC_SAFE_RELEASE(m_pSprite);
    m_pSprite = var;
    CC_SAFE_RETAIN(m_pSprite);
}

unsigned int SGPostImage::getClearFlags() const
{
    return m_uClearFlags;
}

void SGPostImage::setClearFlags(unsigned int uClearFlags)
{
    m_uClearFlags = uClearFlags;
}

const ccColor4F& SGPostImage::getClearColor() const
{
    return m_sClearColor;
}

void SGPostImage::setClearColor(const ccColor4F &clearColor)
{
    m_sClearColor = clearColor;
}

float SGPostImage::getClearDepth() const
{
    return m_fClearDepth;
}

void SGPostImage::setClearDepth(float fClearDepth)
{
    m_fClearDepth = fClearDepth;
}

int SGPostImage::getClearStencil() const
{
    return m_nClearStencil;
}

void SGPostImage::setClearStencil(float fClearStencil)
{
    m_nClearStencil = fClearStencil;
}

bool SGPostImage::isAutoDraw() const
{
    return m_bAutoDraw;
}

void SGPostImage::setAutoDraw(bool bAutoDraw)
{
    m_bAutoDraw = bAutoDraw;
}

SGPostImage * SGPostImage::create(int w, int h, CCTexture2DPixelFormat eFormat)
{
    SGPostImage *pRet = new SGPostImage();
    
    if(pRet && pRet->initWithWidthAndHeight(w, h, eFormat))
    {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return NULL;
}

SGPostImage * SGPostImage::create(int w ,int h, CCTexture2DPixelFormat eFormat, GLuint uDepthStencilFormat)
{
    SGPostImage *pRet = new SGPostImage();
    
    if(pRet && pRet->initWithWidthAndHeight(w, h, eFormat, uDepthStencilFormat))
    {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return NULL;
}

SGPostImage * SGPostImage::create(int w, int h)
{
    SGPostImage *pRet = new SGPostImage();
    
    if(pRet && pRet->initWithWidthAndHeight(w, h, kCCTexture2DPixelFormat_RGBA8888, 0))
    {
        pRet->autorelease();
        return pRet;
    }
    CC_SAFE_DELETE(pRet);
    return NULL;
}

bool SGPostImage::initWithWidthAndHeight(int w, int h, CCTexture2DPixelFormat eFormat)
{
    return initWithWidthAndHeight(w, h, eFormat, 0);
}

bool SGPostImage::initWithWidthAndHeight(int w, int h, CCTexture2DPixelFormat eFormat, GLuint uDepthStencilFormat)
{
    CCAssert(eFormat != kCCTexture2DPixelFormat_A8, "only RGB and RGBA formats are valid for a render texture");
    
    bool bRet = false;
    void *data = NULL;
    do
    {
        w = (int)(w * CC_CONTENT_SCALE_FACTOR());
        h = (int)(h * CC_CONTENT_SCALE_FACTOR());
        
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &m_nOldFBO);
        
        // textures must be power of two squared
        unsigned int powW = 0;
        unsigned int powH = 0;
        
        if (CCConfiguration::sharedConfiguration()->supportsNPOT())
        {
            powW = w;
            powH = h;
        }
        else
        {
            powW = ccNextPOT(w);
            powH = ccNextPOT(h);
        }
        
        data = malloc((int)(powW * powH * 4));
        CC_BREAK_IF(! data);
        
        memset(data, 0, (int)(powW * powH * 4));
        m_ePixelFormat = eFormat;
        
        m_pTexture = new CCTexture2D();
        if (m_pTexture)
        {
            m_pTexture->initWithData(data, (CCTexture2DPixelFormat)m_ePixelFormat, powW, powH, CCSizeMake((float)w, (float)h));
        }
        else
        {
            break;
        }
        m_pTexture->setAliasTexParameters();
        
        _gBuffer = new CCTexture2D();
        if (_gBuffer) {
            _gBuffer->initWithData(data, (CCTexture2DPixelFormat)m_ePixelFormat, powW, powH, CCSizeMake((float)w, (float)h));
        }
        else {
            break;
        }
        _gBuffer->setAliasTexParameters();
        
        GLint oldRBO;
        glGetIntegerv(GL_RENDERBUFFER_BINDING, &oldRBO);
        
        if (CCConfiguration::sharedConfiguration()->checkForGLExtension("GL_QCOM"))
        {
            m_pTextureCopy = new CCTexture2D();
            if (m_pTextureCopy)
            {
                m_pTextureCopy->initWithData(data, (CCTexture2DPixelFormat)m_ePixelFormat, powW, powH, CCSizeMake((float)w, (float)h));
            }
            else
            {
                break;
            }
        }
        
        // generate FBO
        glGenFramebuffers(2, m_uFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, m_uFBO[0]);
        
        // associate texture with FBO
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_pTexture->getName(), 0);
        
        if (uDepthStencilFormat != 0)
        {
            //create and attach depth buffer
            glGenRenderbuffers(1, &m_uDepthRenderBufffer);
            glBindRenderbuffer(GL_RENDERBUFFER, m_uDepthRenderBufffer);
            glRenderbufferStorage(GL_RENDERBUFFER, uDepthStencilFormat, (GLsizei)powW, (GLsizei)powH);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_uDepthRenderBufffer);
            
            // if depth format is the one with stencil part, bind same render buffer as stencil attachment
            if (uDepthStencilFormat == GL_DEPTH24_STENCIL8)
            {
                glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_uDepthRenderBufffer);
            }
        }
        
        // check if it worked (probably worth doing :) )
        CCAssert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Could not attach texture to framebuffer");
        
        // setup g-buffer FBO
        glBindFramebuffer(GL_FRAMEBUFFER, m_uFBO[1]);
        
        // associate texture with FBO
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _gBuffer->getName(), 0);
        
        // check if it worked (probably worth doing :) )
        CCAssert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Could not attach texture to g-buffer");
        
        // retained
        setSprite(CCSprite::createWithTexture(m_pTexture));
        
        m_pTexture->release();
        m_pSprite->setScaleY(-1);
        
        ccBlendFunc tBlendFunc = {GL_ONE, GL_ONE_MINUS_SRC_ALPHA };
        m_pSprite->setBlendFunc(tBlendFunc);
        
        glBindRenderbuffer(GL_RENDERBUFFER, oldRBO);
        glBindFramebuffer(GL_FRAMEBUFFER, m_nOldFBO);
        
        // Diabled by default.
        m_bAutoDraw = false;
        
        // add sprite for backward compatibility
        addChild(m_pSprite);
        
        // setup GL program
        CCGLProgram* postImageProgram = CCShaderCache::sharedShaderCache()->programForKey(kCCShader_SGPostImage_EdgeDetection);
        if (postImageProgram == NULL) {
            postImageProgram = new CCGLProgram();
            postImageProgram->initWithVertexShaderByteArray(ccShader_SGPostImage_EdgeDetection_vert, ccShader_SGPostImage_EdgeDetection_frag);
            
            postImageProgram->addAttribute(kCCAttributeNamePosition, kCCVertexAttrib_Position);
            postImageProgram->addAttribute(kCCAttributeNameColor, kCCVertexAttrib_Color);
            postImageProgram->addAttribute(kCCAttributeNameTexCoord, kCCVertexAttrib_TexCoords);
            
            postImageProgram->link();
            postImageProgram->updateUniforms();
            
            CHECK_GL_ERROR_DEBUG();
            
            CCShaderCache::sharedShaderCache()->addProgram(postImageProgram, kCCShader_SGPostImage_EdgeDetection);
        }
        else {
            postImageProgram->retain();
        }
        
        _gBufferUniformLocation = postImageProgram->getUniformLocationForName("u_texGBuffer");
        postImageProgram->setUniformLocationWith1i(_gBufferUniformLocation, 1);
        
        m_pSprite->setShaderProgram(postImageProgram);
        
        postImageProgram->release();
        
        bRet = true;
    } while (0);
    
    CC_SAFE_FREE(data);
    
    return bRet;
}

void SGPostImage::begin()
{
    kmGLMatrixMode(KM_GL_PROJECTION);
	kmGLPushMatrix();
	kmGLMatrixMode(KM_GL_MODELVIEW);
    kmGLPushMatrix();
    
    CCDirector *director = CCDirector::sharedDirector();
    director->setProjection(director->getProjection());
    
    const CCSize& texSize = m_pTexture->getContentSizeInPixels();
    
    // Calculate the adjustment ratios based on the old and new projections
//    CCSize size = director->getWinSizeInPixels();
//    float widthRatio = size.width / texSize.width;
//    float heightRatio = size.height / texSize.height;
    
    // Adjust the orthographic projection and viewport
    glViewport(0, 0, (GLsizei)texSize.width, (GLsizei)texSize.height);
    
//    kmMat4 orthoMatrix;
//    kmMat4OrthographicProjection(&orthoMatrix, (float)-1.0 / widthRatio,  (float)1.0 / widthRatio,
//                                 (float)-1.0 / heightRatio, (float)1.0 / heightRatio, -1,1 );
//    kmGLMultMatrix(&orthoMatrix);
    
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &m_nOldFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_uFBO[0]);
    
    /*  Certain Qualcomm Andreno gpu's will retain data in memory after a frame buffer switch which corrupts the render to the texture. The solution is to clear the frame buffer before rendering to the texture. However, calling glClear has the unintended result of clearing the current texture. Create a temporary texture to overcome this. At the end of CCRenderTexture::begin(), switch the attached texture to the second one, call glClear, and then switch back to the original texture. This solution is unnecessary for other devices as they don't have the same issue with switching frame buffers.
     */
    if (CCConfiguration::sharedConfiguration()->checkForGLExtension("GL_QCOM"))
    {
        // -- bind a temporary texture so we can clear the render buffer without losing our texture
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_pTextureCopy->getName(), 0);
        CHECK_GL_ERROR_DEBUG();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_pTexture->getName(), 0);
    }
}

void SGPostImage::beginWithClear(float r, float g, float b, float a)
{
    beginWithClear(r, g, b, a, 0, 0, GL_COLOR_BUFFER_BIT);
}

void SGPostImage::beginWithClear(float r, float g, float b, float a, float depthValue)
{
    beginWithClear(r, g, b, a, depthValue, 0, GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
}

void SGPostImage::beginWithClear(float r, float g, float b, float a, float depthValue, int stencilValue)
{
    beginWithClear(r, g, b, a, depthValue, stencilValue, GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
}

void SGPostImage::beginWithClear(float r, float g, float b, float a, float depthValue, int stencilValue, GLbitfield flags)
{
    this->begin();
    
    // save clear color
    GLfloat	clearColor[4] = {0.0f};
    GLfloat depthClearValue = 0.0f;
    int stencilClearValue = 0;
    
    if (flags & GL_COLOR_BUFFER_BIT)
    {
        glGetFloatv(GL_COLOR_CLEAR_VALUE,clearColor);
        glClearColor(r, g, b, a);
    }
    
    if (flags & GL_DEPTH_BUFFER_BIT)
    {
        glGetFloatv(GL_DEPTH_CLEAR_VALUE, &depthClearValue);
        glClearDepth(depthValue);
    }
    
    if (flags & GL_STENCIL_BUFFER_BIT)
    {
        glGetIntegerv(GL_STENCIL_CLEAR_VALUE, &stencilClearValue);
        glClearStencil(stencilValue);
    }
    
    glClear(flags);
    
    // restore
    if (flags & GL_COLOR_BUFFER_BIT)
    {
        glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);
    }
    if (flags & GL_DEPTH_BUFFER_BIT)
    {
        glClearDepth(depthClearValue);
    }
    if (flags & GL_STENCIL_BUFFER_BIT)
    {
        glClearStencil(stencilClearValue);
    }
}

void SGPostImage::end()
{
    CCDirector *director = CCDirector::sharedDirector();
    
    glBindFramebuffer(GL_FRAMEBUFFER, m_nOldFBO);
    
    // restore viewport
    director->setViewport();
    
    kmGLMatrixMode(KM_GL_PROJECTION);
	kmGLPopMatrix();
	kmGLMatrixMode(KM_GL_MODELVIEW);
	kmGLPopMatrix();
}

void SGPostImage::clear(float r, float g, float b, float a)
{
    this->beginWithClear(r, g, b, a);
    this->end();
}

void SGPostImage::clearDepth(float depthValue)
{
    this->begin();
    //! save old depth value
    GLfloat depthClearValue;
    glGetFloatv(GL_DEPTH_CLEAR_VALUE, &depthClearValue);
    
    glClearDepth(depthValue);
    glClear(GL_DEPTH_BUFFER_BIT);
    
    // restore clear color
    glClearDepth(depthClearValue);
    this->end();
}

void SGPostImage::clearStencil(int stencilValue)
{
    // save old stencil value
    int stencilClearValue;
    glGetIntegerv(GL_STENCIL_CLEAR_VALUE, &stencilClearValue);
    
    glClearStencil(stencilValue);
    glClear(GL_STENCIL_BUFFER_BIT);
    
    // restore clear color
    glClearStencil(stencilClearValue);
}

void SGPostImage::visit()
{
    if (!m_bVisible)
        return;
	kmGLPushMatrix();
	
    // g-buffer and color texture draw
    draw();
    
    transform();
    
    
    // post image draw BEGIN
    m_pSprite->transform();
    
    CC_PROFILER_START_CATEGORY(kCCProfilerCategorySprite, "SGPostImage - draw");
    
    ccGLEnable(m_eGLServerState);
    CCAssert(m_pSprite->getShaderProgram(), "No shader program set for post image");
    {
        m_pSprite->getShaderProgram()->use();
        m_pSprite->getShaderProgram()->setUniformsForBuiltins();
    }
    
    ccGLBlendFunc( m_pSprite->getBlendFunc().src, m_pSprite->getBlendFunc().dst );
    
    if (m_pSprite->getTexture() != NULL)
    {
        ccGLBindTexture2D( m_pSprite->getTexture()->getName() );
        ccGLEnableVertexAttribs( kCCVertexAttribFlag_PosColorTex );
        ccGLBindTexture2DN( 1, _gBuffer->getName());
    }
    else
    {
        ccGLBindTexture2D(0);
        ccGLEnableVertexAttribs( kCCVertexAttribFlag_Position | kCCVertexAttribFlag_Color );
    }
    
    ccV3F_C4B_T2F_Quad quad = m_pSprite->getQuad();
#ifdef EMSCRIPTEN
    long offset = 0;
    setGLBufferData(&quad, 4 * sizeof(ccV3F_C4B_T2F), 0);
#else
    long offset = (long)&quad;
#endif // EMSCRIPTEN
    
    // vertex
    int diff = offsetof( ccV3F_C4B_T2F, vertices);
    glVertexAttribPointer(kCCVertexAttrib_Position, 3, GL_FLOAT, GL_FALSE, sizeof(ccV3F_C4B_T2F), (void*) (offset + diff));
    
    if (m_pSprite->getTexture() != NULL)
    {
        // texCoods
        diff = offsetof( ccV3F_C4B_T2F, texCoords);
        glVertexAttribPointer(kCCVertexAttrib_TexCoords, 2, GL_FLOAT, GL_FALSE, sizeof(ccV3F_C4B_T2F), (void*)(offset + diff));
    }
    
    // color
    diff = offsetof( ccV3F_C4B_T2F, colors);
    glVertexAttribPointer(kCCVertexAttrib_Color, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ccV3F_C4B_T2F), (void*)(offset + diff));
    
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    
    CHECK_GL_ERROR_DEBUG();
    
    CC_INCREMENT_GL_DRAWS(1);
    
    CC_PROFILER_STOP_CATEGORY(kCCProfilerCategorySprite, "SGPostImage - draw");
	// post image draw END
    
	
	kmGLPopMatrix();
    m_uOrderOfArrival = 0;
}

void SGPostImage::draw()
{
    if( m_bAutoDraw)
    {
		//! make sure all children are drawn
        sortAllChildren();
        
        beginGBuffer();
        
        if (m_uClearFlags)
        {
            GLfloat oldClearColor[4] = {0.0f};
			GLfloat oldDepthClearValue = 0.0f;
			GLint oldStencilClearValue = 0;
			
			// backup and set
			if (m_uClearFlags & GL_COLOR_BUFFER_BIT)
            {
				glGetFloatv(GL_COLOR_CLEAR_VALUE, oldClearColor);
				glClearColor(m_sClearColor.r, m_sClearColor.g, m_sClearColor.b, m_sClearColor.a);
			}
			
			if (m_uClearFlags & GL_DEPTH_BUFFER_BIT)
            {
				glGetFloatv(GL_DEPTH_CLEAR_VALUE, &oldDepthClearValue);
				glClearDepth(m_fClearDepth);
			}
			
			if (m_uClearFlags & GL_STENCIL_BUFFER_BIT)
            {
				glGetIntegerv(GL_STENCIL_CLEAR_VALUE, &oldStencilClearValue);
				glClearStencil(m_nClearStencil);
			}
			
			// clear
			glClear(m_uClearFlags);
			
			// restore
			if (m_uClearFlags & GL_COLOR_BUFFER_BIT)
            {
				glClearColor(oldClearColor[0], oldClearColor[1], oldClearColor[2], oldClearColor[3]);
            }
			if (m_uClearFlags & GL_DEPTH_BUFFER_BIT)
            {
				glClearDepth(oldDepthClearValue);
            }
			if (m_uClearFlags & GL_STENCIL_BUFFER_BIT)
            {
				glClearStencil(oldStencilClearValue);
            }
		}
		
		CCObject *pElement;
		CCARRAY_FOREACH(m_pChildren, pElement)
        {
            CCNode *pChild = (CCNode*)pElement;
            SGHierarchiesSprite* sprite = dynamic_cast<SGHierarchiesSprite*>(pChild);
            
            if (pChild != m_pSprite
                && (sprite))
            {
                sprite->gBufferVisit(sprite->getZOrder());
            }
		}
        
        endGBuffer();
        
        
        
        begin();
		
        if (m_uClearFlags)
        {
            GLfloat oldClearColor[4] = {0.0f};
			GLfloat oldDepthClearValue = 0.0f;
			GLint oldStencilClearValue = 0;
			
			// backup and set
			if (m_uClearFlags & GL_COLOR_BUFFER_BIT)
            {
				glGetFloatv(GL_COLOR_CLEAR_VALUE, oldClearColor);
				glClearColor(m_sClearColor.r, m_sClearColor.g, m_sClearColor.b, m_sClearColor.a);
			}
			
			if (m_uClearFlags & GL_DEPTH_BUFFER_BIT)
            {
				glGetFloatv(GL_DEPTH_CLEAR_VALUE, &oldDepthClearValue);
				glClearDepth(m_fClearDepth);
			}
			
			if (m_uClearFlags & GL_STENCIL_BUFFER_BIT)
            {
				glGetIntegerv(GL_STENCIL_CLEAR_VALUE, &oldStencilClearValue);
				glClearStencil(m_nClearStencil);
			}
			
			// clear
			glClear(m_uClearFlags);
			
			// restore
			if (m_uClearFlags & GL_COLOR_BUFFER_BIT)
            {
				glClearColor(oldClearColor[0], oldClearColor[1], oldClearColor[2], oldClearColor[3]);
            }
			if (m_uClearFlags & GL_DEPTH_BUFFER_BIT)
            {
				glClearDepth(oldDepthClearValue);
            }
			if (m_uClearFlags & GL_STENCIL_BUFFER_BIT)
            {
				glClearStencil(oldStencilClearValue);
            }
		}
		
		CCARRAY_FOREACH(m_pChildren, pElement)
        {
            CCNode *pChild = (CCNode*)pElement;
            
            if (pChild != m_pSprite)
            {
                pChild->visit();
            }
		}
        
        end();
	}
}

void SGPostImage::beginGBuffer () {
    kmGLMatrixMode(KM_GL_PROJECTION);
	kmGLPushMatrix();
	kmGLMatrixMode(KM_GL_MODELVIEW);
    kmGLPushMatrix();
    
    CCDirector *director = CCDirector::sharedDirector();
    director->setProjection(director->getProjection());
    
    const CCSize& texSize = _gBuffer->getContentSizeInPixels();
    
    // Calculate the adjustment ratios based on the old and new projections
    //    CCSize size = director->getWinSizeInPixels();
    //    float widthRatio = size.width / texSize.width;
    //    float heightRatio = size.height / texSize.height;
    
    // Adjust the orthographic projection and viewport
    glViewport(0, 0, (GLsizei)texSize.width, (GLsizei)texSize.height);
    
    //    kmMat4 orthoMatrix;
    //    kmMat4OrthographicProjection(&orthoMatrix, (float)-1.0 / widthRatio,  (float)1.0 / widthRatio,
    //                                 (float)-1.0 / heightRatio, (float)1.0 / heightRatio, -1,1 );
    //    kmGLMultMatrix(&orthoMatrix);
    
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &m_nOldFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_uFBO[1]);
}

void SGPostImage::endGBuffer () {
    CCDirector *director = CCDirector::sharedDirector();
    
    glBindFramebuffer(GL_FRAMEBUFFER, m_nOldFBO);
    
    // restore viewport
    director->setViewport();
    
    kmGLMatrixMode(KM_GL_PROJECTION);
	kmGLPopMatrix();
	kmGLMatrixMode(KM_GL_MODELVIEW);
	kmGLPopMatrix();
}
