//
//  CCHierarchiesSpriteAnimation.h
//  CCHierarchiesSprite
//
//  Created by bman <zx123xz321hm3@hotmail.com>.
//  Copyright (c) 2013. All rights reserved.
//

#ifndef _CCHierarchiesSpriteAnimation_H_
#define _CCHierarchiesSpriteAnimation_H_

#include "cocos2d.h"
#include "ExtensionMacros.h"
#include "CCHierarchiesSpriteConfig.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <math.h>
#include <stdlib.h>
#include "rapidxml.hpp"


NS_CC_EXT_BEGIN

// Quaternion 2D
struct CC_DLL Spinor {
    float real;
    float complex;
    
    Spinor () {}
    
    Spinor (float real, float complex)
    : real(real), complex(complex) {
    }
    
    Spinor (const Spinor& copy) {
        this->real = copy.real;
        this->complex = copy.complex;
    }
    
    Spinor& operator= (const Spinor& rhs) {
        this->real = rhs.real;
        this->complex = rhs.complex;
        return *this;
    }
    
    Spinor (float radian) {
        real = cosf(radian);
        complex = sinf(radian);
    }
    
    Spinor operator* (const float rhs) {
        return Spinor(real * rhs, complex * rhs);
    }
    
    Spinor operator* (const Spinor& rhs) {
        return Spinor(real * rhs.real - complex * rhs.complex, real * rhs.complex + complex * rhs.real);
    }
    
    Spinor operator+ (const Spinor& rhs) {
        return Spinor(real + rhs.real, complex + rhs.complex);
    }
    
    float length () {
        return sqrtf(real * real + complex * complex);
    }
    
    float lengthSquared () {
        return (real * real + complex * complex);
    }
    
    Spinor invert () {
        Spinor s(real, -complex);
        return Spinor(s * s.lengthSquared());
    }
    
    Spinor normalized () {
        float l = length();
        return Spinor(real / l, complex / l);
    }
    
    float getRadian () {
        return atan2f(complex, real) * 2;
    }
    
    Spinor lerp (Spinor& target, float t) {
        Spinor s = (((*this) * (1 - t)) + (target * t)).normalized();
        return s;
    }
    
    Spinor slerp (Spinor& target, float t) {
        float tr;
        float tc;
        float omega, cosom, sinom, scale0, scale1;
        
        // calc cosine
        cosom = this->real * target.real + this->complex * target.complex;
        
        // adjust signs
        if (cosom < 0) {
            cosom = -cosom;
            tc = -target.complex;
            tr = -target.real;
        }
        else {
            tc = target.complex;
            tr = target.real;
        }
        
        // coefficients
        if ((1 - cosom) > 0.001f) { // threshold, use linear interp if too close
            omega = acosf(cosom);
            sinom = sinf(omega);
            scale0 = sinf((1 - t) * omega) / sinom;
            scale1 = sinf(t * omega) / sinom;
        }
        else {
            scale0 = 1 - t;
            scale1 = t;
        }
        
        // calc final
        Spinor s;
        s.complex = scale0 * this->complex + scale1 * tc;
        s.real = scale0 * this->real + scale1 * tr;
        
        return s;
    }
    
    static float radianSlerp (float from, float to, float t) {
        Spinor fromS(cosf(from / 2), sinf(from / 2));
        Spinor toS(cosf(to / 2), sinf(to / 2));
        return fromS.slerp(toS, t).getRadian();
    }
    
};
    
// Flash 2D matrix manipulations
struct CC_DLL FMatrix2D {
    float a;
    float b;
    float c;
    float d;
    float tx;
    float ty;
    
    void setIdentity () {
        a = 1;
        b = 0;
        c = 0;
        d = 1;
        tx = 0;
        ty = 0;
    }
    
    FMatrix2D () {
        setIdentity();
    }
    
    float getScaleX () {
        return sqrtf(a*a + b*b);
    }
    
    float getScaleY () {
        return sqrtf(c*c + d*d);
    }
    
    float getSkewXRadians ()
    {
        return atan2f(-c, d);
    }
    
    float getSkewYRadians ()
    {
        return atan2f(b, a);
    }
    
    float getSkewX () {
        return atan2f(-c, d) * (180.0f/M_PI);
    }
    
    float getSkewY () {
        return atan2f(b, a) * (180.0f/M_PI);
    }
    
