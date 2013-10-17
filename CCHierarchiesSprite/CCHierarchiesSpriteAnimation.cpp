//
//  CCHierarchiesSpriteAnimation.cpp
//  CCHierarchiesSprite
//
//  Created by bman <zx123xz321hm3@hotmail.com>.
//  Copyright (c) 2013. All rights reserved.
//

#include "CCHierarchiesSpriteAnimation.h"

using namespace rapidxml;


NS_CC_EXT_BEGIN

CCHierarchiesSpriteAnimation::CCHierarchiesSpriteAnimation (std::string xmlFile) {
	// load file
    std::string filePath = CCFileUtils::sharedFileUtils()->fullPathForFilename(xmlFile.c_str());
	unsigned long length = 0;
	unsigned char* data = CCFileUtils::sharedFileUtils()->getFileData(filePath.c_str(), "rb", &length);
	char* xml = (char*)malloc(length + 1);
	memcpy(xml, data, length);
	xml[length] = '\0';
	CC_SAFE_DELETE_ARRAY(data);
    
    _euidCount = 0;
	
    // parse xml
    xml_document<> doc;
	doc.parse<0>(xml);
    
    // <Sprite version=S frameRate=N frameCount=N>
	xml_node<>* spriteNode = doc.first_node("Sprite");
	if (NULL == spriteNode) {
		CCLOG("parse <Sprite> Node error");
		free(xml);
		return;
	}
    
	xml_attribute<>* sprite_version = spriteNode->first_attribute("version");
	if (NULL == sprite_version) {
		CCLOG("parse <Sprite> Node <version> Attr error");
		free(xml);
		return;
	}
    _version = sprite_version->value();
    
	xml_attribute<>* sprite_frameRate = spriteNode->first_attribute("frameRate");
	if (NULL == sprite_frameRate) {
		CCLOG("parse <Sprite> Node <frameRate> Attr error");
		free(xml);
		return;
	}
    _frameRate = atoi(sprite_frameRate->value());
    
    xml_attribute<>* sprite_frameCount = spriteNode->first_attribute("frameCount");
    if (NULL == sprite_frameCount) {
        CCLOG("parse <Sprite> Node <frameCount> Attr error");
        free(xml);
        return;
    }
    _frameCount = atoi(sprite_frameCount->value());
    
    // <symbols>
	xml_node<>* symbolsNode = spriteNode->first_node("symbols");
	if (NULL == symbolsNode) {
		CCLOG("parse <symbols> Node error");
		free(xml);
		return;
	}
    
    // parse Symbol
	parseSymbols(symbolsNode);
    
    // <anims>
	xml_node<>* animsNode = spriteNode->first_node("anims");
	if (NULL == animsNode) {
		CCLOG("parse <anims> Node error");
		free(xml);
		return;
	}
    
    // parse Animation
	parseAnimations(animsNode);
    
    // <events>
	xml_node<>* eventsNode = spriteNode->first_node("events");
	if (NULL == eventsNode) {
		CCLOG("parse <events> Node error");
		free(xml);
		return;
	}
    
    // parse Event
	parseEvents(eventsNode);
    
    // <layers>
	xml_node<>* layersNode = spriteNode->first_node("layers");
	if (NULL == layersNode) {
		CCLOG("parse <layers> Node error");
		free(xml);
		return;
	}
    
    // parse Layer, KeyFrame and Element
	parseLayers(layersNode);
    
    // parse xml end
	free(xml);
}

void CCHierarchiesSpriteAnimation::parseAnimations (rapidxml::xml_node<>* animationsNode) {
	// <ANIM name=S sid=N eid=N>
	xml_node<>* animationNode = animationsNode->first_node("ANIM");
	while (animationNode) {
		xml_attribute<>* attribute = NULL;
		
		attribute = animationNode->first_attribute("name");
		if (NULL == attribute) {
			CCLOG("parse <ANIM> Node <name> Attr error");
			return;
		}
		std::string Animation_name(attribute->value());
		
		attribute = animationNode->first_attribute("sid");
		if (NULL == attribute) {
			CCLOG("parse <ANIM> Node <sid> Attr error");
			return;
		}
		unsigned int Animation_startFrameIndex = atoi(attribute->value());
		
		attribute = animationNode->first_attribute("eid");
		if (NULL == attribute) {
			CCLOG("parse <ANIM> Node <eid> Attr error");
			return;
		}
		unsigned int Animation_endFrameIndex = atoi(attribute->value());
		
		std::pair<std::string, Animation> item(Animation_name, Animation(Animation_name, Animation_startFrameIndex, Animation_endFrameIndex));
        _anims.insert(item);
		
		animationNode = animationNode->next_sibling("ANIM");
	}
}

