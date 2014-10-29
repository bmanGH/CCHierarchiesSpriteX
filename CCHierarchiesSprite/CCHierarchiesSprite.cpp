//
//  CCHierarchiesSprite.cpp
//  HierarchiesSpriteDemo
//
//  Created by bman on 10/26/14.
//
//

#include "CCHierarchiesSprite.h"

NS_CC_EXT_BEGIN


#pragma mark - Math

// void hierarchiesCalcMatrixSocket (HierarchiesSpriteAnimation::Element* element, AffineTransform* matrix) {
//    FMatrix2D m0;
//    FMatrix2D m1;
//
//    m0.setAnchorX(element->anchorX);
//    m0.setAnchorY(element->anchorY);
//
//    m1.setScaleX(element->scaleX);
//    m1.setScaleY(element->scaleY);
//    m1.setSkewX(element->skewX);
//    m1.setSkewY(element->skewY);
//    m1.setTransformX(element->x);
//    m1.setTransformY(element->y);
//
//    FMatrix2D m = m0.concat(m1);
//
//    *matrix = AffineTransformMake(m.a, -m.b, -m.c, m.d, m.tx, -m.ty);
//}

void hierarchiesCalcMatrix (const HierarchiesSpriteAnimation::Symbol* symbol,
                            const HierarchiesSpriteSheet::Spr* spr,
                            const HierarchiesSpriteAnimation::Element* element,
                            AffineTransform* matrix) {
    FMatrix2D m0;
    FMatrix2D m1;
    
    m0.setAnchorX(element->anchorX - symbol->left);
    m0.setAnchorY(element->anchorY - symbol->bottom);
    
    m1.setScaleX(element->scaleX);
    m1.setScaleY(element->scaleY);
    m1.setSkewX(element->skewX);
    m1.setSkewY(element->skewY);
    m1.setTransformX(element->x);
    m1.setTransformY(element->y);
    
    FMatrix2D m = m0.concat(m1);
    
    *matrix = AffineTransformMake(m.a, -m.b, -m.c, m.d, m.tx, -m.ty);
}

void hierarchiesCalcMatrixSocket (const HierarchiesSpriteAnimation::Element* element, AffineTransform* matrix) {
    FMatrix2D m0;
    FMatrix2D m1;
    
    m0.setAnchorX(element->anchorX);
    m0.setAnchorY(element->anchorY);
    
    m1.setScaleX(element->scaleX);
    m1.setScaleY(element->scaleY);
    m1.setSkewX(element->skewX);
    m1.setSkewY(element->skewY);
    m1.setTransformX(element->x);
    m1.setTransformY(element->y);
    
    FMatrix2D m = m0.concat(m1);
    
    *matrix = AffineTransformMake(m.a, -m.b, -m.c, m.d, m.tx, -m.ty);
}

void hierarchiesUpdateQuadVertices (Size size, const AffineTransform* matrix, HierarchiesSprite_V3F_C4B_T2F_Quad* quad, const float z, const bool isRotation) {
    if (isRotation) {
        float tmp = size.width;
        size.width = size.height;
        size.height = tmp;
    }
    
    float x1 = 0;
    float y1 = 0;
    
    float x2 = x1 + size.width;
    float y2 = y1 + size.height;
    float x = matrix->tx;
    float y = matrix->ty;
    
    float cr = matrix->a;
    float sr = matrix->b;
    float cr2 = matrix->d;
    float sr2 = -matrix->c;
    float ax = x1 * cr - y1 * sr2 + x;
    float ay = x1 * sr + y1 * cr2 + y;
    
    float bx = x2 * cr - y1 * sr2 + x;
    float by = x2 * sr + y1 * cr2 + y;
    
    float cx = x2 * cr - y2 * sr2 + x;
    float cy = x2 * sr + y2 * cr2 + y;
    
    float dx = x1 * cr - y2 * sr2 + x;
    float dy = x1 * sr + y2 * cr2 + y;
    
	quad->bl.vertices = (Vec3) { ax, ay, z };
    quad->br.vertices = (Vec3) { bx, by, z };
    quad->tl.vertices = (Vec3) { dx, dy, z };
    quad->tr.vertices = (Vec3) { cx, cy, z };
}

