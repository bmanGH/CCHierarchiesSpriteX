//
//  CCHierarchiesSpriteReader.cpp
//  HierarchiesSpriteDemo
//
//  Created by xuxiaocheng on 10/23/14.
//
//

#include "CCHierarchiesSpriteReader.h"

using namespace rapidxml;

NS_CC_EXT_BEGIN


#pragma mark - HierarchiesSpriteAnimationReader

HierarchiesSpriteAnimationReader::HierarchiesSpriteAnimationReader (const std::string& fileName)
: _fileName(fileName)
, _euidCount(0) {
}

HierarchiesSpriteAnimation::~HierarchiesSpriteAnimation () {
}

bool HierarchiesSpriteAnimationReader::parse (HierarchiesSpriteAnimation& out) {
	// load file
    _filePath = FileUtils::getInstance()->fullPathForFilename(_fileName);
    Data&& data = FileUtils::getInstance()->getDataFromFile(_filePath);
    if (data.getSize() == 0)
        return false;
    
	char* xml = (char*)malloc(data.getSize() + 1);
	memcpy(xml, data.getBytes(), data.getSize());
	xml[data.getSize()] = '\0';
	
    // parse xml
    xml_document<> doc;
	doc.parse<0>(xml);
    
    // <Sprite version=S frameRate=N frameCount=N>
	xml_node<>* spriteNode = doc.first_node("Sprite");
	if (NULL == spriteNode) {
		CCLOG("parse <Sprite> Node error");
		free(xml);
		return false;
	}
    
	xml_attribute<>* sprite_version = spriteNode->first_attribute("version");
	if (NULL == sprite_version) {
		CCLOG("parse <Sprite> Node <version> Attr error");
		free(xml);
		return false;
	}
    out._version = sprite_version->value();
    
	xml_attribute<>* sprite_frameRate = spriteNode->first_attribute("frameRate");
	if (NULL == sprite_frameRate) {
		CCLOG("parse <Sprite> Node <frameRate> Attr error");
		free(xml);
		return false;
	}
    out._frameRate = atoi(sprite_frameRate->value());
    
    xml_attribute<>* sprite_frameCount = spriteNode->first_attribute("frameCount");
    if (NULL == sprite_frameCount) {
        CCLOG("parse <Sprite> Node <frameCount> Attr error");
        free(xml);
        return false;
    }
    out._frameCount = atoi(sprite_frameCount->value());
    
    // <symbols>
	xml_node<>* symbolsNode = spriteNode->first_node("symbols");
	if (NULL == symbolsNode) {
		CCLOG("parse <symbols> Node error");
		free(xml);
		return false;
	}
    
    // parse Symbol
	if (parseSymbols(out, symbolsNode) == false)
    {
        free(xml);
        return false;
    }
    
    // <anims>
	xml_node<>* animsNode = spriteNode->first_node("anims");
	if (NULL == animsNode) {
		CCLOG("parse <anims> Node error");
		free(xml);
		return false;
	}
    
    // parse Animation
	if (parseAnimations(out, animsNode) == false)
    {
        free(xml);
        return false;
    }
    
    // <events>
	xml_node<>* eventsNode = spriteNode->first_node("events");
	if (NULL == eventsNode) {
		CCLOG("parse <events> Node error");
		free(xml);
		return false;
	}
    
    // parse Event
	if (parseEvents(out, eventsNode) == false)
    {
        free(xml);
        return false;
    }
    
    // <layers>
	xml_node<>* layersNode = spriteNode->first_node("layers");
	if (NULL == layersNode) {
		CCLOG("parse <layers> Node error");
		free(xml);
		return false;
	}
    
    // parse Layer, KeyFrame and Element
    if (parseLayers(out, layersNode) == false)
    {
        free(xml);
        return false;
    }
    
    // parse xml end
	free(xml);
    
    return true;
}

