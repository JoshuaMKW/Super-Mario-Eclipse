#include <Dolphin/OS.h>
#include <JSystem/J2D/J2DOrthoGraph.hxx>
#include <JSystem/J2D/J2DTextBox.hxx>

#include <SMS/System/Application.hxx>

#include <BetterSMS/application.hxx>
#include <BetterSMS/debug.hxx>
#include <BetterSMS/game.hxx>
#include <BetterSMS/module.hxx>
#include <BetterSMS/object.hxx>
#include <BetterSMS/player.hxx>
#include <BetterSMS/stage.hxx>

#include "enemy/firey_petey.hxx"
#include "object/darkness_effect.hxx"
#include "object/launch_star.hxx"
#include "object/tornado_obj.hxx"
#include "object/water_balloon.hxx"
#include "p_settings.hxx"

// Application
extern bool directCharacterSelectMenu(TApplication *app);
extern void setIntroStage(TApplication *application);

// Light
extern void initToDefault(TMarDirector *director);
extern void manageShineDarkness(TMarDirector *director);

// Yoshi
extern void forceYoshiUnlock(TMarDirector *director);
extern void adjustYoshiTounge(TMario *player, bool isMario);

// Stage
extern void initializeStageInfo(TApplication *app);
void resetForExStage(TMarDirector *director);

// Player
extern void initEclipseData(TMario *player, bool isMario);
extern void initCharacterArchives(TMarDirector *director);
extern void initializeWaterBalloons(TMario *player);
extern void createWaterBalloonAndThrow(TMario *player, bool isMario);
extern bool holdPlayerState(TMario *player);
extern bool launchPlayerState(TMario *player);
extern void blazePlayer(TMario *player, bool isMario);
extern void initColdState(TMarDirector *director);
extern void processColdState(TMario *player, bool isMario);

// HUD
extern void updatePlayerHUD(TMarDirector *, const J2DOrthoGraph *);

// DEMO
extern void lockModuleSettings(TApplication *app);
extern void unlockSettings(TMarDirector *director);
extern void setPlayerPosRotOnLoad(TMario *player);
extern void checkForBlueCoinTrade(TMarDirector *director);
extern void resetTutorialIceStageCheckpoints(TMarDirector *director);
extern void checkTutorialIceStageCheckpoints(TMario *player, bool isMario);
extern void resetTutorialCasinoStageCheckpoints(TMarDirector *director);
extern void checkTutorialCasinoStageCheckpoints(TMario *player, bool isMario);
extern void resetTutorialPiantaPitStageCheckpoints(TMarDirector *director);
extern void checkTutorialPiantaPitStageCheckpoints(TMario *player, bool isMario);
extern void checkTutorialCollisionRespawn(TMario *player, bool isMario);

// SETTINGS
extern void checkForCompletionAwards(TApplication *);

static BetterSMS::ModuleInfo sModuleInfo("Super Mario Eclipse", 1, 0, &gSettingsGroup);

