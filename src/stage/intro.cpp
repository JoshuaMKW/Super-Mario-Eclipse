#include <JSystem/JParticle/JPAResourceManager.hxx>
#include <SMS/Camera/PolarSubCamera.hxx>
#include <SMS/MSound/MSound.hxx>
#include <SMS/MSound/MSoundSESystem.hxx>
#include <SMS/Manager/FlagManager.hxx>
#include <SMS/raw_fn.hxx>

#include <BetterSMS/libs/constmath.hxx>
#include <BetterSMS/module.hxx>

#include "settings.hxx"
#include "stage.hxx"

extern bool gFromShineSelectForIntro;

static bool isStartCameraPresent(const char *default_path) {
    bool is_extended_area = gpApplication.mCurrentScene.mAreaID >= SME::STAGE_ERTO &&
                            gpApplication.mCurrentScene.mAreaID <= SME::STAGE_TUTORIAL;
    bool is_mare = gpApplication.mCurrentScene.mAreaID == TGameSequence::AREA_MARE;

    if (gpApplication.mCurrentScene.mAreaID != TGameSequence::AREA_DOLPIC &&
        gpApplication.mCurrentScene.mAreaID != SME::STAGE_CRUISER) {
        if (!gFromShineSelectForIntro && (is_extended_area || is_mare) &&
            SMS_getShineStage__FUc(gpApplication.mCurrentScene.mAreaID) ==
                SMS_getShineStage__FUc(gpApplication.mPrevScene.mAreaID)) {
            return false;
        }
    }

    gFromShineSelectForIntro = false;

    char camera_name[32] = {};
    sprintf(camera_name, "startcamera%d", gpMarDirector->mEpisodeID);

    char camera_path[64] = {};
    sprintf(camera_path, "/scene/map/camera/%s.bck", camera_name);

    if (JKRFileLoader::getGlbResource(camera_path) != nullptr) {
        return true;
    }

    return JKRFileLoader::getGlbResource(default_path) != nullptr;
}
SMS_PATCH_BL(0x80298F64, isStartCameraPresent);

static void extendedNextStateInitialize(TMarDirector *director, s8 next_state) {
    bool is_extended_area =
        director->mAreaID >= SME::STAGE_ERTO && director->mAreaID <= SME::STAGE_TUTORIAL;
    if (next_state != TMarDirector::STATE_INTRO_PLAYING || !is_extended_area) {
        director->nextStateInitialize(next_state);
        return;
    }

    director->mGamePads[0]->mState._02 = 1;
    *(u32 *)((u8 *)director + 0x68)    = 0;

    char camera_name[32] = {};
    sprintf(camera_name, "startcamera%d", director->mEpisodeID);

    char camera_path[64] = {};
    sprintf(camera_path, "/scene/map/camera/%s.bck", camera_name);

    if (JKRFileLoader::getGlbResource(camera_path) != nullptr) {
        gpCamera->startDemoCamera(camera_name, nullptr, -1, 0.0f, true);
    } else {
        gpCamera->startDemoCamera("startcamera", nullptr, -1, 0.0f, true);
    }

    bool isExStage = SMS_isExMap__Fv();
    if (!isExStage && director->mAreaID > 60) {
        *(u16 *)((u8 *)director + 0x50) |= 6;
    }

    u16 unk_state = *(u16 *)((u8 *)director + 0x50);

    if ((unk_state & 0x4) != 0) {
        director->mGCConsole->mConsoleStr->startAppearScenario();
        *(u16 *)((u8 *)director + 0x50) &= ~0x4;
    }

    startStageEntranceDemo__10MSMainProcFUcUc(director->mAreaID, director->mEpisodeID);
}
SMS_PATCH_BL(0x802995C8, extendedNextStateInitialize);