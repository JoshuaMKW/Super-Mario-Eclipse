#ifndef CARDLOAD_H
#define CARDLOAD_H

#include "types.h"
#include "sms/option/OptionControl.hxx"

class TCardBookmarkInfo
{

public:
    struct
    {
        u32 _00[0x1C / 4]; //0x0000
        u16 mShineCount;   //0x001C
        u16 _01;           //0x001E
    } FileData[3];
};

class TCardManager
{

public:
    u32 _00[0x46C / 4];            //0x0000
    TCardBookmarkInfo *mBookMarks; //0x046C
};

class TCardLoad
{

public:
    u32 _00[0x14 / 4];              //0x0000
    u32 mState;                     //0x0014
    u32 _01[0x75C / 4];             //0x0018
    TOptionControl *mOptionControl; //0x0774
};

#endif