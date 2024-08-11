#include <Dolphin/OS.h>
#include <JSystem/J2D/J2DOrthoGraph.hxx>
#include <JSystem/J2D/J2DTextBox.hxx>

#include <SMS/SPC/SpcInterp.hxx>
#include <SMS/System/Application.hxx>

#include <BetterSMS/application.hxx>
#include <BetterSMS/debug.hxx>
#include <BetterSMS/game.hxx>
#include <BetterSMS/module.hxx>
#include <BetterSMS/object.hxx>
#include <BetterSMS/player.hxx>
#include <BetterSMS/stage.hxx>
#include <BetterSMS/sunscript.hxx>

#include "enemy/dark_zhine.hxx"
#include "enemy/firey_petey.hxx"
#include "menu/character_select.hxx"
#include "object/button.hxx"
#include "object/cannonball.hxx"
#include "object/cannonbox.hxx"
#include "object/darkness_effect.hxx"
#include "object/follow_key.hxx"
#include "object/jizo_stone.hxx"
#include "object/key_chest.hxx"
#include "object/launch_star.hxx"
#include "object/tornado_obj.hxx"
#include "object/water_balloon.hxx"
#include "object/water_mine.hxx"
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
extern void forcePlayerZOn2D(TMario *player, bool isMario);
extern void resetCoinsOnUniqueStage(TMarDirector *director);
extern void updateWarpStatesForCruiserCabin(TMarDirector *director);

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
extern void setPlayerStartPos(TMario *player);

// Camera
extern void resetFixedCameraOnLoad(TMarDirector *director);

// HUD
extern void updatePlayerHUD(TMarDirector *, const J2DOrthoGraph *);

// GAME BEHAVIOR
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
extern void setTutorialVisited(TApplication *director);
extern void resetCruiserUnlocked(TMarDirector *director);
extern void checkForCruiserUnlocked(TMarDirector *director);

// SETTINGS
extern void checkForCompletionAwards(TApplication *);

// SPC
extern void evSetFixedCameraState(TSpcInterp *spc, u32 argc);
extern void evSetFixedCameraPos(TSpcInterp *spc, u32 argc);
extern void evSetFixedCameraLookAt(TSpcInterp *spc, u32 argc);
extern void evSetFixedCameraFollowActor(TSpcInterp *spc, u32 argc);
extern void evSetFixedCameraFov(TSpcInterp *spc, u32 argc);

extern void evGetAddressFromGraphWebName(TSpcInterp *spc, u32 argc);
extern void evNpcSetGraphWeb(TSpcInterp *spc, u32 argc);
extern void evNpcStepOn(TSpcInterp *spc, u32 argc);
extern void evNpcWaitOn(TSpcInterp *spc, u32 argc);
extern void evNpcMadOn(TSpcInterp *spc, u32 argc);
extern void evIsInSightOfActor(TSpcInterp *spc, u32 argc);
extern void evSetLiveActorFlag(TSpcInterp *spc, u32 argc);
extern void evFadeScreen(TSpcInterp *spc, u32 argc);
extern void evIsFadingScreen(TSpcInterp *spc, u32 argc);

// Objects
extern void cannonBallCollideInteractor(THitActor *self, TMario *player);

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
    Application::showSettingsOnFirstBoot(true);

    // Register callbacks
    Application::registerContextCallback(CONTEXT_CHARACTER_SELECT, directCharacterSelectMenu);
    Game::addInitCallback(initializeStageInfo);
    Game::addChangeCallback(setIntroStage);

    Game::addInitCallback(initializePoundJumpAnimation);

    Stage::addInitCallback(initToDefault);
    Stage::addUpdateCallback(manageShineDarkness);

    // Demo
    Stage::addInitCallback(forceYoshiUnlock);
    Stage::addUpdateCallback(unlockSettings);
    Game::addBootCallback(lockModuleSettings);

    Stage::addInitCallback(resetCoinsOnUniqueStage);
    Stage::addInitCallback(resetForExStage);
    Stage::addInitCallback(resetCruiserUnlocked);
    Stage::addUpdateCallback(checkForCruiserUnlocked);
    Stage::addUpdateCallback(updateWarpStatesForCruiserCabin);
    Player::addUpdateCallback(forcePlayerZOn2D);

    Stage::addInitCallback(resetFixedCameraOnLoad);

    Spc::registerBuiltinFunction("setFixedCameraState", evSetFixedCameraState);
    Spc::registerBuiltinFunction("setFixedCameraPos", evSetFixedCameraPos);
    Spc::registerBuiltinFunction("setFixedCameraLookAt", evSetFixedCameraLookAt);
    Spc::registerBuiltinFunction("setFixedCameraFollowActor", evSetFixedCameraFollowActor);
    Spc::registerBuiltinFunction("setFixedCameraFov", evSetFixedCameraFov);
    Spc::registerBuiltinFunction("getAddressFromGraphWebName", evGetAddressFromGraphWebName);
    Spc::registerBuiltinFunction("npcSetGraphWeb", evNpcSetGraphWeb);
    Spc::registerBuiltinFunction("npcStepOn", evNpcStepOn);
    Spc::registerBuiltinFunction("npcWaitOn", evNpcWaitOn);
    Spc::registerBuiltinFunction("npcMadOn", evNpcMadOn);
    Spc::registerBuiltinFunction("isInSightOfActor", evIsInSightOfActor);
    Spc::registerBuiltinFunction("setLiveActorFlag", evSetLiveActorFlag);
    Spc::registerBuiltinFunction("fadeScreen", evFadeScreen);
    Spc::registerBuiltinFunction("isFadingScreen", evIsFadingScreen);

    Stage::addInitCallback(initCharacterArchives);
    Stage::addDraw2DCallback(updatePlayerHUD);
    Stage::addExitCallback(setTutorialVisited);

    Player::addInitCallback(initEclipseData);
    Player::addLoadAfterCallback(initializeWaterBalloons);
    Player::addLoadAfterCallback(setPlayerStartPos);

    Stage::addInitCallback(resetTutorialIceStageCheckpoints);
    Stage::addInitCallback(resetTutorialCasinoStageCheckpoints);
    Stage::addInitCallback(resetTutorialPiantaPitStageCheckpoints);
    Player::addUpdateCallback(checkTutorialIceStageCheckpoints);
    Player::addUpdateCallback(checkTutorialCasinoStageCheckpoints);
    Player::addUpdateCallback(checkTutorialPiantaPitStageCheckpoints);

    Stage::addInitCallback(initColdState);
    Player::addUpdateCallback(processColdState);
    Player::addUpdateCallback(checkTutorialCollisionRespawn);
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
    Objects::registerObjectAsMapObj("WaterMine", &waterMineData, TWaterMine::instantiate);

    Objects::registerObjectAsMapObj("CannonBall", &cannonBallData, TCannonBall::instantiate);
    Objects::registerObjectCollideInteractor(cannonBallData.mObjectID, cannonBallCollideInteractor);

    Objects::registerObjectAsMapObj("CannonBox", &cannonBoxData, TCannonBox::instantiate);
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

// ObjHitCheck fixes for Bianco
SMS_WRITE_32(SMS_PORT_REGION(0x8021B340, 0, 0, 0), 0x60638004);  // Allocate about 1.8x memory
SMS_WRITE_32(SMS_PORT_REGION(0x8021B354, 0, 0, 0), 0x38E03000);  // Allocate about 1.8x array slices

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