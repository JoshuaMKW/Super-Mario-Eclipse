#include <Dolphin/DVD.h>
#include <Dolphin/printf.h>
#include <Dolphin/types.h>

#include <JSystem/JGadget/Vector.hxx>
#include <JSystem/JKernel/JKRDecomp.hxx>
#include <JSystem/JKernel/JKRDvdRipper.hxx>
#include <JSystem/JKernel/JKRHeap.hxx>
#include <JSystem/JKernel/JKRMemArchive.hxx>
#include <SMS/Camera/PolarSubCamera.hxx>
#include <SMS/Manager/PollutionManager.hxx>
#include <SMS/System/Application.hxx>
#include <SMS/assert.h>
#include <SMS/macros.h>

#include <BetterSMS/libs/container.hxx>
#include <BetterSMS/libs/global_vector.hxx>
#include <BetterSMS/module.hxx>
#include <BetterSMS/player.hxx>
#include <BetterSMS/stage.hxx>

#include "globals.hxx"
#include "object/water_balloon.hxx"
#include "player.hxx"

using WaterBalloonBuffer = TRingBuffer<TWaterBalloon>;

BETTER_SMS_FOR_CALLBACK void initializeWaterBalloons(TMario *player) {
    TWaterBalloon::sEmitInfo = new TWaterEmitInfo("/common/prm/waterballoon.prm");

    void *balloon_prm = JKRFileLoader::getGlbResource("/common/prm/waterballoon.prm");
    JSUMemoryInputStream in(balloon_prm, JKRFileLoader::getResSize(balloon_prm, nullptr));

    TWaterBalloon::sEmitInfo->load(in);

    auto *water_balloons = new WaterBalloonBuffer(4, false);
    Player::deregisterData(player, "sme_balloon_info");
    Player::registerData(player, "sme_balloon_info", water_balloons);

    for (int i = 0; i < water_balloons->capacity(); ++i) {
        TWaterBalloon *balloon = new TWaterBalloon("waterballoon");
        balloon->initAndRegister("waterballoon");
        balloon->mSprayPushSpeed       = 0.1f;
        balloon->mAdditionalHeldHeight = 24.0f;
        water_balloons->push(balloon);
    }
}

BETTER_SMS_FOR_CALLBACK void createWaterBalloonAndThrow(TMario *player, bool isMario) {
    if (gpMarDirector->mCurState != 4 || !isMario)
        return;

    if (player->mState == 0x200009 || player->mState == 0x80000588 ||
        (player->mState & 0x800000) != 0 || player->mState == TMario::STATE_SHINE_C ||
        (player->mState & TMario::STATE_WATERBORN) != 0)
        return;

    if (player->mHeldObject || player->mAttributes.mHasFludd)
        return;

    if (!(player->mController->mButtons.mFrameInput & TMarioGamePad::R))
        return;

    auto *water_balloons = reinterpret_cast<WaterBalloonBuffer *>(
        Player::getRegisteredData(player, "sme_balloon_info"));

    TWaterBalloon *balloon = water_balloons->next();
    balloon->mTranslation.set(player->mTranslation);
    balloon->mSpeed.set(0, 0, 0);
    balloon->mForwardSpeed = 0;
    balloon->appear();

    // Have player grab balloon
    if (balloon->receiveMessage(player, 4)) {  // 4 = grab
        player->mHeldObject      = balloon;
        player->mFluddUsageState = 2;
    } else {
        player->mHeldObject = nullptr;
        return;
    }

    if (player->mState == static_cast<u32>(TMario::STATE_IDLE)) {
        player->changePlayerStatus(0x80000588, 0, 1);  // 0x80000387
    } else if (player->mState == TMario::STATE_JUMP || player->mState == TMario::STATE_D_JUMP ||
               player->mState == TMario::STATE_FALL || player->mState == TMario::STATE_JUMPSPINL ||
               player->mState == TMario::STATE_JUMPSPINR ||
               player->mState == TMario::STATE_JUMPSPIN) {
        player->changePlayerStatus(0x820008AB, 0, 1);
    } else {
        player->changePlayerStatus(0x80000588, 0, 1);
    }
}

static void adjustCleanPower(TPollutionManager *manager, f32 x, f32 y, f32 z, f32 radius) {
    manager->clean(x, y, z, radius * 1.5f);
}
SMS_PATCH_BL(SMS_PORT_REGION(0x8027F7DC, 0, 0, 0), adjustCleanPower);