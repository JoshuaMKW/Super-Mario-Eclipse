#include "SME.hxx"

using namespace SME;

// 0x80299230
// extern -> SME.cpp
static void setStageOnExit(TGameSequence *gpSequence, s8 stage, s8 episode) {
  TMarioGamePad *gpGamePad = gpApplication.mGamePad1;

  if (gpGamePad->mButtons.mInput & TMarioGamePad::EButtons::Z) {
    stage = gpApplication.mCurrentScene.mAreaID;
    episode = -1;
  }
  set__13TGameSequenceFUcUcQ26JDrama10TFlagT_U(gpSequence, stage, episode);
}
SME_PATCH_BL(SME_PORT_REGION(0x80299230, 0, 0, 0), setStageOnExit);

// 0x80175F58
// extern -> SME.cpp
static void startEpisodeSelect(void *selectMenu) {
  TMarioGamePad *gpGamePad = gpApplication.mGamePad1;

  if (!(gpGamePad->mButtons.mInput & TMarioGamePad::EButtons::Z)) {
    startOpenWindow__11TSelectMenuFv(selectMenu);
  }
}
SME_PATCH_BL(SME_PORT_REGION(0x80175F58, 0, 0, 0), startEpisodeSelect);