void hierarchiesUpdateQuadTextureCoords (const Rect rect, const float texWidth, const float texHeight, HierarchiesSprite_V3F_C4B_T2F_Quad* quad, const bool isRotation) {
    float left, right, top, bottom;
    
#if CC_FIX_ARTIFACTS_BY_STRECHING_TEXEL
    left	= (2 * rect.origin.x + 1) / (2 * texWidth);
    right	= left + (rect.size.width * 2 - 2) / (2 * texWidth);
    top		= (2 * rect.origin.y + 1) / (2 * texHeight);
    bottom	= top + (rect.size.height * 2 - 2) / (2 * texHeight);
#else
    left	= rect.origin.x / texWidth;
    right	= left + rect.size.width / texWidth;
    top		= rect.origin.y / texHeight;
    bottom	= top + rect.size.height / texHeight;
#endif
    
    if (!isRotation) {
        quad->bl.texCoords.u = left;
        quad->bl.texCoords.v = bottom;
        quad->br.texCoords.u = right;
        quad->br.texCoords.v = bottom;
        quad->tl.texCoords.u = left;
        quad->tl.texCoords.v = top;
        quad->tr.texCoords.u = right;
        quad->tr.texCoords.v = top;
    }
    else {
        quad->bl.texCoords.u = left; // tl
        quad->bl.texCoords.v = top; // tl
        quad->br.texCoords.u = left; // bl
        quad->br.texCoords.v = bottom; // bl
        quad->tl.texCoords.u = right; // tr
        quad->tl.texCoords.v = top; // tr
        quad->tr.texCoords.u = right; // br
        quad->tr.texCoords.v = bottom; // br
    }
}

void hierarchiesUpdateQuadTextureColorFromAnimation (const float alpha_percent, const int alpha_amount,
                                                     const float red_percent, const int red_amount,
                                                     const float green_percent, const int green_amount,
                                                     const float blue_percent, const int blue_amount,
                                                     HierarchiesSprite_V3F_C4B_T2F_Quad* quad) {
    int value;
    
    value = 255 * alpha_percent;
    quad->bl.colorsMul.a = value;
    quad->br.colorsMul.a = value;
    quad->tl.colorsMul.a = value;
    quad->tr.colorsMul.a = value;
    quad->bl.colorsAdd.a = alpha_amount;
    quad->br.colorsAdd.a = alpha_amount;
    quad->tl.colorsAdd.a = alpha_amount;
    quad->tr.colorsAdd.a = alpha_amount;
    
    value = 255 * red_percent;
    quad->bl.colorsMul.r = value;
    quad->br.colorsMul.r = value;
    quad->tl.colorsMul.r = value;
    quad->tr.colorsMul.r = value;
    quad->bl.colorsAdd.r = red_amount;
    quad->br.colorsAdd.r = red_amount;
    quad->tl.colorsAdd.r = red_amount;
    quad->tr.colorsAdd.r = red_amount;
    
    value = 255 * green_percent;
    quad->bl.colorsMul.g = value;
    quad->br.colorsMul.g = value;
    quad->tl.colorsMul.g = value;
    quad->tr.colorsMul.g = value;
    quad->bl.colorsAdd.g = green_amount;
    quad->br.colorsAdd.g = green_amount;
    quad->tl.colorsAdd.g = green_amount;
    quad->tr.colorsAdd.g = green_amount;
    
    value = 255 * blue_percent;
    quad->bl.colorsMul.b = value;
    quad->br.colorsMul.b = value;
    quad->tl.colorsMul.b = value;
    quad->tr.colorsMul.b = value;
    quad->bl.colorsAdd.b = blue_amount;
    quad->br.colorsAdd.b = blue_amount;
    quad->tl.colorsAdd.b = blue_amount;
    quad->tr.colorsAdd.b = blue_amount;
}

