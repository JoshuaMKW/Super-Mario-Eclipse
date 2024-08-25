#include <SMS/Camera/CameraShake.hxx>
#include <SMS/Manager/FlagManager.hxx>

#include <BetterSMS/libs/constmath.hxx>

#include "camera.hxx"
#include "enemy/bowser_car.hxx"

extern TBossBowserCar *gpBowserCar;

static size_t getCurrentFrame() { return gpBowserCar->mSpineBase->mNerveTimer; }

static TVec3f sFixedCameraPos              = {};
static TVec3f sFixedCameraLookAt           = {};
static f32 sFixedCameraFovy                = 50.0f;
static u32 sFixedCameraState               = 0;
static f32 sFixedCameraShakeStrength       = 0.0f;
static f32 sFixedCameraShakeStrengthTarget = 0.0f;
static f32 sFixedCameraShakePhase          = 0.0f;

void ControlBowserPrekillCam_(CPolarSubCamera *camera) {
    f32 angle      = DEGREES_TO_RADIANS(gpBowserCar->mRotation.y);
    TVec3f forward = {sinf(angle), 0.0f, cosf(angle)};

    if (getCurrentFrame() == 0) {
        sFixedCameraFovy = 45.0f;
    }

    TVec3f delta = forward;
    delta.scale(2800.0f * (45.0f / sFixedCameraFovy));

    sFixedCameraPos   = gpBowserCar->mTranslation + delta;
    sFixedCameraPos.y += 400.0f;

    sFixedCameraLookAt   = gpBowserCar->mTranslation;
    sFixedCameraLookAt.y += 200.0f * (sFixedCameraFovy / 45.0f);

    if (getCurrentFrame() < 560) {
        sFixedCameraFovy += 0.08f;
    }
}

void ControlBowserKillCam_(CPolarSubCamera *camera) {
    if (getCurrentFrame() == 0) {
        sFixedCameraShakePhase          = 0.0f;
        sFixedCameraShakeStrength       = 1.0f;
        sFixedCameraShakeStrengthTarget = 0.0f;
        gpCameraShake->startShake(UnknownShake27, 1.0f);
    }

    sFixedCameraLookAt   = gpBowserCar->mTranslation;
    sFixedCameraLookAt.y += 200.0f * (sFixedCameraFovy / 45.0f);

    if (getCurrentFrame() > 800) {
        sFixedCameraState = 2;
        TFlagManager::smInstance->setFlag(0x6001B, false);
    }
}

static bool sCanSwitchMode = false;

void ControlBowserKillCam(CPolarSubCamera *camera) {
    camera->mTranslation    = sFixedCameraPos;
    camera->mTargetPos      = sFixedCameraLookAt;
    camera->mProjectionFovy = sFixedCameraFovy;

    camera->mAnglePitch = 0;
    camera->mAngleYaw   = convertAngleFloatToS16(-gpBowserCar->mRotation.y);

    C_MTXPerspective(camera->mProjectionMatrix, camera->mProjectionFovy, camera->mProjectionAspect,
                     camera->mProjectionNear, camera->mProjectionFar);
    C_MTXLookAt(camera->mTRSMatrix, camera->mTranslation, TVec3f::up(), camera->mTargetPos);

    camera->mWorldTranslation = camera->mTranslation;

    *(f32 *)((u8 *)camera + 0x130) = camera->mTranslation.x;
    *(f32 *)((u8 *)camera + 0x134) = camera->mTranslation.y;
    *(f32 *)((u8 *)camera + 0x138) = camera->mTranslation.z;

    *(f32 *)((u8 *)camera + 0x148) = camera->mTargetPos.x;
    *(f32 *)((u8 *)camera + 0x14C) = camera->mTargetPos.y;
    *(f32 *)((u8 *)camera + 0x150) = camera->mTargetPos.z;

    *(f32 *)((u8 *)camera + 0x154) = camera->mTargetPos.x;
    *(f32 *)((u8 *)camera + 0x158) = camera->mTargetPos.y;
    *(f32 *)((u8 *)camera + 0x15C) = camera->mTargetPos.z;

    gpCameraShake->execShake(camera->mTranslation, (TVec3f *)((u8 *)camera + 0x148),
                             &camera->mUpVector);

    sCanSwitchMode = true;
}