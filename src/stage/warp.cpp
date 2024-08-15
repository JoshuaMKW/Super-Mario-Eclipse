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

static void assignExitAreaDestination(JDrama::TFlagT<u16> flags, u16 flag) {
    flags.set(flag);

    switch (gpMarDirector->mAreaID) {
    case SME::STAGE_ISLE_DELFINO:
    case SME::STAGE_LIGHTHOUSE_BOSS:
        return;
    case SME::STAGE_TUTORIAL:
        gpApplication.mNextScene.mAreaID    = TGameSequence::AREA_OPTION;
        gpApplication.mNextScene.mEpisodeID = 0;
        return;
    case SME::STAGE_PEACH_CASTLE:
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

// static void assignGameOverDestination(TMarDirector *director) {
//     if (gpApplication.mCurrentScene.mAreaID == 6 && gpApplication.mCurrentScene.mEpisodeID == 1)
//         gpApplication.mNextScene = gpApplication.mCurrentScene;
//     else if (gpApplication.mCurrentScene.mAreaID == 11 &&
//              gpApplication.mCurrentScene.mEpisodeID == 2)
//         gpApplication.mNextScene.set(15, 0, 0);
//     else
//         gpApplication.mNextScene.set(6, 2, 0);
//     director->moveStage();
// }
// SMS_PATCH_BL(SMS_PORT_REGION(0x8029933C, 0, 0, 0), assignGameOverDestination);

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
// SMS_PATCH_BL(SMS_PORT_REGION(0x8029A3E0, 0, 0, 0), checkMariosDreamWarp);

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