bool HierarchiesSpriteAnimationReader::parseSymbols (HierarchiesSpriteAnimation& out, rapidxml::xml_node<>* symbolsNode) {
	// <S name=S soc=B le=F bo=F>
	xml_node<>* symbolNode = symbolsNode->first_node("S");
	while (symbolNode) {
		xml_attribute<>* attribute = NULL;
        
        attribute = symbolNode->first_attribute("name");
        if (NULL == attribute) {
            CCLOG("parse <S> Node <name> Attr error");
            return false;
        }
        std::string Symbol_name(attribute->value());
        
        attribute = symbolNode->first_attribute("soc");
        if (NULL == attribute) {
            CCLOG("parse <S> Node <soc> Attr error");
            return false;
        }
        bool Symbol_isSocket = false;
        if (strcmp(attribute->value(), "true") == 0) {
            Symbol_isSocket = true;
        }
        else {
            Symbol_isSocket = false;
        }
        
        attribute = symbolNode->first_attribute("le");
		if (NULL == attribute) {
			CCLOG("parse <S> Node <le> Attr error");
			return false;
		}
        float Symbol_left = atof(attribute->value());
        
        attribute = symbolNode->first_attribute("bo");
		if (NULL == attribute) {
			CCLOG("parse <S> Node <bo> Attr error");
			return false;
		}
        float Symbol_bottom = atof(attribute->value());
		
		out._symbols.push_back(HierarchiesSpriteAnimation::Symbol(Symbol_name, Symbol_isSocket, Symbol_left, Symbol_bottom));
		
		symbolNode = symbolNode->next_sibling("S");
	}
    
    return true;
}

bool HierarchiesSpriteAnimationReader::parseAnimations (HierarchiesSpriteAnimation& out, rapidxml::xml_node<>* animationsNode) {
	// <ANIM name=S sid=N eid=N>
	xml_node<>* animationNode = animationsNode->first_node("ANIM");
	while (animationNode) {
		xml_attribute<>* attribute = NULL;
		
		attribute = animationNode->first_attribute("name");
		if (NULL == attribute) {
			CCLOG("parse <ANIM> Node <name> Attr error");
			return false;
		}
		std::string Animation_name(attribute->value());
		
		attribute = animationNode->first_attribute("sid");
		if (NULL == attribute) {
			CCLOG("parse <ANIM> Node <sid> Attr error");
			return false;
		}
		unsigned int Animation_startFrameIndex = atoi(attribute->value());
		
		attribute = animationNode->first_attribute("eid");
		if (NULL == attribute) {
			CCLOG("parse <ANIM> Node <eid> Attr error");
			return false;
		}
		unsigned int Animation_endFrameIndex = atoi(attribute->value());
		
		std::pair<std::string, HierarchiesSpriteAnimation::Animation> item(Animation_name, HierarchiesSpriteAnimation::Animation(Animation_name, Animation_startFrameIndex, Animation_endFrameIndex));
        out._anims.insert(item);
		
		animationNode = animationNode->next_sibling("ANIM");
	}
    
    return true;
}

bool HierarchiesSpriteAnimationReader::parseEvents (HierarchiesSpriteAnimation& out, rapidxml::xml_node<>* eventsNode) {
	// <EVE fid=N con=S>
	xml_node<>* eventNode = eventsNode->first_node("EVE");
	while (eventNode) {
		xml_attribute<>* attribute = NULL;
		
		attribute = eventNode->first_attribute("fid");
		if (NULL == attribute) {
			CCLOG("parse <EVE> Node <fid> Attr error");
			return false;
		}
		unsigned int Event_frameId = atoi(attribute->value());
		
		attribute = eventNode->first_attribute("con");
		if (NULL == attribute) {
			CCLOG("parse <EVE> Node <con> Attr error");
			return false;
		}
		std::string Event_content(attribute->value());
		
		out._events.push_back(HierarchiesSpriteAnimation::Event(Event_frameId, Event_content));
		
		eventNode = eventNode->next_sibling("EVE");
	}
    
    return true;
}

