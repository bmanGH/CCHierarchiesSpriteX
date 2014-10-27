//
//  CCHierarchiesSpriteMath.h
//  HierarchiesSpriteDemo
//
//  Created by xuxiaocheng on 10/23/14.
//
//

#ifndef __HierarchiesSpriteDemo__CCHierarchiesSpriteMath__
#define __HierarchiesSpriteDemo__CCHierarchiesSpriteMath__

#include "cocos2d.h"
#include "ExtensionMacros.h"
#include <math.h>
#include <stdlib.h>

NS_CC_EXT_BEGIN


// 2D Quaternion
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

// 2D Matrix
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
    
    FMatrix2D (int a, int b, int c, int d, int tx, int ty)
    : a(a), b(b), c(c), d(d), tx(tx), ty(ty) {
    }
    
    // property
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
    
    void setTransformX (float transformX) {
        tx = transformX;
    }
    
    void setTransformY (float transformY) {
        ty = transformY;
    }
    
    void setAnchorX (float anchorX) {
        tx -= anchorX;
    }
    
    void setAnchorY (float anchorY) {
        ty -= anchorY;
    }
    
    // transform
    void translate (float tx, float ty) {
        tx = tx + a * tx + c * ty;
        ty = ty + b * tx + d * ty;
    }
    
    void scale (float sx, float sy) {
        a = a * sx;
        b = b * sx;
        c = c * sy;
        d = d * sy;
    }
    
    void rotate (float angle) {
        float sine = sinf(angle);
        float cosine = cosf(angle);
        
        a = a * cosine + c * sine;
        b = b * cosine + d * sine;
        c = c * cosine + a * sine;
        d = d * cosine - b * sine;
    }
    
    FMatrix2D invert () {
        float determinant = 1.0f / (a * d - b * c);
        
        FMatrix2D ret;
        ret.a = d * determinant;
        ret.b = -b * determinant;
        ret.c = -c * determinant;
        ret.d = a * determinant;
        ret.tx = (c * ty - d * tx) * determinant;
        ret.ty = -(a * ty - b * tx) * determinant;
        return ret;
    }
    
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
    
    // misc
    Point apply (Point& p) {
        Point ret;
        ret.x = (float)(a * p.x + c * p.y + tx);
        ret.y = (float)(b * p.x + d * p.y + ty);
        return ret;
    }
};


// scalar lerp
static inline float lerpf (float src, float target, float t) {
    return (src + t * (target - src));
}

static inline int lerpi (int src, int target, float t) {
    return (src + t * (target - src));
}


NS_CC_EXT_END

#endif /* defined(__HierarchiesSpriteDemo__CCHierarchiesSpriteMath__) */
