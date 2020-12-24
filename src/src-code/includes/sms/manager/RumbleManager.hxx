#ifndef RUMBLEMANAGER_H
#define RUMBLEMANAGER_H

#include "types.h"

class RumbleMgr
{
public:
    f32 _0;
    f32 _4;
    u8 _8;
    u8 _9;
    u8 _A;
    u8 _B;
    u8 _C[0x1C - 0xC];
    u32 _1C;
    u32 _20;
    u32 _24;
    u32 _28;
    u32 *_2C;
};

#endif