//
//  CCHierarchiesSpriteAnimation.cpp
//  HierarchiesSpriteDemo
//
//  Created by xuxiaocheng on 10/23/14.
//
//

#include "CCHierarchiesSpriteAnimation.h"

NS_CC_EXT_BEGIN


HierarchiesSpriteAnimation::HierarchiesSpriteAnimation ()
: _frameRate(0)
, _frameCount(0) {
}

HierarchiesSpriteAnimation::~HierarchiesSpriteAnimation () {
}

std::string HierarchiesSpriteAnimation::getVersion () const {
    return _version;
}

unsigned int HierarchiesSpriteAnimation::getFrameRate () const {
    return _frameRate;
}

size_t HierarchiesSpriteAnimation::getAnimationCount () const {
    return _anims.size();
}

size_t HierarchiesSpriteAnimation::getFrameCount() const {
	return _frameCount;
}

const std::vector<HierarchiesSpriteAnimation::Layer>& HierarchiesSpriteAnimation::getLayers () {
    return _layers;
}

const std::vector<HierarchiesSpriteAnimation::Symbol>& HierarchiesSpriteAnimation::getSymbols () {
    return _symbols;
}

const std::vector<HierarchiesSpriteAnimation::Event>& HierarchiesSpriteAnimation::getEvents () {
    return _events;
}

bool HierarchiesSpriteAnimation::getAnimationByName (std::string name, Animation& out) {
    std::unordered_map<std::string, Animation>::iterator iter = _anims.find(name);
    if (iter != _anims.end()) {
        out = iter->second;
        return true;
    }
    return false;
}

std::vector<HierarchiesSpriteAnimation::Animation> HierarchiesSpriteAnimation::getAnimationList () {
	std::vector<Animation> ret;
	std::unordered_map<std::string, Animation>::iterator iter;
	for (iter = _anims.begin(); iter != _anims.end(); iter++) {
		ret.push_back(iter->second);
	}
	return ret;
}

size_t HierarchiesSpriteAnimation::getLayerCount () {
    return _layers.size();
}

bool HierarchiesSpriteAnimation::getLayerByIndex (unsigned int index, Layer& out) {
    if (index >= _layers.size())
        return false;
    
    out = _layers[index];
    return true;
}

bool HierarchiesSpriteAnimation::getKeyFrameIncludeIndexAtLayer (unsigned int layerIndex, unsigned int index, KeyFrame& out) {
    if (layerIndex >= _layers.size())
        return false;
    
    std::vector<KeyFrame>::iterator iter;
    for (iter = _layers[layerIndex].frames.begin(); iter != _layers[layerIndex].frames.end(); iter++) {
        if (index >= iter->index && index < (iter->index + iter->duration)) {
            out = *iter;
            return true;
        }
    }
    
    return false;
}

bool HierarchiesSpriteAnimation::getSymbolByIndex(int symbolIndex, Symbol& out) const {
    if (symbolIndex >= _symbols.size())
        return false;
    
    out = _symbols[symbolIndex];
    return true;
}

size_t HierarchiesSpriteAnimation::getSymbolCount() {
    return _symbols.size();
}

bool HierarchiesSpriteAnimation::getEventByFrameId (unsigned int frameId, Event& out) {
    std::vector<Event>::iterator evtIter;
    for (evtIter = _events.begin(); evtIter != _events.end(); evtIter++) {
        if (evtIter->frameId == frameId) {
            out = *evtIter;
            return true;
        }
    }
    return false;
}

int HierarchiesSpriteAnimation::getFrameElementsAtFrameIndex (ElementLoopMode loopMode, int frameOffset, unsigned int& frameIndex, FrameElements& out) const {
    switch (loopMode) {
        case kNoneLoopMode: { // same as kLoopLoopMode
            frameIndex = (frameIndex + frameOffset) % _frameCount;
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
            if (frameIndex >= keyFrameIter->index && frameIndex < (keyFrameIter->index + keyFrameIter->duration)) {
                
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
								float t = (float)(frameIndex - keyFrameIter->index) / (float)(keyFrameIter->duration);
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