bool HierarchiesSpriteAnimationReader::parseLayers (HierarchiesSpriteAnimation& out, rapidxml::xml_node<>* layersNode) {
	// <L>
	xml_node<>* layerNode = layersNode->first_node("L");
	while (layerNode) {
        HierarchiesSpriteAnimation::Layer layer;
		if (parseKeyFrames(layer, layerNode) == false)
            return false;
        
		out._layers.push_back(layer);
		
		layerNode = layerNode->next_sibling("L");
	}
    
    return true;
}

bool HierarchiesSpriteAnimationReader::parseKeyFrames (HierarchiesSpriteAnimation::Layer& layer, rapidxml::xml_node<>* layerNode) {
	// <F id=N dur=N mot=B>
	xml_node<>* keyFrameNode = layerNode->first_node("F");
	while (keyFrameNode) {
		xml_attribute<>* attribute = NULL;
		
		attribute = keyFrameNode->first_attribute("id");
		if (NULL == attribute) {
			CCLOG("parse <F> Node <id> Attr error");
			return false;
		}
		unsigned int KeyFrame_id = atoi(attribute->value());
		
		attribute = keyFrameNode->first_attribute("dur");
		if (NULL == attribute) {
			CCLOG("parse <F> Node <dur> Attr error");
			return false;
		}
		unsigned int KeyFrame_duration = atoi(attribute->value());
		
		attribute = keyFrameNode->first_attribute("mot");
		if (NULL == attribute) {
			CCLOG("parse <F> Node <mot> Attr error");
			return false;
		}
		bool KeyFrame_isMotion = false;
        if (strcmp(attribute->value(), "true") == 0) {
            KeyFrame_isMotion = true;
        }
        else {
            KeyFrame_isMotion = false;
        }
		
        HierarchiesSpriteAnimation::KeyFrame frame(KeyFrame_id, KeyFrame_duration, KeyFrame_isMotion);
		parseElements(frame, keyFrameNode);
		layer.frames.push_back(frame);
		
		keyFrameNode = keyFrameNode->next_sibling("F");
	}
    
    return true;
}

