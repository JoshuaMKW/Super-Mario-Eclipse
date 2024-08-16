#include <JSystem/JParticle/JPAResourceManager.hxx>
#include <SMS/MSound/MSound.hxx>
#include <SMS/MSound/MSoundSESystem.hxx>
#include <SMS/Manager/FlagManager.hxx>
#include <SMS/raw_fn.hxx>

#include <BetterSMS/libs/constmath.hxx>
#include <BetterSMS/module.hxx>

#include "settings.hxx"
#include "stage.hxx"

static u8 s_no_exit_areas[] = {TGameSequence::AREA_AIRPORT, TGameSequence::AREA_DOLPIC,
                               SME::STAGE_CRUISER, SME::STAGE_PEACH_CASTLE};

static void assignExitAreaStages(TApplication *app) {
    u32 *tpausemenu;
    SMS_FROM_GPR(29, tpausemenu);

    bool needs_exit_area = tpausemenu[0x104 / 4] == 3;

    for (u32 i = 0; i < sizeof(s_no_exit_areas); i++) {
        if (app->mCurrentScene.mAreaID == s_no_exit_areas[i]) {
            needs_exit_area = false;
            break;
        }
    }

    tpausemenu[0x104 / 4] = needs_exit_area ? 3 : 2;
}
SMS_PATCH_BL(SMS_PORT_REGION(0x80156B70, 0, 0, 0), assignExitAreaStages);
SMS_WRITE_32(SMS_PORT_REGION(0x80156B80, 0, 0, 0), 0x60000000);

static void assignExitAreaDestination(TGameSequence *sequence, u8 area, u8 episode, u16 flags) {
    sequence->set(area, episode, flags);

    switch (gpMarDirector->mAreaID) {
    case SME::STAGE_ISLE_DELFINO:
    case SME::STAGE_LIGHTHOUSE_BOSS:
        return;
    case SME::STAGE_TUTORIAL:
        gpApplication.mNextScene.mAreaID    = TGameSequence::AREA_OPTION;
        gpApplication.mNextScene.mEpisodeID = 0;
        return;
    case SME::STAGE_PEACH_CASTLE:
        if (gpApplication.mCurrentScene.mEpisodeID <= 1) {
            gpApplication.mNextScene = gpApplication.mCurrentScene;
            return;
        }
        gpApplication.mNextScene.mAreaID    = SME::STAGE_PEACH_CASTLE;
        gpApplication.mNextScene.mEpisodeID = 2;
        return;
    case SME::STAGE_VAPORWAVE:
    case SME::STAGE_VAPORWAVE_EX:
    case SME::STAGE_LANCIA:
    case SME::STAGE_LANCIA_EX:
    case SME::STAGE_RED_LILY:
    case SME::STAGE_RED_LILY_EX:
    case SME::STAGE_PEACH_BEACH:
    case SME::STAGE_PEACH_BEACH_EX:
    case SME::STAGE_YOSHI_VILLAGE:
    case SME::STAGE_YOSHI_EX:
        gpApplication.mNextScene.mAreaID    = SME::STAGE_CRUISER;
        gpApplication.mNextScene.mEpisodeID = 4;
        return;
    case SME::STAGE_SPETTRO_EX:
        gpApplication.mNextScene.mAreaID    = SME::STAGE_CRUISER;
        gpApplication.mNextScene.mEpisodeID = 0;
        return;
    }

    if (gpMarDirector->mAreaID >= SME::STAGE_CRUISER &&
        gpMarDirector->mAreaID < SME::STAGE_ERTO_EX) {
        gpApplication.mNextScene.mAreaID    = SME::STAGE_CRUISER;
        gpApplication.mNextScene.mEpisodeID = 0;
        return;
    }
}
SMS_PATCH_BL(SMS_PORT_REGION(0x80299230, 0, 0, 0), assignExitAreaDestination);
SMS_PATCH_BL(SMS_PORT_REGION(0x80299808, 0, 0, 0), assignExitAreaDestination);

static void assignShineExitDestination(TApplication *app, u8 area, u8 episode,
                                       JDrama::TFlagT<u16> flag) {
    if (!(gpMarDirector->mCollectedShine->mType & 0x10)) {
        app->mNextScene.set(area, episode, flag);

        switch (gpMarDirector->mAreaID) {
        case SME::STAGE_ISLE_DELFINO:
        case SME::STAGE_LIGHTHOUSE_BOSS:
            return;
        case SME::STAGE_TUTORIAL:
            app->mNextScene.mAreaID    = TGameSequence::AREA_OPTION;
            app->mNextScene.mEpisodeID = 0;
            return;
        case SME::STAGE_PEACH_CASTLE:
            if (app->mCurrentScene.mEpisodeID <= 1) {
                app->mNextScene = app->mCurrentScene;
                return;
            }
            app->mNextScene.mAreaID    = SME::STAGE_PEACH_CASTLE;
            app->mNextScene.mEpisodeID = 2;
            return;
        case SME::STAGE_VAPORWAVE:
        case SME::STAGE_VAPORWAVE_EX:
        case SME::STAGE_LANCIA:
        case SME::STAGE_LANCIA_EX:
        case SME::STAGE_RED_LILY:
        case SME::STAGE_RED_LILY_EX:
        case SME::STAGE_PEACH_BEACH:
        case SME::STAGE_PEACH_BEACH_EX:
        case SME::STAGE_YOSHI_VILLAGE:
        case SME::STAGE_YOSHI_EX:
            app->mNextScene.mAreaID    = SME::STAGE_CRUISER;
            app->mNextScene.mEpisodeID = 4;
            return;
        case SME::STAGE_SPETTRO_EX:
            app->mNextScene.mAreaID    = SME::STAGE_CRUISER;
            app->mNextScene.mEpisodeID = 0;
            return;
        }

        if (gpMarDirector->mAreaID >= SME::STAGE_CRUISER &&
            gpMarDirector->mAreaID < SME::STAGE_ERTO_EX) {
            app->mNextScene.mAreaID    = SME::STAGE_CRUISER;
            app->mNextScene.mEpisodeID = 0;
            return;
        }
    }
}
SMS_PATCH_BL(SMS_PORT_REGION(0x80297C80, 0, 0, 0), assignShineExitDestination);
SMS_WRITE_32(SMS_PORT_REGION(0x80297C84, 0, 0, 0), 0x60000000);

