#include "includes/eclipse.hxx"

void setStageOnExit(TGameSequence* gpSequence, s8 stage, s8 episode) {
    TApplication* gpApplication = (TApplication*)TApplicationInstance;
    TMarioGamePad* gpGamePad = gpApplication->mGamePad1;

    if (gpGamePad->Buttons.mZButton) {
        stage = gpApplication->mCurrentScene.mAreaID;
        episode = -1;
    }
    set__13TGameSequenceFUcUcQ26JDrama10TFlagT_U(gpSequence, stage, episode);
}

void startEpisodeSelect(void* selectMenu) {
    TApplication* gpApplication = (TApplication*)TApplicationInstance;
    TMarioGamePad* gpGamePad = gpApplication->mGamePad1;

    if (!gpGamePad->Buttons.mZButton) {
        startOpenWindow__11TSelectMenuFv(selectMenu);
    }
}