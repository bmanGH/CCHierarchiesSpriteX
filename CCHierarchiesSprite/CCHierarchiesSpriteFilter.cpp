//
//  CCHierarchiesSpriteFilter.cpp
//  CCHierarchiesSprite
//
//  Created by bman <zx123xz321hm3@hotmail.com>.
//  Copyright (c) 2013. All rights reserved.
//

#include "CCHierarchiesSpriteFilter.h"
#include "CCHierarchiesSpriteShader.h"
#include "support/ccUtils.h"


NS_CC_EXT_BEGIN


#pragma mark - CCHierarchiesSpriteFilter

//TODO: implement RTT texture and grabber cache
#define ENABLE_RTT_AND_GRABBER_CACHE 0
#if ENABLE_RTT_AND_GRABBER_CACHE == 1

struct RenderTextureCache {
    CCTexture2D* texture;
    CCHierarchiesSpriteGrabber* grabber;
    bool isUsed;
};

static RenderTextureCache g_textureCache[32] = {0};

static RenderTextureCache* getIdleRenderTexture (CCSize size) {
//    unsigned long POTWide = ccNextPOT(size.width * CCEGLView::sharedOpenGLView()->getScaleX());
//    unsigned long POTHigh = ccNextPOT(size.height * CCEGLView::sharedOpenGLView()->getScaleY());
    unsigned long POTWide = size.width * CCEGLView::sharedOpenGLView()->getScaleX();
    unsigned long POTHigh = size.height * CCEGLView::sharedOpenGLView()->getScaleY();
    
    for (int i = 0; i < 32; i++) {
        if (g_textureCache[i].isUsed == false) {
            if (g_textureCache[i].texture == NULL) {
                // we only use rgba8888
                CCTexture2DPixelFormat format = kCCTexture2DPixelFormat_RGBA8888;
                
                CCTexture2D *texture = new CCTexture2D();
                texture->initWithData(NULL, format, POTWide, POTHigh, size);
                
                if (!texture)
                {
                    CCAssert(0, "cocos2d: CCGrid: error creating texture");
                }
                texture->setAntiAliasTexParameters();
                
                g_textureCache[i].texture = texture;
                
                CCHierarchiesSpriteGrabber* grabber = new CCHierarchiesSpriteGrabber();
                
                g_textureCache[i].grabber = grabber;
            }
            else if (g_textureCache[i].texture->getPixelsWide() != POTWide ||
                     g_textureCache[i].texture->getPixelsHigh() != POTHigh) {
                continue;
            }
            
            g_textureCache[i].isUsed = true;
            return &g_textureCache[i];
        }
    }
    
    CCAssert(0, "render texture is full");
    return NULL;
}

static void setIdleRenderTexture (CCTexture2D* texture) {
    for (int i = 0; i < 32; i++) {
        if (g_textureCache[i].texture == texture) {
            g_textureCache[i].isUsed = false;
            return;
        }
    }
    CCAssert(0, "");
}

#endif

CCHierarchiesSpriteFilter::CCHierarchiesSpriteFilter ()
: _isTextureFlipped(false), _shaderProgram(NULL), _texture(NULL), _grabber(NULL), _frame(CCRectZero), _anchorPoint(CCPointZero) {
}

CCHierarchiesSpriteFilter::~CCHierarchiesSpriteFilter () {
    CC_SAFE_RELEASE(_shaderProgram);
#if ENABLE_RTT_AND_GRABBER_CACHE == 0
    CC_SAFE_RELEASE(_texture);
    CC_SAFE_RELEASE(_grabber);
#endif
}

bool CCHierarchiesSpriteFilter::init(bool flipped) {
    bool ret = true;
    
    _isTextureFlipped = flipped;
    
    _grabber = new CCHierarchiesSpriteGrabber();
    if (!_grabber)
    {
        ret = false;
    }
    
    _shaderProgram = this->setupShaderProgram();
    CC_SAFE_RETAIN(_shaderProgram);
    
    this->setupVertexPoints();
    
    return ret;
}

void CCHierarchiesSpriteFilter::setupVertexPoints () {
    // setup index
    GLushort a = (GLushort)(0);
    GLushort b = (GLushort)(2);
    GLushort c = (GLushort)(3);
    GLushort d = (GLushort)(1);
    
    GLushort tempidx[6] = {a, b, d, b, c, d};
    
    memcpy(_indices, tempidx, 6*sizeof(GLushort));
}