    float getTransformX () {
        return tx;
    }
    
    float getTransformY () {
        return ty;
    }
    
    void setScaleX (float scaleX) {
        float oldValue = getScaleX();
        // avoid division by zero
        if (oldValue)
        {
            float ratio = scaleX / oldValue;
            a *= ratio;
            b *= ratio;
        }
        else
        {
            float skewYRad = getSkewYRadians();
            a = cosf(skewYRad) * scaleX;
            b = sinf(skewYRad) * scaleX;
        }
    }
    
    void setScaleY (float scaleY) {
        float oldValue = getScaleY();
        // avoid division by zero
        if (oldValue)
        {
            float ratio = scaleY / oldValue;
            c *= ratio;
            d *= ratio;
        }
        else
        {
            float skewXRad = getSkewXRadians();
            c = -sinf(skewXRad) * scaleY;
            d =  cosf(skewXRad) * scaleY;
        }
    }
    
    void setSkewXRadians (float skewX) {
        float scaleY = getScaleY();
        c = -scaleY * sinf(skewX);
        d =  scaleY * cosf(skewX);
    }
    
    void setSkewYRadians (float skewY) {
        float scaleX = getScaleX();
        a = scaleX * cosf(skewY);
        b = scaleX * sinf(skewY);
    }
    
    void setSkewX (float skewX) {
        setSkewXRadians(skewX*(M_PI/180.0f));
    }
    
    void setSkewY (float skewY) {
        setSkewYRadians(skewY*(M_PI/180.0f));
    }
    
    void setAnchorX (float anchorX) {
        tx = -anchorX;
//            tx -= anchorX;
    }
    
    void setAnchorY (float anchorY) {
        ty = -anchorY;
//            ty -= anchorY;
    }
    
    void setTransformX (float transformX) {
        tx = transformX;
    }
    
    void setTransformY (float transformY) {
        ty = transformY;
    }
    
    //    FMatrix2D invert () {
    //        FMatrix2D ret;
    //        ret.a = d/(a*d-b*c);
    //        ret.b = -b/(a*d-b*c);
    //        ret.c = -c/(a*d-b*c);
    //        ret.d = a/(a*d-b*c);
    //        ret.tx = (c*ty-d*tx)/(a*d-b*c);
    //        ret.ty = -(a*ty-b*tx)/(a*d-b*c);
    //        return ret;
    //    }
    
    //    void translate (float translateX, float translateY) {
    //        tx = tx + a * translateX + c * translateY;
    //        ty = ty + b * translateX + d * translateY;
    //    }
    
    FMatrix2D concat (FMatrix2D& m) {
        FMatrix2D ret;
        ret.a = a * m.a + b * m.c;
        ret.b = a * m.b + b * m.d;
        ret.c = c * m.a + d * m.c;
        ret.d = c * m.b + d * m.d;
        ret.tx = tx * m.a + ty * m.c + m.tx;
        ret.ty = tx * m.b + ty * m.d + m.ty;
        return ret;
    }
    
};

class CC_DLL CCHierarchiesSpriteAnimation {
    
public:
    static inline float lerpf (float src, float target, float t) {
        return (src + t * (target - src));
    }
    
    static inline int lerpi (int src, int target, float t) {
        return (src + t * (target - src));
    }
    
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
    
    // filter name enum
    typedef enum _FilterName {
        AdjustColorFilter,
        BevelFilter,
        BlurFilter,
        DropShadowFilter,
        GlowFilter,
        GradientBevelFilter,
        GradientGlowFilter,
    } FilterName;
    
    // filter color
    typedef struct _FilterColor4F {
        float r;
        float g;
        float b;
        float a;
        
        void setByString (const char* colorString) {
            size_t len = strlen(colorString);
            if (len == 7) { // case "#RRGGBB"
                long value = strtol(&colorString[1], NULL, 16);
                r = (((value & 0xFF0000) >> 16) / 255.0f);
                g = (((value & 0xFF00) >> 8) / 255.0f);
                b = ((value & 0xFF) / 255.0f);
                a = 1;
            }
            else if (len == 9) { // case "#RRGGBBAA"
                long value = strtol(&colorString[1], NULL, 16);
                r = (((value & 0xFF000000) >> 24) / 255.0f);
                g = (((value & 0xFF0000) >> 16) / 255.0f);
                b = (((value & 0xFF00) >> 8) / 255.0f);
                a = ((value & 0xFF) / 255.0f);
            }
            else if (len == 1 && (strcmp(colorString, "0") == 0)) { // case "0"
                r = 0;
                g = 0;
                b = 0;
                a = 0;
            }
            else {
                assert(false);
            }
        }
    } FilterColor4F;
    
