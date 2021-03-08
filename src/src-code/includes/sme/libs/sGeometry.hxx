#pragma once

#include "types.h"
#include "MTX.h"
#include "sms/JGeometry.hxx"

#include "sMath.hxx"

class TVectorTriangle
{
public:
    JGeometry::TVec3<f32> a;
    JGeometry::TVec3<f32> b;
    JGeometry::TVec3<f32> c;

    TVectorTriangle() {};
    TVectorTriangle(JGeometry::TVec3<f32> *a, JGeometry::TVec3<f32> *b, JGeometry::TVec3<f32> *c);

    TVectorTriangle &operator =(TVectorTriangle &other);

    static f32 bearingAngleY(JGeometry::TVec3<f32> a, JGeometry::TVec3<f32> b);
    static JGeometry::TVec3<f32> cross(JGeometry::TVec3<f32> a, JGeometry::TVec3<f32> b);
    static f32 dot(JGeometry::TVec3<f32> a, JGeometry::TVec3<f32> b);
    
    JGeometry::TVec3<f32> center();
    JGeometry::TVec3<f32> normal(bool normalize = true);
    f32 yPosAtXZ(f32 x, f32 z);
};