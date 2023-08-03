#include <Dolphin/OS.h>
#include <JSystem/J2D/J2DOrthoGraph.hxx>
#include <JSystem/J2D/J2DTextBox.hxx>

#include <SMS/System/Application.hxx>

#include <BetterSMS/application.hxx>
#include <BetterSMS/game.hxx>
#include <BetterSMS/module.hxx>
#include <BetterSMS/object.hxx>
#include <BetterSMS/player.hxx>
#include <BetterSMS/stage.hxx>
#include <BetterSMS/debug.hxx>

#include "enemy/firey_petey.hxx"
#include "object/darkness_effect.hxx"
#include "object/water_balloon.hxx"
#include "object/tornado_obj.hxx"
#include "object/launch_star.hxx"
#include "p_settings.hxx"

// Application
extern bool directCharacterSelectMenu(TApplication *app);

// Light
extern void initToDefault(TMarDirector *director);
extern void manageShineDarkness(TMarDirector *director);

// Yoshi
extern void forceYoshiUnlock(TMarDirector *director);
extern void adjustYoshiTounge(TMario *player, bool isMario);

// Player
extern void initEclipseData(TMario *player, bool isMario);
extern void initCharacterArchives(TMarDirector *director);
extern void initializeWaterBalloons(TMario *player);
extern void createWaterBalloonAndThrow(TMario *player, bool isMario);
extern bool holdPlayerState(TMario *player);
extern bool launchPlayerState(TMario *player);
extern void blazePlayer(TMario *player, bool isMario);

// HUD
extern void updatePlayerHUD(TMarDirector *, const J2DOrthoGraph *);

// DEMO
extern void lockModuleSettings(TApplication *app);
extern void unlockSettings(TMarDirector *director);
extern void setPlayerPosRotOnLoad(TMario *player);
extern void checkForBlueCoinTrade(TMarDirector *director);
extern void resetTutorialIceStageCheckpoints(TMarDirector *director);
extern void checkTutorialIceStageCheckpoints(TMario *player, bool isMario);

// SETTINGS
extern void checkForCompletionAwards(TApplication *);

static BetterSMS::ModuleInfo sModuleInfo("Super Mario Eclipse", 1, 0, &gSettingsGroup);

static void initModule() {
    // Register settings
    // gSettingsGroup.addSetting(&gHUDSetting);
    gSettingsGroup.addSetting(&gBugsSetting);

    extern void initDemoCredits(Settings::SettingsGroup &group);
    initDemoCredits(gSettingsGroup);
    {
        auto& saveInfo = gSettingsGroup.getSaveInfo();
        saveInfo.mSaveName = Settings::getGroupName(gSettingsGroup);
        saveInfo.mBlocks = 1;
        saveInfo.mGameCode = 'GMSE';
        saveInfo.mCompany = 0x3034;  // '04'
        saveInfo.mBannerFmt = CARD_BANNER_CI;
        saveInfo.mBannerImage = reinterpret_cast<const ResTIMG *>(gSaveBnr);
        saveInfo.mIconFmt = CARD_ICON_CI;
        saveInfo.mIconSpeed = CARD_SPEED_SLOW;
        saveInfo.mIconCount = 2;
        saveInfo.mIconTable = reinterpret_cast<const ResTIMG *>(gSaveIcon);
        saveInfo.mSaveGlobal = true;
    }

    // Register module
    BetterSMS::registerModule(&sModuleInfo);

    // Register callbacks
    Application::registerContextCallback(11, directCharacterSelectMenu);

    Stage::registerInitCallback("__reset_darkness", initToDefault);
    Stage::registerUpdateCallback("__update_darkness", manageShineDarkness);

    // Demo
    Stage::registerInitCallback("__force_yoshi_unlock", forceYoshiUnlock);
    Stage::registerUpdateCallback("__check_for_bugs_unlock", unlockSettings);
    Stage::registerUpdateCallback("__check_for_blue_coin_shine", checkForBlueCoinTrade);
    Game::registerBootCallback("__lock_demo_settings", lockModuleSettings);

    Stage::registerInitCallback("__init_player_models", initCharacterArchives);
    Stage::registerInitCallback("__reset_tutorial_ice_stage", resetTutorialIceStageCheckpoints);
    Stage::registerDraw2DCallback("__update_player_hud", updatePlayerHUD);
    Player::registerInitCallback("__init_eclipse_data", initEclipseData);
    Player::registerLoadAfterCallback("__init_water_balloons", initializeWaterBalloons);
    Player::registerLoadAfterCallback("__init_player_pos_rot", setPlayerPosRotOnLoad);
    Player::registerUpdateCallback("__update_tutorial_ice_stage", checkTutorialIceStageCheckpoints);
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
    KURIBO_EXECUTE_ON_LOAD{ initModule(); }
    KURIBO_EXECUTE_ON_UNLOAD{ deinitModule(); }
}
KURIBO_MODULE_END()