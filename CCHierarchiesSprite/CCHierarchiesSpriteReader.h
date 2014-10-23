//
//  CCHierarchiesSpriteReader.h
//  HierarchiesSpriteDemo
//
//  Created by xuxiaocheng on 10/23/14.
//
//

#ifndef __HierarchiesSpriteDemo__CCHierarchiesSpriteReader__
#define __HierarchiesSpriteDemo__CCHierarchiesSpriteReader__

#include "cocos2d.h"
#include "ExtensionMacros.h"
#include "CCHierarchiesSpriteAnimation.h"
#include "rapidxml.hpp"

NS_CC_EXT_BEGIN


// HierarchiesSpriteAnimationReader
class CC_DLL HierarchiesSpriteAnimationReader {
    
protected:
    std::string _fileName;
    int _euidCount;
    
protected:
    bool parseItems (HierarchiesSpriteAnimation& out, rapidxml::xml_node<>* itemsNode);
    bool parseSymbols (HierarchiesSpriteAnimation& out, rapidxml::xml_node<>* symbolsNode);
    bool parseAnimations (HierarchiesSpriteAnimation& out, rapidxml::xml_node<>* animationsNode);
    bool parseEvents (HierarchiesSpriteAnimation& out, rapidxml::xml_node<>* eventsNode);
    bool parseLayers (HierarchiesSpriteAnimation& out, rapidxml::xml_node<>* layersNode);
    bool parseKeyFrames (HierarchiesSpriteAnimation::Layer& layer, rapidxml::xml_node<>* layerNode);
    bool parseElements (HierarchiesSpriteAnimation::KeyFrame& frame, rapidxml::xml_node<>* keyFrameNode);
    
public:
    HierarchiesSpriteAnimationReader (const std::string& fileName);
    virtual ~HierarchiesSpriteAnimationReader ();
    
    bool parse (HierarchiesSpriteAnimation& out);
    
};


NS_CC_EXT_END

#endif /* defined(__HierarchiesSpriteDemo__CCHierarchiesSpriteReader__) */
