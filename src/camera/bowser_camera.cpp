#include <SMS/Camera/CameraShake.hxx>
#include <SMS/MSound/MSoundSESystem.hxx>
#include <SMS/Manager/FlagManager.hxx>
#include <SMS/Manager/MarioParticleManager.hxx>
#include <SMS/Manager/RumbleManager.hxx>
#include <SMS/raw_fn.hxx>

#include <BetterSMS/libs/constmath.hxx>
#include <BetterSMS/music.hxx>
#include <BetterSMS/objects/generic.hxx>

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

    sFixedCameraPos = gpBowserCar->mTranslation + delta;
    sFixedCameraPos.y += 400.0f;

    sFixedCameraLookAt = gpBowserCar->mTranslation;
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
        SMSRumbleMgr->start(0x15, 0x1E, (f32 *)nullptr);
    }

    sFixedCameraLookAt = gpBowserCar->mTranslation;
    sFixedCameraLookAt.y += 200.0f * (sFixedCameraFovy / 45.0f);

    if (getCurrentFrame() > 800) {
        sFixedCameraState = 2;
        TFlagManager::smInstance->setFlag(0x6001B, false);
    }
}

static size_t s_shine_cam_frame     = 0;
static TGenericRailObj *s_shine_ptr = nullptr;

// 0x127 -> 0x12a

