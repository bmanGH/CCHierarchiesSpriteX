//
//  CCHierarchiesSpriteDynamic.cpp
//  CCHierarchiesSprite
//
//  Created by bman <zx123xz321hm3@hotmail.com>.
//  Copyright (c) 2013. All rights reserved.
//

#include "CCHierarchiesSpriteDynamic.h"
#include "CCHierarchiesSpriteSheetCache.h"
#include "CCHierarchiesSpriteAnimationCache.h"


NS_CC_EXT_BEGIN

CCHierarchiesSpriteDynamic* CCHierarchiesSpriteDynamic::create (const char* sheetFileName,
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
    
	return CCHierarchiesSpriteDynamic::create(sheetFileName, animationFileNameBase.c_str(), animationFileNameSub.c_str(), delegate);
}

CCHierarchiesSpriteDynamic* CCHierarchiesSpriteDynamic::create (const char* sheetFileName,
                                    const char* animationFileNameBase,
                                    const char* animationFileNameSub,
                                    CCHierarchiesSpriteEventDelegate* delegate) {
    CCHierarchiesSpriteDynamic* ret = new CCHierarchiesSpriteDynamic();
	if (ret->initWithFile(sheetFileName, animationFileNameBase, animationFileNameSub, delegate))
	{
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);
	return NULL;
}

CCHierarchiesSpriteDynamic::CCHierarchiesSpriteDynamic ()
: _quads(20) {
}

CCHierarchiesSpriteDynamic::~CCHierarchiesSpriteDynamic () {
}

bool CCHierarchiesSpriteDynamic::initWithFileAndAvatar (const char* sheetFileName,
                                                        const char* animationFileNameBase,
                                                        const char* animationFileNameSub,
                                                        CCHierarchiesSpriteEventDelegate* delegate,
                                                        const AvatarMapType& avatarMap) {
    if (CCHierarchiesSpriteBase::initWithFileAndAvatar(sheetFileName,
                                                   animationFileNameBase,
                                                   animationFileNameSub,
                                                   delegate,
                                                   avatarMap)) {
        _avatarMap = avatarMap;
        
        this->displayFrameAtIndex(0);
        
        return true;
    }
    return false;
}


#pragma mark - Avatar

bool CCHierarchiesSpriteDynamic::setAvatarMap (const char* name, const char* mapName) {
	_avatarMap[name] = mapName;
    this->freshCurrentFrame();
    return true;
}

bool CCHierarchiesSpriteDynamic::getAvatarMap (const char* name, std::string& mapName) {
    AvatarMapType::iterator symbolIter = _avatarMap.find(name);
    if (symbolIter == _avatarMap.end()) {
        return false;
    }
    
    mapName = _avatarMap[name];
    return true;
}

void CCHierarchiesSpriteDynamic::resetAvatarMap () {
    _avatarMap.clear();
    this->freshCurrentFrame();
}


#pragma mark - Animation

bool CCHierarchiesSpriteDynamic::displayFrameAtIndex (unsigned int frameIndex) {
    CCHierarchiesSpriteBase::displayFrameAtIndex(frameIndex);
	
    // calc quads
    CC_PROFILER_START_CATEGORY(HIERARCHIES_SPRITE_PROFILER, "CCHierarchiesSpriteDynamic - animation");
    
    //TODO
    
#if HIERARCHIES_ENABLE_FILTER == 1
        this->updateSocketFilters();
#endif
    
    CC_PROFILER_STOP_CATEGORY(HIERARCHIES_SPRITE_PROFILER, "CCHierarchiesSpriteDynamic - animation");
    
    return true;
}


#pragma mark - Draw

