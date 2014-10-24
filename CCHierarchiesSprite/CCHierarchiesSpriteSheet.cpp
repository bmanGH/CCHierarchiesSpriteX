//
//  CCHierarchiesSpriteSheet.cpp
//  HierarchiesSpriteDemo
//
//  Created by xuxiaocheng on 10/23/14.
//
//

#include "CCHierarchiesSpriteSheet.h"

NS_CC_EXT_BEGIN


HierarchiesSpriteSheet::HierarchiesSpriteSheet ()
: _imageWidth(0)
, _imageHeight(0) {
}

HierarchiesSpriteSheet::~HierarchiesSpriteSheet () {
}

std::string HierarchiesSpriteSheet::getVersion () {
    return _version;
}

std::string HierarchiesSpriteSheet::getImageName () {
    return _imageName;
}

float HierarchiesSpriteSheet::getImageWidth() const {
    return _imageWidth;
}

float HierarchiesSpriteSheet::getImageHeight() const {
    return _imageHeight;
}

bool HierarchiesSpriteSheet::getSpr (const char* name, Spr& out) const {
    std::unordered_map<std::string, Spr>::const_iterator iter = _sprList.find(name);
    if (iter != _sprList.end()) {
        out = iter->second;
        return true;
    }
    return false;
}

const std::unordered_map<std::string, HierarchiesSpriteSheet::Spr>& HierarchiesSpriteSheet::getSprList () {
    return _sprList;
}

size_t HierarchiesSpriteSheet::getSprListCount () {
    return _sprList.size();
}


NS_CC_EXT_END