void CCHierarchiesSpriteAnimation::parseLayers (rapidxml::xml_node<>* layersNode) {
	// <L>
	xml_node<>* layerNode = layersNode->first_node("L");
	while (layerNode) {
		Layer layer;
		parseKeyFrames(layer, layerNode);
		_layers.push_back(layer);
		
		layerNode = layerNode->next_sibling("L");
	}
}

void CCHierarchiesSpriteAnimation::parseSymbols (rapidxml::xml_node<>* symbolsNode) {
	// <S name=S soc=B le=F bo=F>
	xml_node<>* symbolNode = symbolsNode->first_node("S");
	while (symbolNode) {
		xml_attribute<>* attribute = NULL;
        
        attribute = symbolNode->first_attribute("name");
        if (NULL == attribute) {
            CCLOG("parse <S> Node <name> Attr error");
            return;
        }
        std::string Symbol_name(attribute->value());
        
        attribute = symbolNode->first_attribute("soc");
        if (NULL == attribute) {
            CCLOG("parse <S> Node <soc> Attr error");
            return;
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
			return;
		}
        float Symbol_left = atof(attribute->value());
        
        attribute = symbolNode->first_attribute("bo");
		if (NULL == attribute) {
			CCLOG("parse <S> Node <bo> Attr error");
			return;
		}
        float Symbol_bottom = atof(attribute->value());
		
		_symbols.push_back(Symbol(Symbol_name, Symbol_isSocket, Symbol_left, Symbol_bottom));
		
		symbolNode = symbolNode->next_sibling("S");
	}
}

void CCHierarchiesSpriteAnimation::parseEvents (rapidxml::xml_node<>* eventsNode) {
	// <EVE fid=N con=S>
	xml_node<>* eventNode = eventsNode->first_node("EVE");
	while (eventNode) {
		xml_attribute<>* attribute = NULL;
		
		attribute = eventNode->first_attribute("fid");
		if (NULL == attribute) {
			CCLOG("parse <EVE> Node <fid> Attr error");
			return;
		}
		unsigned int Event_frameId = atoi(attribute->value());
		
		attribute = eventNode->first_attribute("con");
		if (NULL == attribute) {
			CCLOG("parse <EVE> Node <con> Attr error");
			return;
		}
		std::string Event_content(attribute->value());
		
		_events.push_back(Event(Event_frameId, Event_content));
		
		eventNode = eventNode->next_sibling("EVE");
	}
}

void CCHierarchiesSpriteAnimation::parseKeyFrames (Layer& layer, rapidxml::xml_node<>* layerNode) {
	// <F id=N dur=N mot=B>
	xml_node<>* keyFrameNode = layerNode->first_node("F");
	while (keyFrameNode) {
		xml_attribute<>* attribute = NULL;
		
		attribute = keyFrameNode->first_attribute("id");
		if (NULL == attribute) {
			CCLOG("parse <F> Node <id> Attr error");
			return;
		}
		unsigned int KeyFrame_id = atoi(attribute->value());
		
		attribute = keyFrameNode->first_attribute("dur");
		if (NULL == attribute) {
			CCLOG("parse <F> Node <dur> Attr error");
			return;
		}
		unsigned int KeyFrame_duration = atoi(attribute->value());
		
		attribute = keyFrameNode->first_attribute("mot");
		if (NULL == attribute) {
			CCLOG("parse <F> Node <mot> Attr error");
			return;
		}
		bool KeyFrame_isMotion = false;
        if (strcmp(attribute->value(), "true") == 0) {
            KeyFrame_isMotion = true;
        }
        else {
            KeyFrame_isMotion = false;
        }
		
		KeyFrame frame(KeyFrame_id, KeyFrame_duration, KeyFrame_isMotion);
		parseElements(frame, keyFrameNode);
		layer.frames.push_back(frame);
		
		keyFrameNode = keyFrameNode->next_sibling("F");
	}
}