//void hierarchiesUpdateQuadTextureColor (const bool opacityModifyRGB, const int opacity, const int color_r, const int color_g, const int color_b, CCHierarchiesSprite_V3F_C4B_T2F_Quad* quad) {
//	// If opacityModifyRGB is NO then opacity will be applied as: glColor(R,G,B,opacity);
//	// If opacityModifyRGB is YES then oapcity will be applied as: glColor(opacity, opacity, opacity, opacity );
//	if (opacityModifyRGB) {
//		quad->bl.colorsMul.a = quad->bl.colorsMul.a * (opacity / 255.0f);
//		quad->br.colorsMul.a = quad->br.colorsMul.a * (opacity / 255.0f);
//		quad->tl.colorsMul.a = quad->tl.colorsMul.a * (opacity / 255.0f);
//		quad->tr.colorsMul.a = quad->tr.colorsMul.a * (opacity / 255.0f);
//
//		quad->bl.colorsMul.r = quad->bl.colorsMul.r * (color_r / 255.0f) * (opacity / 255.0f);
//		quad->br.colorsMul.r = quad->br.colorsMul.r * (color_r / 255.0f) * (opacity / 255.0f);
//		quad->tl.colorsMul.r = quad->tl.colorsMul.r * (color_r / 255.0f) * (opacity / 255.0f);
//		quad->tr.colorsMul.r = quad->tr.colorsMul.r * (color_r / 255.0f) * (opacity / 255.0f);
//
//		quad->bl.colorsMul.g = quad->bl.colorsMul.g * (color_g / 255.0f) * (opacity / 255.0f);
//		quad->br.colorsMul.g = quad->br.colorsMul.g * (color_g / 255.0f) * (opacity / 255.0f);
//		quad->tl.colorsMul.g = quad->tl.colorsMul.g * (color_g / 255.0f) * (opacity / 255.0f);
//		quad->tr.colorsMul.g = quad->tr.colorsMul.g * (color_g / 255.0f) * (opacity / 255.0f);
//
//		quad->bl.colorsMul.b = quad->bl.colorsMul.b * (color_b / 255.0f) * (opacity / 255.0f);
//		quad->br.colorsMul.b = quad->br.colorsMul.b * (color_b / 255.0f) * (opacity / 255.0f);
//		quad->tl.colorsMul.b = quad->tl.colorsMul.b * (color_b / 255.0f) * (opacity / 255.0f);
//		quad->tr.colorsMul.b = quad->tr.colorsMul.b * (color_b / 255.0f) * (opacity / 255.0f);
//	}
//	else {
//		quad->bl.colorsMul.a = quad->bl.colorsMul.a * (opacity / 255.0f);
//		quad->br.colorsMul.a = quad->br.colorsMul.a * (opacity / 255.0f);
//		quad->tl.colorsMul.a = quad->tl.colorsMul.a * (opacity / 255.0f);
//		quad->tr.colorsMul.a = quad->tr.colorsMul.a * (opacity / 255.0f);
//
//		quad->bl.colorsMul.r = quad->bl.colorsMul.r * (color_r / 255.0f);
//		quad->br.colorsMul.r = quad->br.colorsMul.r * (color_r / 255.0f);
//		quad->tl.colorsMul.r = quad->tl.colorsMul.r * (color_r / 255.0f);
//		quad->tr.colorsMul.r = quad->tr.colorsMul.r * (color_r / 255.0f);
//
//		quad->bl.colorsMul.g = quad->bl.colorsMul.g * (color_g / 255.0f);
//		quad->br.colorsMul.g = quad->br.colorsMul.g * (color_g / 255.0f);
//		quad->tl.colorsMul.g = quad->tl.colorsMul.g * (color_g / 255.0f);
//		quad->tr.colorsMul.g = quad->tr.colorsMul.g * (color_g / 255.0f);
//
//		quad->bl.colorsMul.b = quad->bl.colorsMul.b * (color_b / 255.0f);
//		quad->br.colorsMul.b = quad->br.colorsMul.b * (color_b / 255.0f);
//		quad->tl.colorsMul.b = quad->tl.colorsMul.b * (color_b / 255.0f);
//		quad->tr.colorsMul.b = quad->tr.colorsMul.b * (color_b / 255.0f);
//	}
//}

void hierarchiesExpandRectByPoint (float* minX, float* maxX, float* minY, float* maxY, float* pX, float* pY) {
    if (*pX < *minX) {
        *minX = *pX;
    }
    if (*pX > *maxX) {
        *maxX = *pX;
    }
    
    if (*pY < *minY) {
        *minY = *pY;
    }
    if (*pY > *maxY) {
        *maxY = *pY;
    }
}

float float_round (float r) {
    return (r > 0.0f) ? floorf(r + 0.5f) : ceilf(r - 0.5f);
}


#pragma mark - HierarchiesSprite

HierarchiesSprite* HierarchiesSprite::create (const std::string& sheetFileName,
                                              const std::string& animationFileName) {
    std::string animationFileNameBase;
    std::string animationFileNameSub;
    size_t found = animationFileName.find_last_of('/');
    if (found != std::string::npos) {
        animationFileNameBase = animationFileName.substr(0, found + 1);
        animationFileNameSub = animationFileName.substr(found + 1);
    }
    else {
        animationFileNameSub = animationFileName;
    }
    
	return HierarchiesSprite::create(sheetFileName,
                                     animationFileNameBase,
                                     animationFileNameSub);
}

HierarchiesSprite* HierarchiesSprite::create (const std::string& sheetFileName,
                                              const std::string& animationFileNameBase,
                                              const std::string& animationFileNameSub) {
    HierarchiesSprite* ret = new (std::nothrow) HierarchiesSprite();
	if (ret && ret->init(sheetFileName,
                         animationFileNameBase,
                         animationFileNameSub,
                         AvatarMapType()))
	{
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);
	return NULL;
}

HierarchiesSprite* HierarchiesSprite::create (const std::string& sheetFileName,
                                              const std::string& animationFileName,
                                              const AvatarMapType& avatarMap) {
    std::string animationFileNameBase;
    std::string animationFileNameSub;
    size_t found = animationFileName.find_last_of('/');
    if (found != std::string::npos) {
        animationFileNameBase = animationFileName.substr(0, found + 1);
        animationFileNameSub = animationFileName.substr(found + 1);
    }
    else {
        animationFileNameSub = animationFileName;
    }
    
    return HierarchiesSprite::create(sheetFileName,
                                       animationFileNameBase,
                                       animationFileNameSub,
                                       avatarMap);
}

HierarchiesSprite* HierarchiesSprite::create (const std::string& sheetFileName,
                                              const std::string& animationFileNameBase,
                                              const std::string& animationFileNameSub,
                                              const AvatarMapType& avatarMap) {
    HierarchiesSprite* ret = new (std::nothrow) HierarchiesSprite();
	if (ret && ret->init(sheetFileName,
                         animationFileNameBase,
                         animationFileNameSub,
                         avatarMap))
	{
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);
	return NULL;
}

