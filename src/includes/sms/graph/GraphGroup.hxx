#pragma once

#include "types.h"
#include "GraphWeb.hxx"

#include "JDrama/JDRGraphics.hxx"

class TGraphGroup {
public:
    TGraphGroup(void *nameOfsArray);

    TGraphWeb *getGraphByName(const char *);
    void initGraphGroup();
    void perform(u32, JDrama::TGraphics *);

    s32 *mNameOffsets;
    size_t mRailNum;
    TGraphWeb **mRails;
    TGraphWeb *mNullRail;
};