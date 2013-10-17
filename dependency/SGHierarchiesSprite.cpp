//
//  SGHierarchiesSprite.cpp
//  SZSG
//
//  Created by xuxiaocheng on 9/3/13.
//
//

#include "SGHierarchiesSprite.h"


#pragma mark - Shader

const GLchar* ccShader_SGHierarchiesSprite_GBufferProgram_frag =
"\n\
#ifdef GL_ES                                                                    \n\
precision mediump float;                                                           \n\
#endif                                                                          \n\
\n\
uniform sampler2D CC_Texture0;                                                  \n\
uniform mediump float u_depth;                                                          \n\
\n\
varying vec4 v_fragmentColor;                                                   \n\
varying vec2 v_texCoord;                                                        \n\
\n\
void main()                                                                     \n\
{                                                                               \n\
vec4 origColor = texture2D(CC_Texture0, v_texCoord);                           \n\
\n\
vec4 color = origColor * v_fragmentColor;                                      \n\
\n\
//float depth = color.a * u_depth;                                               \n\
\n\
if (color.a > 0.0)                                                             \n\
gl_FragColor = vec4(u_depth / 255.0, 0.0, 0.0, 0.0);                            \n\
else                                                                           \n\
discard;                                                                      \n\
}                                                                               \n\
";


#pragma mark - SGHierarchiesSprite

inline static void hierarchiesCalcMatrixSocket (CCHierarchiesSpriteAnimation::Element* element, CCAffineTransform* matrix) {
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
    
    *matrix = CCAffineTransformMake(m.a, -m.b, -m.c, m.d, m.tx, -m.ty);
}

SGHierarchiesSprite* SGHierarchiesSprite::create (const char* sheetFileName,
                                    const char* animationFileName,
                                    CCHierarchiesSpriteEventDelegate* delegate) {
    std::string animationFileNameBase;
    std::string animationFileNameSub;
    std::string animationFilePath(animationFileName);
    size_t found = animationFilePath.find_last_of(HIERARCHIES_SPRITE_PATH_SEPARATOR);
    if (found != std::string::npos) {
        animationFileNameBase = animationFilePath.substr(0, found + 1);
        animationFileNameSub = animationFilePath.substr(found + 1);
    }
    else {
        animationFileNameSub = animationFilePath;
    }
    
	return SGHierarchiesSprite::create(sheetFileName, animationFileNameBase.c_str(), animationFileNameSub.c_str(), delegate);
}

SGHierarchiesSprite* SGHierarchiesSprite::create (const char* sheetFileName,
                                    const char* animationFileNameBase,
                                    const char* animationFileNameSub,
                                    CCHierarchiesSpriteEventDelegate* delegate) {
    SGHierarchiesSprite* ret = new SGHierarchiesSprite();
	if (ret->initWithFile(sheetFileName, animationFileNameBase, animationFileNameSub, delegate))
	{
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);
	return NULL;
}

SGHierarchiesSprite::SGHierarchiesSprite ()
: _gBufferProgram(NULL) {
}

SGHierarchiesSprite::~SGHierarchiesSprite () {
    CC_SAFE_RELEASE(_gBufferProgram);
}

bool SGHierarchiesSprite::initWithFile (const char* sheetFileName,
                   const char* animationFileName,
                   CCHierarchiesSpriteEventDelegate* delegate) {
    std::string animationFileNameBase;
    std::string animationFileNameSub;
    std::string animationFilePath(animationFileName);
    size_t found = animationFilePath.find_last_of(HIERARCHIES_SPRITE_PATH_SEPARATOR);
    if (found != std::string::npos) {
        animationFileNameBase = animationFilePath.substr(0, found + 1);
        animationFileNameSub = animationFilePath.substr(found + 1);
    }
    else {
        animationFileNameSub = animationFilePath;
    }
    
    return this->initWithFile(sheetFileName, animationFileNameBase.c_str(), animationFileNameSub.c_str(), delegate);
}

