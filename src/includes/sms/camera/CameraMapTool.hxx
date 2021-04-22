#pragma once

#include "sms/JSystem/JGeometry.hxx"
#include "sms/JSystem/JSU/JSUMemoryStream.hxx"

class TCameraMapTool {
public:
    TCameraMapTool(const TCameraMapTool &);
    virtual ~TCameraMapTool();

    void calcPosAndAt(JGeometry::TVec3<f32> *, JGeometry::TVec3<f32> *) const;
    void load(JSUMemoryInputStream &);
};