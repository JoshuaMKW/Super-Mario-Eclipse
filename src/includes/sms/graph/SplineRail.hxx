#pragma once

#include "types.h"
#include "MTX.h"

#include "JGeometry.hxx"

class TGraphWeb;

class TSplineRail {
public:
    TSplineRail(TGraphWeb *);

    void getPosAndRot(f32, JGeometry::TVec3<f32> posOut, JGeometry::TVec3<f32> rotOut);
};

class TSplinePath {
public:
    TSplinePath(int);

    f32 calcSpline(f32, f32 *, f32 *, f32 *);
    void calcTable();
    Vec getPoint(f32);
    void makeTable(f32 *, f32 *, f32 *);
};