bool HierarchiesSpriteAnimationReader::parseElements (HierarchiesSpriteAnimation::KeyFrame& frame, rapidxml::xml_node<>* keyFrameNode) {
	// <E si=N x=F y=F ax=F ay=F sx=F sy=F kx=F ky=F
    // ap=F aa=N rp=F ra=N gp=F ga=N
    // bp=F ba=N lo=E fo=N>
	xml_node<>* elementNode = keyFrameNode->first_node("E");
	while (elementNode) {
		xml_attribute<>* attribute = NULL;
		
		attribute = elementNode->first_attribute("si");
		if (NULL == attribute) {
			CCLOG("parse <E> Node <si> Attr error");
			return false;
		}
		int Element_symbolIndex = atoi(attribute->value());
		
		attribute = elementNode->first_attribute("x");
		if (NULL == attribute) {
			CCLOG("parse <E> Node <x> Attr error");
			return false;
		}
		float Element_x = atof(attribute->value());
		
		attribute = elementNode->first_attribute("y");
		if (NULL == attribute) {
			CCLOG("parse <E> Node <y> Attr error");
			return false;
		}
		float Element_y = atof(attribute->value());
		
		attribute = elementNode->first_attribute("ax");
		if (NULL == attribute) {
			CCLOG("parse <E> Node <ax> Attr error");
			return false;
		}
		float Element_anchorX = atof(attribute->value());
		
		attribute = elementNode->first_attribute("ay");
		if (NULL == attribute) {
			CCLOG("parse <E> Node <ay> Attr error");
			return false;
		}
		float Element_anchorY = atof(attribute->value());
		
		attribute = elementNode->first_attribute("sx");
		if (NULL == attribute) {
			CCLOG("parse <E> Node <sx> Attr error");
			return false;
		}
		float Element_scaleX = atof(attribute->value());
		
		attribute = elementNode->first_attribute("sy");
		if (NULL == attribute) {
			CCLOG("parse <E> Node <sy> Attr error");
			return false;
		}
		float Element_scaleY = atof(attribute->value());
		
		attribute = elementNode->first_attribute("kx");
		if (NULL == attribute) {
			CCLOG("parse <E> Node <kx> Attr error");
			return false;
		}
		float Element_skewX = atof(attribute->value());
		
		attribute = elementNode->first_attribute("ky");
		if (NULL == attribute) {
			CCLOG("parse <E> Node <ky> Attr error");
			return false;
		}
		float Element_skewY = atof(attribute->value());
		
		attribute = elementNode->first_attribute("ap");
		if (NULL == attribute) {
			CCLOG("parse <E> Node <ap> Attr error");
			return false;
		}
		float Element_color_alpha_percent = atof(attribute->value());
		
		attribute = elementNode->first_attribute("aa");
		if (NULL == attribute) {
			CCLOG("parse <E> Node <aa> Attr error");
			return false;
		}
		int Element_color_alpha_amount = atoi(attribute->value());
		
		attribute = elementNode->first_attribute("rp");
		if (NULL == attribute) {
			CCLOG("parse <E> Node <rp> Attr error");
			return false;
		}
		float Element_color_red_percent = atof(attribute->value());
		
		attribute = elementNode->first_attribute("ra");
		if (NULL == attribute) {
			CCLOG("parse <E> Node <ra> Attr error");
			return false;
		}
		int Element_color_red_amount = atoi(attribute->value());
		
		attribute = elementNode->first_attribute("gp");
		if (NULL == attribute) {
			CCLOG("parse <E> Node <gp> Attr error");
			return false;
		}
		float Element_color_green_percent = atof(attribute->value());
		
		attribute = elementNode->first_attribute("ga");
		if (NULL == attribute) {
			CCLOG("parse <E> Node <ga> Attr error");
			return false;
		}
		int Element_color_green_amount = atoi(attribute->value());
		
		attribute = elementNode->first_attribute("bp");
		if (NULL == attribute) {
			CCLOG("parse <E> Node <bp> Attr error");
			return false;
		}
		float Element_color_blue_percent = atof(attribute->value());
		
		attribute = elementNode->first_attribute("ba");
		if (NULL == attribute) {
			CCLOG("parse <E> Node <ba> Attr error");
			return false;
		}
		int Element_color_blue_amount = atoi(attribute->value());
        
        attribute = elementNode->first_attribute("lo");
		if (NULL == attribute) {
			CCLOG("parse <E> Node <lo> Attr error");
			return false;
		}
        HierarchiesSpriteAnimation::ElementLoopMode Element_loopMode = HierarchiesSpriteAnimation::kNoneLoopMode;
        if (strcmp(attribute->value(), "loop") == 0) {
            Element_loopMode = HierarchiesSpriteAnimation::kLoopLoopMode;
        }
        else if (strcmp(attribute->value(), "play once") == 0) {
            Element_loopMode = HierarchiesSpriteAnimation::kPlayOnceLoopMode;
        }
        else if (strcmp(attribute->value(), "single frame") == 0) {
            Element_loopMode = HierarchiesSpriteAnimation::kSingleFrameLoopMode;
        }
        
        attribute = elementNode->first_attribute("fo");
		if (NULL == attribute) {
			CCLOG("parse <E> Node <fo> Attr error");
			return false;
		}
        int Element_frameOffset = atoi(attribute->value());
		
		HierarchiesSpriteAnimation::Element element(Element_symbolIndex, Element_x, Element_y, Element_anchorX, Element_anchorY,
                        Element_scaleX, Element_scaleY, Element_skewX, Element_skewY,
                        Element_color_alpha_percent, Element_color_alpha_amount, Element_color_red_percent, Element_color_red_amount,
                        Element_color_green_percent, Element_color_green_amount, Element_color_blue_percent, Element_color_blue_amount,
                        Element_loopMode, Element_frameOffset, _euidCount, frame.index);
        _euidCount++;
		
		frame.elements.push_back(element);
		
		elementNode = elementNode->next_sibling("E");
	}
    
    return true;
}


