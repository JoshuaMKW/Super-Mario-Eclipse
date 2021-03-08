#pragma once

#include "types.h"

class Talk2D2
{

public:
    u32 _00[0x28 / 4];  //0x0000
    bool isBillBoard;   //0x0028
    u8 _01;             //0x0029
    u16 _02;            //0x002A
    u32 _03[0x248 / 4]; //0x002C
    u32 lastMsgIndex;   //0x0274
    u32 curMsgIndex;    //0x0278
};