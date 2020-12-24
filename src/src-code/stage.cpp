#include "SME.hxx"

//0x80299230
void setStageOnExit(TGameSequence *gpSequence, s8 stage, s8 episode)
{
    TMarioGamePad *gpGamePad = gpApplication.mGamePad1;

    if (gpGamePad->isPressed(TMarioGamePad::Z))
    {
        stage = gpApplication.mCurrentScene.mAreaID;
        episode = -1;
    }
    set__13TGameSequenceFUcUcQ26JDrama10TFlagT_U(gpSequence, stage, episode);
}
kmCall(0x80299230, &setStageOnExit);

//0x80175F58
void startEpisodeSelect(void *selectMenu)
{
    TMarioGamePad *gpGamePad = gpApplication.mGamePad1;

    if (!gpGamePad->isPressed(TMarioGamePad::Z))
    {
        startOpenWindow__11TSelectMenuFv(selectMenu);
    }
}
kmCall(0x80175F58, &startEpisodeSelect);