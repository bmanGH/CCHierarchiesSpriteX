//
//  CCHierarchiesSprite.cpp
//  HierarchiesSpriteDemo
//
//  Created by bman on 10/26/14.
//
//

#include "CCHierarchiesSprite.h"

NS_CC_EXT_BEGIN


// void hierarchiesCalcMatrixSocket (CCHierarchiesSpriteAnimation::Element* element, CCAffineTransform* matrix) {
//    FMatrix2D m0;
//    FMatrix2D m1;
//
//    m0.setAnchorX(element->anchorX);
//    m0.setAnchorY(element->anchorY);
//
//    m1.setScaleX(element->scaleX);
//    m1.setScaleY(element->scaleY);
//    m1.setSkewX(element->skewX);
//    m1.setSkewY(element->skewY);
//    m1.setTransformX(element->x);
//    m1.setTransformY(element->y);
//
//    FMatrix2D m = m0.concat(m1);
//
//    *matrix = CCAffineTransformMake(m.a, -m.b, -m.c, m.d, m.tx, -m.ty);
//}

void hierarchiesCalcMatrix (const CCHierarchiesSpriteAnimation::Symbol* symbol,
                            const CCHierarchiesSpriteSheet::Spr* spr,
                            const CCHierarchiesSpriteAnimation::Element* element,
                            CCAffineTransform* matrix) {
    FMatrix2D m0;
    FMatrix2D m1;
    
    m0.setAnchorX(element->anchorX - symbol->left);
    m0.setAnchorY(element->anchorY - symbol->bottom);
    
    m1.setScaleX(element->scaleX);
    m1.setScaleY(element->scaleY);
    m1.setSkewX(element->skewX);
    m1.setSkewY(element->skewY);
    m1.setTransformX(element->x);
    m1.setTransformY(element->y);
    
    FMatrix2D m = m0.concat(m1);
    
    *matrix = CCAffineTransformMake(m.a, -m.b, -m.c, m.d, m.tx, -m.ty);
}

void hierarchiesCalcMatrixSocket (const CCHierarchiesSpriteAnimation::Element* element, CCAffineTransform* matrix) {
    FMatrix2D m0;
    FMatrix2D m1;
    
    m0.setAnchorX(element->anchorX);
    m0.setAnchorY(element->anchorY);
    
    m1.setScaleX(element->scaleX);
    m1.setScaleY(element->scaleY);
    m1.setSkewX(element->skewX);
    m1.setSkewY(element->skewY);
    m1.setTransformX(element->x);
    m1.setTransformY(element->y);
    
    FMatrix2D m = m0.concat(m1);
    
    *matrix = CCAffineTransformMake(m.a, -m.b, -m.c, m.d, m.tx, -m.ty);
}

void hierarchiesUpdateQuadVertices (CCSize size, const CCAffineTransform* matrix, CCHierarchiesSprite_V3F_C4B_T2F_Quad* quad, const float z, const bool isRotation) {
    if (isRotation) {
        float tmp = size.width;
        size.width = size.height;
        size.height = tmp;
    }
    
    float x1 = 0;
    float y1 = 0;
    
    float x2 = x1 + size.width;
    float y2 = y1 + size.height;
    float x = matrix->tx;
    float y = matrix->ty;
    
    float cr = matrix->a;
    float sr = matrix->b;
    float cr2 = matrix->d;
    float sr2 = -matrix->c;
    float ax = x1 * cr - y1 * sr2 + x;
    float ay = x1 * sr + y1 * cr2 + y;
    
    float bx = x2 * cr - y1 * sr2 + x;
    float by = x2 * sr + y1 * cr2 + y;
    
    float cx = x2 * cr - y2 * sr2 + x;
    float cy = x2 * sr + y2 * cr2 + y;
    
    float dx = x1 * cr - y2 * sr2 + x;
    float dy = x1 * sr + y2 * cr2 + y;
    
	quad->bl.vertices = (ccVertex3F) { ax, ay, z };
    quad->br.vertices = (ccVertex3F) { bx, by, z };
    quad->tl.vertices = (ccVertex3F) { dx, dy, z };
    quad->tr.vertices = (ccVertex3F) { cx, cy, z };
}

