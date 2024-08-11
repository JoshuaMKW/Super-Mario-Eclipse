#include "object/cannonball.hxx"

#include <SMS/Enemy/Conductor.hxx>
#include <SMS/MSound/MSoundSESystem.hxx>
#include <SMS/Manager/MarioParticleManager.hxx>
#include <SMS/Map/MapCollisionData.hxx>
#include <SMS/raw_fn.hxx>

Mtx44 *TCannonBall::getTakingMtx() { return TMapObjBall::getTakingMtx(); }

void TCannonBall::makeMActors() {
    mActorKeeper = new TMActorKeeper(mLiveManager, 1);
    if (mModelLoadFlags & 0x8000)
        mActorKeeper->mModelFlags = 0x112F0000;
    else
        mActorKeeper->mModelFlags = 0x102F0000;

    char modelPath[128];
    snprintf(modelPath, 128, "%s.bmd", mModelName);

    mActorData = mActorKeeper->createMActor(modelPath, getSDLModelFlag());

    if (mModelLoadFlags & 0x4000) {
        mActorData->setLightID(0);
        mActorData->_40 = 0;
    }

    calcRootMatrix();
    mActorData->calc();
    mActorData->viewCalc();
}

void TCannonBall::checkGroundCollision(TVec3f *out) {
    mGroundY = gpMapCollisionData->checkGround(out->x, out->y + _C0, out->z, 8, &m_water_surface);
    if (mGroundY >= out->y && m_water_surface->isWaterSurface()) {
        touchWaterSurface();
    }

    if (mState == 6) {
        m_was_in_air = true;
    }

    mGroundY = gpMapCollisionData->checkGround(out->x, out->y + _C0, out->z, 1, &mFloorBelow);
    if (mGroundY < out->y) {
        m_was_in_air = true;
        if (mRidingInfo.mHitActor == nullptr) {
            mStateFlags.asU32 |= 0x80;
        }
    } else {
        touchGround(out);
    }
}

void TCannonBall::touchGround(TVec3f *out) {
    TMapObjBall::touchGround(out);

    if (m_was_in_air && mState != 6) {
        if (gpMSound->gateCheck(MSD_SE_NPC_MONTE_C_METAL_DW)) {
            MSoundSE::startSoundActor(MSD_SE_NPC_MONTE_C_METAL_DW, mTranslation, 0, nullptr, 0, 4);
        }

        if (gpMSound->gateCheck(MSD_SE_MA_HIP_ATTACK)) {
            MSoundSE::startSoundActor(MSD_SE_MA_HIP_ATTACK, mTranslation, 0, nullptr, 0, 4);
        }

        gpMarioParticleManager->emit(20, &mTranslation, 0, nullptr);
        gpMarioParticleManager->emit(19, &mTranslation, 0, nullptr);
        gpMarioParticleManager->emit(18, &mTranslation, 0, nullptr);

        if (mFloorBelow->mOwner) {
            if (mFloorBelow->mOwner->mObjectID == 0x40000F90) {
                mFloorBelow->mOwner->receiveMessage(this, 0);
                makeObjDead();
            }
        }

        m_was_in_air = false;
    }

    // if (mState == 6) {
    //     return;
    // }

    // if (gpMSound->gateCheck(MSD_SE_NPC_MONTE_C_METAL_DW)) {
    //     MSoundSE::startSoundActor(MSD_SE_NPC_MONTE_C_METAL_DW, mTranslation, 0, nullptr, 0, 4);
    // }
}

void TCannonBall::touchWall(TVec3f *out, TBGWallCheckRecord *record) {
    TMapObjBall::touchWall(out, record);

    // if (mState == 6) {
    //     return;
    // }

    // if (gpMSound->gateCheck(MSD_SE_NPC_MONTE_C_METAL_DW)) {
    //     MSoundSE::startSoundActor(MSD_SE_NPC_MONTE_C_METAL_DW, mTranslation, 0, nullptr, 0, 4);
    // }
}

