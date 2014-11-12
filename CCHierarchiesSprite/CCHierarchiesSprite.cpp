//
//  CCHierarchiesSprite.cpp
//  HierarchiesSpriteDemo
//
//  Created by bman on 10/26/14.
//
//

#include "CCHierarchiesSprite.h"
#include "CCHierarchiesSpriteShader.h"

NS_CC_EXT_BEGIN


#pragma mark - Math

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
	if (ret && ret->initWithFile(sheetFileName,
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
	if (ret && ret->initWithFile(sheetFileName,
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
, _curFrameIndex(UINT_MAX)
, _blendFunc(BlendFunc::DISABLE)
, _opacityModifyRGB(false)
, _flipX(false)
, _flipY(false)
, _displayedColorAdd(0, 0, 0, 0)
, _bbox(Rect::ZERO)
, _insideBounds(true)
, _texture(nullptr)
, _vertexBuffer(nullptr)
, _indexBuffer(nullptr)
, _vertexData(nullptr)
, _primitive(nullptr)
{
#if CC_SPRITE_DEBUG_DRAW
    _debugDrawNode = DrawNode::create();
    addChild(_debugDrawNode);
#endif
}

HierarchiesSprite::~HierarchiesSprite ()
{
    CC_SAFE_RELEASE(_texture);
    CC_SAFE_RELEASE(_vertexBuffer);
    CC_SAFE_RELEASE(_indexBuffer);
    CC_SAFE_RELEASE(_vertexData);
    CC_SAFE_RELEASE(_primitive);
    
    HierarchiesSpriteRuntime::getInstance()->removeSheet(_sheetName);
    HierarchiesSpriteRuntime::getInstance()->removeAnimation(_animationName);
}

bool HierarchiesSprite::initWithFile (const std::string& sheetFileName,
                                      const std::string& animationFileNameBase,
                                      const std::string& animationFileNameSub,
                                      const AvatarMapType& avatarMap) {
    if (Node::init()) {
        _sheetName = sheetFileName;
        _animationNameBase = animationFileNameBase;
        _animationNameSub = animationFileNameSub;
        _animationName = _animationNameBase + _animationNameSub;
        
        _sheet = HierarchiesSpriteRuntime::getInstance()->addSheet(_sheetName);
        _animation = HierarchiesSpriteRuntime::getInstance()->addAnimation(_animationName);
        
        // image should load from the same directory with spritesheet file
        std::string imageName(_sheet->getImageName());
        size_t found = _sheetName.find_last_of('/');
        if (found != std::string::npos)
            imageName = _sheetName.substr(0, found + 1) + imageName;
        
        Texture2D* texture = Director::getInstance()->getTextureCache()->addImage(imageName);
        this->setTexture(texture);
        
        this->displayFrameAtIndex(0);
        
        return true;
    }
    return false;
}


#pragma mark - Property

void HierarchiesSprite::setFlippedX (bool value) {
    _flipX = value;
    _transformUpdated = _transformDirty = _inverseDirty = true;
}

void HierarchiesSprite::setFlippedY (bool value) {
    _flipY = value;
    _transformUpdated = _transformDirty = _inverseDirty = true;
}

void HierarchiesSprite::setColorAdd (const Color4B& value) {
    _displayedColorAdd = value;
}


#pragma mark - Event

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

void HierarchiesSprite::setEventHandle (const std::function<void(int, const std::string&)>& eventHandle) {
    _eventHandle = eventHandle;
}


#pragma mark - Animation

bool HierarchiesSprite::displayFrameAtIndex (unsigned int frameIndex) {
    if (frameIndex > _animation->getFrameCount() - 1)
        return false;
    
	if (_curFrameIndex == frameIndex)
        return true;
    
    _curFrameIndex = frameIndex;
	
	// event dispatch
    if (_eventHandle) {
        HierarchiesSpriteAnimation::Event event;
        if (_animation->getEventByFrameId(_curFrameIndex, event)) {
            _eventHandle(_curFrameIndex, event.name);
        }
    }
    
    _quads.clear();
    float min_X = 10000, max_X = -10000, min_Y = 10000, max_Y = -10000;
    this->buildHierarchiesData(HierarchiesSpriteAnimation::kNoneLoopMode,
                             0,
                             _curFrameIndex,
                             AffineTransformIdentity,
                             _animation,
                             min_X, max_X, min_Y, max_Y,
                             1, 0,
                             1, 0,
                             1, 0,
                             1, 0,
                             0);
    
    if (min_X == 10000 ||
        max_X == -10000 ||
        min_Y == 10000 ||
        max_Y == -10000) {
        // this frame is whole empty
        _bbox = Rect::ZERO;
        
        this->setAnchorPoint(Vec2::ZERO);
        this->setContentSize(Size::ZERO);
    }
    else {
        _bbox = Rect(min_X, min_Y, max_X - min_X, max_Y - min_Y);
        Vec2 anchorPoint = Vec2((0 - _bbox.origin.x) / _bbox.size.width,
                                (0 - _bbox.origin.y) / _bbox.size.height);
        
        this->setAnchorPoint(anchorPoint);
        this->setContentSize(_bbox.size);
    }
    
    _indices.clear(); // need refresh vertex data
    
    return true;
}

bool HierarchiesSprite::freshCurrentFrame () {
	unsigned int frameIndex = _curFrameIndex;
    _curFrameIndex = UINT_MAX;
	return displayFrameAtIndex(frameIndex);
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
    if (_quads.size() == 0)
        return;
    
    // Don't do calculate the culling if the transform was not updated
    _insideBounds = (flags & FLAGS_TRANSFORM_DIRTY) ? renderer->checkVisibility(transform, _contentSize) : _insideBounds;
    if(_insideBounds)
    {
        this->buildPrimitiveData();
        
        // set shader parameter
        _glProgramState->setUniformVec4(kHierarchiesSprite_GLProgram_Uniform_Name_ColorMul,
                                        Vec4(_displayedColor.r / 255.0f, _displayedColor.g / 255.0f, _displayedColor.b / 255.0f, _displayedOpacity / 255.0f));
        _glProgramState->setUniformVec4(kHierarchiesSprite_GLProgram_Uniform_Name_ColorAdd,
                                        Vec4(_displayedColorAdd.r / 255.0f, _displayedColorAdd.g / 255.0f, _displayedColorAdd.b / 255.0f, _displayedColorAdd.a / 255.0f));
        
        // ignore the anchor point while drawing
        Vec2 ap = this->getAnchorPointInPoints();
        Mat4 apMat;
        Mat4::createTranslation(Vec3(ap.x, ap.y, 0), &apMat);
        
        _renderCommand.init(_globalZOrder, _texture->getName(), _glProgramState, _blendFunc, _primitive, transform * apMat);
        renderer->addCommand(&_renderCommand);
        
#if CC_SPRITE_DEBUG_DRAW
        _debugDrawNode->clear();
        Vec2 vertices[4] = {
            Vec2(_bbox.origin.x + ap.x, _bbox.origin.y + ap.y),
            Vec2(_bbox.origin.x + ap.x + _bbox.size.width, _bbox.origin.y + ap.y),
            Vec2(_bbox.origin.x + ap.x + _bbox.size.width, _bbox.origin.y + ap.y + _bbox.size.height),
            Vec2(_bbox.origin.x + ap.x, _bbox.origin.y + ap.y + _bbox.size.height),
        };
        _debugDrawNode->drawPoly(vertices, 4, true, Color4F(1.0, 1.0, 1.0, 1.0));
#endif
    }
}


#pragma mark - Override

const Mat4& HierarchiesSprite::getNodeToParentTransform() const {
    if (_transformDirty)
    {
        // HierarchiesSprite
        // flip anchor point in points
        Vec2 anchorPointInPoints = _anchorPointInPoints;
        if (_flipX && _flipY) {
            anchorPointInPoints = Vec2(-_anchorPointInPoints.x, -_anchorPointInPoints.y);
        }
        else if (_flipX) {
            anchorPointInPoints = Vec2(-_anchorPointInPoints.x, _anchorPointInPoints.y);
        }
        else if (_flipY) {
            anchorPointInPoints = Vec2(_anchorPointInPoints.x, -_anchorPointInPoints.y);
        }
        
        // Translate values
        float x = _position.x;
        float y = _position.y;
        float z = _positionZ;
        
        if (_ignoreAnchorPointForPosition)
        {
            x += anchorPointInPoints.x;
            y += anchorPointInPoints.y;
        }
        
        // Rotation values
		// Change rotation code to handle X and Y
		// If we skew with the exact same value for both x and y then we're simply just rotating
        float cx = 1, sx = 0, cy = 1, sy = 0;
        if (_rotationZ_X || _rotationZ_Y)
        {
            float radiansX = -CC_DEGREES_TO_RADIANS(_rotationZ_X);
            float radiansY = -CC_DEGREES_TO_RADIANS(_rotationZ_Y);
            cx = cosf(radiansX);
            sx = sinf(radiansX);
            cy = cosf(radiansY);
            sy = sinf(radiansY);
        }
        
        bool needsSkewMatrix = ( _skewX || _skewY );
        
        Vec2 anchorPoint;
        anchorPoint.x = anchorPointInPoints.x * _scaleX;
        anchorPoint.y = anchorPointInPoints.y * _scaleY;
        
        // optimization:
        // inline anchor point calculation if skew is not needed
        // Adjusted transform calculation for rotational skew
        if (! needsSkewMatrix && !anchorPointInPoints.equals(Vec2::ZERO))
        {
            x += cy * -anchorPoint.x + -sx * -anchorPoint.y;
            y += sy * -anchorPoint.x +  cx * -anchorPoint.y;
        }
        
        // Build Transform Matrix
        // Adjusted transform calculation for rotational skew
        float mat[] = {
            cy * _scaleX,   sy * _scaleX,   0,          0,
            -sx * _scaleY,  cx * _scaleY,   0,          0,
            0,              0,              _scaleZ,    0,
            x,              y,              z,          1 };
        
        _transform.set(mat);
        
        if(!_ignoreAnchorPointForPosition)
        {
            _transform.translate(anchorPoint.x, anchorPoint.y, 0);
        }
        
        // FIXME:
        // FIX ME: Expensive operation.
        // FIX ME: It should be done together with the rotationZ
        if(_rotationY) {
            Mat4 rotY;
            Mat4::createRotationY(CC_DEGREES_TO_RADIANS(_rotationY), &rotY);
            _transform = _transform * rotY;
        }
        if(_rotationX) {
            Mat4 rotX;
            Mat4::createRotationX(CC_DEGREES_TO_RADIANS(_rotationX), &rotX);
            _transform = _transform * rotX;
        }
        
        if(!_ignoreAnchorPointForPosition)
        {
            _transform.translate(-anchorPoint.x, -anchorPoint.y, 0);
        }
        
        // FIXME:: Try to inline skew
        // If skew is needed, apply skew and then anchor point
        if (needsSkewMatrix)
        {
            float skewMatArray[16] =
            {
                1, (float)tanf(CC_DEGREES_TO_RADIANS(_skewY)), 0, 0,
                (float)tanf(CC_DEGREES_TO_RADIANS(_skewX)), 1, 0, 0,
                0,  0,  1, 0,
                0,  0,  0, 1
            };
            Mat4 skewMatrix(skewMatArray);
            
            _transform = _transform * skewMatrix;
            
            // adjust anchor point
            if (!anchorPointInPoints.equals(Vec2::ZERO))
            {
                // FIXME:: Argh, Mat4 needs a "translate" method.
                // FIXME:: Although this is faster than multiplying a vec4 * mat4
                _transform.m[12] += _transform.m[0] * -anchorPointInPoints.x + _transform.m[4] * -anchorPointInPoints.y;
                _transform.m[13] += _transform.m[1] * -anchorPointInPoints.x + _transform.m[5] * -anchorPointInPoints.y;
            }
        }
        
        if (_useAdditionalTransform)
        {
            _transform = _transform * _additionalTransform;
        }
        
        // HierarchiesSprite
        // flip
        if (_flipX && _flipY) {
            _transform.scale(-1, -1, 1);
        }
        else if (_flipX) {
            _transform.scale(-1, 1, 1);
        }
        else if (_flipY) {
            _transform.scale(1, -1, 1);
        }
        
        _transformDirty = false;
    }
    
    return _transform;
}

void HierarchiesSprite::setOpacityModifyRGB (bool bValue) {
    _opacityModifyRGB = bValue;
}

void HierarchiesSprite::setBlendFunc (const BlendFunc& value) {
	_blendFunc = value;
}

void HierarchiesSprite::setTexture(Texture2D *texture) {
    if (_texture != texture) {
        CC_SAFE_RETAIN(texture);
        CC_SAFE_RELEASE(_texture);
        _texture = texture;
        
        this->setOpacityModifyRGB(_texture->hasPremultipliedAlpha());
        this->updateBlendFunc();
        this->updateShader();
    }
}


#pragma mark - Private

void HierarchiesSprite::updateShader () {
    if (_texture->hasPremultipliedAlpha()) {
        GLProgram *glProgram = GLProgramCache::getInstance()->getGLProgram(kShader_Name_HierarchiesSprite_Premultiplied);
        this->setGLProgramState(GLProgramState::create(glProgram));
    }
    else {
        GLProgram *glProgram = GLProgramCache::getInstance()->getGLProgram(kShader_Name_HierarchiesSprite);
        this->setGLProgramState(GLProgramState::create(glProgram));
    }
}

void HierarchiesSprite::updateBlendFunc () {
    if (_texture->hasPremultipliedAlpha()) {
        _blendFunc = BlendFunc::ALPHA_PREMULTIPLIED;
    }
    else {
        _blendFunc = BlendFunc::ALPHA_NON_PREMULTIPLIED;
    }
}

void HierarchiesSprite::buildHierarchiesData (HierarchiesSpriteAnimation::ElementLoopMode loopMode,
                                            int frameOffset,
                                            unsigned int frameIndex,
                                            const AffineTransform& parentMatrix,
                                            const HierarchiesSpriteAnimation* animation,
                                            float& min_X, float& max_X, float& min_Y, float& max_Y,
                                            const float parent_alpha_percent, const int parent_alpha_amount,
                                            const float parent_red_percent, const int parent_red_amount,
                                            const float parent_green_percent, const int parent_green_amount,
                                            const float parent_blue_percent, const int parent_blue_amount,
                                            float depth)
{
    HierarchiesSpriteAnimation::FrameElements frameElements;
    int eNum = animation->getFrameElementsAtFrameIndex(loopMode, frameOffset, frameIndex, frameElements); //!!! frameIndex will update with loop mode
    
    _quads.reserve(_quads.size() + eNum);
    
    AffineTransform matrix;
    HierarchiesSprite_V3F_C4B_T2F_Quad quad;
    HierarchiesSpriteAnimation::FrameElements::const_iterator layerIter;
    int layerZOrder = 0;
//        unsigned int quadsIndex = 0;
//        unsigned int quadsCount = 0;
    //!!! reverse order to display
    for (layerIter = frameElements.cbegin(); layerIter != frameElements.cend(); layerIter++) { // element layers BEGIN
        const HierarchiesSpriteAnimation::Element* elementIter = &(*layerIter);
        
        HierarchiesSpriteAnimation::Symbol symbol;
        bool result = animation->getSymbolByIndex(elementIter->symbolIndex, symbol);
        assert(result);
        
        if (symbol.isSocket == false) { // simple element BEGIN
            HierarchiesSpriteSheet::Spr spr;
            result = _sheet->getSpr(symbol.name.c_str(), spr);
            assert(result);
            
            // calc matrix
            hierarchiesCalcMatrix(&symbol, &spr, &(*elementIter), &matrix);
            matrix = AffineTransformConcat(matrix, parentMatrix);
            
            // update vertices
            hierarchiesUpdateQuadVertices(Size(spr.w, spr.h),
                                          &matrix,
                                          &quad,
                                          0, // (depth + layerZOrder) * 0.01,
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
            
            hierarchiesUpdateQuadTextureCoords(Rect(spr.x, spr.y, spr.w, spr.h),
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
            HierarchiesSpriteAnimation* subAnimation = HierarchiesSpriteRuntime::getInstance()->addAnimation(subSpriteAnimationName.c_str());
            
            hierarchiesCalcMatrixSocket(&(*elementIter), &matrix);
            matrix = AffineTransformConcat(matrix, parentMatrix);
            
            float alpha_percent = parent_alpha_percent * elementIter->color_alpha_percent;
            int alpha_amount = parent_alpha_percent * elementIter->color_alpha_amount + parent_alpha_amount;
            float red_percent = parent_red_percent * elementIter->color_red_percent;
            int red_amount = parent_red_percent * elementIter->color_red_amount + parent_red_amount;
            float green_percent = parent_green_percent * elementIter->color_green_percent;
            int green_amount = parent_green_percent * elementIter->color_green_amount + parent_green_amount;
            float blue_percent = parent_blue_percent * elementIter->color_blue_percent;
            int blue_amount = parent_blue_percent * elementIter->color_blue_amount + parent_blue_amount;
            this->buildHierarchiesData(elementIter->loopMode,
                                       elementIter->frameOffset,
                                       frameIndex - elementIter->startDisplayFrameIndex,
                                       matrix,
                                       subAnimation,
                                       min_X, max_X, min_Y, max_Y,
                                       alpha_percent, alpha_amount,
                                       red_percent, red_amount,
                                       green_percent, green_amount,
                                       blue_percent, blue_amount,
                                       depth + layerZOrder);
        } // nesting sprite element END
        
        layerZOrder++;
    } // element layers END
}

void HierarchiesSprite::buildPrimitiveData ()
{
    // refresh check first
    if (_indices.size() == 0 && _quads.size() > 0)
    {
//        CCLOG("<HierarchiesSprite> buildPrimitiveData BEGIN:%d", _curFrameIndex);
        
        // build indices
        _indices.resize(_quads.size() * 6);
        for(size_t i = 0; i < _quads.size(); i++)
        {
            _indices[i*6+0] = i*4+0;
            _indices[i*6+1] = i*4+1;
            _indices[i*6+2] = i*4+2;
            _indices[i*6+3] = i*4+3;
            _indices[i*6+4] = i*4+2;
            _indices[i*6+5] = i*4+1;
        }
        
        int vertexNumber = (int)_quads.size() * 4;
        bool vertexBufferRecreated = false;
        if (!_vertexBuffer || _vertexBuffer->getVertexNumber() < vertexNumber)
        {
            CC_SAFE_RELEASE(_vertexBuffer);
            _vertexBuffer = VertexBuffer::create(sizeof(HierarchiesSprite_V3F_C4B_T2F), vertexNumber);
            CC_SAFE_RETAIN(_vertexBuffer);
            vertexBufferRecreated = true;
            
//            CCLOG("<HierarchiesSprite> vertexBufferRecreated:%d", _vertexBuffer->getVertexNumber());
        }
        _vertexBuffer->updateVertices(_quads.data(), vertexNumber, 0);
        
        if (!_vertexData || vertexBufferRecreated)
        {
            CC_SAFE_RELEASE(_vertexData);
            _vertexData = VertexData::create();
            _vertexData->setStream(_vertexBuffer, VertexStreamAttribute(0, cocos2d::GLProgram::VERTEX_ATTRIB_POSITION, GL_FLOAT, 3, false));
            _vertexData->setStream(_vertexBuffer, VertexStreamAttribute(12, kHierarchiesSprite_GLProgram_Attribute_ColorMul, GL_UNSIGNED_BYTE, 4, true));
            _vertexData->setStream(_vertexBuffer, VertexStreamAttribute(12 + 4, kHierarchiesSprite_GLProgram_Attribute_ColorAdd, GL_UNSIGNED_BYTE, 4, true));
            _vertexData->setStream(_vertexBuffer, VertexStreamAttribute(12 + 4 + 4, cocos2d::GLProgram::VERTEX_ATTRIB_TEX_COORD, GL_FLOAT, 2, false));
            CC_SAFE_RETAIN(_vertexData);
            
//            CCLOG("<HierarchiesSprite> vertexDataRecreated");
        }
        
        int indexNumber = (int)_indices.size();
        bool indexBufferRecreated = false;
        if (!_indexBuffer || _indexBuffer->getIndexNumber() < indexNumber)
        {
            CC_SAFE_RELEASE(_indexBuffer);
            _indexBuffer = IndexBuffer::create(IndexBuffer::IndexType::INDEX_TYPE_SHORT_16, indexNumber);
            CC_SAFE_RETAIN(_indexBuffer);
            indexBufferRecreated = true;
            
//            CCLOG("<HierarchiesSprite> indexBufferRecreated:%d", _indexBuffer->getIndexNumber());
        }
        _indexBuffer->updateIndices(_indices.data(), indexNumber, 0);
        
        if (vertexBufferRecreated || indexBufferRecreated)
        {
            CC_SAFE_RELEASE(_primitive);
            _primitive = Primitive::create(_vertexData, _indexBuffer, GL_TRIANGLES);
            CC_SAFE_RETAIN(_primitive);
            
//            CCLOG("<HierarchiesSprite> primitiveRecreated");
        }
        _primitive->setStart(0);
        _primitive->setCount(indexNumber);
        
//        CCLOG("<HierarchiesSprite> buildPrimitiveData END:%d", _curFrameIndex);
    }
}


NS_CC_EXT_END