void CCHierarchiesSpriteFilter::updateFrame (const CCRect& frame) {
#if ENABLE_RTT_AND_GRABBER_CACHE == 1
//    _texture = getIdleRenderTexture();
//    _grabber->grab(_texture);
    RenderTextureCache* cacheItem = getIdleRenderTexture(frame.size);
    _texture = cacheItem->texture;
    _grabber = cacheItem->grabber;
#else
//    unsigned long POTWide = ccNextPOT((unsigned int)frame.size.width * CCEGLView::sharedOpenGLView()->getScaleX());
//    unsigned long POTHigh = ccNextPOT((unsigned int)frame.size.height * CCEGLView::sharedOpenGLView()->getScaleY());
    unsigned long POTWide = frame.size.width * CCEGLView::sharedOpenGLView()->getScaleX();
    unsigned long POTHigh = frame.size.height * CCEGLView::sharedOpenGLView()->getScaleY();

    // we only use rgba8888
    CCTexture2DPixelFormat format = kCCTexture2DPixelFormat_RGBA8888;

    CCTexture2D *texture = new CCTexture2D();
    texture->initWithData(NULL, format, POTWide, POTHigh, frame.size);

    if (!texture)
    {
        CCLOG("cocos2d: CCGrid: error creating texture");
        return;
    }
    texture->setAntiAliasTexParameters();

    CC_SAFE_RELEASE(_texture);
    _texture = texture;
    CC_SAFE_RETAIN(_texture);
    texture->release();
#endif
    
    _frame = frame;
}

void CCHierarchiesSpriteFilter::updateVertexPointsWithTramsform(const cocos2d::CCAffineTransform &matrix, const CCPoint& anchorPoint) {
    _anchorPoint = anchorPoint;
    
    float x1 = _frame.origin.x + anchorPoint.x;
    float y1 = _frame.origin.y + anchorPoint.y;
    
    float x2 = x1 + _frame.size.width;
    float y2 = y1 + _frame.size.height;
    float x = matrix.tx;
    float y = matrix.ty;
    
    float cr = matrix.a;
    float sr = matrix.b;
    float cr2 = matrix.d;
    float sr2 = -matrix.c;
    float ax = x1 * cr - y1 * sr2 + x;
    float ay = x1 * sr + y1 * cr2 + y;
    
    float bx = x2 * cr - y1 * sr2 + x;
    float by = x2 * sr + y1 * cr2 + y;
    
    float cx = x2 * cr - y2 * sr2 + x;
    float cy = x2 * sr + y2 * cr2 + y;
    
    float dx = x1 * cr - y2 * sr2 + x;
    float dy = x1 * sr + y2 * cr2 + y;
    
    // update vertex
    GLushort a = (GLushort)(0);
    GLushort b = (GLushort)(2);
    GLushort c = (GLushort)(3);
    GLushort d = (GLushort)(1);
    
    int l1[4] = {a*3, b*3, c*3, d*3};
    ccVertex3F e = (ccVertex3F) { ax, ay, 0 };
    ccVertex3F f = (ccVertex3F) { bx, by, 0 };
    ccVertex3F g = (ccVertex3F) { cx, cy, 0 };
    ccVertex3F h = (ccVertex3F) { dx, dy, 0 };
    
    ccVertex3F l2[4] = {e, f, g, h};
    
    for (int i = 0; i < 4; ++i)
    {
        _vertices[l1[i]] = l2[i].x;
        _vertices[l1[i] + 1] = l2[i].y;
        _vertices[l1[i] + 2] = l2[i].z;
    }
    
    // update texture crood
    float width = _frame.size.width;
    float height = _frame.size.height;
    float imageW = _texture->getContentSizeInPixels().width;
    float imageH = _texture->getContentSizeInPixels().height;
//    float imageW = CCDirector::sharedDirector()->getWinSize().width;
//    float imageH = CCDirector::sharedDirector()->getWinSize().height;
    
    GLfloat x_1 = 0;
    GLfloat x_2 = width;
    GLfloat y_1 = 0;
    GLfloat y_2 = height;
    
    int tex1[4] = {a*2, b*2, c*2, d*2};
    CCPoint tex2[4] = {ccp(x_1, y_1), ccp(x_2, y_1), ccp(x_2, y_2), ccp(x_1, y_2)};
    
    for (int i = 0; i < 4; ++i)
    {
        _texCoordinates[tex1[i]] = tex2[i].x / imageW;
        if (_isTextureFlipped)
        {
            _texCoordinates[tex1[i] + 1] = (imageH - tex2[i].y) / imageH;
        }
        else
        {
            _texCoordinates[tex1[i] + 1] = tex2[i].y / imageH;
        }
    }
}

