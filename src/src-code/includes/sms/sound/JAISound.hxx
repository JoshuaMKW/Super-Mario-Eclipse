#ifndef JAISOUND_H
#define JAISOUND_H

#include "types.h"

struct JAIMoveParaSet
{
    f32 _0;
    f32 _4;
    f32 _8;
    u32 *_C;
};

class JAISound
{
public:
    u8 _0;
    u8 _1;
    u8 _2;
    u8 _3;
    u32 _4;
    u32 mID; // _8
    u8 _C[0x18 - 0xC];
    u32 _18;
    u32 _1C;
    u32 _20;
    u8 _24[0x34 - 0x24];
    u32 *_34;          // sound pointer, maybe?
    u32 *mStreamParam; // _38
    void *_3C;
    u32 *vTable; // _40
};

#endif