#include <Dolphin/GX.h>
#include <Dolphin/OS.h>

#include <JSystem/J2D/J2DPrint.hxx>
#include <JSystem/JKernel/JKRFileLoader.hxx>
#include <SMS/Enemy/EffectObjBase.hxx>
#include <SMS/GC2D/ConsoleStr.hxx>
#include <SMS/MSound/MSoundSESystem.hxx>
#include <SMS/Manager/MarioParticleManager.hxx>
#include <SMS/Manager/ModelWaterManager.hxx>
#include <SMS/MapObj/MapObjNormalLift.hxx>
#include <SMS/MapObj/MapObjTree.hxx>
#include <SMS/Player/Mario.hxx>
#include <SMS/Strategic/HitActor.hxx>
#include <SMS/Strategic/LiveActor.hxx>
#include <SMS/System/Application.hxx>
#include <SMS/System/CardManager.hxx>
#include <SMS/assert.h>
#include <SMS/macros.h>
#include <SMS/raw_fn.hxx>

#include <BetterSMS/libs/constmath.hxx>
#include <BetterSMS/libs/global_unordered_map.hxx>
#include <BetterSMS/libs/profiler.hxx>
#include <BetterSMS/libs/string.hxx>
#include <BetterSMS/libs/triangle.hxx>

#include <BetterSMS/logging.hxx>
#include <BetterSMS/module.hxx>
#include <BetterSMS/player.hxx>
#include <BetterSMS/stage.hxx>

#include "player.hxx"

#pragma region FireAPI

static constexpr s32 MaxFireDamageTime = 300;
static constexpr s32 MaxFireTime       = MaxFireDamageTime * 3;

BETTER_SMS_FOR_EXPORT void SME::Player::setFire(TMario *player) {
    auto playerData = getEclipseData(player);

    if (playerData->mIsOnFire) {
        playerData->mFireTimer %= MaxFireDamageTime;
        return;
    }

    playerData->mIsOnFire     = true;
    playerData->mFireTimer    = 0;
    playerData->mFireTimerMax = MaxFireTime;
}

BETTER_SMS_FOR_EXPORT void SME::Player::extinguishFire(TMario *player, bool expired) {
    auto playerData = getEclipseData(player);

    if (playerData->mIsOnFire && !expired)
        MSoundSE::startSoundActor(0x28C5, reinterpret_cast<Vec *>(&player->mTranslation), 0,
                                  nullptr, 0, 0);

    playerData->mIsOnFire  = false;
    playerData->mFireTimer = 0;
}

BETTER_SMS_FOR_CALLBACK void blazePlayer(TMario *player, bool isMario) {
    if (!isMario)
        return;

    auto playerData = SME::Player::getEclipseData(player);
    if (!playerData->mIsOnFire)
        return;

    const f32 fireScale = 3.0f - (static_cast<f32>(playerData->mFireTimer) / MaxFireTime) * 2.0f;

    JPABaseEmitter *emitterFire =
        gpMarioParticleManager->emitAndBindToPosPtr(0x135, &player->mTranslation, 1, player);
    emitterFire->mSize2.set(player->mScale.x * fireScale, player->mScale.y * fireScale,
                            player->mScale.z * fireScale);

    JPABaseEmitter *emitterSmoke =
        gpMarioParticleManager->emitAndBindToPosPtr(0x136, &player->mTranslation, 1, player);
    emitterSmoke->mSize2.set(player->mScale.x * fireScale, player->mScale.y * fireScale,
                             player->mScale.z * fireScale);

    JPABaseEmitter *emitterEmber =
        gpMarioParticleManager->emitAndBindToPosPtr(0x137, &player->mTranslation, 1, player);
    emitterEmber->mSize2.set(player->mScale.x * fireScale, player->mScale.y * fireScale,
                             player->mScale.z * fireScale);

    const s32 fireFrame = playerData->mFireTimer % MaxFireDamageTime;
    playerData->mFireTimer += 1;

    if (player->mFludd->mCurrentNozzle == TWaterGun::Hover && player->mFludd->mIsEmitWater)
        playerData->mFireTimerMax -= 1;

    if (fireFrame == 0) {
        player->decHP(1);
        player->changePlayerStatus(0x208B6, 0, false);
        player->startVoice(0x7849);
    }

    if (!(player->mState & static_cast<u32>(TMario::STATE_AIRBORN))) {
        switch (static_cast<TMario::State>(player->mState)) {
        case TMario::STATE_TURNING_MID:
            break;
        case TMario::STATE_IDLE:
            player->changePlayerStatus(TMario::STATE_RUNNING, 0, false);
        default:
            player->setPlayerVelocity(50.0f * player->mScale.z);
            player->mActionState = 1;
        }
    }

    if (player->mTranslation.y - player->mWaterHeight <= -40.0f * player->mScale.y)
        SME::Player::extinguishFire(player, false);

    if (playerData->mFireTimer > playerData->mFireTimerMax)
        SME::Player::extinguishFire(player, true);
}

static void flameMario(TEffectObjBase *fire, u32 message) {
    s32 marioIdx;
    SMS_FROM_GPR(30, marioIdx);

    TMario *player = reinterpret_cast<TMario *>(fire->mCollidingObjs[marioIdx]);
    SME::Player::setFire(player);
}
SMS_PATCH_BL(SMS_PORT_REGION(0x80038148, 0x80038200, 0, 0), flameMario);

#pragma endregion