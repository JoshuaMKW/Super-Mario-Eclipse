#pragma once

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
    enum COMMANDS
    {
        NOP,
        FORMAT,
        CREATE,
        GETBOOKMARKS,
        NOP4,
        LOADBLOCK,
        SAVEBLOCK,
        LOADFILE,
        SAVEFILE,
        EXIT
    };

    enum BLOCKS
    {
        A,
        B,
        C
    };

public:
    s32 mChannel;       //0x0000
    u32 _00[0x120 / 4]; //0x0004
    u8 _124;
    bool mMounted;                 //0x0125
    bool mRefuseOverwriteMount;    //0x0126
    u8 _127;
    s32 mLastStatus;               //0x0128
    void *mCardWorkArea;           //0x012C
    void *mCARDBlock;              //0x0130
    u32 _01[0x314 / 4];            //0x0134
    COMMANDS mCommand;             //0x0448
    u32 _02[0x20 / 4];             //0x044C
    TCardBookmarkInfo *mBookMarks; //0x046C
    BLOCKS mSaveBlock;             //0x0474
};

class TCardLoad
{

public:
    u32 _00[0x14 / 4];              //0x0000
    u32 mState;                     //0x0014
    u32 _01[0xA8 / 4];              //0x0018
    s32 mFramesOpen;                //0x00C0
    u32 _02[0x6B0 / 4];             //0x00C4
    TOptionControl *mOptionControl; //0x0774
};