#pragma once

#include "JGeometry.hxx"
#include "JSU/JSUMemoryStream.hxx"

class TCameraMapTool {
public:
    TCameraMapTool(const TCameraMapTool &);
    virtual ~TCameraMapTool();

    void calcPosAndAt(JGeometry::TVec3<f32> *, JGeometry::TVec3<f32> *) const;
    void load(JSUMemoryInputStream &);
};