#include <SMS/Manager/FlagManager.hxx>
#include <SMS/System/MovieDirector.hxx>

#include <BetterSMS/module.hxx>
#include <BetterSMS/thp.hxx>

#include "stage.hxx"
#include "settings.hxx"

extern Settings::SwitchSetting gSkipMovieSetting;

static s32 decideNextMode_ext(TMovieDirector *director, s32 *out) {
    if (gpApplication.mCutSceneID == 31) {
        gpApplication.mCutSceneID = 32;
        return TApplication::CONTEXT_DIRECT_MOVIE;
    }

    if (gpApplication.mCutSceneID == 32) {
        if (gpApplication.mNextScene.mEpisodeID == 0xFF) {
            return TApplication::CONTEXT_DIRECT_SHINE_SELECT;
        }
        return TApplication::CONTEXT_DIRECT_STAGE;
    }

    if (gpApplication.mCutSceneID == 33) {
        gpApplication.mNextScene.mAreaID = TGameSequence::AREA_DOLPIC;
        gpApplication.mNextScene.mEpisodeID = 2;
        BetterSMS::triggerAutoSave();
        return TApplication::CONTEXT_DIRECT_STAGE;
    }

    if (gpApplication.mCutSceneID == 34) {
        gpApplication.mCutSceneID = 35;
        return TApplication::CONTEXT_DIRECT_MOVIE;
    }

    if (gpApplication.mCutSceneID == 35) {
        gpApplication.mCutSceneID = 36;
        return TApplication::CONTEXT_DIRECT_MOVIE;
    }

    if (gpApplication.mCutSceneID == 36) {
        TFlagManager::smInstance->setFlag(0x10077, true);
        TFlagManager::smInstance->incFlag(0x40000, 1);
        gpApplication.mCutSceneID = 33;
        return TApplication::CONTEXT_DIRECT_MOVIE;
    }

    // This is for modifying existing cutscenes
    // (comes after so watch state is saved)
    if (gpApplication.mCutSceneID == 15) {
        if (!TFlagManager::smInstance->getShineFlag(239)) {
            TFlagManager::smInstance->setShineFlag(239);
        }
        gpApplication.mCutSceneID = 37;
        return TApplication::CONTEXT_DIRECT_MOVIE;
    }

    s32 ret = director->decideNextMode(out);

    if (gpApplication.mCutSceneID == 37) {
        gpApplication.mNextScene.mAreaID = TGameSequence::AREA_DOLPIC;
        gpApplication.mNextScene.mEpisodeID = 3;
        return TApplication::CONTEXT_DIRECT_STAGE;
    }

    return ret;
}
SMS_PATCH_BL(SMS_PORT_REGION(0x802B5F48, 0, 0, 0), decideNextMode_ext);
SMS_PATCH_BL(SMS_PORT_REGION(0x802B606C, 0, 0, 0), decideNextMode_ext);
SMS_WRITE_32(SMS_PORT_REGION(0x802B6524, 0, 0, 0), 0x2C0300F0);

SMS_WRITE_32(SMS_PORT_REGION(0x801FA3A4, 0, 0, 0), 0x38800022);

static bool checkForSkippableCutscene(TFlagManager *manager) {
    if (gSkipMovieSetting.getBool()) {
        return true;
    }

    if (gpApplication.mCutSceneID > 19) {
        return true;
    }

    return manager->getBool(gpApplication.mCutSceneID + 0x10391);
}

// Make all cutscenes skippable
SMS_PATCH_BL(SMS_PORT_REGION(0x802B5E8C, 0, 0, 0), checkForSkippableCutscene);
SMS_PATCH_BL(SMS_PORT_REGION(0x802B5EF4, 0, 0, 0), checkForSkippableCutscene);