void CCHierarchiesSpriteAnimation::parseElements (KeyFrame& frame, rapidxml::xml_node<>* keyFrameNode) {
	// <E si=N x=F y=F ax=F ay=F sx=F sy=F kx=F ky=F
    // ap=F aa=N rp=F ra=N gp=F ga=N
    // bp=F ba=N lo=E fo=N>
	xml_node<>* elementNode = keyFrameNode->first_node("E");
	while (elementNode) {
		xml_attribute<>* attribute = NULL;
		
		attribute = elementNode->first_attribute("si");
		if (NULL == attribute) {
			CCLOG("parse <E> Node <si> Attr error");
			return;
		}
		int Element_symbolIndex = atoi(attribute->value());
		
		attribute = elementNode->first_attribute("x");
		if (NULL == attribute) {
			CCLOG("parse <E> Node <x> Attr error");
			return;
		}
		float Element_x = atof(attribute->value());
		
		attribute = elementNode->first_attribute("y");
		if (NULL == attribute) {
			CCLOG("parse <E> Node <y> Attr error");
			return;
		}
		float Element_y = atof(attribute->value());
		
		attribute = elementNode->first_attribute("ax");
		if (NULL == attribute) {
			CCLOG("parse <E> Node <ax> Attr error");
			return;
		}
		float Element_anchorX = atof(attribute->value());
		
		attribute = elementNode->first_attribute("ay");
		if (NULL == attribute) {
			CCLOG("parse <E> Node <ay> Attr error");
			return;
		}
		float Element_anchorY = atof(attribute->value());
		
		attribute = elementNode->first_attribute("sx");
		if (NULL == attribute) {
			CCLOG("parse <E> Node <sx> Attr error");
			return;
		}
		float Element_scaleX = atof(attribute->value());
		
		attribute = elementNode->first_attribute("sy");
		if (NULL == attribute) {
			CCLOG("parse <E> Node <sy> Attr error");
			return;
		}
		float Element_scaleY = atof(attribute->value());
		
//		attribute = elementNode->first_attribute("rot");
//		if (NULL == attribute) {
//			CCLOG("parse <Element> Node <rot> Attr error");
//			return;
//		}
//		float Element_rotation = atof(attribute->value());
		
		attribute = elementNode->first_attribute("kx");
		if (NULL == attribute) {
			CCLOG("parse <E> Node <kx> Attr error");
			return;
		}
		float Element_skewX = atof(attribute->value());
		
		attribute = elementNode->first_attribute("ky");
		if (NULL == attribute) {
			CCLOG("parse <E> Node <ky> Attr error");
			return;
		}
		float Element_skewY = atof(attribute->value());
        
//		attribute = elementNode->first_attribute("dep");
//		if (NULL == attribute) {
//			CCLOG("parse <Element> Node <dep> Attr error");
//			return;
//		}
//		float Element_depth = atoi(attribute->value());
		
		attribute = elementNode->first_attribute("ap");
		if (NULL == attribute) {
			CCLOG("parse <E> Node <ap> Attr error");
			return;
		}
		float Element_color_alpha_percent = atof(attribute->value());
		
		attribute = elementNode->first_attribute("aa");
		if (NULL == attribute) {
			CCLOG("parse <E> Node <aa> Attr error");
			return;
		}
		int Element_color_alpha_amount = atoi(attribute->value());
		
		attribute = elementNode->first_attribute("rp");
		if (NULL == attribute) {
			CCLOG("parse <E> Node <rp> Attr error");
			return;
		}
		float Element_color_red_percent = atof(attribute->value());
		
		attribute = elementNode->first_attribute("ra");
		if (NULL == attribute) {
			CCLOG("parse <E> Node <ra> Attr error");
			return;
		}
		int Element_color_red_amount = atoi(attribute->value());
		
		attribute = elementNode->first_attribute("gp");
		if (NULL == attribute) {
			CCLOG("parse <E> Node <gp> Attr error");
			return;
		}
		float Element_color_green_percent = atof(attribute->value());
		
		attribute = elementNode->first_attribute("ga");
		if (NULL == attribute) {
			CCLOG("parse <E> Node <ga> Attr error");
			return;
		}
		int Element_color_green_amount = atoi(attribute->value());
		
		attribute = elementNode->first_attribute("bp");
		if (NULL == attribute) {
			CCLOG("parse <E> Node <bp> Attr error");
			return;
		}
		float Element_color_blue_percent = atof(attribute->value());
		
		attribute = elementNode->first_attribute("ba");
		if (NULL == attribute) {
			CCLOG("parse <E> Node <ba> Attr error");
			return;
		}
		int Element_color_blue_amount = atoi(attribute->value());
        
        attribute = elementNode->first_attribute("lo");
		if (NULL == attribute) {
			CCLOG("parse <E> Node <lo> Attr error");
			return;
		}
        ElementLoopMode Element_loopMode = kNoneLoopMode;
        if (strcmp(attribute->value(), "loop") == 0) {
            Element_loopMode = kLoopLoopMode;
        }
        else if (strcmp(attribute->value(), "play once") == 0) {
            Element_loopMode = kPlayOnceLoopMode;
        }
        else if (strcmp(attribute->value(), "single frame") == 0) {
            Element_loopMode = kSingleFrameLoopMode;
        }
        
        attribute = elementNode->first_attribute("fo");
		if (NULL == attribute) {
			CCLOG("parse <E> Node <fo> Attr error");
			return;
		}
        int Element_frameOffset = atoi(attribute->value());
		
		Element element(Element_symbolIndex, Element_x, Element_y, Element_anchorX, Element_anchorY,
                        Element_scaleX, Element_scaleY, /*Element_rotation, */Element_skewX, Element_skewY,
                        Element_color_alpha_percent, Element_color_alpha_amount, Element_color_red_percent, Element_color_red_amount,
                        Element_color_green_percent, Element_color_green_amount, Element_color_blue_percent, Element_color_blue_amount,
                        Element_loopMode, Element_frameOffset, _euidCount, frame.id);
        _euidCount++;
        
        parseFilters(element, elementNode);
		
		frame.elements.push_back(element);
		
		elementNode = elementNode->next_sibling("E");
	}
}