void CCHierarchiesSpriteFilter::beforeDraw () {
    glGetIntegerv(GL_VIEWPORT, (GLint*)&_oldViewPort);
    glViewport(0,
               0,
               (GLsizei)(_frame.size.width * CCEGLView::sharedOpenGLView()->getScaleX()),
               (GLsizei)(_frame.size.height * CCEGLView::sharedOpenGLView()->getScaleY()));
    
    kmGLMatrixMode(KM_GL_PROJECTION);
    kmGLPushMatrix();
    kmGLLoadIdentity();
    
    kmMat4 orthoMatrix;
    kmMat4OrthographicProjection(&orthoMatrix,
                                 _frame.origin.x + _anchorPoint.x,
                                 _frame.origin.x + _anchorPoint.x + _frame.size.width,
                                 _frame.origin.y + _anchorPoint.y,
                                 _frame.origin.y + _anchorPoint.y + _frame.size.height,
                                 -1,
                                 1);
    kmGLMultMatrix( &orthoMatrix );
    
    kmGLMatrixMode(KM_GL_MODELVIEW);
    kmGLPushMatrix();
    kmGLLoadIdentity();
    
    _grabber->beforeRender(_texture->getName());
}

void CCHierarchiesSpriteFilter::afterDraw () {
    _grabber->afterRender();
    
    glViewport(_oldViewPort.x,
               _oldViewPort.y,
               _oldViewPort.w,
               _oldViewPort.h);
    
    kmGLMatrixMode(KM_GL_PROJECTION);
    kmGLPopMatrix();
    
    kmGLMatrixMode(KM_GL_MODELVIEW);
    kmGLPopMatrix();
    
    blit();
    
#if ENABLE_RTT_AND_GRABBER_CACHE == 1
    setIdleRenderTexture(_texture);
#endif
}

void CCHierarchiesSpriteFilter::blit () {
    ccGLBindTexture2D(_texture->getName());
    
    ccGLEnableVertexAttribs( kCCVertexAttribFlag_Position | kCCVertexAttribFlag_TexCoords );
    _shaderProgram->use();
    _shaderProgram->setUniformsForBuiltins();
    
    ccGLBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    
    // position attributes
    glVertexAttribPointer(kCCVertexAttrib_Position, 3, GL_FLOAT, GL_FALSE, 0, _vertices);
    
    // texCoords attributes
    glVertexAttribPointer(kCCVertexAttrib_TexCoords, 2, GL_FLOAT, GL_FALSE, 0, _texCoordinates);
    
    // draw with indices
    glDrawElements(GL_TRIANGLES, (GLsizei)6, GL_UNSIGNED_SHORT, _indices);
    
    CC_INCREMENT_GL_DRAWS(1);
}


#pragma mark - CCHierarchiesSpriteBlurFilter

CCHierarchiesSpriteBlurFilter::CCHierarchiesSpriteBlurFilter () {
}

CCHierarchiesSpriteBlurFilter::~CCHierarchiesSpriteBlurFilter () {
}

CCGLProgram* CCHierarchiesSpriteBlurFilter::setupShaderProgram () {
    CCGLProgram* glProgram = CCShaderCache::sharedShaderCache()->programForKey(kCCShader_HierarchiesSpriteFilter_Blur);
    if (glProgram == NULL) {
        glProgram = new CCGLProgram();
        glProgram->initWithVertexShaderByteArray(ccShader_HierarchiesSpriteFilter_vert, ccShader_HierarchiesSpriteFilter_Blur_frag);
        
        glProgram->addAttribute(kCCAttributeNamePosition, kCCVertexAttrib_Position);
        glProgram->addAttribute(kCCAttributeNameColor, kCCVertexAttrib_Color);
        glProgram->addAttribute(kCCAttributeNameTexCoord, kCCVertexAttrib_TexCoords);
        
        glProgram->link();
        glProgram->updateUniforms();
        
        CHECK_GL_ERROR_DEBUG();
        
        CCShaderCache::sharedShaderCache()->addProgram(glProgram, kCCShader_HierarchiesSpriteFilter_Blur);
        glProgram->autorelease();
    }
    
    //TODO: setup uniform
    
    return glProgram;
}

void CCHierarchiesSpriteBlurFilter::updateFilterParams (CCHierarchiesSpriteAnimation::Filter& filterParams) {
    //TODO
}


#pragma mark - CCHierarchiesSpriteGlowFilter

CCHierarchiesSpriteGlowFilter::CCHierarchiesSpriteGlowFilter () {
}

CCHierarchiesSpriteGlowFilter::~CCHierarchiesSpriteGlowFilter () {
}