HierarchiesSprite::HierarchiesSprite ()
: _sheet(nullptr)
, _animation(nullptr)
//, _mesh(nullptr)
, _curFrameIndex(0)
, _needFresh(false)
, _blendFunc(BlendFunc::DISABLE)
, _opacityModifyRGB(false)
, _flipX(false)
, _flipY(false)
, _bbox(Rect::ZERO)
, _texture(nullptr) {
}

HierarchiesSprite::~HierarchiesSprite () {
    CC_SAFE_RELEASE_NULL(_texture);
//    CC_SAFE_RELEASE_NULL(_mesh);
    
    HierarchiesSpriteRuntime::getInstance()->removeSheet(_sheetName);
    HierarchiesSpriteRuntime::getInstance()->removeAnimation(_animationName);
}

bool HierarchiesSprite::init (const std::string& sheetFileName,
                              const std::string& animationFileNameBase,
                              const std::string& animationFileNameSub,
                              const AvatarMapType& avatarMap) {
    if (CCNodeRGBA::init()) {
        _sheetName = sheetFileName;
        _animationNameBase = animationFileNameBase;
        _animationNameSub = animationFileNameSub;
        _animationName = _animationNameBase + _animationNameSub;
        
        _blendFunc.src = GL_SRC_ALPHA;
        _blendFunc.dst = GL_ONE_MINUS_SRC_ALPHA;
        
        _flipX = false;
        _flipY = false;
        
        _curFrameIndex = UINT_MAX;
        
        _sheet = HierarchiesSpriteRuntime::getInstance()->addSheet(_sheetName);
        _animation = HierarchiesSpriteRuntime::getInstance()->addAnimation(_animationName);
        
        // image should load from the same directory with spritesheet file
        std::string imageName(_sheet->getImageName());
        size_t found = _sheetName.find_last_of('/');
        if (found != std::string::npos)
            imageName = _sheetName.substr(0, found + 1) + imageName;
        
        _texture = Director::getInstance()->getTextureCache()->addImage(imageName);
        CC_SAFE_RETAIN(_texture);
        
        this->updateBlendFunc();
        this->updateOpacityModifyRGB();
        
        return true;
    }
    return false;
}


#pragma mark - Property

void HierarchiesSprite::setFlipX (bool value) {
    _flipX = value;
    _transformDirty = _inverseDirty = true;
}

void HierarchiesSprite::setFlipY (bool value) {
    _flipY = value;
    _transformDirty = _inverseDirty = true;
}

void HierarchiesSprite::setColorAmount (const Color3B& value) {
    _colorAmount = value;
}

void HierarchiesSprite::setTexture(Texture2D *texture) {
    if (_texture != texture) {
        CC_SAFE_RETAIN(texture);
        CC_SAFE_RELEASE(_texture);
        _texture = texture;
        
        this->updateBlendFunc();
        this->updateOpacityModifyRGB();
        
        this->freshCurrentFrame();
    }
}

unsigned int HierarchiesSprite::getEventCount (const std::string& eventName) {
    const std::vector<HierarchiesSpriteAnimation::Event>& events = _animation->getEvents();
    unsigned int ret = 0;
    std::vector<HierarchiesSpriteAnimation::Event>::const_iterator iter;
    for (iter = events.cbegin(); iter != events.cend(); iter++) {
        if (iter->name.compare(eventName) == 0)
            ret++;
    }
    return ret;
}


#pragma mark - Animation

bool HierarchiesSprite::displayFrameAtIndex (unsigned int frameIndex) {
    if (frameIndex > _animation->getFrameCount() - 1)
        return false;
    
	if (_curFrameIndex == frameIndex)
        return true;
	
//	if (_curFrameIndex != frameIndex) {
//		// event dispatch
//		if (_delegate) {
//			CCHierarchiesSpriteAnimation::Event event;
//			if (_animation->getEventByFrameId(frameIndex, event)) {
//				_delegate->onEventContent(this, event.content.c_str());
//			}
//		}
//	}
    
    _needFresh = true;
    
    _curFrameIndex = frameIndex;
    
    return true;
}

bool HierarchiesSprite::freshCurrentFrame () {
	unsigned int frameIndex = _curFrameIndex;
    _curFrameIndex = UINT_MAX;
	return displayFrameAtIndex(frameIndex);
}

unsigned int HierarchiesSprite::getCurrentFrameIndex () {
    return _curFrameIndex;
}


#pragma mark - Avatar

void HierarchiesSprite::setAvatarMap (const std::string& name, const std::string& mapName) {
	_avatarMap[name] = mapName;
    this->freshCurrentFrame();
}

void HierarchiesSprite::setAvatarMap (const AvatarMapType& map) {
    for (auto mapItem : map) {
        _avatarMap[mapItem.first] = mapItem.second;
    }
    this->freshCurrentFrame();
}

