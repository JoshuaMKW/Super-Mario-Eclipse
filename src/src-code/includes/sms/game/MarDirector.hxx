#ifndef MARDIRECTOR_H
#define MARDIRECTOR_H

#include "types.h"
#include "GCConsole2.hxx"

#include "sms/object/DemoCannon.hxx"
#include "sms/actor/item/Shine.hxx"

class TMarDirector
{

public:
    enum STATUS
    {
        INTRO_INIT = 0,
        INTRO_PLAYING = 1,
        NORMAL = 4,
        PAUSE_MENU = 5,
        SAVE_CARD = 11
    };

    enum STATE
    {
        WARP_OUT = 2
    };

    u32 _00[0x4C / 4];       //0x0000
    u16 mGameState;          //0x004C
    u16 _02;                 //0x004E
    u32 _03[0x14 / 4];       //0x0050
    u8 mLastState;           //0x0064
    u8 _04;                  //0x0065
    u16 _05;                 //0x0066
    u32 _06[0xC / 4];        //0x0068
    TGCConsole2 *mGCConsole; //0x0074
    u32 _07;                 //0x0078
    u8 mAreaID;              //0x007C
    u8 mEpisodeID;           //0x007D
    u16 _09;                 //0x007E
    u32 _10[0x2C / 4];       //0x0080
    u32 *sNextState;         //0x00AC
    u32 _11;                 //0x00B0
    u8 mNextState;           //0x00B4
    u32 _12[0x1C / 4];       //0x00B8
    u32 *mGame6Data;         //0x00D4
    u32 *mAramArchive;       //0x00D8
    u32 _13[0x48 / 4];       //0x00DC
    u8 mNextStateA;          //0x0124 ?
    u32 _14[0x12C / 4];      //0x0128
    TDemoCannon *mCannonObj; //0x0254
    u32 _15;                 //0x0258
    TShine *mCollectedShine; //0x025C
};

#endif