#pragma mark - HierarchiesSpriteSheetReader

HierarchiesSpriteSheetReader::HierarchiesSpriteSheetReader (const std::string& fileName)
: _fileName(fileName) {
}

HierarchiesSpriteSheetReader::~HierarchiesSpriteSheetReader () {
}

bool HierarchiesSpriteSheetReader::parse (HierarchiesSpriteSheet& out) {
    // load file
    _filePath = FileUtils::getInstance()->fullPathForFilename(_fileName);
    Data&& data = FileUtils::getInstance()->getDataFromFile(_filePath);
    if (data.getSize() == 0)
        return false;
    
    char* xml = (char*)malloc(data.getSize() + 1);
	memcpy(xml, data.getBytes(), data.getSize());
	xml[data.getSize()] = '\0';
    
    // parse xml
    xml_document<> doc;
	doc.parse<0>(xml);
    
    // <img version=S name=S w=N h=N>
    xml_node<>* imgNode = doc.first_node("img");
    if (NULL == imgNode) {
        CCLOG("parse <img> Node error");
        free(xml);
        return false;
    }
    
    xml_attribute<>* img_version = imgNode->first_attribute("version");
    if (NULL == img_version) {
        CCLOG("parse <img> Node <version> Attr error");
        free(xml);
        return false;
    }
    out._version = img_version->value();
    
    xml_attribute<>* img_name = imgNode->first_attribute("name");
    if (NULL == img_name) {
        CCLOG("parse <img> Node <name> Attr error");
        free(xml);
        return false;
    }
    out._imageName = img_name->value();
    
    xml_attribute<>* img_w = imgNode->first_attribute("w");
    if (NULL == img_w) {
        CCLOG("parse <img> Node <w> Attr error");
        free(xml);
        return false;
    }
    out._imageWidth = atof(img_w->value());
    
    xml_attribute<>* img_h = imgNode->first_attribute("h");
    if (NULL == img_h) {
        CCLOG("parse <img> Node <h> Attr error");
        free(xml);
        return false;
    }
    out._imageHeight = atof(img_h->value());
    
    // parse dir and spr
    if (parseSprNode(out, imgNode) == false)
    {
        free(xml);
        return false;
    }
    
    // parse xml end
    free(xml);
    
    return true;
}

bool HierarchiesSpriteSheetReader::parseSprNode (HierarchiesSpriteSheet& out, rapidxml::xml_node<>* imgNode) {
    xml_node<>* sprNode = imgNode->first_node();
    while (sprNode) {
        // <spr name=S x=F y=F w=F h=F ?rot=B>
        xml_attribute<>* attribute = NULL;
        
        attribute = sprNode->first_attribute("name");
        if (NULL == attribute) {
            CCLOG("parse <spr> Node <name> Attr error");
            return false;
        }
        std::string spr_name(attribute->value());
        
        attribute = sprNode->first_attribute("x");
        if (NULL == attribute) {
            CCLOG("parse <spr> Node <x> Attr error");
            return false;
        }
        float spr_x = atof(attribute->value());
        
        attribute = sprNode->first_attribute("y");
        if (NULL == attribute) {
            CCLOG("parse <spr> Node <y> Attr error");
            return false;
        }
        float spr_y = atof(attribute->value());
        
        attribute = sprNode->first_attribute("w");
        if (NULL == attribute) {
            CCLOG("parse <spr> Node <w> Attr error");
            return false;
        }
        float spr_w = atof(attribute->value());
        
        attribute = sprNode->first_attribute("h");
        if (NULL == attribute) {
            CCLOG("parse <spr> Node <h> Attr error");
            return false;
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
        
        std::pair<std::string, HierarchiesSpriteSheet::Spr> item(spr_name, HierarchiesSpriteSheet::Spr(spr_name, spr_x, spr_y, spr_w, spr_h, spr_isRotation));
        out._sprList.insert(item);
        
        sprNode = sprNode->next_sibling();
    }
    
    return true;
}


NS_CC_EXT_END
