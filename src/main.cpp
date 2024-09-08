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
#include <BetterSMS/thp.hxx>

#include "enemy/bowser_car.hxx"
#include "enemy/dark_zhine.hxx"
#include "enemy/firey_petey.hxx"
#include "menu/character_select.hxx"
#include "object/button.hxx"
#include "object/cannonball.hxx"
#include "object/cannonbox.hxx"
#include "object/darkness_effect.hxx"
#include "object/elevator_object.hxx"
#include "object/follow_key.hxx"
#include "object/iron_crate.hxx"
#include "object/jizo_stone.hxx"
#include "object/key_chest.hxx"
#include "object/launch_star.hxx"
#include "object/pizzabox.hxx"
#include "object/tornado_obj.hxx"
#include "object/water_balloon.hxx"
#include "object/water_mine.hxx"
#include "p_settings.hxx"

extern BugsExploitsSetting gBugsSetting;
extern MirrorModeFlag gMirrorModeSetting;
extern DarknessSetting gDarknessSetting;
extern Settings::SwitchSetting gLivesSetting;
extern Settings::SwitchSetting gSkipMovieSetting;
extern SpeedrunSetting gSpeedrunSetting;

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
extern void resetStateForStage(TMarDirector *director);
extern void updateWarpStatesForCruiserCabin(TMarDirector *director);
extern void warpContextUpdater(TMario *player, bool isMario);
extern void checkForCharacterUnlocks(TMarDirector *director);
extern void resetCoinsOnStageExit(TApplication *app);
extern void reset100CoinState(TMarDirector *director);

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
extern void resetBalloonMessagePool(TMarDirector *director);
extern void checkBalloonMessagePool(TMarDirector *director);
extern void fluddMessageSubsystemReset(TMarDirector *director);
extern void fluddMessageSubsystem(TMarDirector *director);

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

extern void initializeShineBlueTracker(TMarDirector *director);
extern void updateShineBlueTracker(TMarDirector *director);
extern void renderShineBlueTracker(TMarDirector *director, const J2DOrthoGraph *graph);

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
extern void evCheckBrokenWatermelon(TSpcInterp *spc, u32 argc);
extern void evAppearShineFromNPCLocal(TSpcInterp *spc, u32 argc);
extern void evAppearShineFromNPCLocalWithoutDemo(TSpcInterp *spc, u32 argc);
extern void evAppearFluddTip(TSpcInterp *spc, u32 argc);

// Objects
extern void cannonBallCollideInteractor(THitActor *self, TMario *player);

// SPEEDRUN STUFF
extern void checkForCPUOverclock(TMarDirector *app);
extern void initCPUOverclock(TApplication *app);
extern void renderCPUOverclock(TMarDirector *director, const J2DOrthoGraph *graph);
extern void updateSpeedrunTimer(TApplication *app);
extern void renderSpeedrunTimer(TApplication *app, const J2DOrthoGraph *graph);

static BetterSMS::ModuleInfo sModuleInfo("Super Mario Eclipse", 1, 0, &gSettingsGroup);

extern void initDemoCredits();

