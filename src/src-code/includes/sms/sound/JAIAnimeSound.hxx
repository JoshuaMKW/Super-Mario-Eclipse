#pragma once

#include "types.h"
#include "sms/sound/JAISound.hxx"

class JAIAnimeSound
{
public:
    JAIAnimeSound();

    void startAnimSound(void *, u32, JAISound **, u32 *, u8); // JAIActor *
    void setSpeedModifySound(JAISound *, u32 *, f32);         // JAIAnimeFrameSoundData *

    u8 _0;
    u8 _1; // padding?
    u8 _2; // ^^
    u8 _3; // ^^
    u32 _4;
    u32 _8; // padding?
    u8 _C;
    u8 _D; // padding?
    u8 _E; // ^^
    u8 _F; // ^^
    u32 _10;
    u32 _14; // padding?
    u8 _18;
    u8 _19; // padding?
    u8 _1A; // ^^
    u8 _1B; // ^^
    u32 _1C;
    u32 _20; // padding?
    u8 _24;
    u8 _25; // padding?
    u8 _26; // ^^
    u8 _27; // ^^
    u32 _28;
    u32 _2C; // padding?
    u8 _30;
    u8 _31; // padding?
    u8 _32; // ^^
    u8 _33; // ^^
    u32 _34;
    u8 _38[0x94 - 0x38];
};