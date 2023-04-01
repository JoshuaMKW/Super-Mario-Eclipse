#include <SMS/raw_fn.hxx>

#include <BetterSMS/module.hxx>

using namespace BetterSMS;

// 0x80299230
// extern -> SME.cpp
static void setStageOnExit(TGameSequence *gpSequence, s8 stage, s8 episode,
                           JDrama::TFlagT<u16> flag) {
    TMarioGamePad *gpGamePad = gpApplication.mGamePads[0];

    #if 0
    if (gpGamePad->mButtons.mInput & TMarioGamePad::EButtons::Z) {
        stage   = gpApplication.mCurrentScene.mAreaID;
        episode = -1;
    }
    #else
    if (gpMarDirector->mAreaID == 6 &&
        (gpMarDirector->mEpisodeID == 1 || gpMarDirector->mEpisodeID == 2)) {
        stage   = 6;
        episode = gpMarDirector->mEpisodeID;
    }

    else if (gpMarDirector->mAreaID == 25 && gpMarDirector->mEpisodeID == 0) {
        stage   = 26;
        episode = 0;
    }

    else if ((gpMarDirector->mAreaID == 4 && gpMarDirector->mEpisodeID == 5) ||   // Starshine Beach
        (gpMarDirector->mAreaID == 32 && gpMarDirector->mEpisodeID == 0) ||  // Flip Panel
        ((gpMarDirector->mAreaID == 25 || gpMarDirector->mAreaID == 26) &&
         gpMarDirector->mEpisodeID == 0) ||
        ((gpMarDirector->mAreaID == 13 || gpMarDirector->mAreaID == 50) &&
         gpMarDirector->mEpisodeID == 0)) {
        stage   = 6;
        episode = 3;
    }
    #endif
    gpSequence->set(stage, episode, flag);
}
SMS_PATCH_BL(SMS_PORT_REGION(0x80299230, 0x802910C8, 0, 0), setStageOnExit);

// 0x80175F58
// extern -> SME.cpp
static void startEpisodeSelect(void *selectMenu) {
    TMarioGamePad *gpGamePad = gpApplication.mGamePads[0];

    if (!(gpGamePad->mButtons.mInput & TMarioGamePad::EButtons::Z)) {
        startOpenWindow__11TSelectMenuFv(selectMenu);
    }
}
SMS_PATCH_BL(SMS_PORT_REGION(0x80175F58, 0x8016BEFC, 0, 0), startEpisodeSelect);