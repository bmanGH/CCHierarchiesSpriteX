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
#include "CCHierarchiesSpriteSheet.h"
#include "rapidxml.hpp"

NS_CC_EXT_BEGIN


// HierarchiesSpriteAnimationReader
class CC_DLL HierarchiesSpriteAnimationReader {
    
protected:
    std::string _fileName;
    std::string _filePath;
    int _euidCount;
    
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
    
    std::string getFilePath () {
        return _filePath;
    }
    
    bool parse (HierarchiesSpriteAnimation& out);
    
};


// HierarchiesSpriteSheetReader
class CC_DLL HierarchiesSpriteSheetReader {
    
protected:
    std::string _fileName;
    std::string _filePath;
    
	bool parseSprNode (HierarchiesSpriteSheet& out, rapidxml::xml_node<>* imgNode);
    
public:
    HierarchiesSpriteSheetReader (const std::string& fileName);
    virtual ~HierarchiesSpriteSheetReader ();
    
    std::string getFilePath () {
        return _filePath;
    }
    
    bool parse (HierarchiesSpriteSheet& out);
    
};


NS_CC_EXT_END

#endif /* defined(__HierarchiesSpriteDemo__CCHierarchiesSpriteReader__) */