    // filter quality
    typedef enum _FilterQuality {
        LowQuality,
        MediumQuality,
        HighQuality,
    } FilterQuality;
    
    // filter type
    typedef enum _FilterType {
        InnerType,
        OuterType,
        FullType,
    } FilterType;
    
    struct Filter {
        // filter data struct
        struct AdjustColor {
            float brightness; // -100~100
            float contrast; // -100~100
            float saturation; // -100~100
            float hue; // -180~180
        };
        struct Bevel {
            float blurX; // 0~255
            float blurY; // 0~255
            int strength; // 0~25500
            FilterQuality quality;
            FilterColor4F shadowColor;
            FilterColor4F highlightColor;
            float angle; // 0~360
            float distance; // -255~255
            bool knockout;
            FilterType type;
        };
        struct Blur {
            float blurX; // 0~255
            float blurY; // 0~255
            FilterQuality quality;
        };
        struct DropShadow {
            float blurX; // 0~255
            float blurY; // 0~255
            int strength; // 0~25500
            FilterQuality quality;
            float angle; // 0~360
            float distance; // -255~255
            bool knockout;
            bool inner;
            bool hideObject;
            FilterColor4F color;
        };
        struct Glow {
            float blurX; // 0~255
            float blurY; // 0~255
            int strength; // 0~25500
            FilterQuality quality;
            FilterColor4F color;
            bool knockout;
            bool inner;
        };
        struct GradientBevel {
            float blurX; // 0~255
            float blurY; // 0~255
            int strength; // 0~25500
            FilterQuality quality;
            float angle; // 0~360
            float distance; // -255~255
            bool knockout;
            FilterType type;
            // ?? MISSING gradient
        };
        struct GradientGlow {
            float blurX; // 0~255
            float blurY; // 0~255
            int strength; // 0~25500
            FilterQuality quality;
            float angle; // 0~360
            float distance; // -255~255
            bool knockout;
            FilterType type;
            // ?? MISSING gradient
        };
        
        FilterName name;
        union {
            AdjustColor adjustColor;
            Bevel bevel;
            Blur blur;
            DropShadow dropShadow;
            Glow glow;
            GradientBevel gradientBevel;
            GradientGlow gradientGlow;
        };
        
//            float angle; // 0~360
//            float blurX; // 0~255
//            float blurY; // 0~255
//            float brightness; // -100~100
//            FilterColor4F color;
//            float contrast; // -100~100
//            float distance; // -255~255
//            bool hideObject;
//            FilterColor4F highlightColor;
//            float hue; // -180~180
//            bool inner;
//            bool knockout;
//            FilterQuality quality;
//            float saturation; // -100~100
//            FilterColor4F shadowColor;
//            int strength; // 0~25500
//            FilterType type;
        
        Filter () {}
        
        Filter (FilterName name, float brightness, float contrast, float saturation, float hue)
        : name(name) {
            adjustColor.brightness = brightness;
            adjustColor.contrast = contrast;
            adjustColor.saturation = saturation;
            adjustColor.hue = hue;
        }
        
        Filter (FilterName name, float blurX, float blurY, int strength, FilterQuality quality, FilterColor4F shadowColor,
                FilterColor4F highlightColor, float angle, float distance, bool knockout, FilterType type)
        : name(name) {
            bevel.blurX = blurX;
            bevel.blurY = blurY;
            bevel.strength = strength;
            bevel.quality = quality;
            bevel.shadowColor = shadowColor;
            bevel.highlightColor = highlightColor;
            bevel.angle = angle;
            bevel.distance = distance;
            bevel.knockout = knockout;
            bevel.type = type;
        }
        
        Filter (FilterName name, float blurX, float blurY, FilterQuality quality)
        : name(name) {
            blur.blurX = blurX;
            blur.blurY = blurY;
            blur.quality = quality;
        }
        
