#include <Dolphin/GX.h>
#include <Dolphin/MTX.h>
#include <Dolphin/types.h>

#include <JSystem/J3D/J3DModel.hxx>
#include <JSystem/J3D/J3DModelLoaderDataBase.hxx>
#include <JSystem/JDrama/JDRNameRef.hxx>
#include <SMS/M3DUtil/MActor.hxx>
#include <SMS/M3DUtil/MActorKeeper.hxx>
#include <SMS/MSound/MSoundSESystem.hxx>
#include <SMS/Manager/ModelWaterManager.hxx>
#include <SMS/Map/BGCheck.hxx>
#include <SMS/Player/Mario.hxx>
#include <SMS/rand.h>

#include <BetterSMS/libs/constmath.hxx>
#include <BetterSMS/module.hxx>

#include "object/follow_key.hxx"
#include "player.hxx"

TFollowKey::TFollowKey(const char *name)
    : TMapObjGeneral(name), mKeyState(State::KEY_IDLE), mFollowActor(nullptr), mChestActor(nullptr),
      mUnlockSoundFrame(0) {
    mPrevPos = TVec3f::zero();
}

void TFollowKey::loadAfter() { playIdleAnim(); }

void TFollowKey::control() {
    TMapObjGeneral::control();

    // Early exit if the object is dead.
    if (mStateFlags.asFlags.mIsObjDead) {
        return;
    }

    mPrevPos = mTranslation;

    auto *particle =
        gpMarioParticleManager->emitAndBindToMtxPtr(297, mActorData->mModel->mBaseMtx, 1, this);
    if (particle) {
        // particle->mSize1 = {5.0f, 5.0f, 5.0f};
    }

    switch (mKeyState) {
    case State::KEY_IDLE:
        for (size_t i = 0; i < mNumObjs; i++) {
            if (mCollidingObjs[i]->mObjectID == OBJECT_ID_MARIO) {
                mFollowActor = mCollidingObjs[i];

                TMario *player = reinterpret_cast<TMario *>(mFollowActor);
                SME::Player::getEclipseData(player)->mIsHoldingKey = true;

                mScale    = TVec3f(mScale.x * .75f, mScale.y * .75f, mScale.z * .75f);
                mKeyState = State::KEY_FOLLOW;

                // Play the collection sound.
                if (gpMSound->gateCheck(0x2988)) {
                    MSoundSE::startSoundActor(0x2988, mTranslation, 0, nullptr, 0, 4);
                }

                break;
            }
        }
        break;
    case State::KEY_FOLLOW:
        followActor(mFollowActor);
        break;
    case State::KEY_OPEN_CHEST:
        calcOpenChestSeq(mChestActor);
        break;
    case State::KEY_USED:
        calcUsedSeq(mChestActor);
        break;
    default:
        break;
    }
}

bool TFollowKey::receiveMessage(THitActor *sender, u32 message) {
    if (TMapObjGeneral::receiveMessage(sender, message)) {
        return true;
    }

    if (message == MESSAGE_KEY_OPEN_CHEST) {
        mKeyState   = State::KEY_OPEN_CHEST;
        mChestActor = sender;
        return true;
    }

    return false;
}

void TFollowKey::playIdleAnim() {
    mActorData->setBck("key_rotate");
    auto *ctrl       = mActorData->getFrameCtrl(MActor::BCK);
    ctrl->mAnimState = J3DFrameCtrl::LOOP;
    mModelLoadFlags &= ~0x100;
}

#define MAX_SPIN_SPEED 40.0f

void TFollowKey::calcOpenChestSeq(THitActor *actor) {
    // Make the key spin faster over time.
    J3DFrameCtrl *ctrl = mActorData->getFrameCtrl(MActor::BCK);
    ctrl->mFrameRate   = Min(ctrl->mFrameRate + 0.2f, MAX_SPIN_SPEED);

    // Move the key up to the chest.
    TVec3f targetPos = actor->mTranslation;
    targetPos.y += 300.0f;

    float mLerpX = mTranslation.x * (1 - .08f) + (targetPos.x * .08f);
    float mLerpY = mTranslation.y * (1 - .08f) + (targetPos.y * .08f);
    float mLerpZ = mTranslation.z * (1 - .08f) + (targetPos.z * .08f);

    mTranslation = TVec3f(mLerpX, mLerpY, mLerpZ);

    // If the key is close enough to the chest, stop moving and mark used.
    if (PSVECDistance(mTranslation, targetPos) < 10.0f) {
        mKeyState = State::KEY_USED;
    }
}