void CCHierarchiesSpriteAnimation::parseFilters (Element& element, rapidxml::xml_node<>* elementNode) {
    // <Filter name=S angle=F blurX=F blurY=F brightness=F color=S contrast=F distance=F hideObject=B highlightColor=S
    //  hue=F inner=B knockout=B quality=S saturation=F shadowColor=S strength=N type=S>
    xml_node<>* filterNode = elementNode->first_node("Filter");
    std::vector<Filter> filters;
    while (filterNode) {
        xml_attribute<>* attribute = NULL;
        
        attribute = filterNode->first_attribute("name");
        if (NULL == attribute) {
            CCLOG("parse <Filter> Node <name> Attr error");
            return;
        }
        FilterName filter_name;
        if (strcmp(attribute->value(), "adjustColorFilter") == 0)
            filter_name = AdjustColorFilter;
        else if (strcmp(attribute->value(), "bevelFilter") == 0)
            filter_name = BevelFilter;
        else if (strcmp(attribute->value(), "blurFilter") == 0)
            filter_name = BlurFilter;
        else if (strcmp(attribute->value(), "dropShadowFilter") == 0)
            filter_name = DropShadowFilter;
        else if (strcmp(attribute->value(), "glowFilter") == 0)
            filter_name = GlowFilter;
        else if (strcmp(attribute->value(), "gradientBevelFilter") == 0)
            filter_name = GradientBevelFilter;
        else if (strcmp(attribute->value(), "gradientGlowFilter") == 0)
            filter_name = GradientGlowFilter;
        else {
            CCLOG("parse <Filter> Node <name> Attr fail");
            return;
        }
        
        switch (filter_name) {
            case AdjustColorFilter: {
                attribute = filterNode->first_attribute("brightness");
                if (NULL == attribute) {
                    CCLOG("parse <Filter> Node <brightness> Attr error");
                    return;
                }
                float filter_brightness = atof(attribute->value());
                
                attribute = filterNode->first_attribute("contrast");
                if (NULL == attribute) {
                    CCLOG("parse <Filter> Node <contrast> Attr error");
                    return;
                }
                float filter_contrast = atof(attribute->value());
                
                attribute = filterNode->first_attribute("saturation");
                if (NULL == attribute) {
                    CCLOG("parse <Filter> Node <saturation> Attr error");
                    return;
                }
                float filter_saturation = atof(attribute->value());
                
                attribute = filterNode->first_attribute("hue");
                if (NULL == attribute) {
                    CCLOG("parse <Filter> Node <hue> Attr error");
                    return;
                }
                float filter_hue = atof(attribute->value());
                
                Filter filter(filter_name, filter_brightness, filter_contrast, filter_saturation, filter_hue);
                filters.push_back(filter);
                break;
            }
            case BevelFilter: {
                attribute = filterNode->first_attribute("blurX");
                if (NULL == attribute) {
                    CCLOG("parse <Filter> Node <blurX> Attr error");
                    return;
                }
                float filter_blurX = atof(attribute->value());
                
                attribute = filterNode->first_attribute("blurY");
                if (NULL == attribute) {
                    CCLOG("parse <Filter> Node <blurY> Attr error");
                    return;
                }
                float filter_blurY = atof(attribute->value());
                
                attribute = filterNode->first_attribute("strength");
                if (NULL == attribute) {
                    CCLOG("parse <Filter> Node <strength> Attr error");
                    return;
                }
                int filter_strength = atoi(attribute->value());
                
                attribute = filterNode->first_attribute("quality");
                if (NULL == attribute) {
                    CCLOG("parse <Filter> Node <quality> Attr error");
                    return;
                }
                FilterQuality filter_quality;
                if (strcmp(attribute->value(), "low") == 0) {
                    filter_quality = LowQuality;
                }
                else if (strcmp(attribute->value(), "medium") == 0) {
                    filter_quality = MediumQuality;
                }
                else if (strcmp(attribute->value(), "high") == 0) {
                    filter_quality = HighQuality;
                }
                else {
                    CCLOG("parse <Filter> Node <quality> Attr fail");
                    return;
                }
                
                attribute = filterNode->first_attribute("shadowColor");
                if (NULL == attribute) {
                    CCLOG("parse <Filter> Node <shadowColor> Attr error");
                    return;
                }
                FilterColor4F filter_shadowColor;
                filter_shadowColor.setByString(attribute->value());
                
                attribute = filterNode->first_attribute("highlightColor");
                if (NULL == attribute) {
                    CCLOG("parse <Filter> Node <highlightColor> Attr error");
                    return;
                }
                FilterColor4F filter_highlightColor;
                filter_highlightColor.setByString(attribute->value());
                
                attribute = filterNode->first_attribute("angle");
                if (NULL == attribute) {
                    CCLOG("parse <Filter> Node <angle> Attr error");
                    return;
                }
                float filter_angle = atof(attribute->value());
                
                attribute = filterNode->first_attribute("distance");
                if (NULL == attribute) {
                    CCLOG("parse <Filter> Node <distance> Attr error");
                    return;
                }
                float filter_distance = atof(attribute->value());
                
                attribute = filterNode->first_attribute("knockout");
                if (NULL == attribute) {
                    CCLOG("parse <Filter> Node <knockout> Attr error");
                    return;
                }
                bool filter_knockout = false;
                if (strcmp(attribute->value(), "true") == 0) {
                    filter_knockout = true;
                }
                else {
                    filter_knockout = false;
                }
                
                attribute = filterNode->first_attribute("type");
                if (NULL == attribute) {
                    CCLOG("parse <Filter> Node <type> Attr error");
                    return;
                }
                FilterType filter_type;
                if (strcmp(attribute->value(), "inner") == 0) {
                    filter_type = InnerType;
                }
                else if (strcmp(attribute->value(), "outer") == 0) {
                    filter_type = OuterType;
                }
                else if (strcmp(attribute->value(), "full")) {
                    filter_type = FullType;
                }
                else {
                    CCLOG("parse <Filter> Node <type> Attr error");
                    return;
                }
                
                Filter filter (filter_name, filter_blurX, filter_blurY, filter_strength, filter_quality,
                               filter_shadowColor, filter_highlightColor, filter_angle, filter_distance,
                               filter_knockout, filter_type);
                filters.push_back(filter);
                break;
            }
            case BlurFilter: {
//                Filter (FilterName name, float blurX, float blurY, FilterQuality quality)
//                : name(name) {
//                    blur.blurX = blurX;
//                    blur.blurY = blurY;
//                    blur.quality = quality;
                //                }
                attribute = filterNode->first_attribute("blurX");
                if (NULL == attribute) {
                    CCLOG("parse <Filter> Node <blurX> Attr error");
                    return;
                }
                float filter_blurX = atof(attribute->value());
                
                attribute = filterNode->first_attribute("blurY");
                if (NULL == attribute) {
                    CCLOG("parse <Filter> Node <blurY> Attr error");
                    return;
                }
                float filter_blurY = atof(attribute->value());
                
                attribute = filterNode->first_attribute("quality");
                if (NULL == attribute) {
                    CCLOG("parse <Filter> Node <quality> Attr error");
                    return;
                }
                FilterQuality filter_quality;
                if (strcmp(attribute->value(), "low") == 0) {
                    filter_quality = LowQuality;
                }
                else if (strcmp(attribute->value(), "medium") == 0) {
                    filter_quality = MediumQuality;
                }
                else if (strcmp(attribute->value(), "high") == 0) {
                    filter_quality = HighQuality;
                }
                else {
                    CCLOG("parse <Filter> Node <quality> Attr fail");
                    return;
                }
                
                Filter filter(filter_name, filter_blurX, filter_blurY, filter_quality);
                filters.push_back(filter);
                break;
            }
            case DropShadowFilter: {
                attribute = filterNode->first_attribute("blurX");
                if (NULL == attribute) {
                    CCLOG("parse <Filter> Node <blurX> Attr error");
                    return;
                }
                float filter_blurX = atof(attribute->value());
                
                attribute = filterNode->first_attribute("blurY");
                if (NULL == attribute) {
                    CCLOG("parse <Filter> Node <blurY> Attr error");
                    return;
                }
                float filter_blurY = atof(attribute->value());
                
                attribute = filterNode->first_attribute("strength");
                if (NULL == attribute) {
                    CCLOG("parse <Filter> Node <strength> Attr error");
                    return;
                }
                int filter_strength = atoi(attribute->value());
                
                attribute = filterNode->first_attribute("quality");
                if (NULL == attribute) {
                    CCLOG("parse <Filter> Node <quality> Attr error");
                    return;
                }
                FilterQuality filter_quality;
                if (strcmp(attribute->value(), "low") == 0) {
                    filter_quality = LowQuality;
                }
                else if (strcmp(attribute->value(), "medium") == 0) {
                    filter_quality = MediumQuality;
                }
                else if (strcmp(attribute->value(), "high") == 0) {
                    filter_quality = HighQuality;
                }
                else {
                    CCLOG("parse <Filter> Node <quality> Attr fail");
                    return;
                }
                
                attribute = filterNode->first_attribute("angle");
                if (NULL == attribute) {
                    CCLOG("parse <Filter> Node <angle> Attr error");
                    return;
                }
                float filter_angle = atof(attribute->value());
                
                attribute = filterNode->first_attribute("distance");
                if (NULL == attribute) {
                    CCLOG("parse <Filter> Node <distance> Attr error");
                    return;
                }
                float filter_distance = atof(attribute->value());
                
                attribute = filterNode->first_attribute("knockout");
                if (NULL == attribute) {
                    CCLOG("parse <Filter> Node <knockout> Attr error");
                    return;
                }
                bool filter_knockout = false;
                if (strcmp(attribute->value(), "true") == 0) {
                    filter_knockout = true;
                }
                else {
                    filter_knockout = false;
                }
                
                attribute = filterNode->first_attribute("inner");
                if (NULL == attribute) {
                    CCLOG("parse <Filter> Node <inner> Attr error");
                    return;
                }
                bool filter_inner = false;
                if (strcmp(attribute->value(), "true") == 0) {
                    filter_inner = true;
                }
                else {
                    filter_inner = false;
                }
                
                attribute = filterNode->first_attribute("hideObject");
                if (NULL == attribute) {
                    CCLOG("parse <Filter> Node <hideObject> Attr error");
                    return;
                }
                bool filter_hideObject = false;
                if (strcmp(attribute->value(), "true") == 0) {
                    filter_hideObject = true;
                }
                else {
                    filter_hideObject = false;
                }
                
                attribute = filterNode->first_attribute("color");
                if (NULL == attribute) {
                    CCLOG("parse <Filter> Node <color> Attr error");
                    return;
                }
                FilterColor4F filter_color;
                filter_color.setByString(attribute->value());
                
                Filter filter(filter_name, filter_blurX, filter_blurY, filter_strength, filter_quality, filter_angle,
                              filter_distance, filter_knockout, filter_inner, filter_hideObject, filter_color);
                filters.push_back(filter);
                break;
            }
            case GlowFilter: {
                attribute = filterNode->first_attribute("blurX");
                if (NULL == attribute) {
                    CCLOG("parse <Filter> Node <blurX> Attr error");
                    return;
                }
                float filter_blurX = atof(attribute->value());
                
                attribute = filterNode->first_attribute("blurY");
                if (NULL == attribute) {
                    CCLOG("parse <Filter> Node <blurY> Attr error");
                    return;
                }
                float filter_blurY = atof(attribute->value());
                
                attribute = filterNode->first_attribute("strength");
                if (NULL == attribute) {
                    CCLOG("parse <Filter> Node <strength> Attr error");
                    return;
                }
                int filter_strength = atoi(attribute->value());
                
                attribute = filterNode->first_attribute("quality");
                if (NULL == attribute) {
                    CCLOG("parse <Filter> Node <quality> Attr error");
                    return;
                }
                FilterQuality filter_quality;
                if (strcmp(attribute->value(), "low") == 0) {
                    filter_quality = LowQuality;
                }
                else if (strcmp(attribute->value(), "medium") == 0) {
                    filter_quality = MediumQuality;
                }
                else if (strcmp(attribute->value(), "high") == 0) {
                    filter_quality = HighQuality;
                }
                else {
                    CCLOG("parse <Filter> Node <quality> Attr fail");
                    return;
                }
                
                attribute = filterNode->first_attribute("color");
                if (NULL == attribute) {
                    CCLOG("parse <Filter> Node <color> Attr error");
                    return;
                }
                FilterColor4F filter_color;
                filter_color.setByString(attribute->value());
                
                attribute = filterNode->first_attribute("knockout");
                if (NULL == attribute) {
                    CCLOG("parse <Filter> Node <knockout> Attr error");
                    return;
                }
                bool filter_knockout = false;
                if (strcmp(attribute->value(), "true") == 0) {
                    filter_knockout = true;
                }
                else {
                    filter_knockout = false;
                }
                
                attribute = filterNode->first_attribute("inner");
                if (NULL == attribute) {
                    CCLOG("parse <Filter> Node <inner> Attr error");
                    return;
                }
                bool filter_inner = false;
                if (strcmp(attribute->value(), "true") == 0) {
                    filter_inner = true;
                }
                else {
                    filter_inner = false;
                }
                
                Filter filter(filter_name, filter_blurX, filter_blurY, filter_strength, filter_quality,
                              filter_color, filter_knockout, filter_inner);
                filters.push_back(filter);
                break;
            }
            case GradientBevelFilter: // GradientBevelFilter and GradientGlowFilter are same params
            case GradientGlowFilter: {
                attribute = filterNode->first_attribute("blurX");
                if (NULL == attribute) {
                    CCLOG("parse <Filter> Node <blurX> Attr error");
                    return;
                }
                float filter_blurX = atof(attribute->value());
                
                attribute = filterNode->first_attribute("blurY");
                if (NULL == attribute) {
                    CCLOG("parse <Filter> Node <blurY> Attr error");
                    return;
                }
                float filter_blurY = atof(attribute->value());
                
                attribute = filterNode->first_attribute("strength");
                if (NULL == attribute) {
                    CCLOG("parse <Filter> Node <strength> Attr error");
                    return;
                }
                int filter_strength = atoi(attribute->value());
                
                attribute = filterNode->first_attribute("quality");
                if (NULL == attribute) {
                    CCLOG("parse <Filter> Node <quality> Attr error");
                    return;
                }
                FilterQuality filter_quality;
                if (strcmp(attribute->value(), "low") == 0) {
                    filter_quality = LowQuality;
                }
                else if (strcmp(attribute->value(), "medium") == 0) {
                    filter_quality = MediumQuality;
                }
                else if (strcmp(attribute->value(), "high") == 0) {
                    filter_quality = HighQuality;
                }
                else {
                    CCLOG("parse <Filter> Node <quality> Attr fail");
                    return;
                }
                
                attribute = filterNode->first_attribute("angle");
                if (NULL == attribute) {
                    CCLOG("parse <Filter> Node <angle> Attr error");
                    return;
                }
                float filter_angle = atof(attribute->value());
                
                attribute = filterNode->first_attribute("distance");
                if (NULL == attribute) {
                    CCLOG("parse <Filter> Node <distance> Attr error");
                    return;
                }
                float filter_distance = atof(attribute->value());
                
                attribute = filterNode->first_attribute("hideObject");
                if (NULL == attribute) {
                    CCLOG("parse <Filter> Node <hideObject> Attr error");
                    return;
                }
                bool filter_hideObject = false;
                if (strcmp(attribute->value(), "true") == 0) {
                    filter_hideObject = true;
                }
                else {
                    filter_hideObject = false;
                }
                
                attribute = filterNode->first_attribute("knockout");
                if (NULL == attribute) {
                    CCLOG("parse <Filter> Node <knockout> Attr error");
                    return;
                }
                bool filter_knockout = false;
                if (strcmp(attribute->value(), "true") == 0) {
                    filter_knockout = true;
                }
                else {
                    filter_knockout = false;
                }
                
                attribute = filterNode->first_attribute("type");
                if (NULL == attribute) {
                    CCLOG("parse <Filter> Node <type> Attr error");
                    return;
                }
                FilterType filter_type;
                if (strcmp(attribute->value(), "inner") == 0) {
                    filter_type = InnerType;
                }
                else if (strcmp(attribute->value(), "outer") == 0) {
                    filter_type = OuterType;
                }
                else if (strcmp(attribute->value(), "full")) {
                    filter_type = FullType;
                }
                else {
                    CCLOG("parse <Filter> Node <type> Attr error");
                    return;
                }
                
                Filter filter(filter_name, filter_blurX, filter_blurY, filter_strength, filter_quality, filter_angle,
                              filter_distance, filter_knockout, filter_type);
                filters.push_back(filter);
                break;
            }
        }
        
        filterNode = filterNode->next_sibling("Filter");
    }
    filters.shrink_to_fit();
    element.filters = filters;
}

