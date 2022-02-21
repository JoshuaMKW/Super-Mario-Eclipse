#include "SME.hxx"

using namespace SME;

#if SME_YOSHI_EGG_FREE

static bool isYoshiEggNeedFruit(THitActor *gpFruit) {
  if (!Class::TStageParams::sStageConfig->mIsEggFree.get())
    return TMapObjBase::isFruit(gpFruit);

  return true;
}
SME_PATCH_BL(SME_PORT_REGION(0x801BC868, 0, 0, 0), isYoshiEggNeedFruit);

#define EGG_IGNORE 0
#define EGG_SUCCESS 1
#define EGG_REJECT 2

static u8 isYoshiEggFree(TEggYoshi *gpEgg, THitActor *gpFruit) {
  const Class::TPlayerData *playerParams =
      SME::TGlobals::getPlayerData(gpMarioAddress);
  if (gpEgg->mState == 14 || gpEgg->mState == 6)
    return EGG_IGNORE;
  else if (!playerParams->getParams()->mCanRideYoshi.get())
    return EGG_REJECT;
  else if (!Class::TStageParams::sStageConfig->mIsEggFree.get())
    return gpEgg->mWantedFruit == gpFruit->mObjectID ? EGG_SUCCESS : EGG_REJECT;
  else
    return EGG_SUCCESS;
}
SME_PATCH_BL(SME_PORT_REGION(0x801BC8B4, 0, 0, 0), isYoshiEggFree);
SME_WRITE_32(SME_PORT_REGION(0x801BC8B8, 0, 0, 0), 0xA01E00FC);
SME_WRITE_32(SME_PORT_REGION(0x801BC8BC, 0, 0, 0), 0x2C00000B);
SME_WRITE_32(SME_PORT_REGION(0x801BC8C0, 0, 0, 0), 0x418200E4);
SME_WRITE_32(SME_PORT_REGION(0x801BC8C4, 0, 0, 0), 0x2C030001);
SME_WRITE_32(SME_PORT_REGION(0x801BC8C8, 0, 0, 0), 0x4182003C);
SME_WRITE_32(SME_PORT_REGION(0x801BC8CC, 0, 0, 0), 0x418100D8);
SME_WRITE_32(SME_PORT_REGION(0x801BC8D0, 0, 0, 0), 0x48000134);

#undef EGG_IGNORE
#undef EGG_SUCCESS
#undef EGG_REJECT

#define EGG_CARD_EXCLAIM 11.0f

static void initFreeEggCard(J3DFrameCtrl *frameCtrl) {
  if (!Class::TStageParams::sStageConfig->mIsEggFree.get())
    return;

  frameCtrl->mCurFrame = EGG_CARD_EXCLAIM;
}
SME_PATCH_B(SME_PORT_REGION(0x801BC128, 0, 0, 0), initFreeEggCard);

static u32 checkFreeEggCard(J3DFrameCtrl *frameCtrl) {
  if (!Class::TStageParams::sStageConfig->mIsEggFree.get())
    return 0;

  frameCtrl->mCurFrame = EGG_CARD_EXCLAIM;
  return 0;
}
SME_PATCH_BL(SME_PORT_REGION(0x801BC380, 0, 0, 0), checkFreeEggCard);

static u32 updateFreeEggCard(J3DFrameCtrl *frameCtrl) {
  if (!Class::TStageParams::sStageConfig->mIsEggFree.get())
    return 1;

  frameCtrl->mCurFrame = EGG_CARD_EXCLAIM;
  return 1;
}
SME_PATCH_BL(SME_PORT_REGION(0x801BC6B8, 0, 0, 0), updateFreeEggCard);
SME_WRITE_32(SME_PORT_REGION(0x801BC6BC, 0, 0, 0), 0xB07F00FC);

#undef EGG_CARD_EXCLAIM

#endif