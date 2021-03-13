#pragma once

#include "types.h"
#include "sms/JSystem/JGeometry.hxx"
#include "sms/actor/Item.hxx"

class TShine : public TItem
{

public:
    u32 mType;                         //0x0154
    u32 _02[0x38 / 4];                 //0x0158
    bool isNormalCamera;               //0x0190
    u8 _03;                            //0x0191
    u16 _04;                           //0x0192
    u32 _05[0x14 / 4];                 //0x0194
    JGeometry::TVec3<f32> mGlowSize;   //0x01A8
    u8 isAlreadyObtained;              //0x01B4
    u8 _06;                            //0x01B5
    u16 _07;                           //0x01B6
    u32 _08[0x40 / 4];                 //0x01B8
};