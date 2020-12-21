#ifndef JAIBASIC_H
#define JAIBASIC_H

#include "types.h"
#include "sms/sound/JAISound.hxx"

class JAIBasic
{
public:
    u32 _0;
    u32 *vTable; // _4
    u8 _8[0x38 - 0x8];
    JAISound *mSound; // _38
    u8 _3C[0x50 - 0x3C];
    char *mParamWavePath; // _50
    u8 _54[0x64 - 0x54];
    char *mParamSequenceArchivesPath; // _64
    u8 _64[0x74 - 0x64];
    char *mParamStreamPath; // _74
    u32 _78;
};

#endif