bool SGHierarchiesSprite::initWithFile (const char* sheetFileName,
                   const char* animationFileNameBase,
                   const char* animationFileNameSub,
                   CCHierarchiesSpriteEventDelegate* delegate) {
    if (CCHierarchiesSprite::initWithFile(sheetFileName, animationFileNameBase, animationFileNameSub, delegate)) {
        CCGLProgram* glProgram = CCShaderCache::sharedShaderCache()->programForKey(kCCShader_SGHierarchiesSprite_GBufferProgram_Frag);
        if (glProgram == NULL) {
            glProgram = new CCGLProgram();
            glProgram->initWithVertexShaderByteArray(ccPositionTextureColor_vert, ccShader_SGHierarchiesSprite_GBufferProgram_frag);
            
            glProgram->addAttribute(kCCAttributeNamePosition, kCCVertexAttrib_Position);
            glProgram->addAttribute(kCCAttributeNameColor, kCCVertexAttrib_Color);
            glProgram->addAttribute(kCCAttributeNameTexCoord, kCCVertexAttrib_TexCoords);
            
            glProgram->link();
            glProgram->updateUniforms();
            
            CHECK_GL_ERROR_DEBUG();
            
            CCShaderCache::sharedShaderCache()->addProgram(glProgram, kCCShader_SGHierarchiesSprite_GBufferProgram_Frag);
        }
        else {
            glProgram->retain();
        }
        _gBufferProgram = glProgram;
        CC_SAFE_RETAIN(_gBufferProgram);
        
        _depthUniformLocation = glProgram->getUniformLocationForName("u_depth");
        
        glProgram->release();
        
        return true;
    }
    return false;
}

void SGHierarchiesSprite::updateSocketElements () {
    // update socket element
    std::vector<DisplayElement>::iterator displayElementIter;
    for (displayElementIter = _displayList.begin(); displayElementIter != _displayList.end(); displayElementIter++) {
        if (displayElementIter->quadsCount > 0)
            continue;
        
        CCAffineTransform matrix;
        CCHierarchiesSpriteAnimation::Element* elementIter = &(displayElementIter->socketElement);
        SGHierarchiesSprite* subSprite = NULL;
        
        if (_subSprites.count(elementIter->euid) == 0) {
            CCHierarchiesSpriteAnimation::Symbol symbol;
            bool result = _animation->getSymbolByIndex(elementIter->symbolIndex, symbol);
            assert(result);
            
            // instance sub sprite
            std::string subSpriteNameSub = symbol.name + ".hanims";
            subSprite = SGHierarchiesSprite::create(_sheetName.c_str(), _animationNameBase.c_str(), subSpriteNameSub.c_str());
            subSprite->setup();
            
            SubSpriteSocket socket;
            socket.lastDisplayFrameIndex = _curFrameIndex;
            subSprite->retain();
            socket.subSprite = subSprite;
            
            // instance filter
            std::vector<CCHierarchiesSpriteAnimation::Filter>::const_iterator elementFilterIter;
            for (elementFilterIter = elementIter->filters.begin(); elementFilterIter != elementIter->filters.end(); elementFilterIter++) {
                //TODO: add filter instance
                
                break; //TEMP
            }
            
            std::pair<int, SubSpriteSocket> subSpritePair;
            subSpritePair.first = elementIter->euid;
            subSpritePair.second = socket;
            
            _subSprites.insert(subSpritePair);
        }
        else {
            const SubSpriteSocket& socket = _subSprites[elementIter->euid];
            subSprite = (SGHierarchiesSprite*)socket.subSprite;
            
            // lerp filter
            std::vector<CCHierarchiesSpriteFilter*>::const_iterator filterIter;
            for (filterIter = socket.filters.begin(); filterIter != socket.filters.end(); filterIter++) {
                //TODO: lerp filter params
            }
        }
        
        // update sub sprite
        _subSprites[elementIter->euid].lastDisplayFrameIndex = _curFrameIndex;
        
        hierarchiesCalcMatrixSocket(&(*elementIter), &matrix);
        subSprite->setAdditionalTransform(matrix);
        subSprite->setOpacity(elementIter->color_alpha_percent * 255);
        subSprite->setColor(ccc3(elementIter->color_red_percent * 255,
                                 elementIter->color_green_percent * 255,
                                 elementIter->color_blue_percent * 255));
        
        subSprite->displayFrameAtIndex(_curFrameIndex - elementIter->startDisplayFrameIndex);
    }
    
    // remove non-displaying sub sprite
    std::unordered_map<int, SubSpriteSocket>::iterator subSpritesIter;
    subSpritesIter = _subSprites.begin();
    while (subSpritesIter != _subSprites.end()) {
        if (subSpritesIter->second.lastDisplayFrameIndex != _curFrameIndex) {
            SGHierarchiesSprite* subSprite = (SGHierarchiesSprite*)subSpritesIter->second.subSprite;
            subSprite->shutdown();
            subSprite->release();
            subSpritesIter->second.subSprite = NULL;
            
            // remove filters on sub sprite
            std::vector<CCHierarchiesSpriteFilter*>::iterator filterIter;
            for (filterIter = subSpritesIter->second.filters.begin();
                 filterIter != subSpritesIter->second.filters.end();
                 filterIter++) {
                (*filterIter)->release();
            }
            
            _subSprites.erase(subSpritesIter++);
            continue;
        }
        subSpritesIter++;
    }
}

