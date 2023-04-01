#include <Dolphin/types.h>

#include <JSystem/JDrama/JDRNameRef.hxx>
#include <JSystem/JParticle/JPAResourceManager.hxx>

#include <SMS/Manager/MarioParticleManager.hxx>
#include <SMS/Manager/FlagManager.hxx>
#include <SMS/MapObj/MapObjInit.hxx>
#include <SMS/MapObj/MapObjRail.hxx>
#include <SMS/Graph/GraphGroup.hxx>
#include <SMS/Enemy/Conductor.hxx>
#include <SMS/raw_fn.hxx>

#include <BetterSMS/libs/constmath.hxx>
#include <BetterSMS/module.hxx>
#include <BetterSMS/player.hxx>

#include "object/launch_star.hxx"

TLaunchStarObj::TLaunchStarObj(const char *name)
    : TMapObjGeneral(name), mTravelRail(nullptr), mCameraDemo(nullptr), mTravelSpeed(1.0f), mInactiveTimer(0), mLaunchTimer(0), mPlayer(nullptr) {}

TLaunchStarObj::~TLaunchStarObj() {}

void TLaunchStarObj::load(JSUMemoryInputStream &stream) {
    TMapObjGeneral::load(stream);
    mTravelRail = stream.readString();
    mCameraDemo = stream.readString();
    stream.read(&mTravelSpeed, 4);

    gpResourceManager->load("/scene/mapobj/ms_launchstar_wind.jpa", 397);

    if (TFlagManager::smInstance->getFlag(0x40000) < 20) {
        makeObjDead();
    }
}

void TLaunchStarObj::perform(u32 flags, JDrama::TGraphics *graphics) {
    TMapObjGeneral::perform(flags, graphics);
}

static void clampRotation(TVec3f &rotation) {
    auto clampPreserve = [](f32 degrees) {
        if (degrees > 360.0f)
            degrees -= 360.0f;
        else if (degrees < -360.0f)
            degrees += 360.0f;
        return degrees;
    };

    rotation.x = clampPreserve(rotation.x);
    rotation.y = clampPreserve(rotation.y);
    rotation.z = clampPreserve(rotation.z);
}

void TLaunchStarObj::loadAfter() {
    TMapObjGeneral::loadAfter();
    playIdleAnim();
}

void TLaunchStarObj::control() {
    TMapObjGeneral::control();

    if (mActorData->curAnmEndsNext(MActor::BCK, nullptr)) {
        playIdleAnim();
    }

    if (mActorData->getCurAnmIdx(MActor::BCK) == 3) {
        auto *sparkles = gpMarioParticleManager->emitAndBindToPosPtr(14, &mTranslation, 1, this);
        if (sparkles) {
            sparkles->mSize1 = {5.0f, 1.0f, 5.0f};
            sparkles->mSize3 = {1.5f, 1.5f, 1.5f};
        }
    }

    if (mInactiveTimer > 0) {
        mInactiveTimer -= 1;
        return;
    }

    if (mLaunchTimer > 0 && mPlayer) {
        launchPlayer(mPlayer);
        return;
    }

    for (int i = 0; i < mNumObjs; ++i) {
        THitActor *actor = mCollidingObjs[i];
        if (actor->mObjectID != OBJECT_ID_MARIO)
            continue;


        auto *player = reinterpret_cast<TMario *>(actor);
        auto *controller = player->mController;

        if (mPlayer && mPlayer != actor)
            continue;

        if (player->mState == TMario::STATE_G_POUND)
            continue;

        if (attractPlayer(player)) {
            mPlayer = player;
            if ((controller->mButtons.mFrameInput & TMarioGamePad::B) != 0) {
                releasePlayer(player);
            } else if ((controller->mButtons.mFrameInput & TMarioGamePad::A) != 0) {
                playSpinAnim();
                launchPlayer(player);
            }
        }
    }

}

void TLaunchStarObj::calcRootMatrix() {
    TMapObjGeneral::calcRootMatrix();
}