void CCHierarchiesSpriteDynamic::draw () {
    // ignore the anchor point while drawing
    CCPoint ap = this->getAnchorPointInPoints();
    kmGLTranslatef(ap.x, ap.y, 0);
    
    //TODO: draw with Vertex Array
    
//    // nesting draw
//    std::vector<DisplayElement>::iterator displayElementIter;
//    for (displayElementIter = _displayList.begin(); displayElementIter != _displayList.end(); displayElementIter++) {
//        if (displayElementIter->quadsCount > 0) {
//            // quads draw
//            CC_PROFILER_START_CATEGORY(HIERARCHIES_SPRITE_PROFILER, "CCHierarchiesSpriteDynamic - draw");
//            
//            CC_NODE_DRAW_SETUP();
//            
//            GLfloat colors[4];
//            colors[0] = _displayedColor.r / 255.0f;
//            colors[1] = _displayedColor.g / 255.0f;
//            colors[2] = _displayedColor.b / 255.0f;
//            colors[3] = _displayedOpacity / 255.0f;
//            this->getShaderProgram()->setUniformLocationWith4fv(_uniformColorLocation, colors, 1);
//            
//            ccGLBlendFunc( _blendFunc.src, _blendFunc.dst );
//            
//            ccGLBindTexture2D(_texture->getName());
//            
//            _mesh->drawNumberOfQuads(displayElementIter->quadsCount, _frameStartQuadIndex + displayElementIter->quadsIndex);
//            
//            CHECK_GL_ERROR_DEBUG();
//            
//#if HIERARCHIES_DEBUG_DRAW == 1
//            // draw bounding box
//            ccDrawColor4B(255, 0, 255, 255);
//            CCPoint vertices[4] = {
//                ccp(_bbox.origin.x, _bbox.origin.y),
//                ccp(_bbox.origin.x + _bbox.size.width, _bbox.origin.y),
//                ccp(_bbox.origin.x + _bbox.size.width, _bbox.origin.y + _bbox.size.height),
//                ccp(_bbox.origin.x, _bbox.origin.y + _bbox.size.height),
//            };
//            ccDrawPoly(vertices, 4, true);
//            
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
//            
//#elif HIERARCHIES_DEBUG_DRAW == 3
//            // draw quads
//            ccDrawColor4B(0, 255, 0, 255);
//            CCHierarchiesSprite_V3F_C4B_T2F_Quad* quad = _mesh->getQuads();
//            for (int i = 0; i < displayElementIter->quadsCount; i++) {
//                int verxIndex = _frameStartQuadIndex + displayElementIter->quadsIndex + i;
//                CCPoint vertices3[4] = {
//                    ccp(quad[verxIndex].tl.vertices.x, quad[verxIndex].tl.vertices.y),
//                    ccp(quad[verxIndex].tr.vertices.x, quad[verxIndex].tr.vertices.y),
//                    ccp(quad[verxIndex].br.vertices.x, quad[verxIndex].br.vertices.y),
//                    ccp(quad[verxIndex].bl.vertices.x, quad[verxIndex].bl.vertices.y),
//                };
//                ccDrawPoly(vertices3, 4, true);
//            }
//            
//            ccDrawColor4B(255, 255, 255, 255);
//#endif
//            
//            CC_PROFILER_STOP_CATEGORY(HIERARCHIES_SPRITE_PROFILER, "CCHierarchiesSpriteDynamic - draw");
//        }
////        else {
////            // sub sprite draw
////            if (_subSprites.count(displayElementIter->socketElement.euid)) {
////                const SubSpriteSocket& socket = _subSprites[displayElementIter->socketElement.euid];
////#if HIERARCHIES_ENABLE_FILTER == 1
////                if (socket.filters.size() > 0) {
////                    std::vector<CCHierarchiesSpriteFilter*>::const_iterator filterIter;
////                    for (filterIter = socket.filters.begin(); filterIter != socket.filters.end(); filterIter++) {
////                        (*filterIter)->beforeDraw();
////                    }
////
////                    socket.subSprite->draw();
////                    glFlush();
////
////                    for (filterIter = socket.filters.begin(); filterIter != socket.filters.end(); filterIter++) {
////                        (*filterIter)->afterDraw();
////                    }
////                } // if (socket.filters.size() > 0) END
////                else {
////                    socket.subSprite->visit();
////                }
////#else
////                socket.subSprite->visit();
////#endif
////            } // // sub sprite draw END
////        }
//    }
}

NS_CC_EXT_END