static void assignGameOverDestination() {
    TMarDirector *director;
    SMS_FROM_GPR(31, director);

    director->mGameState &= ~0x100;

    switch (gpMarDirector->mAreaID) {
    case SME::STAGE_ISLE_DELFINO:
    case SME::STAGE_LIGHTHOUSE_BOSS:
        return;
    case SME::STAGE_TUTORIAL:
        gpApplication.mNextScene.mAreaID    = TGameSequence::AREA_OPTION;
        gpApplication.mNextScene.mEpisodeID = 0;
        return;
    case SME::STAGE_PEACH_CASTLE:
        if (gpApplication.mCurrentScene.mEpisodeID <= 1) {
            gpApplication.mNextScene = gpApplication.mCurrentScene;
            return;
        }
        gpApplication.mNextScene.mAreaID    = SME::STAGE_PEACH_CASTLE;
        gpApplication.mNextScene.mEpisodeID = 0;
        return;
    case SME::STAGE_VAPORWAVE:
    case SME::STAGE_VAPORWAVE_EX:
    case SME::STAGE_LANCIA:
    case SME::STAGE_LANCIA_EX:
    case SME::STAGE_RED_LILY:
    case SME::STAGE_RED_LILY_EX:
    case SME::STAGE_PEACH_BEACH:
    case SME::STAGE_PEACH_BEACH_EX:
    case SME::STAGE_YOSHI_VILLAGE:
    case SME::STAGE_YOSHI_EX:
        gpApplication.mNextScene.mAreaID    = SME::STAGE_CRUISER;
        gpApplication.mNextScene.mEpisodeID = 0;
        return;
    case SME::STAGE_SPETTRO_EX:
        gpApplication.mNextScene.mAreaID    = SME::STAGE_CRUISER;
        gpApplication.mNextScene.mEpisodeID = 0;
        return;
    }

    if (gpMarDirector->mAreaID >= SME::STAGE_CRUISER &&
        gpMarDirector->mAreaID < SME::STAGE_ERTO_EX) {
        gpApplication.mNextScene.mAreaID    = SME::STAGE_CRUISER;
        gpApplication.mNextScene.mEpisodeID = 0;
        return;
    }
}
SMS_PATCH_BL(SMS_PORT_REGION(0x8029932C, 0, 0, 0), assignGameOverDestination);
SMS_WRITE_32(SMS_PORT_REGION(0x80299334, 0, 0, 0), 0x60000000);
SMS_WRITE_32(SMS_PORT_REGION(0x80299338, 0, 0, 0), 0x60000000);

// On Airstrip from file select
SMS_WRITE_32(SMS_PORT_REGION(0x80164E30, 0, 0, 0), 0x38804E01);

// On Airstrip movie from castle
// SMS_WRITE_32(SMS_PORT_REGION(0x8029A1F0, 0, 0, 0), 0x38800000);

//// On Delfino from file select
// SMS_WRITE_32(SMS_PORT_REGION(0x80164E44, 0, 0, 0), 0x38800702);
//
//// On Delfino Plaza from shine exit
// SMS_WRITE_32(SMS_PORT_REGION(0x80297C2C, 0, 0, 0), 0x60000000);
// SMS_WRITE_32(SMS_PORT_REGION(0x80297C4C, 0, 0, 0), 0x38000006);
// SMS_WRITE_32(SMS_PORT_REGION(0x80297C54, 0, 0, 0), 0x38600000);
//
//// On Delfino Plaza from death
// SMS_WRITE_32(SMS_PORT_REGION(0x802997A4, 0, 0, 0), 0x38000006);
// SMS_WRITE_32(SMS_PORT_REGION(0x802997AC, 0, 0, 0), 0x38000000);
// SMS_WRITE_32(SMS_PORT_REGION(0x802997BC, 0, 0, 0), 0x38000006);
// SMS_WRITE_32(SMS_PORT_REGION(0x802997C4, 0, 0, 0), 0x38600000);
//
//// On Delfino Plaza from exit area
// SMS_WRITE_32(SMS_PORT_REGION(0x802991E0, 0, 0, 0), 0x38000006);
// SMS_WRITE_32(SMS_PORT_REGION(0x802991E8, 0, 0, 0), 0x38000000);
// SMS_WRITE_32(SMS_PORT_REGION(0x802991F8, 0, 0, 0), 0x38000006);
// SMS_WRITE_32(SMS_PORT_REGION(0x80299200, 0, 0, 0), 0x38600000);

static void setNextStage_ChangeStageOverride(TMarDirector *director, u16 warpID) {
    bool isFloodedPlaza =
        director->mAreaID == TGameSequence::AREA_DOLPIC && director->mEpisodeID == 9;
    if (!isFloodedPlaza && warpID == TGameSequence::AREA_COROEX6) {
        if (!TFlagManager::smInstance->getFlag(0x10077)) {
            warpID = 0x31;
        }
    }
    director->setNextStage(warpID, nullptr);
}
SMS_PATCH_BL(SMS_PORT_REGION(0x801C1790, 0, 0, 0), setNextStage_ChangeStageOverride);