void hierarchiesUpdateQuadTextureCoords (const CCRect rect, const float texWidth, const float texHeight, CCHierarchiesSprite_V3F_C4B_T2F_Quad* quad, const bool isRotation) {
    float left, right, top, bottom;
    
#if CC_FIX_ARTIFACTS_BY_STRECHING_TEXEL
    left	= (2 * rect.origin.x + 1) / (2 * texWidth);
    right	= left + (rect.size.width * 2 - 2) / (2 * texWidth);
    top		= (2 * rect.origin.y + 1) / (2 * texHeight);
    bottom	= top + (rect.size.height * 2 - 2) / (2 * texHeight);
#else
    left	= rect.origin.x / texWidth;
    right	= left + rect.size.width / texWidth;
    top		= rect.origin.y / texHeight;
    bottom	= top + rect.size.height / texHeight;
#endif
    
    if (!isRotation) {
        quad->bl.texCoords.u = left;
        quad->bl.texCoords.v = bottom;
        quad->br.texCoords.u = right;
        quad->br.texCoords.v = bottom;
        quad->tl.texCoords.u = left;
        quad->tl.texCoords.v = top;
        quad->tr.texCoords.u = right;
        quad->tr.texCoords.v = top;
    }
    else {
        quad->bl.texCoords.u = left; // tl
        quad->bl.texCoords.v = top; // tl
        quad->br.texCoords.u = left; // bl
        quad->br.texCoords.v = bottom; // bl
        quad->tl.texCoords.u = right; // tr
        quad->tl.texCoords.v = top; // tr
        quad->tr.texCoords.u = right; // br
        quad->tr.texCoords.v = bottom; // br
    }
}

void hierarchiesUpdateQuadTextureColorFromAnimation (const float alpha_percent, const int alpha_amount,
                                                     const float red_percent, const int red_amount,
                                                     const float green_percent, const int green_amount,
                                                     const float blue_percent, const int blue_amount,
                                                     CCHierarchiesSprite_V3F_C4B_T2F_Quad* quad) {
    int value;
    
    value = 255 * alpha_percent;
    quad->bl.colorsMul.a = value;
    quad->br.colorsMul.a = value;
    quad->tl.colorsMul.a = value;
    quad->tr.colorsMul.a = value;
    quad->bl.colorsAdd.a = alpha_amount;
    quad->br.colorsAdd.a = alpha_amount;
    quad->tl.colorsAdd.a = alpha_amount;
    quad->tr.colorsAdd.a = alpha_amount;
    
    value = 255 * red_percent;
    quad->bl.colorsMul.r = value;
    quad->br.colorsMul.r = value;
    quad->tl.colorsMul.r = value;
    quad->tr.colorsMul.r = value;
    quad->bl.colorsAdd.r = red_amount;
    quad->br.colorsAdd.r = red_amount;
    quad->tl.colorsAdd.r = red_amount;
    quad->tr.colorsAdd.r = red_amount;
    
    value = 255 * green_percent;
    quad->bl.colorsMul.g = value;
    quad->br.colorsMul.g = value;
    quad->tl.colorsMul.g = value;
    quad->tr.colorsMul.g = value;
    quad->bl.colorsAdd.g = green_amount;
    quad->br.colorsAdd.g = green_amount;
    quad->tl.colorsAdd.g = green_amount;
    quad->tr.colorsAdd.g = green_amount;
    
    value = 255 * blue_percent;
    quad->bl.colorsMul.b = value;
    quad->br.colorsMul.b = value;
    quad->tl.colorsMul.b = value;
    quad->tr.colorsMul.b = value;
    quad->bl.colorsAdd.b = blue_amount;
    quad->br.colorsAdd.b = blue_amount;
    quad->tl.colorsAdd.b = blue_amount;
    quad->tr.colorsAdd.b = blue_amount;
}

