//
//  CCHierarchiesSpriteSheet.cpp
//  CCHierarchiesSprite
//
//  Created by bman <zx123xz321hm3@hotmail.com>.
//  Copyright (c) 2013. All rights reserved.
//

#include "CCHierarchiesSpriteSheet.h"

using namespace rapidxml;


NS_CC_EXT_BEGIN

CCHierarchiesSpriteSheet::CCHierarchiesSpriteSheet (std::string xmlFile) {
    // load file
    std::string filePath = CCFileUtils::sharedFileUtils()->fullPathForFilename(xmlFile.c_str());
	unsigned long length = 0;
	unsigned char* data = CCFileUtils::sharedFileUtils()->getFileData(filePath.c_str(), "rb", &length);
	char* xml = (char*)malloc(length + 1);
	memcpy(xml, data, length);
	xml[length] = '\0';
	CC_SAFE_DELETE_ARRAY(data);
    
    // parse xml
    xml_document<> doc;
	doc.parse<0>(xml);
    
    // <img name=S w=N h=N>
    xml_node<>* imgNode = doc.first_node("img");
    if (NULL == imgNode) {
        CCLOG("parse <img> Node error");
        free(xml);
        return;
    }
    
    xml_attribute<>* img_name = imgNode->first_attribute("name");
    if (NULL == img_name) {
        CCLOG("parse <img> Node <name> Attr error");
        free(xml);
        return;
    }
    _imageName = img_name->value();
    
    xml_attribute<>* img_w = imgNode->first_attribute("w");
    if (NULL == img_w) {
        CCLOG("parse <img> Node <w> Attr error");
        free(xml);
        return;
    }
    _imageWidth = atof(img_w->value());
    
    xml_attribute<>* img_h = imgNode->first_attribute("h");
    if (NULL == img_h) {
        CCLOG("parse <img> Node <h> Attr error");
        free(xml);
        return;
    }
    _imageHeight = atof(img_h->value());
    
    // parse dir and spr
    parseSprNode(imgNode);
    
    // parse xml end
    free(xml);
}

CCHierarchiesSpriteSheet::~CCHierarchiesSpriteSheet () {
}

void CCHierarchiesSpriteSheet::parseSprNode (rapidxml::xml_node<>* imgNode) {
    xml_node<>* sprNode = imgNode->first_node();
    while (sprNode) {
        // <spr name=S x=F y=F w=F h=F ?rot=B>
        xml_attribute<>* attribute = NULL;
        
        attribute = sprNode->first_attribute("name");
        if (NULL == attribute) {
            CCLOG("parse <spr> Node <name> Attr error");
            return;
        }
        std::string spr_name(attribute->value());
        
        attribute = sprNode->first_attribute("x");
        if (NULL == attribute) {
            CCLOG("parse <spr> Node <x> Attr error");
            return;
        }
        float spr_x = atof(attribute->value());
        
        attribute = sprNode->first_attribute("y");
        if (NULL == attribute) {
            CCLOG("parse <spr> Node <y> Attr error");
            return;
        }
        float spr_y = atof(attribute->value());
        
        attribute = sprNode->first_attribute("w");
        if (NULL == attribute) {
            CCLOG("parse <spr> Node <w> Attr error");
            return;
        }
        float spr_w = atof(attribute->value());
        
        attribute = sprNode->first_attribute("h");
        if (NULL == attribute) {
            CCLOG("parse <spr> Node <h> Attr error");
            return;
        }
        float spr_h = atof(attribute->value());
        
        attribute = sprNode->first_attribute("rot");
        bool spr_isRotation = false;
        if (NULL != attribute) {
            if (strcmp(attribute->value(), "true") == 0) {
                spr_isRotation = true;
            }
            else {
                spr_isRotation = false;
            }
        }
        
        std::pair<std::string, Spr> item(spr_name, Spr(spr_name, spr_x, spr_y, spr_w, spr_h, spr_isRotation));
        _sprList.insert(item);
        
        sprNode = sprNode->next_sibling();
    }
}

const char* CCHierarchiesSpriteSheet::getImageName () {
    return _imageName.c_str();
}

float CCHierarchiesSpriteSheet::getImageWidth() const {
    return _imageWidth;
}

float CCHierarchiesSpriteSheet::getImageHeight() const {
    return _imageHeight;
}

bool CCHierarchiesSpriteSheet::getSpr (std::string name, Spr& out) const {
    std::unordered_map<std::string, Spr>::const_iterator iter = _sprList.find(name);
    if (iter != _sprList.end()) {
        out = iter->second;
        return true;
    }
    return false;
}

NS_CC_EXT_END