std::string HierarchiesSprite::getAvatarMap (const std::string& name) {
    AvatarMapType::iterator mapIter = _avatarMap.find(name);
    if (mapIter != _avatarMap.end()) {
        return mapIter->second;
    }
    return "";
}

void HierarchiesSprite::resetAvatarMap () {
    _avatarMap.clear();
    this->freshCurrentFrame();
}


#pragma mark - Draw

void HierarchiesSprite::draw(Renderer *renderer, const Mat4& transform, uint32_t flags) {
    if (0 == _quads.size())
    {
        return;
    }
    
    if (_needFresh) {
        _indices.clear();
        _indices.resize(_quads.size() * 6);
        
        for(size_t i = 0; i < _quads.size(); i++)
        {
#if HIERARCHIES_USE_TRIANGLE_STRIP
            _indices[i*6+0] = i*4+0;
            _indices[i*6+1] = i*4+0;
            _indices[i*6+2] = i*4+2;
            _indices[i*6+3] = i*4+1;
            _indices[i*6+4] = i*4+3;
            _indices[i*6+5] = i*4+3;
#else
            _indices[i*6+0] = i*4+0;
            _indices[i*6+1] = i*4+1;
            _indices[i*6+2] = i*4+2;
            _indices[i*6+3] = i*4+3;
            _indices[i*6+4] = i*4+2;
            _indices[i*6+5] = i*4+1;
#endif
        }
        
        _needFresh = false;
    }
    
    // ignore the anchor point while drawing
    CCPoint ap = this->getAnchorPointInPoints();
    kmGLTranslatef(ap.x, ap.y, 0);
    
    // draw with Vertex Array
    CC_PROFILER_START_CATEGORY(HIERARCHIES_SPRITE_PROFILER, "CCHierarchiesSpriteDynamic - draw");
    
    CC_NODE_DRAW_SETUP();
    
    GLfloat colors[4];
    colors[0] = _displayedColor.r / 255.0f;
    colors[1] = _displayedColor.g / 255.0f;
    colors[2] = _displayedColor.b / 255.0f;
    colors[3] = _displayedOpacity / 255.0f;
    this->getShaderProgram()->setUniformLocationWith4fv(_uniformColorLocation, colors, 1);
    
    ccGLBlendFunc( _blendFunc.src, _blendFunc.dst );
    
    ccGLBindTexture2D(_texture->getName());
    
    ccGLEnableVertexAttribs(kCCVertexAttribFlag_Position | kCCVertexAttribFlag_TexCoords);
    glEnableVertexAttribArray(kCCHierarchiesSprite_VertexAttrib_Color_Mul);
    glEnableVertexAttribArray(kCCHierarchiesSprite_VertexAttrib_Color_Add);
    
#   define kQuadSize sizeof(_quads[0].tl)
    // vertices
    glVertexAttribPointer(kCCVertexAttrib_Position, 3, GL_FLOAT, GL_FALSE, kQuadSize,
                          (GLvoid*) &_quads[0].tl.vertices);
    
    // colors
    glVertexAttribPointer(kCCHierarchiesSprite_VertexAttrib_Color_Mul, 4, GL_UNSIGNED_BYTE, GL_TRUE, kQuadSize,
                          (GLvoid*) &_quads[0].tl.colorsMul);
    glVertexAttribPointer(kCCHierarchiesSprite_VertexAttrib_Color_Add, 4, GL_UNSIGNED_BYTE, GL_TRUE, kQuadSize,
                          (GLvoid*) &_quads[0].tl.colorsAdd);
    
    // tex coords
    glVertexAttribPointer(kCCVertexAttrib_TexCoords, 2, GL_FLOAT, GL_FALSE, kQuadSize,
                          (GLvoid*) &_quads[0].tl.texCoords);
    
    
#if HIERARCHIES_USE_TRIANGLE_STRIP
    glDrawElements(GL_TRIANGLE_STRIP, (GLsizei)_quads.size()*6, GL_UNSIGNED_SHORT, (GLvoid*) &_indices[0]);
#else
    glDrawElements(GL_TRIANGLES, (GLsizei)_quads.size()*6, GL_UNSIGNED_SHORT, (GLvoid*) &_indices[0]);
#endif // HIERARCHIES_USE_TRIANGLE_STRIP
    
    glDisableVertexAttribArray(kCCHierarchiesSprite_VertexAttrib_Color_Mul);
    glDisableVertexAttribArray(kCCHierarchiesSprite_VertexAttrib_Color_Add);
    
    CC_INCREMENT_GL_DRAWS(1);
    CHECK_GL_ERROR_DEBUG();
    
#if HIERARCHIES_DEBUG_DRAW == 1
    // draw bounding box
    ccDrawColor4B(255, 0, 255, 255);
    CCPoint vertices[4] = {
        ccp(_bbox.origin.x, _bbox.origin.y),
        ccp(_bbox.origin.x + _bbox.size.width, _bbox.origin.y),
        ccp(_bbox.origin.x + _bbox.size.width, _bbox.origin.y + _bbox.size.height),
        ccp(_bbox.origin.x, _bbox.origin.y + _bbox.size.height),
    };
    ccDrawPoly(vertices, 4, true);
    
    //#elif HIERARCHIES_DEBUG_DRAW == 2
    //            // draw nesting bounding box
    //            ccDrawColor4B(0, 255, 255, 255);
    //            CCRect bbox = this->nestingBoundingBoxInner();
    //            CCPoint vertices2[4] = {
    //                ccp(bbox.origin.x, bbox.origin.y),
    //                ccp(bbox.origin.x + bbox.size.width, bbox.origin.y),
    //                ccp(bbox.origin.x + bbox.size.width, bbox.origin.y + bbox.size.height),
    //                ccp(bbox.origin.x, bbox.origin.y + bbox.size.height),
    //            };
    //            ccDrawPoly(vertices2, 4, true);
    
#elif HIERARCHIES_DEBUG_DRAW == 3
    // draw quads
    ccDrawColor4B(0, 255, 0, 255);
    for (int i = 0; i < _quads.size(); i++) {
        CCPoint vertices3[4] = {
            ccp(_quads[i].tl.vertices.x, _quads[i].tl.vertices.y),
            ccp(_quads[i].tr.vertices.x, _quads[i].tr.vertices.y),
            ccp(_quads[i].br.vertices.x, _quads[i].br.vertices.y),
            ccp(_quads[i].bl.vertices.x, _quads[i].bl.vertices.y),
        };
        ccDrawPoly(vertices3, 4, true);
    }
    
    ccDrawColor4B(255, 255, 255, 255);
#endif
    
    CC_PROFILER_STOP_CATEGORY(HIERARCHIES_SPRITE_PROFILER, "CCHierarchiesSpriteDynamic - draw");
}


