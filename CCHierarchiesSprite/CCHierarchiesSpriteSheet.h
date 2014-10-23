//
//  CCHierarchiesSpriteSheet.h
//  HierarchiesSpriteDemo
//
//  Created by xuxiaocheng on 10/23/14.
//
//

#ifndef __HierarchiesSpriteDemo__CCHierarchiesSpriteSheet__
#define __HierarchiesSpriteDemo__CCHierarchiesSpriteSheet__

#include "cocos2d.h"
#include "ExtensionMacros.h"
#include <string>
#include <unordered_map>
#include "rapidxml.hpp"


NS_CC_EXT_BEGIN

class CC_DLL CCHierarchiesSpriteSheet {
    
public:
    struct Spr {
        std::string name;
        float x;
        float y;
        float w;
        float h;
        bool isRotation;
        
        Spr () {}
        
        Spr (std::string name, float x, float y, float w, float h, bool isRotation)
        : name(name), x(x), y(y), w(w), h(h), isRotation(isRotation) {
        }
        
        Spr (const Spr& copy) {
            this->name = copy.name;
            this->x = copy.x;
            this->y = copy.y;
            this->w = copy.w;
            this->h = copy.h;
            this->isRotation = copy.isRotation;
        }
        
        Spr& operator= (const Spr& rhs) {
            this->name = rhs.name;
            this->x = rhs.x;
            this->y = rhs.y;
            this->w = rhs.w;
            this->h = rhs.h;
            this->isRotation = rhs.isRotation;
            return *this;
        }
    };
    
protected:
    std::string _imageName;
    float _imageWidth;
    float _imageHeight;
    std::unordered_map<std::string, Spr> _sprList;
    
protected:
	void parseSprNode (rapidxml::xml_node<>* imgNode);
    
public:
    CCHierarchiesSpriteSheet (std::string xmlFile);
    virtual ~CCHierarchiesSpriteSheet ();
    
    const char* getImageName ();
    float getImageWidth () const;
    float getImageHeight () const;
    bool getSpr (const char* name, Spr& out) const;
    const std::unordered_map<std::string, Spr>& getSprList ();
    size_t getSprListCount ();
    
};

NS_CC_EXT_END

#endif /* defined(__HierarchiesSpriteDemo__CCHierarchiesSpriteSheet__) */
