#include <BetterSMS/module.hxx>
#include <BetterSMS/player.hxx>
#include <BetterSMS/stage.hxx>

using namespace BetterSMS;

static bool isYoshiEggNeedFruit(THitActor *gpFruit) {
    if (!Stage::getStageConfiguration()->mIsEggFree.get())
        return TMapObjBase::isFruit(gpFruit);

    return true;
}
SMS_PATCH_BL(SMS_PORT_REGION(0x801BC868, 0x801B4720, 0, 0), isYoshiEggNeedFruit);

#define EGG_IGNORE  0
#define EGG_SUCCESS 1
#define EGG_REJECT  2

static u8 isYoshiEggFree(TEggYoshi *gpEgg, THitActor *gpFruit) {
    auto *player_data = Player::getData(gpMarioAddress);
    if (gpEgg->mState == 14 || gpEgg->mState == 6)
        return EGG_IGNORE;
    else if (!player_data->getParams()->mCanRideYoshi.get())
        return EGG_REJECT;
    else if (!Stage::getStageConfiguration()->mIsEggFree.get())
        return gpEgg->mWantedFruit == gpFruit->mObjectID ? EGG_SUCCESS : EGG_REJECT;
    else
        return EGG_SUCCESS;
}
SMS_PATCH_BL(SMS_PORT_REGION(0x801BC8B4, 0x801B476C, 0, 0), isYoshiEggFree);
SMS_WRITE_32(SMS_PORT_REGION(0x801BC8B8, 0x801B4770, 0, 0), 0xA01E00FC);
SMS_WRITE_32(SMS_PORT_REGION(0x801BC8BC, 0x801B4774, 0, 0), 0x2C00000B);
SMS_WRITE_32(SMS_PORT_REGION(0x801BC8C0, 0x801B4778, 0, 0), 0x418200E4);
SMS_WRITE_32(SMS_PORT_REGION(0x801BC8C4, 0x801B477C, 0, 0), 0x2C030001);
SMS_WRITE_32(SMS_PORT_REGION(0x801BC8C8, 0x801B4780, 0, 0), 0x4182003C);
SMS_WRITE_32(SMS_PORT_REGION(0x801BC8CC, 0x801B4784, 0, 0), 0x418100D8);
SMS_WRITE_32(SMS_PORT_REGION(0x801BC8D0, 0x801B4788, 0, 0), 0x48000134);

#undef EGG_IGNORE
#undef EGG_SUCCESS
#undef EGG_REJECT

#define EGG_CARD_EXCLAIM 11.0f

static void initFreeEggCard(J3DFrameCtrl *frameCtrl) {
    if (!Stage::getStageConfiguration()->mIsEggFree.get())
        return;

    frameCtrl->mCurFrame = EGG_CARD_EXCLAIM;
}
SMS_PATCH_B(SMS_PORT_REGION(0x801BC128, 0x801B3FE0, 0, 0), initFreeEggCard);

static u32 checkFreeEggCardA(J3DFrameCtrl *frameCtrl) {
    if (!Stage::getStageConfiguration()->mIsEggFree.get())
        return 1;

    frameCtrl->mCurFrame = EGG_CARD_EXCLAIM;
    return 1;
}
SMS_PATCH_BL(SMS_PORT_REGION(0x801BC2A0, 0, 0, 0), checkFreeEggCardA);

static u32 checkFreeEggCardB(TEggYoshi *egg) {
    if (!Stage::getStageConfiguration()->mIsEggFree.get()) {
        egg->decideRandomLoveFruit();
        return egg->mWantedFruit;
    }

    auto *frameCtrl      = egg->mActor->getFrameCtrl(MActor::BTP);
    frameCtrl->mCurFrame = EGG_CARD_EXCLAIM;
    return 0xFFFFFFFF;
}
SMS_PATCH_BL(SMS_PORT_REGION(0x801BC2C8, 0, 0, 0), checkFreeEggCardB);
SMS_WRITE_32(SMS_PORT_REGION(0x801BC2CC, 0, 0, 0), 0x7C601B78);
SMS_WRITE_32(SMS_PORT_REGION(0x801BC2D4, 0, 0, 0), 0x60000000);

static u32 updateFreeEggCard(J3DFrameCtrl *frameCtrl) {
    if (!Stage::getStageConfiguration()->mIsEggFree.get())
        return 1;

    frameCtrl->mCurFrame = EGG_CARD_EXCLAIM;
    return 1;
}
SMS_PATCH_BL(SMS_PORT_REGION(0x801BC6B8, 0x801B4570, 0, 0), updateFreeEggCard);
SMS_WRITE_32(SMS_PORT_REGION(0x801BC6BC, 0x801B4574, 0, 0), 0xB07F00FC);

#undef EGG_CARD_EXCLAIM