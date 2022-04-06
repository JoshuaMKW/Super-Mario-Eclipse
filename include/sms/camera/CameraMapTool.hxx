#pragma once

#include "JGeometry.hxx"
#include "JSU/JSUMemoryStream.hxx"

class TCameraMapTool {
public:
    TCameraMapTool(const TCameraMapTool &);
    virtual ~TCameraMapTool();

    void calcPosAndAt(TVec3f *, TVec3f *) const;
    void load(JSUMemoryInputStream &);
};