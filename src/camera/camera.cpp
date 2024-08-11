#include <SMS/System/MarDirector.hxx>

#include <BetterSMS/libs/constmath.hxx>
#include <BetterSMS/module.hxx>

#include "camera.hxx"

static bool sIsFixedCameraMode = false;
static bool sIsFixedCameraLerped = false;
static TVec3f sFixedCameraPos;
static TVec3f sFixedCameraLookAt;
static f32 sFixedCameraFovy = 50.0f;
static JDrama::TActor *sFixedCameraActor = nullptr;

bool SME::isCameraFixedMode() { return sIsFixedCameraMode; }
void SME::setCameraFixedMode(bool fixed, bool lerped) {
    sIsFixedCameraMode   = fixed;
    sIsFixedCameraLerped = lerped;
}

TVec3f SME::getCameraFixedPos() { return sFixedCameraPos; }
void SME::setCameraFixedPos(const TVec3f &pos) { sFixedCameraPos = pos; }

TVec3f SME::getCameraFixedLookAt() { return sFixedCameraLookAt; }
void SME::setCameraFixedLookAt(const TVec3f &lookat) { sFixedCameraLookAt = lookat; }

JDrama::TActor *SME::getCameraFixedFollowActor() { return sFixedCameraActor; }
void SME::setCameraFixedFollowActor(JDrama::TActor *actor) { sFixedCameraActor = actor; }

f32 SME::getCameraFixedFovy() { return sFixedCameraFovy; }
void SME::setCameraFixedFovy(f32 fovy) { sFixedCameraFovy = fovy; }

extern void ControlCamera2D(CPolarSubCamera *camera);

static void controlCameraByCodeExt(CPolarSubCamera *camera) {
    if (SME::isCameraFixedMode()) {
        *(s32 *)((u8 *)camera->mInbetween + 0x4) = 0;

        TVec3f followPos = sFixedCameraPos;
        if (sFixedCameraActor != nullptr) {
            TVec3f diff = camera->mTranslation - sFixedCameraActor->mTranslation;
            diff.y      = 0;
            diff.normalize();
            diff.scale(600.0f);
            diff.y    = 150.0f;
            followPos = sFixedCameraActor->mTranslation + diff;
        }

        TVec3f lookAtPos = sFixedCameraLookAt;
        if (sFixedCameraActor != nullptr) {
            lookAtPos = sFixedCameraActor->mTranslation;
            lookAtPos.y += 80.0f;
        }

        if (sIsFixedCameraLerped) {
            camera->mTranslation.x = lerp<f32>(camera->mTranslation.x, followPos.x, 0.05f);
            camera->mTranslation.y = lerp<f32>(camera->mTranslation.y, followPos.y, 0.05f);
            camera->mTranslation.z = lerp<f32>(camera->mTranslation.z, followPos.z, 0.05f);

            camera->mTargetPos.x = lerp<f32>(camera->mTargetPos.x, lookAtPos.x, 0.05f);
            camera->mTargetPos.y = lerp<f32>(camera->mTargetPos.y, lookAtPos.y, 0.05f);
            camera->mTargetPos.z = lerp<f32>(camera->mTargetPos.z, lookAtPos.z, 0.05f);

            camera->mProjectionFovy = lerp<f32>(camera->mProjectionFovy, sFixedCameraFovy, 0.05f);
        } else {
            camera->mTranslation    = followPos;
            camera->mTargetPos      = lookAtPos;
            camera->mProjectionFovy = sFixedCameraFovy;
        }

        camera->mAnglePitch = 0;
        camera->mAngleYaw   = convertAngleFloatToS16(-180.0f);

        C_MTXPerspective(camera->mProjectionMatrix, camera->mProjectionFovy,
                         camera->mProjectionAspect, camera->mProjectionNear,
                         camera->mProjectionFar);
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

        return;
    }

    if (gpMarDirector->mAreaID == 88) {
        ControlCamera2D(camera);
        return;
    }

    camera->ctrlGameCamera_();
}
SMS_PATCH_BL(0x80023148, controlCameraByCodeExt);

BETTER_SMS_FOR_CALLBACK void resetFixedCameraOnLoad(TMarDirector *director) {
    SME::setCameraFixedMode(false, false);
    SME::setCameraFixedPos(TVec3f::zero());
    SME::setCameraFixedLookAt(TVec3f::zero());
    SME::setCameraFixedFovy(50.0f);
    SME::setCameraFixedFollowActor(nullptr);
}