CCHierarchiesSpriteAnimation::~CCHierarchiesSpriteAnimation () {
}

std::string CCHierarchiesSpriteAnimation::getVersion () const {
    return _version;
}

unsigned int CCHierarchiesSpriteAnimation::getFrameRate () const {
    return _frameRate;
}

size_t CCHierarchiesSpriteAnimation::getAnimationCount () const {
    return _anims.size();
}

size_t CCHierarchiesSpriteAnimation::getFrameCount() const {
	return _frameCount;
}

int CCHierarchiesSpriteAnimation::getEUIDCount() {
    return _euidCount;
}

const std::vector<CCHierarchiesSpriteAnimation::Layer>& CCHierarchiesSpriteAnimation::getLayers () {
    return _layers;
}

const std::vector<CCHierarchiesSpriteAnimation::Symbol>& CCHierarchiesSpriteAnimation::getSymbols () {
    return _symbols;
}

const std::vector<CCHierarchiesSpriteAnimation::Event>& CCHierarchiesSpriteAnimation::getEvents () {
    return _events;
}

bool CCHierarchiesSpriteAnimation::getAnimationByName (std::string name, Animation& out) {
    std::unordered_map<std::string, Animation>::iterator iter = _anims.find(name);
    if (iter != _anims.end()) {
        out = iter->second;
        return true;
    }
    return false;
}

