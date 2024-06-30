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

#include "enemy/dark_zhine.hxx"
#include "enemy/firey_petey.hxx"
#include "object/darkness_effect.hxx"
#include "object/follow_key.hxx"
#include "object/jizo_stone.hxx"
#include "object/key_chest.hxx"
#include "object/launch_star.hxx"
#include "object/button.hxx"
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
extern void adjustYoshiTongue(TMario *player, bool isMario);

// Stage
extern void initializeStageInfo(TApplication *app);
extern void resetForExStage(TMarDirector *director);

// Player
extern void initializePoundJumpAnimation(TApplication *app);
extern void initEclipseData(TMario *player, bool isMario);
extern void initCharacterArchives(TMarDirector *director);
extern void initializeWaterBalloons(TMario *player);
extern void createWaterBalloonAndThrow(TMario *player, bool isMario);
extern bool holdPlayerState(TMario *player);
extern bool launchPlayerState(TMario *player);
extern void initColdState(TMarDirector *director);
extern void processColdState(TMario *player, bool isMario);

// HUD
extern void updatePlayerHUD(TMarDirector *, const J2DOrthoGraph *);

// DEMO
extern void lockModuleSettings(TApplication *app);
extern void unlockSettings(TMarDirector *director);
extern void setPlayerPosRotOnLoad(TMario *player);
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
    BetterSMS::registerModule(sModuleInfo);

    Game::setMaxShines(MaxShineCount);

    // Register callbacks
    Application::registerContextCallback(11, directCharacterSelectMenu);
    Game::addInitCallback(initializeStageInfo);
    Game::addChangeCallback(setIntroStage);

    Game::addInitCallback(initializePoundJumpAnimation);

    Stage::addInitCallback(initToDefault);
    Stage::addUpdateCallback(manageShineDarkness);

    // Demo
    Stage::addInitCallback(forceYoshiUnlock);
    Stage::addUpdateCallback(unlockSettings);
    Game::addBootCallback(lockModuleSettings);

    Stage::addInitCallback(initCharacterArchives);
    Stage::addInitCallback(resetForExStage);
    Stage::addInitCallback(resetTutorialIceStageCheckpoints);
    Stage::addInitCallback(resetTutorialCasinoStageCheckpoints);
    Stage::addInitCallback(resetTutorialPiantaPitStageCheckpoints);
    Stage::addDraw2DCallback(updatePlayerHUD);
    Player::addInitCallback(initEclipseData);
    Player::addLoadAfterCallback(initializeWaterBalloons);
    Player::addLoadAfterCallback(setPlayerPosRotOnLoad);
    Player::addUpdateCallback(checkTutorialIceStageCheckpoints);
    Player::addUpdateCallback(checkTutorialCasinoStageCheckpoints);
    Player::addUpdateCallback(checkTutorialPiantaPitStageCheckpoints);
    Stage::addInitCallback(initColdState);
    Player::addUpdateCallback(processColdState);
    // Player::addUpdateCallback(checkTutorialCollisionRespawn);
    Player::addUpdateCallback(createWaterBalloonAndThrow);
    Player::addUpdateCallback(adjustYoshiTongue);
    Player::registerStateMachine(PlayerLaunchStarWait, holdPlayerState);
    Player::registerStateMachine(PlayerLaunchStarLaunch, launchPlayerState);
    Debug::addUpdateCallback(checkForCompletionAwards);

    Objects::registerObjectAsMisc("DarknessEffect", TDarknessEffect::instantiate);
    Objects::registerObjectAsMapObj("Tornado", &tornadoData, TTornadoMapObj::instantiate);
    Objects::registerObjectAsMapObj("WaterBalloon", &waterBalloonData, TWaterBalloon::instantiate);
    Objects::registerObjectAsMapObj("LaunchStar", &launchStarData, TLaunchStarObj::instantiate);
    Objects::registerObjectAsMapObj("KeyFollow", &followKeyData, TFollowKey::instantiate);
    Objects::registerObjectAsMapObj("KeyChest", &keyChestData, TKeyChest::instantiate);
    Objects::registerObjectAsMapObj("JizoStone", &jizoStoneData, TJizoStone::instantiate);
    Objects::registerObjectAsMapObj("ButtonSwitch", &buttonSwitchData, TButtonSwitch::instantiate);
    Objects::registerObjectAsMisc("FireyPetey", TFireyPetey::instantiate);
    Objects::registerObjectAsMisc("FireyPeteyManager", TFireyPeteyManager::instantiate);
    Objects::registerObjectAsMisc("DarkZhine", TDarkZhine::instantiate);
    Objects::registerObjectAsMisc("DarkZhineManager", TDarkZhineManager::instantiate);
}

// Definition block
KURIBO_MODULE_BEGIN("Super Mario Eclipse", "JoshuaMK", SUPER_MARIO_ECLIPSE_VERSION) {
    // Set the load and unload callbacks to our registration functions
    KURIBO_EXECUTE_ON_LOAD { initModule(); }
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