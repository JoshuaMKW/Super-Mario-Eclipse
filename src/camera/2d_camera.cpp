#include <BetterSMS/libs/constmath.hxx>
#include <BetterSMS/music.hxx>
#include <BetterSMS/player.hxx>

#include <SMS/Camera/PolarSubCamera.hxx>
#include <SMS/MarioUtil/MathUtil.hxx>

#define CAMERA_TYPE_2D     128
#define CAMERA_PAN_DIST    250.0f
#define CAMERA_FOLLOW_LERP 0.07f

static void controlCameraByCodeExt(CPolarSubCamera *camera) {
    if (gpMarDirector->mAreaID == 88) {
        f32 c_stick_x = gpMarioAddress->mController->mCStick.mStickX;
        f32 c_stick_y = gpMarioAddress->mController->mCStick.mStickY;

        if (!camera->isNormalDeadDemo() && !camera->isHellDeadDemo()) {
            camera->mTranslation.x =
                lerp<f32>(camera->mTranslation.x, gpMarioPos->x + c_stick_x * CAMERA_PAN_DIST,
                          CAMERA_FOLLOW_LERP);
            camera->mTranslation.y =
                lerp<f32>(camera->mTranslation.y,
                          gpMarioPos->y + 400.0f + c_stick_y * CAMERA_PAN_DIST, CAMERA_FOLLOW_LERP);
            camera->mTranslation.z =
                lerp<f32>(camera->mTranslation.z, gpMarioPos->z + 2500.0f, CAMERA_FOLLOW_LERP);
        }

        camera->mProjectionFovy = 50.0f;

        camera->mAnglePitch = 0;
        camera->mAngleYaw   = convertAngleFloatToS16(-180.0f);

        C_MTXPerspective(camera->mProjectionMatrix, camera->mProjectionFovy,
                         camera->mProjectionAspect, camera->mProjectionNear,
                         camera->mProjectionFar);
        C_MTXLookAt(camera->mTRSMatrix, camera->mTranslation, TVec3f::up(), *gpMarioPos);

        camera->mTargetPos.x      = camera->mTranslation.x;
        camera->mTargetPos.y      = camera->mTranslation.y;
        camera->mTargetPos.z      = gpMarioPos->z;
        camera->mWorldTranslation = camera->mTranslation;

        *(f32 *)((u8 *)camera + 0x130) = camera->mTranslation.x;
        *(f32 *)((u8 *)camera + 0x134) = camera->mTranslation.y;
        *(f32 *)((u8 *)camera + 0x138) = camera->mTranslation.z;

        *(f32 *)((u8 *)camera + 0x148) = camera->mTranslation.x;
        *(f32 *)((u8 *)camera + 0x14C) = camera->mTranslation.y;
        *(f32 *)((u8 *)camera + 0x150) = gpMarioPos->z;

        *(f32 *)((u8 *)camera + 0x154) = camera->mTranslation.x;
        *(f32 *)((u8 *)camera + 0x158) = camera->mTranslation.y;
        *(f32 *)((u8 *)camera + 0x15C) = gpMarioPos->z;

        return;
    }

    camera->ctrlGameCamera_();
}
SMS_PATCH_BL(0x80023148, controlCameraByCodeExt);