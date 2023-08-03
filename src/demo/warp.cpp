#include <JSystem/JParticle/JPAResourceManager.hxx>
#include <SMS/MSound/MSound.hxx>
#include <SMS/MSound/MSoundSESystem.hxx>
#include <SMS/Manager/FlagManager.hxx>
#include <SMS/raw_fn.hxx>

#include <BetterSMS/module.hxx>

#include "settings.hxx"

void setPlayerPosRotOnLoad(TMario *player) {
    auto &cur_scene  = gpApplication.mCurrentScene;
    auto &prev_scene = gpApplication.mPrevScene;

    if (cur_scene.mAreaID == 6 && cur_scene.mEpisodeID == 0 ||   // Daisy Cruiser
        cur_scene.mAreaID == 32 && cur_scene.mEpisodeID == 0 ||  // Flip Panel
        cur_scene.mAreaID == 14 && cur_scene.mEpisodeID == 1) {  // Casino Table
        TFlagManager::smInstance->setFlag(0x40002, 0);
    }

    // Daisy Cruiser
    if (cur_scene.mAreaID == 6 && cur_scene.mEpisodeID == 0) {
        if (prev_scene.mAreaID == 3) {
            // Mario's Dream
            player->mTranslation = {-17030, 6518, 3750};
            player->mAngle.y     = -180 * 182;
        } else if ((prev_scene.mAreaID == 14 && prev_scene.mEpisodeID == 1) ||
                   (prev_scene.mAreaID == 41 && prev_scene.mEpisodeID == 0)) {
            // Casino
            player->mTranslation = {5500, 1476, -1530};
            player->mAngle.y     = 60 * 182;
        } else if (prev_scene.mAreaID == 40 && prev_scene.mEpisodeID == 0) {
            // Planes and Trains
            player->mTranslation = {6060, 6612, -960};
            player->mAngle.y     = -60 * 182;
        } else if (prev_scene.mAreaID != 6 ||
                   (prev_scene.mEpisodeID != 1 && prev_scene.mEpisodeID != 2)) {
            player->mTranslation = {22400, 5400, 0};
            player->mAngle.y     = 90 * 182;
        }
    } else if (cur_scene.mAreaID == 6 && (cur_scene.mEpisodeID == 1 || cur_scene.mEpisodeID == 2)) {
        if (prev_scene.mAreaID == 6 && prev_scene.mEpisodeID == 0) {
            player->mTranslation = {6965, 1390, 12730};
            player->mAngle.y     = 180 * 182;
        }
    } else if (cur_scene.mAreaID == 26 && cur_scene.mEpisodeID == 0) {
        // Vaporwave
        if (prev_scene.mAreaID == 25 && prev_scene.mEpisodeID == 0) {
            player->mTranslation = {-4450, 7130, -100};
            player->mAngle.y     = 60 * 182;
        }
    }
}

static void assignExitAreaStages(TApplication *app) {
    u32 *tpausemenu;
    SMS_FROM_GPR(29, tpausemenu);
    tpausemenu[0x104 / 4] = app->mCurrentScene.mAreaID == 6 ? 2 : 3;
}
SMS_PATCH_BL(SMS_PORT_REGION(0x80156B70, 0, 0, 0), assignExitAreaStages);
SMS_WRITE_32(SMS_PORT_REGION(0x80156B80, 0, 0, 0), 0x60000000);

static void assignExitAreaDestination(JDrama::TFlagT<u16> flags, u16 flag) {
    flags.set(flag);
    if (gpMarDirector->mAreaID == 11 && gpMarDirector->mEpisodeID == 2) {
        gpApplication.mNextScene.mAreaID    = 15;
        gpApplication.mNextScene.mEpisodeID = 0;
        return;
    }

    if (gpMarDirector->mAreaID == 6 &&
        (gpMarDirector->mEpisodeID == 1 || gpMarDirector->mEpisodeID == 2)) {
        gpApplication.mNextScene.mAreaID    = 6;
        gpApplication.mNextScene.mEpisodeID = gpMarDirector->mEpisodeID;
        return;
    }

    if (gpMarDirector->mAreaID == 25 && gpMarDirector->mEpisodeID == 0) {
        gpApplication.mNextScene.mAreaID    = 26;
        gpApplication.mNextScene.mEpisodeID = 0;
        return;
    }

    if ((gpMarDirector->mAreaID == 4 && gpMarDirector->mEpisodeID == 5) ||   // Starshine Beach
        (gpMarDirector->mAreaID == 32 && gpMarDirector->mEpisodeID == 0) ||  // Flip Panel
        (gpMarDirector->mAreaID == 26 && gpMarDirector->mEpisodeID == 0) ||
        ((gpMarDirector->mAreaID == 13 || gpMarDirector->mAreaID == 50) &&
         gpMarDirector->mEpisodeID == 0)) {
        gpApplication.mNextScene.mAreaID    = 6;
        gpApplication.mNextScene.mEpisodeID = 3;
        return;
    }
}
SMS_PATCH_BL(SMS_PORT_REGION(0x80299808, 0, 0, 0), assignExitAreaDestination);

