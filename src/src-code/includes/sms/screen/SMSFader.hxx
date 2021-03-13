#pragma once

#include "types.h"
#include "sms/JSystem/JUT/JUTColor.hxx"

class TSMSFader
{
public:
    enum FadeStatus
    {
        OPAQUE,
        TRANSPARENT
    };

    u32 _00[0x18 / 4];       //0x0000
    JUtility::TColor mColor; //0x0018
    bool _01;                //0x001C
    u32 mFadeStatus;         //0x0020
    u32 mStatusID;           //0x0024
    f32 mFadeTime;         //0x0028
    f32 _02;               //0x002C
};