        Filter (FilterName name, float blurX, float blurY, int strength, FilterQuality quality, float angle,
                float distance, bool knockout, bool inner, bool hideObject, FilterColor4F color)
        : name(name) {
            dropShadow.blurX = blurX;
            dropShadow.blurY = blurY;
            dropShadow.strength = strength;
            dropShadow.quality = quality;
            dropShadow.angle = angle;
            dropShadow.distance = distance;
            dropShadow.knockout = knockout;
            dropShadow.inner = inner;
            dropShadow.hideObject = hideObject;
            dropShadow.color = color;
        }
        
        Filter (FilterName name, float blurX, float blurY, int strength, FilterQuality quality,
                FilterColor4F color, bool knockout, bool inner)
        : name(name) {
            glow.blurX = blurX;
            glow.blurY = blurY;
            glow.strength = strength;
            glow.quality = quality;
            glow.color = color;
            glow.knockout = knockout;
            glow.inner = inner;
        }
        
        // GradientBevelFilter and GradientGlowFilter are same params
        Filter (FilterName name, float blurX, float blurY, int strength, FilterQuality quality,
                float angle, float distance, bool knockout, FilterType type)
        : name(name) {
            if (name == GradientBevelFilter) {
                gradientBevel.blurX = blurX;
                gradientBevel.blurY = blurY;
                gradientBevel.strength = strength;
                gradientBevel.quality = quality;
                gradientBevel.angle = angle;
                gradientBevel.distance = distance;
                gradientBevel.knockout = knockout;
                gradientBevel.type = type;
            }
            else {
                gradientGlow.blurX = blurX;
                gradientGlow.blurY = blurY;
                gradientGlow.strength = strength;
                gradientGlow.quality = quality;
                gradientGlow.angle = angle;
                gradientGlow.distance = distance;
                gradientGlow.knockout = knockout;
                gradientGlow.type = type;
            }
        }
        
        Filter (const Filter& copy) {
            this->name = copy.name;
            switch (this->name) {
                case AdjustColorFilter: {
                    this->adjustColor = copy.adjustColor;
                    break;
                }
                case BevelFilter: {
                    this->bevel = copy.bevel;
                    break;
                }
                case BlurFilter: {
                    this->blur = copy.blur;
                    break;
                }
                case DropShadowFilter: {
                    this->dropShadow = copy.dropShadow;
                    break;
                }
                case GlowFilter: {
                    this->glow = copy.glow;
                    break;
                }
                case GradientBevelFilter: {
                    this->gradientBevel = copy.gradientBevel;
                    break;
                }
                case GradientGlowFilter: {
                    this->gradientGlow = copy.gradientGlow;
                    break;
                }
            }
        }
        
        Filter& operator= (const Filter& rhs) {
            this->name = rhs.name;
            switch (this->name) {
                case AdjustColorFilter: {
                    this->adjustColor = rhs.adjustColor;
                    break;
                }
                case BevelFilter: {
                    this->bevel = rhs.bevel;
                    break;
                }
                case BlurFilter: {
                    this->blur = rhs.blur;
                    break;
                }
                case DropShadowFilter: {
                    this->dropShadow = rhs.dropShadow;
                    break;
                }
                case GlowFilter: {
                    this->glow = rhs.glow;
                    break;
                }
                case GradientBevelFilter: {
                    this->gradientBevel = rhs.gradientBevel;
                    break;
                }
                case GradientGlowFilter: {
                    this->gradientGlow = rhs.gradientGlow;
                    break;
                }
            }
            return *this;
        }
        
        Filter lerp (const Filter& target, float t) const {
            assert(this->name == target.name);
            
            //TODO: implement filter lerp
            Filter ret;
            ret.name = this->name;
            switch (this->name) {
                case AdjustColorFilter: {
                    
                    break;
                }
                case BevelFilter: {
                    
                    break;
                }
                case BlurFilter: {
                    
                    break;
                }
                case DropShadowFilter: {
                    
                    break;
                }
                case GlowFilter: {
                    
                    break;
                }
                case GradientBevelFilter: {
                    
                    break;
                }
                case GradientGlowFilter: {
                    
                    break;
                }
            }
            return ret;
        }
    };
        
    typedef enum _ElementLoopMode {
        kNoneLoopMode,
        kLoopLoopMode,
        kPlayOnceLoopMode,
        kSingleFrameLoopMode,
    } ElementLoopMode;
    
