#include <SMS/raw_fn.hxx>

#include <BetterSMS/module.hxx>

using namespace BetterSMS;

// 0x80175F58
// extern -> SME.cpp
static void startEpisodeSelect(void *selectMenu) {
    TMarioGamePad *gpGamePad = gpApplication.mGamePads[0];

    if (!(gpGamePad->mButtons.mInput & TMarioGamePad::EButtons::Z)) {
        startOpenWindow__11TSelectMenuFv(selectMenu);
    }
}
//SMS_PATCH_BL(SMS_PORT_REGION(0x80175F58, 0x8016BEFC, 0, 0), startEpisodeSelect);