bool TLaunchStarObj::attractPlayer(TMario *player) {
    if (player->mState != PlayerLaunchStarWait) {
        player->changePlayerStatus(PlayerLaunchStarWait, 0, false);
    }

    TVec3f diff = mTranslation - player->mTranslation;
    TVec3f diffXZ = {diff.x, 0.0f, diff.z};
    diff.y += (sinf(angleToRadians(static_cast<f32>(player->mSubStateTimer) * 1.5f)) *
               (200 - diff.magnitude()) * 0.25f) - 50;
    diff.scale(0.04f);
    player->mTranslation += diff;

    return diffXZ.magnitude() < 50.0f;
}

void TLaunchStarObj::launchPlayer(TMario *player) {
    if (mLaunchTimer > 92) {
        auto *rail = gpConductor->getGraphByName(mTravelRail);
        if (rail && false) {
            playCameraDemo(mCameraDemo, gpCamera);
            assignPlayerToRail(player, gpConductor->getGraphByName(mTravelRail));
        } else {
            player->mSpeed = TVec3f::up();
            player->mSpeed.scale(180.0f);
            player->changePlayerStatus(PlayerLaunchStarLaunch, 0, false);
            player->startVoice(TMario::VOICE_CANNON_WAIL);
        }
        mPlayer        = nullptr;
        mInactiveTimer = 60;
        mLaunchTimer   = 0;
        return;
    }
    if (mLaunchTimer < 80)
        player->mModelAngleY = -(mLaunchTimer * 11.25) * 182;
    else
        player->mModelAngleY = 0;
    player->mTranslation.y -= (400.0f - (mTranslation.y - player->mTranslation.y + 200.0f)) / 30.0f;
    mLaunchTimer += 1;
}

void TLaunchStarObj::releasePlayer(TMario *player) {
    player->changePlayerStatus(TMario::STATE_FALL, 0, false);
    mInactiveTimer = 60;
}

void TLaunchStarObj::playCameraDemo(const char *fp, CPolarSubCamera *camera) {
    if (!fp || !camera)
        return;

    TVec3f *pos;
    camera->startDemoCamera(fp, nullptr, -1, 0.0f, true);
}

void TLaunchStarObj::assignPlayerToRail(TMario *player, TGraphWeb *rail) {
    if (!rail)
        return;
}

void TLaunchStarObj::playSpinAnim() {
    mActorData->setBck("launch_star_spin");
    auto *ctrl       = mActorData->getFrameCtrl(MActor::BCK);
    ctrl->mAnimState = J3DFrameCtrl::ONCE;
    mModelLoadFlags &= ~0x100;
}

void TLaunchStarObj::playIdleAnim() {
    mActorData->setBck("launch_star_idle");
    auto *ctrl       = mActorData->getFrameCtrl(MActor::BCK);
    ctrl->mAnimState = J3DFrameCtrl::LOOP;
    mModelLoadFlags &= ~0x100;
}

static hit_data launch_star_hit_data{.mAttackRadius  = 300.0f,
                                     .mAttackHeight  = 400.0f,
                                     .mReceiveRadius = 300.0f,
                                     .mReceiveHeight = 400.0f};

static obj_hit_info launch_star_collision_data{
    ._00 = 1, .mType = 0xFC000000, ._08 = 0, .mHitData = &launch_star_hit_data};

static sound_data launch_star_sound_data{
    ._00             = 0xFFFFFFFF,
    ._04             = 0xFFFFFFFF,
    ._08             = 0xFFFFFFFF,
    .mUnkSoundID     = 0xFFFFFFFF,
    .mReboundSoundID = 0xFFFFFFFF,
    ._14             = 0xFFFFFFFF,
    ._18             = 0xFFFFFFFF,
    ._1C             = 0xFFFFFFFF,
    ._20             = 0xFFFFFFFF,
    ._24             = 0xFFFFFFFF,
};

static sound_info launch_star_sound_info{.mLength = 10, .mSoundData = &launch_star_sound_data};