void TFollowKey::calcUsedSeq(THitActor *actor) {
    // Make the key spin faster over time.
    J3DFrameCtrl *ctrl = mActorData->getFrameCtrl(MActor::BCK);
    ctrl->mFrameRate   = Min(ctrl->mFrameRate + 0.2f, MAX_SPIN_SPEED);

    auto *particle =
        gpMarioParticleManager->emitAndBindToMtxPtr(296, mActorData->mModel->mBaseMtx, 1, this);
    if (particle) {
        particle->mSize1 = {ctrl->mFrameRate / MAX_SPIN_SPEED, ctrl->mFrameRate / MAX_SPIN_SPEED,
                            ctrl->mFrameRate / MAX_SPIN_SPEED};
    }

    // Play chest unlock sound at random.
    if (rand() < 0x800 && gpMSound->gateCheck(0x3883)) {
        MSoundSE::startSoundActor(0x3883, mTranslation, 0, nullptr, 0, 4);
    }

    // If the key is spinning fast enough, mark it as dead and open the chest.
    if (ctrl->mFrameRate >= MAX_SPIN_SPEED) {
        mStateFlags.asFlags.mIsObjDead = true;
        auto *particle                 = gpMarioParticleManager->emitAndBindToMtxPtr(
            297, mActorData->mModel->mBaseMtx, 30, nullptr);
        if (particle) {
            particle->mSize1 = {3.0f, 3.0f, 3.0f};
        }
        mChestActor->receiveMessage(this, MESSAGE_KEY_OPEN_CHEST);
    }
}

void TFollowKey::followActor(THitActor *actor) {
    float yAngle  = angleToRadians(actor->mRotation.y) + M_PI;
    float xOffset = sinf(yAngle) * 150.0f;
    float yOffset = cosf(yAngle) * 150.0f;

    TVec3f newLoc = TVec3f(xOffset, 60.0f, yOffset);
    newLoc += actor->mTranslation;

    float mLerpX = mPrevPos.x * (1 - .10f) + (newLoc.x * .10f);
    float mLerpZ = mPrevPos.z * (1 - .10f) + (newLoc.z * .10f);
    float mLerpY = mPrevPos.y * (1 - .10f) + (newLoc.y * .10f);

    mTranslation = TVec3f(mLerpX, mLerpY, mLerpZ);

    mSpeed   = TVec3f::zero();
    mGravity = 0.0f;
}

static hit_data follow_key_hit_data{.mAttackRadius  = 100.0f,
                                    .mAttackHeight  = 100.0f,
                                    .mReceiveRadius = 100.0f,
                                    .mReceiveHeight = 100.0f};

static obj_hit_info follow_key_collision_data{
    ._00 = 1, .mType = 0xFC000000, ._08 = 0, .mHitData = &follow_key_hit_data};

ObjData followKeyData{.mMdlName  = "key_follow",
                      .mObjectID = 0x80000400 /*0x80000FFF*/,
                      .mLiveManagerName = gLiveManagerName,  // const_cast<char *>("木マネージャー")
                      .mObjKey           = gUnkManagerName,  // const_cast<char *>("waterballoon"),
                      .mAnimInfo         = nullptr,
                      .mObjCollisionData = &follow_key_collision_data,
                      .mMapCollisionInfo = nullptr,
                      .mSoundInfo        = nullptr,
                      .mPhysicalInfo     = nullptr,
                      .mSinkData         = nullptr,
                      ._28               = nullptr,
                      .mBckMoveData      = nullptr,
                      ._30               = 50.0f,
                      .mUnkFlags         = 0x10004000 /*0x02130100*/,
                      .mKeyCode          = cexp_calcKeyCode("TreasureKey")};
