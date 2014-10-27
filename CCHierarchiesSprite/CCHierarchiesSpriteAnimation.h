//
//  CCHierarchiesSpriteAnimation.h
//  HierarchiesSprite
//
//  Created by xuxiaocheng on 10/23/14.
//
//

#ifndef __HierarchiesSpriteDemo__CCHierarchiesSpriteAnimation__
#define __HierarchiesSpriteDemo__CCHierarchiesSpriteAnimation__

#include "cocos2d.h"
#include "ExtensionMacros.h"
#include <string>
#include <vector>
#include <unordered_map>
#include "CCHierarchiesSpriteMath.h"

NS_CC_EXT_BEGIN


class CC_DLL HierarchiesSpriteAnimation {
    
    friend class HierarchiesSpriteAnimationReader;
    
public:
    struct Animation {
        std::string name;
        unsigned int startFrameIndex;
        unsigned int endFrameIndex;
        
        Animation () {
        }
        
        Animation (std::string name, unsigned int startFrameIndex, unsigned int endFrameIndex)
        : name(name), startFrameIndex(startFrameIndex), endFrameIndex(endFrameIndex) {
        }
        
        Animation (const Animation& copy) {
            this->name = copy.name;
            this->startFrameIndex = copy.startFrameIndex;
            this->endFrameIndex = copy.endFrameIndex;
        }
        
        Animation& operator= (const Animation& rhs) {
            this->name = rhs.name;
            this->startFrameIndex = rhs.startFrameIndex;
            this->endFrameIndex = rhs.endFrameIndex;
            return *this;
        }
    };
    
    enum ElementLoopMode {
        kNoneLoopMode,
        kLoopLoopMode,
        kPlayOnceLoopMode,
        kSingleFrameLoopMode,
    };
    
    struct Element {
        int symbolIndex;
        
        float x;
        float y;
        float anchorX;
        float anchorY;
        float scaleX;
        float scaleY;
        float skewX;
        float skewY;
        
        float color_alpha_percent;
        int color_alpha_amount;
        float color_red_percent;
        int color_red_amount;
        float color_green_percent;
        int color_green_amount;
        float color_blue_percent;
        int color_blue_amount;
        
        ElementLoopMode loopMode;
        int frameOffset;
        
        // element unique identifier
        // ONLY exist in runtime
        int euid;
        // the parent sprite's frame index that sub sprite start display,
        // use this to calc sub sprite's display frame index
        // ONLY exist in runtime
        unsigned int startDisplayFrameIndex;
        
        Element ()
        : color_alpha_percent(1), color_red_percent(1), color_green_percent(1), color_blue_percent(1),
        loopMode(kNoneLoopMode), frameOffset(0), euid(0), startDisplayFrameIndex(0) {
        }
        
        Element (int symbolIndex, float x, float y, float anchorX, float anchorY, float scaleX,
                 float scaleY, /*float rotation, */float skewX, float skewY, float color_alpha_percent,
                 int color_alpha_amount, float color_red_percent, int color_red_amount,
                 float color_green_percent, int color_green_amount, float color_blue_percent,
                 int color_blue_amount, ElementLoopMode loopMode, int frameOffset, int euid, unsigned int startDisplayFrameIndex)
        : symbolIndex(symbolIndex), x(x), y(y), anchorX(anchorX), anchorY(anchorY), scaleX(scaleX),
        scaleY(scaleY), /*rotation(rotation), */skewX(skewX), skewY(skewY),
        color_alpha_percent(color_alpha_percent), color_alpha_amount(color_alpha_amount),
        color_red_percent(color_red_percent), color_red_amount(color_red_amount),
        color_green_percent(color_green_percent), color_green_amount(color_green_amount),
        color_blue_percent(color_blue_percent), color_blue_amount(color_blue_amount),
        loopMode(loopMode), frameOffset(frameOffset), euid(euid), startDisplayFrameIndex(startDisplayFrameIndex) {
        }
        
