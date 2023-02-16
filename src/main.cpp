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

#include "object/darkness_effect.hxx"
#include "object/water_balloon.hxx"
#include "object/tornado_obj.hxx"
#include "p_settings.hxx"

// Application
extern bool directCharacterSelectMenu(TApplication *app);

// Light
extern void manageShineDarkness(TMarDirector *director);

// Yoshi
extern void adjustYoshiTounge(TMario *player, bool isMario);

// Player
extern void initCharacterArchives(TMarDirector *director);
extern void initializeWaterBalloons(TMario *player);
extern void createWaterBalloonAndThrow(TMario *player, bool isMario);

static BetterSMS::ModuleInfo sModuleInfo("Super Mario Eclipse", 1, 0, &gSettingsGroup);

static void initModule() {
    // Register settings
    gSettingsGroup.addSetting(&gHUDSetting);
    gSettingsGroup.addSetting(&gBugsSetting);
    {
        auto& saveInfo = gSettingsGroup.getSaveInfo();
        saveInfo.mSaveName = Settings::getGroupName(gSettingsGroup);
        saveInfo.mBlocks = 1;
        saveInfo.mGameCode = 'GMSE';
        saveInfo.mCompany = 0x3034;  // '04'
        saveInfo.mBannerFmt = CARD_BANNER_CI;
        saveInfo.mBannerImage = reinterpret_cast<const ResTIMG *>(gSaveBnr);
        saveInfo.mIconFmt = CARD_ICON_CI;
        saveInfo.mIconSpeed = CARD_SPEED_FAST;
        saveInfo.mIconCount = 2;
        saveInfo.mIconTable = reinterpret_cast<const ResTIMG *>(gSaveIcon);
        saveInfo.mSaveGlobal = true;
    }

    // Register module
    BetterSMS::registerModule(&sModuleInfo);

    // Register callbacks
    Application::registerContextCallback(11, directCharacterSelectMenu);

    Stage::registerUpdateCallback("__update_darkness", manageShineDarkness);

    Stage::registerInitCallback("__init_player_models", initCharacterArchives);
    Player::registerLoadAfterCallback("__init_water_balloons", initializeWaterBalloons);
    Player::registerUpdateCallback("__update_water_balloons", createWaterBalloonAndThrow);
    Player::registerUpdateCallback("__update_yoshi_tounge", adjustYoshiTounge);

    Objects::registerObjectAsMisc("DarknessEffect", TDarknessEffect::instantiate);
    Objects::registerObjectAsMapObj("Tornado", &tornadoData, TTornadoMapObj::instantiate);
    Objects::registerObjectAsMapObj("WaterBalloon", &waterBalloonData, TWaterBalloon::instantiate);
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

    BetterSMS::deregisterModule(&sModuleInfo);
}

// Definition block
KURIBO_MODULE_BEGIN("Super Mario Eclipse", "JoshuaMK", "v1.0") {
    // Set the load and unload callbacks to our registration functions
    KURIBO_EXECUTE_ON_LOAD{ initModule(); }
    KURIBO_EXECUTE_ON_UNLOAD{ deinitModule(); }
}
KURIBO_MODULE_END()