static void assignShineExitDestination(TApplication *app, u8 area, u8 episode,
                                       JDrama::TFlagT<u16> flag) {

    if (!(gpMarDirector->mCollectedShine->mType & 0x10)) {
        if (gpMarDirector->mAreaID == 11 && gpMarDirector->mEpisodeID == 2) {
            gpApplication.mNextScene.mAreaID    = 15;
            gpApplication.mNextScene.mEpisodeID = 0;
            return;
        }
        if (gpMarDirector->mAreaID == 6 &&
            (gpMarDirector->mEpisodeID == 1 || gpMarDirector->mEpisodeID == 2)) {
            area    = 6;
            episode = gpMarDirector->mEpisodeID;
        }
        if ((gpMarDirector->mAreaID == 4 && gpMarDirector->mEpisodeID == 5) ||   // Starshine Beach
            (gpMarDirector->mAreaID == 32 && gpMarDirector->mEpisodeID == 0) ||  // Flip Panel
            ((gpMarDirector->mAreaID == 25 || gpMarDirector->mAreaID == 26) &&
             gpMarDirector->mEpisodeID == 0) ||
            ((gpMarDirector->mAreaID == 13 || gpMarDirector->mAreaID == 50) &&
             gpMarDirector->mEpisodeID == 0)) {
            area    = 6;
            episode = 3;
        }
        app->mNextScene.set(area, episode, flag);
    }
}
SMS_PATCH_BL(SMS_PORT_REGION(0x80297C80, 0, 0, 0), assignShineExitDestination);
SMS_WRITE_32(SMS_PORT_REGION(0x80297C84, 0, 0, 0), 0x60000000);

static void assignGameOverDestination(TMarDirector *director) {
    if (gpApplication.mCurrentScene.mAreaID == 6 && gpApplication.mCurrentScene.mEpisodeID == 1)
        gpApplication.mNextScene = gpApplication.mCurrentScene;
    else if (gpApplication.mCurrentScene.mAreaID == 11 &&
             gpApplication.mCurrentScene.mEpisodeID == 0)
        gpApplication.mNextScene.set(15, 0, 0);
    else
        gpApplication.mNextScene.set(6, 2, 0);
    director->moveStage();
}
SMS_PATCH_BL(SMS_PORT_REGION(0x8029933C, 0, 0, 0), assignGameOverDestination);

static void checkMariosDreamWarp(JDrama::TFlagT<u16> flags, u16 flag) {
    flags.set(flag);

    if (gpApplication.mNextScene.mAreaID != 3 || gpApplication.mNextScene.mEpisodeID != 255)
        return;

    if (TFlagManager::smInstance->getShineFlag(7)) {
        gpApplication.mNextScene.mEpisodeID = 1;
    } else {
        gpApplication.mNextScene.mEpisodeID = 0;
    }
}
SMS_PATCH_BL(SMS_PORT_REGION(0x8029A3E0, 0, 0, 0), checkMariosDreamWarp);

// On Airstrip from file select
SMS_WRITE_32(SMS_PORT_REGION(0x80164E30, 0, 0, 0), 0x38800701);
// On Delfino from file select
SMS_WRITE_32(SMS_PORT_REGION(0x80164E44, 0, 0, 0), 0x38800702);

// On Delfino Plaza from shine exit
SMS_WRITE_32(SMS_PORT_REGION(0x80297C2C, 0, 0, 0), 0x60000000);
SMS_WRITE_32(SMS_PORT_REGION(0x80297C4C, 0, 0, 0), 0x38000006);
SMS_WRITE_32(SMS_PORT_REGION(0x80297C54, 0, 0, 0), 0x38600000);

// On Delfino Plaza from death
SMS_WRITE_32(SMS_PORT_REGION(0x802997A4, 0, 0, 0), 0x38000006);
SMS_WRITE_32(SMS_PORT_REGION(0x802997AC, 0, 0, 0), 0x38000000);
SMS_WRITE_32(SMS_PORT_REGION(0x802997BC, 0, 0, 0), 0x38000006);
SMS_WRITE_32(SMS_PORT_REGION(0x802997C4, 0, 0, 0), 0x38600000);

// On Delfino Plaza from exit area
SMS_WRITE_32(SMS_PORT_REGION(0x802991E0, 0, 0, 0), 0x38000006);
SMS_WRITE_32(SMS_PORT_REGION(0x802991E8, 0, 0, 0), 0x38000000);
SMS_WRITE_32(SMS_PORT_REGION(0x802991F8, 0, 0, 0), 0x38000006);
SMS_WRITE_32(SMS_PORT_REGION(0x80299200, 0, 0, 0), 0x38600000);