        Element (const Element& copy) {
            this->symbolIndex = copy.symbolIndex;
            this->x = copy.x;
            this->y = copy.y;
            this->anchorX = copy.anchorX;
            this->anchorY = copy.anchorY;
            this->scaleX = copy.scaleX;
            this->scaleY = copy.scaleY;
            this->skewX = copy.skewX;
            this->skewY = copy.skewY;
            this->color_alpha_percent = copy.color_alpha_percent;
            this->color_alpha_amount = copy.color_alpha_amount;
            this->color_red_percent = copy.color_red_percent;
            this->color_red_amount = copy.color_red_amount;
            this->color_green_percent = copy.color_green_percent;
            this->color_green_amount = copy.color_green_amount;
            this->color_blue_percent = copy.color_blue_percent;
            this->color_blue_amount = copy.color_blue_amount;
            this->loopMode = copy.loopMode;
            this->frameOffset = copy.frameOffset;
            this->euid = copy.euid;
            this->startDisplayFrameIndex = copy.startDisplayFrameIndex;
        }
        
        Element& operator= (const Element& rhs) {
            this->symbolIndex = rhs.symbolIndex;
            this->x = rhs.x;
            this->y = rhs.y;
            this->anchorX = rhs.anchorX;
            this->anchorY = rhs.anchorY;
            this->scaleX = rhs.scaleX;
            this->scaleY = rhs.scaleY;
            this->skewX = rhs.skewX;
            this->skewY = rhs.skewY;
            this->color_alpha_percent = rhs.color_alpha_percent;
            this->color_alpha_amount = rhs.color_alpha_amount;
            this->color_red_percent = rhs.color_red_percent;
            this->color_red_amount = rhs.color_red_amount;
            this->color_green_percent = rhs.color_green_percent;
            this->color_green_amount = rhs.color_green_amount;
            this->color_blue_percent = rhs.color_blue_percent;
            this->color_blue_amount = rhs.color_blue_amount;
            this->loopMode = rhs.loopMode;
            this->frameOffset = rhs.frameOffset;
            this->euid = rhs.euid;
            this->startDisplayFrameIndex = rhs.startDisplayFrameIndex;
            return *this;
        }
        
        Element lerp (const Element& target, float t) const {
            Element ret;
            
            ret.symbolIndex = this->symbolIndex;
            
            ret.x = lerpf(this->x, target.x, t);
            ret.y = lerpf(this->y, target.y, t);
            ret.anchorX = this->anchorX;
            ret.anchorY = this->anchorY;
            ret.scaleX = lerpf(this->scaleX, target.scaleX, t);
            ret.scaleY = lerpf(this->scaleY, target.scaleY, t);
            ret.skewX = Spinor::radianSlerp(this->skewX * (M_PI / 180.0f), target.skewX * (M_PI / 180.0f), t) * (180.0f / M_PI);
            ret.skewY = Spinor::radianSlerp(this->skewY * (M_PI / 180.0f), target.skewY * (M_PI / 180.0f), t) * (180.0f / M_PI);
            
            ret.color_alpha_percent = lerpf(this->color_alpha_percent, target.color_alpha_percent, t);
            ret.color_alpha_amount = lerpi(this->color_alpha_amount, target.color_alpha_amount, t);
            ret.color_red_percent = lerpf(this->color_red_percent, target.color_red_percent, t);
            ret.color_red_amount = lerpi(this->color_red_amount, target.color_red_amount, t);
            ret.color_green_percent = lerpf(this->color_green_percent, target.color_green_percent, t);
            ret.color_green_amount = lerpi(this->color_green_amount, target.color_green_amount, t);
            ret.color_blue_percent = lerpf(this->color_blue_percent, target.color_blue_percent, t);
            ret.color_blue_amount = lerpi(this->color_blue_amount, target.color_blue_amount, t);
            
            ret.loopMode = this->loopMode;
            ret.frameOffset = this->frameOffset;
            ret.euid = this->euid;
            ret.startDisplayFrameIndex = this->startDisplayFrameIndex;
            
            return ret;
        }
    };
    