static void initModule() {
    // Register settings
    // gSettingsGroup.addSetting(&gHUDSetting);
    gSettingsGroup.addSetting(&gBugsSetting);
    gSettingsGroup.addSetting(&gTutorialSetting);

    extern void initDemoCredits(Settings::SettingsGroup & group);
    initDemoCredits(gSettingsGroup);
    {
        auto &saveInfo        = gSettingsGroup.getSaveInfo();
        saveInfo.mSaveName    = Settings::getGroupName(gSettingsGroup);
        saveInfo.mBlocks      = 1;
        saveInfo.mGameCode    = 'GMSE';
        saveInfo.mCompany     = 0x3034;  // '04'
        saveInfo.mBannerFmt   = CARD_BANNER_CI;
        saveInfo.mBannerImage = reinterpret_cast<const ResTIMG *>(gSaveBnr);
        saveInfo.mIconFmt     = CARD_ICON_CI;
        saveInfo.mIconSpeed   = CARD_SPEED_SLOW;
        saveInfo.mIconCount   = 2;
        saveInfo.mIconTable   = reinterpret_cast<const ResTIMG *>(gSaveIcon);
        saveInfo.mSaveGlobal  = true;
    }

    // Register module
    BetterSMS::registerModule(&sModuleInfo);

    Game::setMaxShines(300);

    // Register callbacks
    Application::registerContextCallback(11, directCharacterSelectMenu);
    Game::registerInitCallback("__init_stage_info", initializeStageInfo);
    Game::registerChangeCallback("__set_intro_stage", setIntroStage);

    Stage::registerInitCallback("__reset_darkness", initToDefault);
    Stage::registerUpdateCallback("__update_darkness", manageShineDarkness);

    // Demo
    Stage::registerInitCallback("__force_yoshi_unlock", forceYoshiUnlock);
    Stage::registerUpdateCallback("__check_for_bugs_unlock", unlockSettings);
    Stage::registerUpdateCallback("__check_for_blue_coin_shine", checkForBlueCoinTrade);
    Game::registerBootCallback("__lock_demo_settings", lockModuleSettings);

    Stage::registerInitCallback("__init_player_models", initCharacterArchives);
    Stage::registerInitCallback("__init_ex_stage", resetForExStage);
    Stage::registerInitCallback("__reset_tutorial_ice_stage", resetTutorialIceStageCheckpoints);
    Stage::registerInitCallback("__reset_tutorial_casino_stage",
                                resetTutorialCasinoStageCheckpoints);
    Stage::registerInitCallback("__reset_tutorial_pianta_pit_stage",
                                resetTutorialPiantaPitStageCheckpoints);
    Stage::registerDraw2DCallback("__update_player_hud", updatePlayerHUD);
    Player::registerInitCallback("__init_eclipse_data", initEclipseData);
    Player::registerLoadAfterCallback("__init_water_balloons", initializeWaterBalloons);
    Player::registerLoadAfterCallback("__init_player_pos_rot", setPlayerPosRotOnLoad);
    Player::registerUpdateCallback("__update_tutorial_ice_stage", checkTutorialIceStageCheckpoints);
    Player::registerUpdateCallback("__update_tutorial_casino_stage",
                                   checkTutorialCasinoStageCheckpoints);
    Player::registerUpdateCallback("__update_tutorial_pianta_pit_stage",
                                   checkTutorialPiantaPitStageCheckpoints);
    Stage::registerInitCallback("__init_cold_state", initColdState);
    Player::registerUpdateCallback("__check_for_freezing_water", processColdState);
    // Player::registerUpdateCallback("__update_tutorial_respawn", checkTutorialCollisionRespawn);
    Player::registerUpdateCallback("__update_water_balloons", createWaterBalloonAndThrow);
    Player::registerUpdateCallback("__update_yoshi_tounge", adjustYoshiTounge);
    Player::registerUpdateCallback("__update_blaze_state", blazePlayer);
    Player::registerStateMachine(PlayerLaunchStarWait, holdPlayerState);
    Player::registerStateMachine(PlayerLaunchStarLaunch, launchPlayerState);
    Debug::registerUpdateCallback("__check_awards", checkForCompletionAwards);

    Objects::registerObjectAsMisc("DarknessEffect", TDarknessEffect::instantiate);
    Objects::registerObjectAsMapObj("Tornado", &tornadoData, TTornadoMapObj::instantiate);
    Objects::registerObjectAsMapObj("WaterBalloon", &waterBalloonData, TWaterBalloon::instantiate);
    Objects::registerObjectAsMapObj("LaunchStar", &launchStarData, TLaunchStarObj::instantiate);

    Objects::registerObjectAsMisc("FireyPetey", TFireyPetey::instantiate);
    Objects::registerObjectAsMisc("FireyPeteyManager", TFireyPeteyManager::instantiate);
}

static void deinitModule() {
    Application::deregisterContextCallback(11);

    Stage::deregisterUpdateCallback("__update_darkness");

    Stage::deregisterInitCallback("__init_player_models");
    Player::deregisterLoadAfterCallback("__init_water_balloons");
    Player::deregisterUpdateCallback("__update_water_balloons");
    Player::deregisterUpdateCallback("__update_yoshi_tounge");

    Objects::deregisterObject("DarknessEffect");
    Objects::deregisterObject("Tornado");
    Objects::deregisterObject("WaterBalloon");
    Objects::deregisterObject("LaunchStar");

    BetterSMS::deregisterModule(&sModuleInfo);
}

// Definition block
KURIBO_MODULE_BEGIN("Super Mario Eclipse", "JoshuaMK", "v1.0") {
    // Set the load and unload callbacks to our registration functions
    KURIBO_EXECUTE_ON_LOAD { initModule(); }
    KURIBO_EXECUTE_ON_UNLOAD { deinitModule(); }
}
KURIBO_MODULE_END()

SMS_WRITE_32(0x802bf47c, 0x808da0d4);  // Use current heap for files

// static u32 sAddresses[0x10000] = {0};
//
// static void printAllocSpace(OSMutex *mutex) {
//     u32 alloced;
//     SMS_FROM_GPR(31, alloced);
//
//     for (int i = 0; i < 0x10000; i++) {
//         if (sAddresses[i] == 0) {
//             sAddresses[i] = alloced;
//             break;
//         }
//     }
//
//     OSUnlockMutex(mutex);
// }
// SMS_PATCH_BL(0x802C14A8, printAllocSpace);
//
// static void printFreeSpace(OSMutex *mutex) {
//     u32 alloced;
//     SMS_FROM_GPR(31, alloced);
//
//     for (int i = 0; i < 0x10000; i++) {
//         if (sAddresses[i] == alloced) {
//             sAddresses[i] = 0;
//             break;
//         }
//     }
//
//     OSLockMutex(mutex);
// }
// SMS_PATCH_BL(0x802C1B34, printFreeSpace);