ObjData launchStarData{.mMdlName  = "launch_star",
                    .mObjectID = 0x80000400,
                    .mLiveManagerName = gLiveManagerName,
                    .mObjKey           = nullptr,
                    .mAnimInfo         = nullptr,
                    .mObjCollisionData = &launch_star_collision_data,
                    .mMapCollisionInfo = nullptr,
                    .mSoundInfo        = &launch_star_sound_info,
                    .mPhysicalInfo     = nullptr,  //&tornado_physical_info,
                    .mSinkData         = nullptr,
                    ._28               = nullptr,
                    .mBckMoveData      = nullptr,
                    ._30               = 50.0f,
                    .mUnkFlags         = 0x10004000,
                    .mKeyCode          = cexp_calcKeyCode("LaunchStar")};

BETTER_SMS_FOR_CALLBACK bool holdPlayerState(TMario *player) {
    auto *controller = player->mController;
    if ((controller->mButtons.mFrameInput & TMarioGamePad::L) != 0) {
        player->changePlayerStatus(TMario::STATE_G_POUND, 0, false);
        return true;
    }

    player->mSpeed.scale(0.92f);
    player->mSubStateTimer += 1;
    //player->mTranslation += player->mSpeed;

    float normalThing = player->mFloorTriangle ? player->mFloorTriangle->mNormal.y : 0.0f;
    TVec3f succ{player->mTranslation.x + (player->mSpeed.x * normalThing * 0.25f),
                player->mTranslation.y,
                player->mTranslation.z + (player->mSpeed.z * normalThing * 0.25f)};
    //player->checkCollision();

    player->setAnimation(TMario::ANIMATION_FALL, 1.0f);

    return false;
}

BETTER_SMS_FOR_CALLBACK bool launchPlayerState(TMario *player) {
    /*TVec3f succ{player->mTranslation.x + (player->mSpeed.x * 0.25f),
                player->mTranslation.y,
                player->mTranslation.z + (player->mSpeed.z * 0.25f)};

    player->checkGroundAtJumping(*reinterpret_cast<Vec *>(&succ), 0);*/

    player->setAnimation(TMario::ANIMATION_SPINJUMP, 0.1f);

    player->mActionState |= 0x80;
    player->jumpProcess(0);
    player->checkCollision();

    player->mModelAngleY = (player->mSubStateTimer++ * 5.625) * 182;

    // WAIST = 2
    {
        auto *main_trail =
            gpMarioParticleManager->emitAndBindToPosPtr(270, &player->mTranslation, 1, player);
        if (main_trail) {
            main_trail->mSize1 = {1.0f, 1.0f, 1.0f};
            main_trail->mSize3 = {1.0f, 1.0f, 1.0f};
        }
    }


    {
        Mtx *lh_mtx    = &player->mModelData->mModel->mJointArray[0x18];
        auto *trail_lh = gpMarioParticleManager->emitAndBindToMtxPtr(397, *lh_mtx, 1, ((u8 *)player));
        if (trail_lh) {
            trail_lh->mSize1 = {0.3f, 0.3f, 0.3f};
            trail_lh->mSize3 = {0.3f, 0.3f, 0.3f};
        }
    }

    {
        Mtx *rh_mtx     = &player->mModelData->mModel->mJointArray[0x12];
        auto *trail_rh = gpMarioParticleManager->emitAndBindToMtxPtr(397, *rh_mtx, 1, ((u8 *)player) + 0x100);
        if (trail_rh) {
            trail_rh->mSize1 = {0.3f, 0.3f, 0.3f};
            trail_rh->mSize3 = {0.3f, 0.3f, 0.3f};
        }
    }

    player->mPrevSpeed = {0.0f, 0.0f, 0.0f};
    player->mForwardSpeed = 0.0f;

    if (SMS_IsMarioTouchGround4cm__Fv() && player->mSpeed.y <= 0.0f) {
        player->changePlayerStatus(TMario::STATE_D_LAND, 0, false);
        return true;
    }

    if (player->mSpeed.y <= 0.0f) {
        player->changePlayerStatus(TMario::STATE_FALL, 0, false);
    }

    return false;
}