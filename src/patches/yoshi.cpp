#include "sms/actor/Yoshi.hxx"
#include "sms/actor/Mario.hxx"

#include "SME.hxx"

#if SME_GREEN_YOSHI

static void fixYoshiJuiceDecrement() {
  TYoshi *yoshi;
  SME_FROM_GPR(30, yoshi);

  TMario *player = yoshi->mMario;
  if (player->mFludd->mIsEmitWater && yoshi->mState == TYoshi::MOUNTED)
    yoshi->mCurJuice -= player->mFludd->mEmitInfo->mNum.get();
}
SME_PATCH_BL(SME_PORT_REGION(0x8026E810, 0, 0, 0), fixYoshiJuiceDecrement);

#endif