    struct KeyFrame {
        unsigned int index;
        unsigned int duration;
        std::vector<Element> elements;
        bool isMotion;
        
        KeyFrame () {
        }
        
        KeyFrame (unsigned int index, unsigned int duration, bool isMotion)
        : index(index), duration(duration), isMotion(isMotion) {
        }
        
        KeyFrame (const KeyFrame& copy) {
            this->index = copy.index;
            this->duration = copy.duration;
            this->elements = copy.elements;
            this->isMotion = copy.isMotion;
        }
        
        KeyFrame& operator= (const KeyFrame& rhs) {
            this->index = rhs.index;
            this->duration = rhs.duration;
            this->elements = rhs.elements;
            this->isMotion = rhs.isMotion;
            return *this;
        }
        
        bool findElementBySymbolIndex (int symbolIndex, Element& out) {
            std::vector<Element>::iterator iter;
            for (iter = elements.begin(); iter != elements.end(); iter++) {
                if (iter->symbolIndex == symbolIndex) {
                    out = *iter;
                    return true;
                }
            }
            return false;
        }
    };
    
    struct Layer {
        std::vector<KeyFrame> frames;
        
        Layer () {}
        
        Layer (const Layer& copy) {
            this->frames = copy.frames;
        }
        
        Layer& operator= (const Layer& rhs) {
            this->frames = rhs.frames;
            return *this;
        }
    };
    
    struct Symbol {
        std::string name;
        bool isSocket;
        float left;
        float bottom;
        
        Symbol () {}
        
        Symbol (std::string name, bool isSocket, float left, float bottom)
        : name(name), isSocket(isSocket), left(left), bottom(bottom) {
        }
        
        Symbol (const Symbol& copy) {
            this->name = copy.name;
            this->isSocket = copy.isSocket;
            this->left = copy.left;
            this->bottom = copy.bottom;
        }
        
        Symbol& operator= (const Symbol& rhs) {
            this->name = rhs.name;
            this->isSocket = rhs.isSocket;
            this->left = rhs.left;
            this->bottom = rhs.bottom;
            return *this;
        }
    };
    
    struct Event {
        unsigned int frameId;
        std::string name;
        
        Event () {
        }
        
        Event (unsigned int frameId, std::string content)
        : frameId(frameId), name(name) {
        }
        
        Event (const Event& copy) {
            this->frameId = copy.frameId;
            this->name = copy.name;
        }
        
        Event& operator= (const Event& rhs) {
            this->frameId = rhs.frameId;
            this->name = rhs.name;
            return *this;
        }
    };
    
    typedef std::vector< Element > FrameElements;
    
protected:
    std::string _version;
    unsigned int _frameRate;
    unsigned int _frameCount;
    std::unordered_map<std::string, Animation> _anims;
    std::vector<Layer> _layers;
    std::vector<Symbol> _symbols;
    std::vector<Event> _events;
    
public:
    HierarchiesSpriteAnimation ();
    virtual ~HierarchiesSpriteAnimation();
    
    std::string getVersion () const;
    unsigned int getFrameRate () const;
    size_t getAnimationCount () const;
    size_t getFrameCount () const;
    const std::vector<Layer>& getLayers ();
    const std::vector<Symbol>& getSymbols ();
    const std::vector<Event>& getEvents ();
    bool getAnimationByName (std::string name, Animation& out);
    std::vector<Animation> getAnimationList ();
    size_t getLayerCount ();
    bool getLayerByIndex (unsigned int index, Layer& out);
    bool getKeyFrameIncludeIndexAtLayer (unsigned int layerIndex, unsigned int index, KeyFrame& out);
    bool getSymbolByIndex (int symbolIndex, Symbol& out) const;
    size_t getSymbolCount ();
    bool getEventByFrameId (unsigned int frameId, Event& out);
    int getFrameElementsAtFrameIndex (ElementLoopMode loopMode, int frameOffset, unsigned int& frameIndex, FrameElements& out) const;
    
};


NS_CC_EXT_END

#endif /* defined(__HierarchiesSpriteDemo__CCHierarchiesSpriteAnimation__) */
