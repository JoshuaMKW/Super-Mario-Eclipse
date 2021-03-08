#pragma once

#include "types.h"
#include "sms/mapobj/MapObjGeneral.hxx"
#include "sms/actor/Item.hxx"

class TNozzleBox : public TMapObjGeneral
{

public:
    u32 _00[0x10 / 4];  //0x0138
    u32 mNozzleToRegen; //0x0148
    TItem *mNozzleItem; //0x014C
    f32 mThrowLength; //0x0150
    f32 mThrowHeight; //0x0154
    char *mStringID;    //0x0158
    bool mIsValid;      //0x015C
    u8 _01;             //0x015D
    u16 _02;            //0x015E
    u16 _03;            //0x0160
    u16 _04;            //0x0162
};