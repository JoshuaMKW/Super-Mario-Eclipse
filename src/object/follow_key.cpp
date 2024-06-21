#include <Dolphin/GX.h>
#include <Dolphin/MTX.h>
#include <Dolphin/types.h>

#include "player.hxx"
#include <BetterSMS/libs/constmath.hxx>
#include <BetterSMS/module.hxx>
#include <JSystem/J3D/J3DModel.hxx>
#include <JSystem/J3D/J3DModelLoaderDataBase.hxx>
#include <JSystem/JDrama/JDRNameRef.hxx>
#include <SMS/M3DUtil/MActor.hxx>
#include <SMS/M3DUtil/MActorKeeper.hxx>
#include <SMS/MSound/MSoundSESystem.hxx>
#include <SMS/Manager/ModelWaterManager.hxx>
#include <SMS/Map/BGCheck.hxx>
#include <SMS/Player/Mario.hxx>
#include <object/follow_key.hxx>
TFollowKey::TFollowKey(const char *name) : TMapObjGeneral(name), mIsFollowing(false) {
    prevPos = TVec3f::zero();
}

TFollowKey::~TFollowKey() {}

void TFollowKey::kill() {}
void TFollowKey::control() {
    TMapObjGeneral::control();

    if (!mIsFollowing) {

        for (size_t i = 0; i < mNumObjs; i++) {
            if (mCollidingObjs[i]->mObjectID == OBJECT_ID_MARIO) {

                mIsFollowing = true;

                break;
            }
        }
    } else {

        prevPos = mTranslation;
        mScale        = TVec3f(mScale.x * .75f, mScale.y * .75f, mScale.z * .75f);
        float yAngle  = angleToRadians(convertAngleS16ToFloat(mFollowActor->mAngle.y)) + M_PI;
        float xOffset = sinf(yAngle) * 200;
        float yOffset = cosf(yAngle) * 200;

        TVec3f newLoc = TVec3f(xOffset, 60.0f, yOffset);
        newLoc += mFollowActor->mTranslation;

       
        float mLerpX = prevPos.x * (1 - .55f) + (newLoc.x * .55f);
        float mLerpZ = prevPos.z * (1 - .55f) + (newLoc.z * .55f);
        float mLerpY = prevPos.y * (1 - .55f) + (newLoc.y * .55f);

        mTranslation = TVec3f(mLerpX, mLerpY, mLerpZ);

        prevPos = mTranslation;

        mSpeed   = TVec3f::zero();
        mGravity = 0.0f;

        auto *particle = gpMarioParticleManager->emitAndBindToPosPtr(6, &mTranslation, 1, this);
        if (particle) {

            particle->mSize1.scale(5.5f);
        }
    }
}

void TFollowKey::playIdleAnim() {
    mActorData->setBck("rotation_key");
    auto *ctrl       = mActorData->getFrameCtrl(MActor::BCK);
    ctrl->mAnimState = J3DFrameCtrl::LOOP;
    mModelLoadFlags &= ~0x100;
}

void TFollowKey::touchPlayer(THitActor *actor) {

    OSReport("Touched Actor ID %X: \n ", actor->mObjectID);

    if (actor->mObjectID == OBJECT_ID_MARIO) {
        mFollowActor        = reinterpret_cast<TMario *>(actor);
        auto *data          = SME::Player::getEclipseData(mFollowActor);
        data->mIsHoldingKey = true;
        mIsFollowing        = true;
    }
}
static hit_data follow_key_hit_data{.mAttackRadius  = 100.0f,
                                    .mAttackHeight  = 100.0f,
                                    .mReceiveRadius = 100.0f,
                                    .mReceiveHeight = 100.0f};

static obj_hit_info follow_key_collision_data{
    ._00 = 1, .mType = 0xFC000000, ._08 = 0, .mHitData = &follow_key_hit_data};

ObjData followKeyData{.mMdlName  = "follow_key",
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
                      .mKeyCode          = cexp_calcKeyCode("follow_key")};