#pragma mark - Node

AffineTransform HierarchiesSprite::nodeToParentTransform () {
    if (_transformDirty)
    {
        // flip anchor point in points
        Point obAnchorPointInPoints = _obAnchorPointInPoints;
        if (_flipX && _flipY) {
            obAnchorPointInPoints = Point(-_obAnchorPointInPoints.x, -_obAnchorPointInPoints.y);
        }
        else if (_flipX) {
            obAnchorPointInPoints = Point(-_obAnchorPointInPoints.x, _obAnchorPointInPoints.y);
        }
        else if (_flipY) {
            obAnchorPointInPoints = Point(_obAnchorPointInPoints.x, -_obAnchorPointInPoints.y);
        }
        
        // Translate values
        float x = _obPosition.x;
        float y = _obPosition.y;
        
        if (_ignoreAnchorPointForPosition)
        {
            x += obAnchorPointInPoints.x;
            y += obAnchorPointInPoints.y;
        }
        
        // Rotation values
		// Change rotation code to handle X and Y
		// If we skew with the exact same value for both x and y then we're simply just rotating
        float cx = 1, sx = 0, cy = 1, sy = 0;
        if (_rotationX || _rotationY)
        {
            float radiansX = -CC_DEGREES_TO_RADIANS(_rotationX);
            float radiansY = -CC_DEGREES_TO_RADIANS(_rotationY);
            cx = cosf(radiansX);
            sx = sinf(radiansX);
            cy = cosf(radiansY);
            sy = sinf(radiansY);
        }
        
        bool needsSkewMatrix = ( _skewX || _skewY );
        
        
        // optimization:
        // inline anchor point calculation if skew is not needed
        // Adjusted transform calculation for rotational skew
        if (! needsSkewMatrix && !obAnchorPointInPoints.equals(Point::ZERO))
        {
            x += cy * -obAnchorPointInPoints.x * _scaleX + -sx * -obAnchorPointInPoints.y * _scaleY;
            y += sy * -obAnchorPointInPoints.x * _scaleX +  cx * -obAnchorPointInPoints.y * _scaleY;
        }
        
        
        // Build Transform Matrix
        // Adjusted transform calculation for rotational skew
        _transform = AffineTransformMake( cy * _scaleX,  sy * _scaleX,
                                         -sx * _scaleY, cx * _scaleY,
                                         x, y );
        
        // XXX: Try to inline skew
        // If skew is needed, apply skew and then anchor point
        if (needsSkewMatrix)
        {
            AffineTransform skewMatrix = AffineTransformMake(1.0f, tanf(CC_DEGREES_TO_RADIANS(_skewY)),
                                                                 tanf(CC_DEGREES_TO_RADIANS(_skewX)), 1.0f,
                                                                 0.0f, 0.0f );
            _transform = AffineTransformConcat(skewMatrix, _transform);
            
            // adjust anchor point
            if (!obAnchorPointInPoints.equals(Point::ZERO))
            {
                _transform = AffineTransformTranslate(_transform, -obAnchorPointInPoints.x, -obAnchorPointInPoints.y);
            }
        }
        
        if (_additionalTransformDirty)
        {
            _transform = AffineTransformConcat(_transform, _additionalTransform);
            _additionalTransformDirty = false;
        }
        
        // flip
        if (_flipX && _flipY) {
            _transform = AffineTransformScale(_transform, -1, -1);
        }
        else if (_flipX) {
            _transform = AffineTransformScale(_transform, -1, 1);
        }
        else if (_flipY) {
            _transform = AffineTransformScale(_transform, 1, -1);
        }
        
        _transformDirty = false;
    }
    
    return _transform;
}

