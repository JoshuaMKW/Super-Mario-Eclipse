#include "sms/actor/Yoshi.hxx"
#include "sms/actor/Mario.hxx"

#include "SME.hxx"

using namespace SME;
using namespace SME::Class;

bool TYoshi::isMounted() { return this->mState == TYoshi::MOUNTED; }

bool TYoshi::isMounted(TMario *player) {
  return player->mYoshi->mState == TYoshi::MOUNTED;
}

bool TYoshi::isGreenYoshi() { return this->mType == TYoshi::GREEN; }

bool TYoshi::isGreenYoshi(TMario *player) {
  return player->mYoshi->isGreenYoshi();
}

bool TYoshi::isGreenYoshiMounted() {
  return this->isGreenYoshi() && this->mState == TYoshi::MOUNTED;
}

bool TYoshi::isGreenYoshiMounted(TMario *player) {
  return player->mYoshi->isGreenYoshiMounted();
}

bool TYoshi::isGreenYoshiAscendingWater(TMario *player) {
  return player->mAttributes.mIsWater &&
         player->mController->mButtons.mInput & TMarioGamePad::EButtons::A &&
         player->mYoshi->isGreenYoshiMounted();
}