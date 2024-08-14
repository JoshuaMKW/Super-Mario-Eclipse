#include "object/pizzabox.hxx"

#include <SMS/Enemy/Conductor.hxx>
#include <SMS/MSound/MSoundSESystem.hxx>
#include <SMS/Manager/MarioParticleManager.hxx>
#include <SMS/Map/MapCollisionData.hxx>
#include <SMS/raw_fn.hxx>

#include <BetterSMS/libs/constmath.hxx>
#include <BetterSMS/libs/geometry.hxx>

using namespace BetterSMS;

void TPizzaBox::control() {
     TMapObjBall::control();
  
     if ((mTranslation.y - mGroundY) > 10.0f) {
         mRotationalVelocity *= 0.995f;
     } else {
         mRotationalVelocity *= 0.95f;
     }

     mRotation.y += mRotationalVelocity;
}

void TPizzaBox::calcRootMatrix() {
    TMapObjBall::calcRootMatrix();

    if (mState != 6 || mHolder == nullptr) {
        return;
    }

    const float *take_mtx = (const float *)mHolder->getTakingMtx();

    Mtx mtx;
    mtx[0][0] = (take_mtx + 0)[0];
    mtx[0][1] = (take_mtx + 0)[1];
    mtx[0][2] = (take_mtx + 0)[2];
    mtx[1][0] = (take_mtx + 4)[0];
    mtx[1][1] = (take_mtx + 4)[1];
    mtx[1][2] = (take_mtx + 4)[2];
    mtx[2][0] = (take_mtx + 8)[0];
    mtx[2][1] = (take_mtx + 8)[1];
    mtx[2][2] = (take_mtx + 8)[2];
    mtx[0][3] = 0.0f;
    mtx[1][3] = 0.0f;
    mtx[2][3] = 0.0f;

    TMario *player        = (TMario *)mHolder;
    J3DModel *playerModel = player->mModelData->mModel;

#if 1
    TVec3f forwardVec = {-mtx[2][0], mtx[2][1], mtx[2][2]};
    forwardVec.normalize();

    TVec3f rightVec = {-mtx[0][0], mtx[0][1], mtx[0][2]};
    rightVec.normalize();

    TVec3f upVec = {-mtx[1][0], mtx[1][1], mtx[1][2]};
    upVec.normalize();

    TVec3f rotAxis;
    PSVECCrossProduct(upVec, TVec3f::up(), rotAxis);

    f32 a = PSVECDotProduct(upVec, TVec3f::up());
    f32 b = PSVECMag(upVec) * PSVECMag(TVec3f::up());

    f32 angle = acosf(a / b);

    Mtx localToWorld;
    PSMTXRotAxisRad(localToWorld, rotAxis, angle);

    PSMTXMultVec(localToWorld, forwardVec, forwardVec);
    PSMTXMultVec(localToWorld, rightVec, rightVec);
#else
    TVec3f forwardVec = {sinf(angleToRadians(mHolder->mRotation.y)), 0.0f,
                         cosf(angleToRadians(mHolder->mRotation.y))};

    TVec3f rightVec;
    PSVECCrossProduct(forwardVec, TVec3f::up(), rightVec);
#endif

    J3DModel *model = getModel();

    Mtx rotMtxf;
    PSMTXRotAxisRad(rotMtxf, forwardVec, -M_PI / 4.257f);

    Mtx rotMtxr;
    PSMTXRotAxisRad(rotMtxr, rightVec, -M_PI / 2.17f);

    Mtx rotMtx;
    PSMTXConcat(rotMtxf, rotMtxr, rotMtx);

    PSMTXConcat(rotMtx, mtx, model->mBaseMtx);
    model->mBaseMtx[0][3] = (take_mtx + 0)[3] + (-10.0f * forwardVec.x);
    model->mBaseMtx[1][3] = (take_mtx + 4)[3] + 20.0f;
    model->mBaseMtx[2][3] = (take_mtx + 8)[3] + (-10.0f * forwardVec.z);
}

void TPizzaBox::touchWaterSurface() {}

void TPizzaBox::thrown() {
    TMapObjBall::thrown();
    mTranslation.y += 30.0f;
    mRotationalVelocity = 12.0f;
}

static hit_data pizzaBox_hit_data{.mAttackRadius  = 70.0f,
                                  .mAttackHeight  = 20.0f,
                                  .mReceiveRadius = 70.0f,
                                  .mReceiveHeight = 20.0f};

static obj_hit_info pizzaBox_collision_data{
    ._00 = 1, .mType = 0xDC000000, ._08 = 0, .mHitData = &pizzaBox_hit_data};

static sound_data pizzaBox_sound_data{
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

static sound_info pizzaBox_sound_info{.mLength = 10, .mSoundData = &pizzaBox_sound_data};

static ObjPhysicalData pizzaBox_physical_data{
    .mGravity            = 0.1f,
    .mFloorBounceSpeed   = 0.0f,
    .mWallBounceSpeed    = 0.8f,
    .mRotationStopFactor = 0.0f,
    .mFloorBrakeFactor   = 0.97f,
    .mRollBrakeFactor    = 0.97f,
    .mAirBrakeFactor     = 1.0f,
    .mRollBrakeFactor2   = 1.0f,
    ._20                 = {0.0f, 0.7f, 0.6f},
    .mThrowDistance      = 4.0f,
    .mThrowHeight        = 5.0f
};

static ObjPhysicalInfo pizzaBox_physical_info{
    ._00 = 13, .mPhysicalData = &pizzaBox_physical_data, ._0C = 2};

ObjData pizzaBoxData{.mMdlName  = "pizzabox",
                     .mObjectID = 0x400003A2 /*0x80000FFF*/,
                     .mLiveManagerName = gLiveManagerName,  // const_cast<char *>("木マネージャー")
                     .mObjKey           = gUnkManagerName,  // const_cast<char *>("cannonball"),
                     .mAnimInfo         = nullptr,
                     .mObjCollisionData = &pizzaBox_collision_data,
                     .mMapCollisionInfo = nullptr,
                     .mSoundInfo        = &pizzaBox_sound_info,
                     .mPhysicalInfo     = &pizzaBox_physical_info,
                     .mSinkData         = nullptr,
                     ._28               = nullptr,
                     .mBckMoveData      = nullptr,
                     ._30               = 50.0f,
                     .mUnkFlags         = 0x02130000 /*0x02130100*/,
                     .mKeyCode          = cexp_calcKeyCode("PizzaBox")};

BETTER_SMS_FOR_CALLBACK void pizzaBoxCollideInteractor(THitActor *self, TMario *player) {
    player->keepDistance(*self, 0.0f);
    if (player->canTake(self)) {
        player->mGrabTarget = (TTakeActor *)self;
        player->changePlayerStatus(899, 0, false);
    }
}