void SGHierarchiesSprite::gBufferVisit (float depth) {
    // quick return if not visible. children won't be drawn.
    if (!m_bVisible)
    {
        return;
    }
    kmGLPushMatrix();
    
    this->transform();
    
    CCNode* pNode = NULL;
    unsigned int i = 0;
    
    if(m_pChildren && m_pChildren->count() > 0)
    {
        sortAllChildren();
        // draw children zOrder < 0
        ccArray *arrayData = m_pChildren->data;
        for( ; i < arrayData->num; i++ )
        {
            pNode = (CCNode*) arrayData->arr[i];
            
            if ( pNode && pNode->getZOrder() < 0
                && (dynamic_cast<SGHierarchiesSprite*>(pNode)) )
            {
                ((SGHierarchiesSprite*)pNode)->gBufferVisit(depth);
            }
            else
            {
                break;
            }
        }
        // self draw
        this->gBufferDraw(depth);
        
        for( ; i < arrayData->num; i++ )
        {
            pNode = (CCNode*) arrayData->arr[i];
            if (pNode
                && (dynamic_cast<SGHierarchiesSprite*>(pNode)))
            {
                ((SGHierarchiesSprite*)pNode)->gBufferVisit(depth);
            }
        }
    }
    else
    {
        this->gBufferDraw(depth);
    }
    
    // reset for next frame
    m_uOrderOfArrival = 0;
    
    kmGLPopMatrix();
}

void SGHierarchiesSprite::gBufferDraw (float depth) {
    CC_PROFILER_START_CATEGORY(HIERARCHIES_SPRITE_PROFILER, "SGHierarchiesSprite - animation");
    
    if (_needFresh) {
        _mesh->flushAllQuadsToBuffer();
        
        this->updateSocketFilters();
        
        _needFresh = false;
    }
    
    CC_PROFILER_STOP_CATEGORY(HIERARCHIES_SPRITE_PROFILER, "SGHierarchiesSprite - animation");
    
    // ignore the anchor point while drawing
    CCPoint ap = this->getAnchorPointInPoints();
    kmGLTranslatef(ap.x, ap.y, 0);
    
    // nesting draw
    std::vector<DisplayElement>::iterator displayElementIter;
    for (displayElementIter = _displayList.begin(); displayElementIter != _displayList.end(); displayElementIter++) {
        if (displayElementIter->quadsCount > 0) {
            // quads draw
            CC_PROFILER_START_CATEGORY(HIERARCHIES_SPRITE_PROFILER, "SGHierarchiesSprite - draw");
            
            ccGLEnable(m_eGLServerState);
            CCAssert(_gBufferProgram, "No shader program set for this node");
            {
                _gBufferProgram->use();
                _gBufferProgram->setUniformsForBuiltins();
                _gBufferProgram->setUniformLocationWith1f(_depthUniformLocation, depth);
            }
            
            ccGLBlendFunc( GL_ONE, GL_ZERO );
            
            _mesh->drawNumberOfQuads(displayElementIter->quadsCount, displayElementIter->quadsIndex);
            
            CHECK_GL_ERROR_DEBUG();
            
            CC_PROFILER_STOP_CATEGORY(HIERARCHIES_SPRITE_PROFILER, "SGHierarchiesSprite - draw");
        }
        else {
            // sub sprite draw
            if (_subSprites.count(displayElementIter->socketElement.euid)) {
                const SubSpriteSocket& socket = _subSprites[displayElementIter->socketElement.euid];
                SGHierarchiesSprite* subSprite = (SGHierarchiesSprite*)socket.subSprite;
                subSprite->gBufferVisit(depth);
            } // // sub sprite draw END
        }
    }
}