void TCannonBall::touchRoof(TVec3f *out) {
    TMapObjBall::touchRoof(out);

    // if (mState == 6) {
    //     return;
    // }

    // if (gpMSound->gateCheck(MSD_SE_NPC_MONTE_C_METAL_DW)) {
    //     MSoundSE::startSoundActor(MSD_SE_NPC_MONTE_C_METAL_DW, mTranslation, 0, nullptr, 0, 4);
    // }
}

static const char *s_column_water_name = "\x83\x47\x83\x74\x83\x46\x83\x4E\x83\x67\x90\x85\x92\x8C"
                                         "\x83\x7D\x83\x6C\x81\x5B\x83\x57\x83\x83\x81\x5B";

void TCannonBall::touchWaterSurface() {
    TVec3f scale(1.0f, 1.0f, 1.0f);
    scale *= mScale;

    emitAndScale(0x1F6, 3, &mTranslation, scale);
    emitAndScale(0x1C6, 1, &mTranslation, scale);

    if (mSpeed.y < -15.0f) {
        void *columwater = gpConductor->makeOneEnemyAppear(mTranslation, s_column_water_name, 1);
        if (columwater) {
            generate__17TEffectColumWaterFRQ29JGeometry8TVec3_f(columwater, mTranslation, mScale);
        }
    }
}

static hit_data cannonBall_hit_data{.mAttackRadius  = 50.0f,
                                    .mAttackHeight  = 50.0f,
                                    .mReceiveRadius = 50.0f,
                                    .mReceiveHeight = 50.0f};

static obj_hit_info cannonBall_collision_data{
    ._00 = 1, .mType = 0xDC000000, ._08 = 0, .mHitData = &cannonBall_hit_data};

static sound_data cannonBall_sound_data{
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

static sound_info cannonBall_sound_info{.mLength = 10, .mSoundData = &cannonBall_sound_data};

static ObjPhysicalData cannonBall_physical_data{
    .mGravity            = 0.4f,
    .mFloorBounceSpeed   = 0.0f,
    .mWallBounceSpeed    = 0.0f,
    .mRotationStopFactor = 0.15f,
    .mFloorBrakeFactor   = 0.95f,
    .mRollBrakeFactor    = 0.95f,
    .mAirBrakeFactor     = 1.0f,
    .mRollBrakeFactor2   = 1.0f,
    ._20                 = {0.0f, 0.7f, 0.6f},
    .mThrowDistance      = 1.8f,
    .mThrowHeight        = 12.0f
};

static ObjPhysicalInfo cannonBall_physical_info{
    ._00 = 13, .mPhysicalData = &cannonBall_physical_data, ._0C = 2};

ObjData cannonBallData{.mMdlName  = "cannonball",
                       .mObjectID = 0x400003A1 /*0x80000FFF*/,
                       .mLiveManagerName =
                           gLiveManagerName,  // const_cast<char *>("木マネージャー")
                       .mObjKey           = gUnkManagerName,  // const_cast<char *>("cannonball"),
                       .mAnimInfo         = nullptr,
                       .mObjCollisionData = &cannonBall_collision_data,
                       .mMapCollisionInfo = nullptr,
                       .mSoundInfo        = &cannonBall_sound_info,
                       .mPhysicalInfo     = &cannonBall_physical_info,
                       .mSinkData         = nullptr,
                       ._28               = nullptr,
                       .mBckMoveData      = nullptr,
                       ._30               = 50.0f,
                       .mUnkFlags         = 0x02130000 /*0x02130100*/,
                       .mKeyCode          = cexp_calcKeyCode("CannonBall")};

BETTER_SMS_FOR_CALLBACK void cannonBallCollideInteractor(THitActor* self, TMario* player) {
    player->keepDistance(*self, 0.0f);
    if (player->canTake(self)) {
        player->mGrabTarget = (TTakeActor *)self;
        player->changePlayerStatus(899, 0, false);
    }
}