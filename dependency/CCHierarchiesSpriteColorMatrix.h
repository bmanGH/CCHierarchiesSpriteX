//
//  CCHierarchiesSpriteColorMatrix.h
//  CCHierarchiesSprite
//
//  Created by bman <zx123xz321hm3@hotmail.com>.
//  Copyright (c) 2013. All rights reserved.
//

#ifndef __SZSG__CCHierarchiesSpriteColorMatrix__
#define __SZSG__CCHierarchiesSpriteColorMatrix__

#include "cocos-ext.h"
#include "CCColorMatrix/CCColorMatrixProtocol.h"

USING_NS_CC;
USING_NS_CC_EXT;

class CC_DLL CCHierarchiesSpriteColorMatrix : public CCHierarchiesSprite, public CCColorMatrixProtocol {
    
protected:
    CCColorMatrix4x4 _colorMatrix;
    GLint _colorMatrixUniformLocation;
    bool _needResetColorMatrix;
    
    virtual void setup ();
    void _updateShader ();
    
public:
	static CCHierarchiesSpriteColorMatrix* create (const char* sheetFileName, const char* animationFileName, CCHierarchiesSpriteEventDelegate* delegate = NULL);
    
    CCHierarchiesSpriteColorMatrix ();
    virtual ~CCHierarchiesSpriteColorMatrix ();
    
    virtual void draw ();
    
    // CCColorMatrixProtocol
    virtual void setColorMatrix (const CCColorMatrix4x4& colorMatrix);
    virtual CCColorMatrix4x4 getColorMatrix ();
    virtual void concatColorMatrix (const CCColorMatrix4x4& colorMatrix);
    virtual void setNeedResetColorMatrixAfterDraw (bool value);
    virtual bool isNeedResetColorMatrixAfterDraw ();
    
};

#endif /* defined(__SZSG__CCHierarchiesSpriteColorMatrix__) */