    struct Element {
        int symbolIndex;
        float x;
        float y;
        float anchorX;
        float anchorY;
        float scaleX;
        float scaleY;
//            float rotation;
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
        std::vector<Filter> filters;
        // element unique identifier
        // !!! only exist in runtime
        int euid;
        // the parent sprite's frame index that sub sprite start display,
        // use this to calc sub sprite's display frame index
        // !!! only exist in runtime
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
//                this->rotation = copy.rotation;
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
            this->filters = copy.filters;
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
//                this->rotation = rhs.rotation;
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
            this->filters = rhs.filters;
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
//                ret.rotation = Spinor::radianSlerp(this->rotation * (M_PI / 180.0f), target.rotation * (M_PI / 180.0f), t) * (180.0f / M_PI);
//            //HACK: since the rotation direction is different between rotation and skew
//            if ( ret.rotation != ret.rotation ) {
            ret.skewX = Spinor::radianSlerp(this->skewX * (M_PI / 180.0f), target.skewX * (M_PI / 180.0f), t) * (180.0f / M_PI);
            ret.skewY = Spinor::radianSlerp(this->skewY * (M_PI / 180.0f), target.skewY * (M_PI / 180.0f), t) * (180.0f / M_PI);
//            }
//            else {
//                ret.skewX = lerpf(this->skewX, target.skewX, t);
//                ret.skewY = lerpf(this->skewY, target.skewY, t);
//            }
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
            
#if HIERARCHIES_ENABLE_FILTER == 1
            // filters lerp
            // !!! order and name in both filters must same
            if (this->filters.size() > 0) {
                assert(this->filters.size() == target.filters.size());
                
                std::vector<Filter> retFilters;
                for (int filterIndex = 0; filterIndex < this->filters.size(); filterIndex++) {
                    Filter retFilter = this->filters[filterIndex].lerp(target.filters[filterIndex], t);
                    retFilters.push_back(retFilter);
                }
                retFilters.shrink_to_fit();
                ret.filters = retFilters;
            }
#endif
            
            return ret;
        }
        
//        bool isRotationTransform () {
//            return !(rotation != rotation); // if rotation is NaN means this element use skew transform
//        }
    };
    
    struct KeyFrame {
        unsigned int id;
        unsigned int duration;
        std::vector<Element> elements;
        bool isMotion;
        
        KeyFrame () {
        }
        
        KeyFrame (unsigned int id, unsigned int duration, bool isMotion)
        : id(id), duration(duration), isMotion(isMotion) {
        }
        
        KeyFrame (const KeyFrame& copy) {
            this->id = copy.id;
            this->duration = copy.duration;
            this->elements = copy.elements;
            this->isMotion = copy.isMotion;
        }
        
        KeyFrame& operator= (const KeyFrame& rhs) {
            this->id = rhs.id;
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
        std::string content;
        
        Event () {
        }
        
        Event (unsigned int frameId, std::string content)
        : frameId(frameId), content(content) {
        }
        
        Event (const Event& copy) {
            this->frameId = copy.frameId;
            this->content = copy.content;
        }
        
        Event& operator= (const Event& rhs) {
            this->frameId = rhs.frameId;
            this->content = rhs.content;
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
    int _euidCount;
    
protected:
    void parseItems (rapidxml::xml_node<>* itemsNode);
    void parseSymbols (rapidxml::xml_node<>* symbolsNode);
    void parseAnimations (rapidxml::xml_node<>* animationsNode);
    void parseEvents (rapidxml::xml_node<>* eventsNode);
    void parseLayers (rapidxml::xml_node<>* layersNode);
    void parseKeyFrames (Layer& layer, rapidxml::xml_node<>* layerNode);
    void parseElements (KeyFrame& frame, rapidxml::xml_node<>* keyFrameNode);
    void parseFilters (Element& element, rapidxml::xml_node<>* elementNode);
    
public:
    CCHierarchiesSpriteAnimation (std::string xmlFile);
    virtual ~CCHierarchiesSpriteAnimation();
    
    std::string getVersion () const;
    unsigned int getFrameRate () const;
    size_t getAnimationCount () const;
    size_t getFrameCount () const;
    int getEUIDCount ();
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
    
#endif
