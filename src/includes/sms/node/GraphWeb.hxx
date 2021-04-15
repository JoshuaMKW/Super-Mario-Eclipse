#pragma once

#include "types.h"
#include "sms/node/RailNode.hxx"

class TGraphWeb
{

public:
    u32 *mNodes;          //0x0000
    TRailNode *mRailNode; //0x0004
    s32 mNodeCount;       //0x0008
    char *mGraphName;     //0x000C
    u32 _00[0x8 / 4];     //0x0010
    void *mVTable;        //0x0018
};