#pragma once

#include "types.h"
#include "sms/JSystem/JDrama.hxx"
#include "sms/game/GameSequence.hxx"
#include "sms/game/MarDirector.hxx"

#include "sms/screen/SMSFader.hxx"
#include "sms/actor/Mario.hxx"


class TApplication
{

public:
    TApplication();

    void checkAdditionalMovie();
    void crTimeAry();
    void drawDVDErr();
    void finalize();
    void gameLoop();
    void initialize();
    void initialize_bootAfter();
    void initialize_nlogoAfter();
    void mountStageArchive();
    void proc();
    void setupThreadFuncLogo();

    u32 _00;                        //0x0000
    TMarDirector *mMarDirector;     //0x0004
    u16 _01;                        //0x0008
    TGameSequence mPrevScene;       //0x000A
    TGameSequence mCurrentScene;    //0x000E
    TGameSequence mNextScene;       //0x0012
    u16 _02;                        //0x0016
    u32 mCutSceneID;                //0x0018
    JDrama::TDisplay *mDisplay;     //0x001C
    TMarioGamePad *mGamePad1;       //0x0020
    TMarioGamePad *mGamePad2;       //0x0024
    TMarioGamePad *mGamePad3;       //0x0028
    TMarioGamePad *mGamePad4;       //0x002C
    AreaEpisodeArray *mStringPaths; //0x0030
    TSMSFader *mFader;              //0x0034
    u32 _04[0x8 / 4];               //0x0038
    u32 *mJKRExpHeapHi;             //0x0040

};

void SetupThreadFuncLogo(void *);
void SetupThreadFuncBoot(void *);