CCGLProgram* CCHierarchiesSpriteGlowFilter::setupShaderProgram () {
    CCGLProgram* glProgram = CCShaderCache::sharedShaderCache()->programForKey(kCCShader_HierarchiesSpriteFilter_Glow);
    if (glProgram == NULL) {
        glProgram = new CCGLProgram();
        glProgram->initWithVertexShaderByteArray(ccShader_HierarchiesSpriteFilter_vert, ccShader_HierarchiesSpriteFilter_Glow_frag);
        
        glProgram->addAttribute(kCCAttributeNamePosition, kCCVertexAttrib_Position);
        glProgram->addAttribute(kCCAttributeNameColor, kCCVertexAttrib_Color);
        glProgram->addAttribute(kCCAttributeNameTexCoord, kCCVertexAttrib_TexCoords);
        
        glProgram->link();
        glProgram->updateUniforms();
        
        CHECK_GL_ERROR_DEBUG();
        
        CCShaderCache::sharedShaderCache()->addProgram(glProgram, kCCShader_HierarchiesSpriteFilter_Glow);
        glProgram->autorelease();
    }
    
    //TODO: setup uniform
    
    return glProgram;
}

void CCHierarchiesSpriteGlowFilter::updateFilterParams (CCHierarchiesSpriteAnimation::Filter& filterParams) {
    //TODO
}


#pragma mark - CCHierarchiesSpriteShadowFilter

CCHierarchiesSpriteShadowFilter::CCHierarchiesSpriteShadowFilter () {
}

CCHierarchiesSpriteShadowFilter::~CCHierarchiesSpriteShadowFilter () {
}

CCGLProgram* CCHierarchiesSpriteShadowFilter::setupShaderProgram () {
    CCGLProgram* glProgram = CCShaderCache::sharedShaderCache()->programForKey(kCCShader_HierarchiesSpriteFilter_Shadow);
    if (glProgram == NULL) {
        glProgram = new CCGLProgram();
        glProgram->initWithVertexShaderByteArray(ccShader_HierarchiesSpriteFilter_vert, ccShader_HierarchiesSpriteFilter_Shadow_frag);
        
        glProgram->addAttribute(kCCAttributeNamePosition, kCCVertexAttrib_Position);
        glProgram->addAttribute(kCCAttributeNameColor, kCCVertexAttrib_Color);
        glProgram->addAttribute(kCCAttributeNameTexCoord, kCCVertexAttrib_TexCoords);
        
        glProgram->link();
        glProgram->updateUniforms();
        
        CHECK_GL_ERROR_DEBUG();
        
        CCShaderCache::sharedShaderCache()->addProgram(glProgram, kCCShader_HierarchiesSpriteFilter_Shadow);
        glProgram->autorelease();
    }
    
    //TODO: setup uniform
    
    return glProgram;
}

void CCHierarchiesSpriteShadowFilter::updateFilterParams (CCHierarchiesSpriteAnimation::Filter& filterParams) {
    //TODO
}


#pragma mark - CCHierarchiesSpriteBevelFilter

CCHierarchiesSpriteBevelFilter::CCHierarchiesSpriteBevelFilter () {
}

CCHierarchiesSpriteBevelFilter::~CCHierarchiesSpriteBevelFilter () {
}

CCGLProgram* CCHierarchiesSpriteBevelFilter::setupShaderProgram () {
    CCGLProgram* glProgram = CCShaderCache::sharedShaderCache()->programForKey(kCCShader_HierarchiesSpriteFilter_Bevel_Outer);
    if (glProgram == NULL) {
        glProgram = new CCGLProgram();
        glProgram->initWithVertexShaderByteArray(ccShader_HierarchiesSpriteFilter_vert, ccShader_HierarchiesSpriteFilter_Bevel_Outer_frag);
        
        glProgram->addAttribute(kCCAttributeNamePosition, kCCVertexAttrib_Position);
        glProgram->addAttribute(kCCAttributeNameColor, kCCVertexAttrib_Color);
        glProgram->addAttribute(kCCAttributeNameTexCoord, kCCVertexAttrib_TexCoords);
        
        glProgram->link();
        glProgram->updateUniforms();
        
        CHECK_GL_ERROR_DEBUG();
        
        CCShaderCache::sharedShaderCache()->addProgram(glProgram, kCCShader_HierarchiesSpriteFilter_Bevel_Outer);
        glProgram->autorelease();
    }
    
    //TODO: setup uniform
    
    return glProgram;
}

void CCHierarchiesSpriteBevelFilter::updateFilterParams (CCHierarchiesSpriteAnimation::Filter& filterParams) {
    //TODO
}


NS_CC_EXT_END
