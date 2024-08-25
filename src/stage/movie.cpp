#include <SMS/Manager/FlagManager.hxx>
#include <SMS/System/MovieDirector.hxx>

#include <BetterSMS/module.hxx>
#include <BetterSMS/thp.hxx>

#include "stage.hxx"
#include "settings.hxx"

static s32 decideNextMode_ext(TMovieDirector *director, s32 *out) {
    if (gpApplication.mCutSceneID == 31) {
        gpApplication.mCutSceneID = 32;
        return TApplication::CONTEXT_DIRECT_MOVIE;
    }

    if (gpApplication.mCutSceneID == 32) {
        return TApplication::CONTEXT_DIRECT_STAGE;
    }

    if (gpApplication.mCutSceneID == 33) {
        return TApplication::CONTEXT_DIRECT_STAGE;
    }

    s32 ret = director->decideNextMode(out);

    // This is for modifying existing cutscenes
    // (comes after so watch state is saved)
    if (gpApplication.mCutSceneID == 15) {
        if (!TFlagManager::smInstance->getShineFlag(239)) {
            TFlagManager::smInstance->setShineFlag(239);
        }
        gpApplication.mCutSceneID = 17;
    }

    return ret;
}
SMS_PATCH_BL(SMS_PORT_REGION(0x802B5F48, 0, 0, 0), decideNextMode_ext);
SMS_PATCH_BL(SMS_PORT_REGION(0x802B606C, 0, 0, 0), decideNextMode_ext);
SMS_WRITE_32(SMS_PORT_REGION(0x802B6524, 0, 0, 0), 0x2C0300F0);