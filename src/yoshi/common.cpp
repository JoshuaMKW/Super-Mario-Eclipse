#include "p_common.hxx"

namespace Yoshi {

    bool isMounted(TYoshi *yoshi) { return yoshi->mState == TYoshi::MOUNTED; }

    bool isMounted(TMario *player) {
      return player->mYoshi->mState == TYoshi::MOUNTED;
    }

    bool isGreenYoshi(TYoshi *yoshi) { return yoshi->mType == TYoshi::GREEN; }

    bool isGreenYoshi(TMario *player) { return isGreenYoshi(player->mYoshi); }

    bool isGreenYoshiMounted(TYoshi *yoshi) {
      return isGreenYoshi(yoshi) && yoshi->mState == TYoshi::MOUNTED;
    }

    bool isGreenYoshiMounted(TMario *player) {
      return isGreenYoshiMounted(player->mYoshi);
    }

    bool isGreenYoshiAscendingWater(TMario *player) {
      return player->mAttributes.mIsWater &&
             player->mController->mButtons.mInput & TMarioGamePad::EButtons::A &&
             isGreenYoshiMounted(player->mYoshi);
    }

} // namespace Util::Yoshi