void ControlBowserShineCam_(CPolarSubCamera *camera) {
    if (s_shine_cam_frame == 0) {
        JDrama::TNameRef *root = TMarNameRefGen::getInstance()->getRootNameRef();
        u16 key_code           = JDrama::TActor::calcKeyCode("FinalShineMock");
        s_shine_ptr            = (TGenericRailObj *)root->searchF(key_code, "FinalShineMock");
        s_shine_ptr->mGravity  = 0.0f;
        s_shine_ptr->mScale    = {2.0f, 2.0f, 2.0f};
        // s_shine_ptr->mStateFlags.asU32 |= 0x200; // Disable particles

        sFixedCameraPos  = {0.0f, 2000.0f, -3000.0f};
        sFixedCameraFovy = 60.0f;

        *((u8 *)gpMarDirector->mGCConsole + 0x50) = 0;

        BetterSMS::Music::setVolumeFade(20, 1.0f);
    }

    if (s_shine_cam_frame > 1200) {
        // Tell sunscript to start movie and exit
        TFlagManager::smInstance->setFlag(0x6001D, true);
        BetterSMS::Music::stopSong(0.2f);
    }

    if (s_shine_cam_frame < 600) {
        s_shine_ptr->mSpeed.y = 3.0f;
        s_shine_ptr->mRotation.y += 5.0f;

        f32 glow_scale = ((f32)s_shine_cam_frame / 600.0f) + 1.0f;

        if (JPABaseEmitter *emitter = gpMarioParticleManager->emitAndBindToMtxPtr(
                0x127, s_shine_ptr->getModel()->mJointArray[2], 1, s_shine_ptr)) {
            emitter->mSize1 = {
                5.0f * glow_scale,
                5.0f * glow_scale,
                5.0f * glow_scale,
            };
            /*emitter->mSize3 = {
                5.0f * glow_scale,
                5.0f * glow_scale,
                5.0f * glow_scale,
            };*/
        }

        if (JPABaseEmitter *emitter = gpMarioParticleManager->emitAndBindToMtxPtr(
                0x128, s_shine_ptr->getModel()->mJointArray[2], 1, s_shine_ptr)) {
            emitter->mSize1 = {
                5.0f * glow_scale,
                5.0f * glow_scale,
                5.0f * glow_scale,
            };
            /*emitter->mSize3 = {
                5.0f * glow_scale,
                5.0f * glow_scale,
                5.0f * glow_scale,
            };*/
        }

        if (JPABaseEmitter *emitter = gpMarioParticleManager->emitAndBindToMtxPtr(
                0x129, s_shine_ptr->getModel()->mJointArray[2], 1, s_shine_ptr)) {
            emitter->mSize1 = {
                5.0f * glow_scale,
                5.0f * glow_scale,
                5.0f * glow_scale,
            };
            /*emitter->mSize3 = {
                5.0f * glow_scale,
                5.0f * glow_scale,
                5.0f * glow_scale,
            };*/
        }

        if (JPABaseEmitter *emitter = gpMarioParticleManager->emitAndBindToMtxPtr(
                0x12a, s_shine_ptr->getModel()->mJointArray[2], 1, s_shine_ptr)) {
            emitter->mSize1 = {
                5.0f * glow_scale,
                5.0f * glow_scale,
                5.0f * glow_scale,
            };
            /*emitter->mSize3 = {
                5.0f * glow_scale,
                5.0f * glow_scale,
                5.0f * glow_scale,
            };*/
        }

        if (gpMSound->gateCheck(MSD_SE_SHINE_EXIST)) {
            if (JAISound *sound = MSoundSE::startSoundSystemSE(MSD_SE_SHINE_EXIST, 0, nullptr, 0)) {
                sound->setVolume(0.3f, 0, 0);
            }
        }

        sFixedCameraFovy -= 0.06f;
    } else {
        if (s_shine_cam_frame == 600) {
            s_shine_ptr->mSpeed.x = 0.0f;
            s_shine_ptr->mSpeed.y = 32.0f;
            s_shine_ptr->mSpeed.z = 45.0f;
            s_shine_ptr->mGravity = 0.1f;

            gpCameraShake->startShake(UnknownShake27, 0.3f);
            SMSRumbleMgr->start(0x15, 0x17, (f32 *)nullptr);

            if (gpMSound->gateCheck(MSD_SE_OBJ_SANDBOMB_BANG)) {
                MSoundSE::startSoundSystemSE(MSD_SE_OBJ_SANDBOMB_BANG, 0, nullptr, 0);
            }

            if (gpMSound->gateCheck(MSD_SE_SHINE_APPEAR)) {
                MSoundSE::startSoundSystemSE(MSD_SE_SHINE_APPEAR, 0, nullptr, 0);
            }

            MSBgm::startBGM(BGM_SHINE_APPEAR);

            if (JPABaseEmitter *emitter =
                    gpMarioParticleManager->emit(0x1b8, &s_shine_ptr->mTranslation, 0, nullptr)) {
            }
        }

        if (gpMSound->gateCheck(MSD_SE_SHINE_EXIST)) {
            MSoundSE::startSoundSystemSE(MSD_SE_SHINE_EXIST, 0, nullptr, 0);
        }

        s_shine_ptr->mRotation.y += 5.0f;

        if (JPABaseEmitter *emitter = gpMarioParticleManager->emitAndBindToMtxPtr(
                0x127, s_shine_ptr->getModel()->mJointArray[2], 1, s_shine_ptr)) {
            emitter->mSize1 = {
                10.0f,
                10.0f,
                10.0f,
            };
            /*emitter->mSize3 = {
                10.0f,
                10.0f,
                10.0f,
            };*/
        }

        if (JPABaseEmitter *emitter = gpMarioParticleManager->emitAndBindToMtxPtr(
                0x128, s_shine_ptr->getModel()->mJointArray[2], 1, s_shine_ptr)) {
            emitter->mSize1 = {
                10.0f,
                10.0f,
                10.0f,
            };
            /*emitter->mSize3 = {
                10.0f,
                10.0f,
                10.0f,
            };*/
        }

        if (JPABaseEmitter *emitter = gpMarioParticleManager->emitAndBindToMtxPtr(
                0x129, s_shine_ptr->getModel()->mJointArray[2], 1, s_shine_ptr)) {
            emitter->mSize1 = {
                5.0f,
                5.0f,
                5.0f,
            };
            /*emitter->mSize3 = {
                5.0f,
                5.0f,
                5.0f,
            };*/
        }

        if (JPABaseEmitter *emitter = gpMarioParticleManager->emitAndBindToMtxPtr(
                0x12a, s_shine_ptr->getModel()->mJointArray[2], 1, s_shine_ptr)) {
            emitter->mSize1 = {
                5.0f,
                5.0f,
                5.0f,
            };
            /*emitter->mSize3 = {
                5.0f,
                5.0f,
                5.0f,
            };*/
        }
    }

    s_shine_ptr->mTranslation += s_shine_ptr->mSpeed;
    s_shine_ptr->mSpeed.y -= s_shine_ptr->mGravity;

    sFixedCameraLookAt = s_shine_ptr->mTranslation;
    sFixedCameraLookAt.y += 200.0f;

    s_shine_cam_frame++;
}

static bool sCanSwitchMode = false;

void ControlBowserKillCam(CPolarSubCamera *camera) {
    if (TFlagManager::smInstance->getFlag(0x6001C)) {
        ControlBowserShineCam_(camera);
    }

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

void initBowserCameraData() {
    TFlagManager::smInstance->setFlag(0x6001C, false);
    TFlagManager::smInstance->setFlag(0x6001D, false);

    sFixedCameraPos    = {};
    sFixedCameraLookAt = {};
    sFixedCameraFovy   = 50.0f;
    sFixedCameraState  = 0;
    s_shine_cam_frame  = 0;
    s_shine_ptr        = nullptr;
}

static bool spoofCameraDemoForUI(CPolarSubCamera *camera) {
    return TFlagManager::smInstance->getFlag(0x6001C) || camera->isSimpleDemoCamera();
}
SMS_PATCH_BL(0x8014087C, spoofCameraDemoForUI);
SMS_PATCH_BL(0x80140928, spoofCameraDemoForUI);