std::vector<CCHierarchiesSpriteAnimation::Animation> CCHierarchiesSpriteAnimation::getAnimationList () {
	std::vector<Animation> ret;
	std::unordered_map<std::string, Animation>::iterator iter;
	for (iter = _anims.begin(); iter != _anims.end(); iter++) {
		ret.push_back(iter->second);
	}
	return ret;
}

size_t CCHierarchiesSpriteAnimation::getLayerCount () {
    return _layers.size();
}

bool CCHierarchiesSpriteAnimation::getLayerByIndex (unsigned int index, Layer& out) {
    if (index >= _layers.size())
        return false;
    
    out = _layers[index];
    return true;
}

bool CCHierarchiesSpriteAnimation::getKeyFrameIncludeIndexAtLayer (unsigned int layerIndex, unsigned int index, KeyFrame& out) {
    if (layerIndex >= _layers.size())
        return false;
    
    std::vector<KeyFrame>::iterator iter;
    for (iter = _layers[layerIndex].frames.begin(); iter != _layers[layerIndex].frames.end(); iter++) {
        if (index >= iter->id && index < (iter->id + iter->duration)) {
            out = *iter;
            return true;
        }
    }
    
    return false;
}

bool CCHierarchiesSpriteAnimation::getSymbolByIndex(int symbolIndex, Symbol& out) const {
    if (symbolIndex >= _symbols.size())
        return false;
    
    out = _symbols[symbolIndex];
    return true;
}