void HierarchiesSprite::setColor (const ccColor3B& color3) {
    Node::setColor(color3);
    this->freshCurrentFrame();
}

void HierarchiesSprite::setOpacity (GLubyte opacity) {
    Node::setOpacity(opacity);
    this->freshCurrentFrame();
}

void HierarchiesSprite::updateDisplayedColor(const ccColor3B& color) {
    Node::updateDisplayedColor(color);
    this->freshCurrentFrame();
}

void HierarchiesSprite::updateDisplayedOpacity(GLubyte opacity) {
    CCNodeRGBA::updateDisplayedOpacity(opacity);
    this->freshCurrentFrame();
}

void HierarchiesSprite::setOpacityModifyRGB (bool bValue) {
    if (_opacityModifyRGB != bValue) {
        _opacityModifyRGB = bValue;
        
        // setup shader program
        if (_opacityModifyRGB) {
            this->setGLProgramState(GLProgramState::getOrCreateWithGLProgramName(kShader_Name_HierarchiesSprite_Premultiplied));
            
            this->freshCurrentFrame();
        }
        else {
            this->setGLProgramState(GLProgramState::getOrCreateWithGLProgramName(kShader_Name_HierarchiesSprite));
            
            this->freshCurrentFrame();
        }
    }
}

bool HierarchiesSprite::isOpacityModifyRGB (void) {
	return _opacityModifyRGB;
}


#pragma mark - BlendProtocol

void HierarchiesSprite::setBlendFunc (ccBlendFunc value) {
	_blendFunc = value;
}

ccBlendFunc HierarchiesSprite::getBlendFunc (void) {
	return _blendFunc;
}


#pragma mark - TextureProtocol

CCTexture2D* HierarchiesSprite::getTexture (void) {
    return _texture;
}

void HierarchiesSprite::setTexture(CCTexture2D *texture) {
    if (_texture != texture) {
        CC_SAFE_RETAIN(texture);
        CC_SAFE_RELEASE(_texture);
        _texture = texture;
        
        this->updateBlendFunc();
        this->updateOpacityModifyRGB();
        
        this->freshCurrentFrame();
    }
}


#pragma mark - Private

void HierarchiesSprite::updateOpacityModifyRGB () {
	_opacityModifyRGB = _texture->hasPremultipliedAlpha();
    
    // setup shader program
    if (_opacityModifyRGB) {
        this->setGLProgramState(GLProgramState::getOrCreateWithGLProgramName(kShader_Name_HierarchiesSprite_Premultiplied));
        
        _uniformColorLocation = glProgram->getUniformLocationForName("u_color");
    }
    else {
        this->setGLProgramState(GLProgramState::getOrCreateWithGLProgramName(kShader_Name_HierarchiesSprite));
        
        _uniformColorLocation = glProgram->getUniformLocationForName("u_color");
    }
}

void HierarchiesSprite::updateBlendFunc () {
	if( !_texture->hasPremultipliedAlpha() ) {
		_blendFunc.src = GL_SRC_ALPHA;
		_blendFunc.dst = GL_ONE_MINUS_SRC_ALPHA;
	}
    else {
        _blendFunc.src = GL_ONE;
		_blendFunc.dst = GL_ONE_MINUS_SRC_ALPHA;
    }
}