static void initModule() {
    // Register settings
    // gSettingsGroup.addSetting(&gHUDSetting);
    gSettingsGroup.addSetting(&gBugsSetting);
    gSettingsGroup.addSetting(&gTutorialSetting);
    gSettingsGroup.addSetting(&gMirrorModeSetting);
    gSettingsGroup.addSetting(&gLivesSetting);
    gSettingsGroup.addSetting(&gDarknessSetting);
    gSettingsGroup.addSetting(&gSkipMovieSetting);
    gSettingsGroup.addSetting(&gSpeedrunSetting);

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

    initDemoCredits();

    Game::setMaxShines(MaxShineCount);
    Application::showSettingsOnFirstBoot(true);

    THP::addTHP(31, "piantissimo_join.thp");
    THP::addTHP(32, "piantissimo.thp");
    THP::addTHP(33, "luigi.thp");
    THP::addTHP(34, "vacationAA.thp");
    THP::addTHP(35, "vacationAB.thp");
    THP::addTHP(36, "vacationAC.thp");
    THP::addTHP(37, "postcard.thp");

    // Register callbacks
    Application::registerContextCallback(CONTEXT_CHARACTER_SELECT, directCharacterSelectMenu);
    Game::addInitCallback(initializeStageInfo);
    Game::addChangeCallback(setIntroStage);

    Game::addInitCallback(initializePoundJumpAnimation);

    Stage::addInitCallback(initToDefault);
    Stage::addUpdateCallback(manageShineDarkness);

    Stage::addInitCallback(reset100CoinState);
    Stage::addInitCallback(initializeShineBlueTracker);
    Stage::addUpdateCallback(updateShineBlueTracker);
    Stage::addDraw2DCallback(renderShineBlueTracker);

    // Demo
    // Stage::addInitCallback(forceYoshiUnlock);
    Stage::addUpdateCallback(unlockSettings);
    Game::addBootCallback(lockModuleSettings);

    Stage::addInitCallback(resetStateForStage);
    Stage::addInitCallback(resetForExStage);
    Stage::addInitCallback(resetCruiserUnlocked);
    Stage::addUpdateCallback(checkForCharacterUnlocks);
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
    Spc::registerBuiltinFunction("checkBrokenWatermelon", evCheckBrokenWatermelon);
    Spc::registerBuiltinFunction("appearShineFromNPCLocal", evAppearShineFromNPCLocal);
    Spc::registerBuiltinFunction("appearShineFromNPCLocalWithoutDemo",
                                 evAppearShineFromNPCLocalWithoutDemo);
    Spc::registerBuiltinFunction("appearFluddTip", evAppearFluddTip);

    Stage::addInitCallback(initCharacterArchives);
    Stage::addDraw2DCallback(updatePlayerHUD);
    Stage::addExitCallback(setTutorialVisited);
    Stage::addExitCallback(resetCoinsOnStageExit);

    Stage::addInitCallback(resetBalloonMessagePool);
    Stage::addUpdateCallback(checkBalloonMessagePool);
    Stage::addInitCallback(fluddMessageSubsystemReset);
    Stage::addUpdateCallback(fluddMessageSubsystem);

    Player::addInitCallback(initEclipseData);
    Player::addLoadAfterCallback(initializeWaterBalloons);
    Player::addLoadAfterCallback(setPlayerStartPos);
    Player::addUpdateCallback(warpContextUpdater);

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
    Objects::registerObjectAsMapObj("IronCrate", &ironCrateData, TIronCrate::instantiate);

    Objects::registerObjectAsMapObj("CannonBall", &cannonBallData, TCannonBall::instantiate);
    Objects::registerObjectCollideInteractor(cannonBallData.mObjectID, cannonBallCollideInteractor);

    Objects::registerObjectAsMapObj("PizzaBox", &pizzaBoxData, TPizzaBox::instantiate);
    Objects::registerObjectCollideInteractor(pizzaBoxData.mObjectID, cannonBallCollideInteractor);

    Objects::registerObjectAsMapObj("CannonBox", &cannonBoxData, TCannonBox::instantiate);
    Objects::registerObjectAsMapObj("ElevatorDoor", &elevatorObjectData,
                                    TElevatorObject::instantiate);
    Objects::registerObjectAsMisc("FireyPetey", TFireyPetey::instantiate);
    Objects::registerObjectAsMisc("FireyPeteyManager", TFireyPeteyManager::instantiate);
    Objects::registerObjectAsMisc("DarkZhine", TDarkZhine::instantiate);
    Objects::registerObjectAsMisc("DarkZhineManager", TDarkZhineManager::instantiate);
    Objects::registerObjectAsMisc("BossBowserCar", TBossBowserCar::instantiate);
    Objects::registerObjectAsMisc("BossBowserCarManager", TBossBowserCarManager::instantiate);

    Game::addBootCallback(initCPUOverclock);
    Stage::addUpdateCallback(checkForCPUOverclock);
    Game::addLoopCallback(updateSpeedrunTimer);
    Stage::addDraw2DCallback(renderCPUOverclock);
    Game::addPostDrawCallback(renderSpeedrunTimer);
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