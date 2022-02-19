#include "sms/actor/Yoshi.hxx"
#include "sms/actor/Mario.hxx"

#include "SME.hxx"

static void fixYoshiJuiceDecrement() {
  TYoshi *yoshi;
  SME_FROM_GPR(30, yoshi);

  TMario *player = yoshi->mMario;
  if (player->mFludd->mIsEmitWater && yoshi->isMounted())
    yoshi->mCurJuice -= player->mFludd->mEmitInfo->mNum.get();
}
SME_PATCH_BL(SME_PORT_REGION(0x8026E810, 0, 0, 0), fixYoshiJuiceDecrement);