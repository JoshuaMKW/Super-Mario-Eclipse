#include "SME.hxx"

// 0x80299230
// extern -> SME.cpp
void setStageOnExit(TGameSequence *gpSequence, s8 stage, s8 episode)
{
    TMarioGamePad *gpGamePad = gpApplication.mGamePad1;

    if (gpGamePad->mButtons.mInput & TMarioGamePad::Buttons::Z)
    {
        stage = gpApplication.mCurrentScene.mAreaID;
        episode = -1;
    }
    set__13TGameSequenceFUcUcQ26JDrama10TFlagT_U(gpSequence, stage, episode);
}

// 0x80175F58
// extern -> SME.cpp
void startEpisodeSelect(void *selectMenu)
{
    TMarioGamePad *gpGamePad = gpApplication.mGamePad1;

    if (!gpGamePad->mButtons.mInput & TMarioGamePad::Buttons::Z)
    {
        startOpenWindow__11TSelectMenuFv(selectMenu);
    }
}