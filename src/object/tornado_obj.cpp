#include <Dolphin/types.h>

#include <JSystem/JDrama/JDRNameRef.hxx>

#include <SMS/Manager/MarioParticleManager.hxx>
#include <SMS/MapObj/MapObjInit.hxx>
#include <SMS/MapObj/MapObjRail.hxx>
#include <SMS/raw_fn.hxx>

#include <BetterSMS/libs/constmath.hxx>
#include <BetterSMS/module.hxx>
#include <BetterSMS/player.hxx>

#include "object/tornado_obj.hxx"

TTornadoMapObj::TTornadoMapObj(const char *name)
    : TRailMapObj(name), mBlowStrength(1.0f), mTrueRotation(0.0f, 0.0f, 0.0f) {}

TTornadoMapObj::~TTornadoMapObj() {}

void TTornadoMapObj::load(JSUMemoryInputStream &stream) {
    TRailMapObj::load(stream);
    stream.read(&mBlowStrength, 4);
    stream.read(&mHasFinalPush, 1);
}

void TTornadoMapObj::perform(u32 flags, JDrama::TGraphics *graphics) {
    TRailMapObj::perform(flags, graphics);
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

void TTornadoMapObj::control() {
    TRailMapObj::control();

    mTrueRotation.y += mBlowStrength;
    mWispRotation.y += mBlowStrength * 1.7f;

    clampRotation(mTrueRotation);
    clampRotation(mWispRotation);

    mRotation = mTrueRotation;

    mSpeed.set(0.0f, 0.0f, 0.0f);
    mGravity = 0.0f;

    for (int i = 0; i < mNumObjs; ++i) {
        THitActor *actor = mCollidingObjs[i];
        if (actor->mObjectID != OBJECT_ID_MARIO) {
            TLiveActor *blowObj = reinterpret_cast<TLiveActor *>(actor);
            blowUp(blowObj);
        } else {
            TMario *player = reinterpret_cast<TMario *>(actor);
            blowUp(player);
        }
    }
}

void TTornadoMapObj::calcRootMatrix() {
    TRailMapObj::calcRootMatrix();

    mWispPos[0] = mTranslation;
    mWispPos[1] = mTranslation;
    mSpinPos    = mTranslation;

    f32 c_r  = cosf(angleToRadians(mWispRotation.y));
    f32 s_r  = sinf(angleToRadians(mWispRotation.y));
    f32 s_r2 = sinf(angleToRadians(mWispRotation.y * 2)) * 0.25f;

    f32 p_scale = scaleLinearAtAnchor((s_r2 * 2.0f + 0.5f), 0.2f, 1.0f);
    f32 n_scale = scaleLinearAtAnchor((-s_r2 * 2.0f + 0.5f), 0.2f, 1.0f);

    mWispPos[0].x += -s_r * (600.0f * p_scale) * mScale.x;
    mWispPos[0].y += s_r2 * 200.0f * mScale.y + 130.0f * mScale.y;
    mWispPos[0].z += -c_r * (600.0f * p_scale) * mScale.z;

    mWispPos[1].x += s_r * (600.0f * n_scale) * mScale.x;
    mWispPos[1].y += -s_r2 * 200.0f * mScale.y + 130.0f * mScale.y;
    mWispPos[1].z += c_r * (600.0f * n_scale) * mScale.z;

    mSpinPos.y -= 200.0f * mScale.y;

    {
        auto *particle =
            gpMarioParticleManager->emitAndBindToPosPtr(394, &mWispPos[0], 1, &mWispPos[0]);
        if (particle) {
            particle->mSize1 = mScale;
            particle->mSize1.scale(p_scale);
        }
    }

    {
        auto *particle =
            gpMarioParticleManager->emitAndBindToPosPtr(394, &mWispPos[1], 1, &mWispPos[1]);
        if (particle) {
            particle->mSize1 = mScale;
            particle->mSize1.scale(n_scale);
        }
    }

    {
        auto *particle = gpMarioParticleManager->emitAndBindToPosPtr(393, &mSpinPos, 1, this);
        if (particle) {
            particle->mSize1 = mScale;
            particle->mSize1.scale(0.5f);
            particle->mSize3 = mScale;
            particle->mSize3.scale(0.5f);
        }
    }
}

bool TTornadoMapObj::blowUp(TLiveActor *actor) {
    f32 startBlowHeight = mTranslation.y - (mReceiveHeight / 2);

    if (actor->mTranslation.y < startBlowHeight)
        return false;

    const f32 progress = (actor->mTranslation.y - startBlowHeight) / (mAttackHeight * 2);

    if (progress > 1.0f && actor->mSpeed.y >= 0.0f) {
        if (mHasFinalPush)
            actor->mSpeed.y += 1.0f * mBlowStrength;
        return true;
    }

    f32 factor;
    if (mHasFinalPush)
        factor = mBlowStrength;
    else
        factor = mBlowStrength * scaleLinearAtAnchor<f32>(1 - progress, 0.5f, 1.0f);

    actor->mSpeed.y += 10.0f * factor;
    return false;
}

bool TTornadoMapObj::blowUp(TMario *actor) {
    auto *playerData    = Player::getData(actor);
    f32 startBlowHeight = mTranslation.y - mAttackHeight;

    if (actor->mTranslation.y < startBlowHeight)
        return false;

    const f32 progress = (actor->mTranslation.y - startBlowHeight) / (mAttackHeight * 1.5f);

    if ((actor->mState & 0x800000) == 0 &&
        actor->mState != static_cast<u32>(TMario::STATE_DIVEJUMP) &&
        actor->mState != static_cast<u32>(TMario::STATE_HOVER) && mBlowStrength >= 0.5f) {
        playerData->mCollisionFlags.mIsSpinBounce = true;
        playerData->setCanSprayFludd(true);
        actor->mState = static_cast<u32>(TMario::STATE_JUMPSPINL);
    }

    f32 factor;
    if (mHasFinalPush)
        factor = mBlowStrength;
    else
        factor = mBlowStrength * scaleLinearAtAnchor<f32>(1 - progress, 0.5f, 1.0f);

    if (((mTranslation.y + mAttackHeight) - actor->mTranslation.y) < 10.0f &&
        actor->mSpeed.y >= 0.0f) {
        if (mHasFinalPush)
            actor->mSpeed.y += 10.0f * factor;
        return true;
    }

    actor->mSpeed.y += 1.5f * factor;
    return false;
}

static hit_data tornado_hit_data{.mAttackRadius  = 450.0f,
                                 .mAttackHeight  = 450.0f,
                                 .mReceiveRadius = 450.0f,
                                 .mReceiveHeight = 450.0f};

static obj_hit_info tornado_collision_data{
    ._00 = 1, .mType = 0xFC000000, ._08 = 0, .mHitData = &tornado_hit_data};

static sound_data tornado_sound_data{
    ._00             = 0x20B5,
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

static sound_info tornado_sound_info{.mLength = 10, .mSoundData = &tornado_sound_data};

static ObjPhysicalData tornado_physical_data{
    .mGravity            = 0.0f,
    .mFloorBounceSpeed   = 0.0f,
    .mWallBounceSpeed    = 0.0f,
    .mRotationStopFactor = 0.0f,
    .mFloorBrakeFactor   = 0.0f,
    .mRollBrakeFactor    = 0.0f,
    .mAirBrakeFactor     = 1.0f,
    .mRollBrakeFactor2   = 5.0f,
    ._20                 = {0.5f, 0.7f, 0.0f},
    .mThrowDistance      = 2.0f,
    .mThrowHeight        = 15.0f
};

static ObjPhysicalInfo tornado_physical_info{
    ._00 = 13, .mPhysicalData = &tornado_physical_data, ._0C = 2};

ObjData tornadoData{.mMdlName  = "tornado",
                    .mObjectID = 0x80000400,
                    .mLiveManagerName = gLiveManagerName,  // const_cast<char *>("木マネージャー")
                    .mObjKey           = nullptr,          // const_cast<char *>("waterballoon"),
                    .mAnimInfo         = nullptr,
                    .mObjCollisionData = &tornado_collision_data,
                    .mMapCollisionInfo = nullptr,
                    .mSoundInfo        = &tornado_sound_info,
                    .mPhysicalInfo     = nullptr,  //&tornado_physical_info,
                    .mSinkData         = nullptr,
                    ._28               = nullptr,
                    .mBckMoveData      = nullptr,
                    ._30               = 50.0f,
                    .mUnkFlags         = 0x10004000,
                    .mKeyCode          = cexp_calcKeyCode("Tornado")};