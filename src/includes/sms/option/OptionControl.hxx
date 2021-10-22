#pragma once

#include "types.h"
#include "J2D/J2DSetScreen.hxx"
#include "sms/option/OptionRumbleUnit.hxx"
#include "sms/option/OptionSoundUnit.hxx"
#include "sms/option/OptionSubtitleUnit.hxx"

enum SelectType
{
    RUMBLE = 0,
    SOUND = 1,
    SUBTITLES = 2,
    UNKNOWN = 3,
};

class TOptionControl
{
public:
    J2DSetScreen *mSetScreen;           // _0
    u32 *_4;                            // TArrowControl*
    TOptionRumbleUnit *mRumbleUnit;     // _8
    TOptionSoundUnit *mSoundUnit;       // _C
    TOptionSubtitleUnit *mSubtitleUnit; // _10
    u32 mSelectType;                    // _14
    bool mIsOptionChanged;              //0x0018
    bool mIsOptionSelected;             //0x0019
    u8 _1A;                             // padding?
    u8 _1B;                             // ^^
    u32 _1C;
};