//void hierarchiesUpdateQuadTextureColor (const bool opacityModifyRGB, const int opacity, const int color_r, const int color_g, const int color_b, CCHierarchiesSprite_V3F_C4B_T2F_Quad* quad) {
//	// If opacityModifyRGB is NO then opacity will be applied as: glColor(R,G,B,opacity);
//	// If opacityModifyRGB is YES then oapcity will be applied as: glColor(opacity, opacity, opacity, opacity );
//	if (opacityModifyRGB) {
//		quad->bl.colorsMul.a = quad->bl.colorsMul.a * (opacity / 255.0f);
//		quad->br.colorsMul.a = quad->br.colorsMul.a * (opacity / 255.0f);
//		quad->tl.colorsMul.a = quad->tl.colorsMul.a * (opacity / 255.0f);
//		quad->tr.colorsMul.a = quad->tr.colorsMul.a * (opacity / 255.0f);
//
//		quad->bl.colorsMul.r = quad->bl.colorsMul.r * (color_r / 255.0f) * (opacity / 255.0f);
//		quad->br.colorsMul.r = quad->br.colorsMul.r * (color_r / 255.0f) * (opacity / 255.0f);
//		quad->tl.colorsMul.r = quad->tl.colorsMul.r * (color_r / 255.0f) * (opacity / 255.0f);
//		quad->tr.colorsMul.r = quad->tr.colorsMul.r * (color_r / 255.0f) * (opacity / 255.0f);
//
//		quad->bl.colorsMul.g = quad->bl.colorsMul.g * (color_g / 255.0f) * (opacity / 255.0f);
//		quad->br.colorsMul.g = quad->br.colorsMul.g * (color_g / 255.0f) * (opacity / 255.0f);
//		quad->tl.colorsMul.g = quad->tl.colorsMul.g * (color_g / 255.0f) * (opacity / 255.0f);
//		quad->tr.colorsMul.g = quad->tr.colorsMul.g * (color_g / 255.0f) * (opacity / 255.0f);
//
//		quad->bl.colorsMul.b = quad->bl.colorsMul.b * (color_b / 255.0f) * (opacity / 255.0f);
//		quad->br.colorsMul.b = quad->br.colorsMul.b * (color_b / 255.0f) * (opacity / 255.0f);
//		quad->tl.colorsMul.b = quad->tl.colorsMul.b * (color_b / 255.0f) * (opacity / 255.0f);
//		quad->tr.colorsMul.b = quad->tr.colorsMul.b * (color_b / 255.0f) * (opacity / 255.0f);
//	}
//	else {
//		quad->bl.colorsMul.a = quad->bl.colorsMul.a * (opacity / 255.0f);
//		quad->br.colorsMul.a = quad->br.colorsMul.a * (opacity / 255.0f);
//		quad->tl.colorsMul.a = quad->tl.colorsMul.a * (opacity / 255.0f);
//		quad->tr.colorsMul.a = quad->tr.colorsMul.a * (opacity / 255.0f);
//
//		quad->bl.colorsMul.r = quad->bl.colorsMul.r * (color_r / 255.0f);
//		quad->br.colorsMul.r = quad->br.colorsMul.r * (color_r / 255.0f);
//		quad->tl.colorsMul.r = quad->tl.colorsMul.r * (color_r / 255.0f);
//		quad->tr.colorsMul.r = quad->tr.colorsMul.r * (color_r / 255.0f);
//
//		quad->bl.colorsMul.g = quad->bl.colorsMul.g * (color_g / 255.0f);
//		quad->br.colorsMul.g = quad->br.colorsMul.g * (color_g / 255.0f);
//		quad->tl.colorsMul.g = quad->tl.colorsMul.g * (color_g / 255.0f);
//		quad->tr.colorsMul.g = quad->tr.colorsMul.g * (color_g / 255.0f);
//
//		quad->bl.colorsMul.b = quad->bl.colorsMul.b * (color_b / 255.0f);
//		quad->br.colorsMul.b = quad->br.colorsMul.b * (color_b / 255.0f);
//		quad->tl.colorsMul.b = quad->tl.colorsMul.b * (color_b / 255.0f);
//		quad->tr.colorsMul.b = quad->tr.colorsMul.b * (color_b / 255.0f);
//	}
//}

void hierarchiesExpandRectByPoint (float* minX, float* maxX, float* minY, float* maxY, float* pX, float* pY) {
    if (*pX < *minX) {
        *minX = *pX;
    }
    if (*pX > *maxX) {
        *maxX = *pX;
    }
    
    if (*pY < *minY) {
        *minY = *pY;
    }
    if (*pY > *maxY) {
        *maxY = *pY;
    }
}

float float_round (float r) {
    return (r > 0.0f) ? floorf(r + 0.5f) : ceilf(r - 0.5f);
}


NS_CC_EXT_END

#pragma mark - CCHierarchiesSpriteBase