size_t CCHierarchiesSpriteAnimation::getSymbolCount() {
    return _symbols.size();
}

bool CCHierarchiesSpriteAnimation::getEventByFrameId (unsigned int frameId, Event& out) {
    std::vector<Event>::iterator evtIter;
    for (evtIter = _events.begin(); evtIter != _events.end(); evtIter++) {
        if (evtIter->frameId == frameId) {
            out = *evtIter;
            return true;
        }
    }
    return false;
}

int CCHierarchiesSpriteAnimation::getFrameElementsAtFrameIndex (ElementLoopMode loopMode, int frameOffset, unsigned int& frameIndex, FrameElements& out) const {
    switch (loopMode) {
        case kNoneLoopMode: { // same as kLoopLoopMode with 0 frame offset
            frameIndex = frameIndex % _frameCount;
            break;
        }
        case kLoopLoopMode: {
            frameIndex = (frameIndex + frameOffset) % _frameCount;
            break;
        }
        case kPlayOnceLoopMode: {
            frameIndex = (frameIndex + frameOffset) > _frameCount - 1 ? _frameCount - 1 : (frameIndex + frameOffset);
            break;
        }
        case kSingleFrameLoopMode: {
            frameIndex = frameOffset;
            break;
        }
    }
    
    int eNum = 0;
    std::vector<Layer>::const_reverse_iterator layerIter;
    for (layerIter = _layers.crbegin(); layerIter != _layers.crend(); layerIter++) { // layer iteration BEGIN
        
        std::vector<KeyFrame>::const_iterator keyFrameIter;
        for (keyFrameIter = layerIter->frames.cbegin(); keyFrameIter != layerIter->frames.cend(); keyFrameIter++) { // key frame iteration BEGIN
            if (frameIndex >= keyFrameIter->id && frameIndex < (keyFrameIter->id + keyFrameIter->duration)) {
                
                if (keyFrameIter->isMotion == false) { // non-motion key frame
                    for (int i = 0; i < keyFrameIter->elements.size(); i++) { // element iteration BEGIN
                        const Element& e = keyFrameIter->elements.at(i);
                        out.push_back(e);
                        eNum++;
                    } // element iteration END
                }
                else { // motion key frame BEGIN
                    
					// lerp frames only happen that there is only one element in both key frames
					if (keyFrameIter->elements.size() == 1) {
						if (keyFrameIter->duration == 1) {
                            const Element& e = keyFrameIter->elements.at(0);
                            out.push_back(e);
                        }
						else {
							const Element& e_src = keyFrameIter->elements.at(0);
							if ( (keyFrameIter+1) != layerIter->frames.end() &&
								(keyFrameIter+1)->elements.size() == 1 ) {
								const Element e_target = (keyFrameIter+1)->elements.at(0);
								float t = (float)(frameIndex - keyFrameIter->id) / (float)(keyFrameIter->duration);
								out.push_back(e_src.lerp(e_target, t));
							}
							else {
								out.push_back(e_src);
							}
						}
						eNum++;
					}
					else {
						for (int i = 0; i < keyFrameIter->elements.size(); i++) { // element iteration BEGIN
							const Element& e = keyFrameIter->elements.at(i);
							out.push_back(e);
							eNum++;
						} // element iteration END
					}
                    
                } // motion key frame END
                
                break;
            }
        } // key frame iteration END
        
    } // layer iteration END
    
    return eNum;
}

NS_CC_EXT_END
