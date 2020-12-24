#ifndef M3UMODEL_H
#define M3UMODEL_H

#include "types.h"
#include "sms/J3D.hxx"

class M3UModel
{

public:
    void *_00[2];      //0x0000
    J3DModel *mModel;  //0x0008
    u32 _01[0x30 / 4]; //0x000C
    u32 mLightID;      // _3C
    u8 _40;
    u8 _41;         // padding?
    u8 _42;         // ^^
    u8 _43;         // ^^
    u32 mLightType; // _44
};

#endif