void HierarchiesSprite::buildAnimationData (HierarchiesSpriteAnimation::ElementLoopMode loopMode,
                                            int frameOffset,
                                            unsigned int frameIndex,
                                            const AffineTransform& parentMatrix,
                                            const HierarchiesSpriteAnimation* animation,
                                            float& min_X, float& max_X, float& min_Y, float& max_Y,
                                            const float parent_alpha_percent, const int parent_alpha_amount,
                                            const float parent_red_percent, const int parent_red_amount,
                                            const float parent_green_percent, const int parent_green_amount,
                                            const float parent_blue_percent, const int parent_blue_amount) {
    CCHierarchiesSpriteAnimation::FrameElements frameElements;
    int eNum = animation->getFrameElementsAtFrameIndex(loopMode, frameOffset, frameIndex, frameElements); //!!! frameIndex will update with loop mode
    
    _quads.reserve(_quads.size() + eNum);
    
    CCAffineTransform matrix;
    CCHierarchiesSprite_V3F_C4B_T2F_Quad quad;
    CCHierarchiesSpriteAnimation::FrameElements::const_iterator layerIter;
    int layerZOrder = 0;
    //        unsigned int quadsIndex = 0;
    //        unsigned int quadsCount = 0;
    //!!! reverse order to display
    for (layerIter = frameElements.cbegin(); layerIter != frameElements.cend(); layerIter++) { // element layers BEGIN
        const CCHierarchiesSpriteAnimation::Element* elementIter = &(*layerIter);
        
        CCHierarchiesSpriteAnimation::Symbol symbol;
        bool result = animation->getSymbolByIndex(elementIter->symbolIndex, symbol);
        assert(result);
        
        if (symbol.isSocket == false) { // simple element BEGIN
            CCHierarchiesSpriteSheet::Spr spr;
            result = _sheet->getSpr(symbol.name.c_str(), spr);
            assert(result);
            
            // calc matrix
            hierarchiesCalcMatrix(&symbol, &spr, &(*elementIter), &matrix);
            matrix = CCAffineTransformConcat(matrix, parentMatrix);
            
            // update vertices
            hierarchiesUpdateQuadVertices(CCSizeMake(spr.w, spr.h),
                                          &matrix,
                                          &quad,
                                          layerZOrder * HIERARCHIES_SPRITE_LAYER_Z_ORDER_SCALE,
                                          spr.isRotation);
            
            // update color from animation
            float alpha_percent = parent_alpha_percent * elementIter->color_alpha_percent;
            int alpha_amount = parent_alpha_percent * elementIter->color_alpha_amount + parent_alpha_amount;
            float red_percent = parent_red_percent * elementIter->color_red_percent;
            int red_amount = parent_red_percent * elementIter->color_red_amount + parent_red_amount;
            float green_percent = parent_green_percent * elementIter->color_green_percent;
            int green_amount = parent_green_percent * elementIter->color_green_amount + parent_green_amount;
            float blue_percent = parent_blue_percent * elementIter->color_blue_percent;
            int blue_amount = parent_blue_percent * elementIter->color_blue_amount + parent_blue_amount;
            hierarchiesUpdateQuadTextureColorFromAnimation (alpha_percent, alpha_amount,
                                                            red_percent, red_amount,
                                                            green_percent, green_amount,
                                                            blue_percent, blue_amount,
                                                            &quad);
            
            hierarchiesExpandRectByPoint(&min_X, &max_X, &min_Y, &max_Y, &quad.bl.vertices.x, &quad.bl.vertices.y);
            hierarchiesExpandRectByPoint(&min_X, &max_X, &min_Y, &max_Y, &quad.br.vertices.x, &quad.br.vertices.y);
            hierarchiesExpandRectByPoint(&min_X, &max_X, &min_Y, &max_Y, &quad.tl.vertices.x, &quad.tl.vertices.y);
            hierarchiesExpandRectByPoint(&min_X, &max_X, &min_Y, &max_Y, &quad.tr.vertices.x, &quad.tr.vertices.y);
            
            hierarchiesUpdateQuadTextureCoords(CCRectMake(spr.x, spr.y, spr.w, spr.h),
                                               _sheet->getImageWidth(),
                                               _sheet->getImageHeight(),
                                               &quad,
                                               spr.isRotation);
            
            _quads.push_back(quad);
            
            //                quadsCount++;
        } // simple element END
        else { // nesting sprite element BEGIN
            std::string subSpriteAnimationName(_animationNameBase);
            if (_avatarMap.count(symbol.name) > 0) { // with avatar support
                subSpriteAnimationName += _avatarMap.at(symbol.name) + ".hanims";
            }
            else {
                subSpriteAnimationName += symbol.name + ".hanims";
            }
            CCHierarchiesSpriteAnimation* subAnimation = CCHierarchiesSpriteAnimationCache::sharedHierarchiesSpriteAnimationCache()->addAnimation(subSpriteAnimationName.c_str());
            
            hierarchiesCalcMatrixSocket(&(*elementIter), &matrix);
            matrix = CCAffineTransformConcat(matrix, parentMatrix);
            
            float alpha_percent = parent_alpha_percent * elementIter->color_alpha_percent;
            int alpha_amount = parent_alpha_percent * elementIter->color_alpha_amount + parent_alpha_amount;
            float red_percent = parent_red_percent * elementIter->color_red_percent;
            int red_amount = parent_red_percent * elementIter->color_red_amount + parent_red_amount;
            float green_percent = parent_green_percent * elementIter->color_green_percent;
            int green_amount = parent_green_percent * elementIter->color_green_amount + parent_green_amount;
            float blue_percent = parent_blue_percent * elementIter->color_blue_percent;
            int blue_amount = parent_blue_percent * elementIter->color_blue_amount + parent_blue_amount;
            this->buildDynamicAnimationData(elementIter->loopMode,
                                            elementIter->frameOffset,
                                            frameIndex - elementIter->startDisplayFrameIndex,
                                            matrix,
                                            subAnimation,
                                            min_X, max_X, min_Y, max_Y,
                                            alpha_percent, alpha_amount,
                                            red_percent, red_amount,
                                            green_percent, green_amount,
                                            blue_percent, blue_amount);
        } // nesting sprite element END
        
        layerZOrder++;
    } // element layers END
}


NS_CC_EXT_END

#pragma mark - HierarchiesSprite