#include <Dolphin/OS.h>
#include <JSystem/J2D/J2DOrthoGraph.hxx>
#include <JSystem/J2D/J2DTextBox.hxx>

#include <SMS/System/Application.hxx>

#include <BetterSMS/application.hxx>
#include <BetterSMS/game.hxx>
#include <BetterSMS/module.hxx>
#include <BetterSMS/player.hxx>
#include <BetterSMS/stage.hxx>

#include "p_settings.hxx"

// Application
extern bool directCharacterSelectMenu(TApplication *app);

// Light
extern void manageShineDarkness(TMarDirector *director);

// Yoshi
extern void adjustYoshiTounge(TMario *player, bool isMario);

static BetterSMS::ModuleInfo sModuleInfo{ "Super Mario Eclipse", 1, 0, &gSettingsGroup };

static void initModule() {
    OSReport("Initializing Super Mario Eclipse...\n");

    // Register settings
    gSettingsGroup.addSetting(&gHUDSetting);
    gSettingsGroup.addSetting(&gBugsSetting);
    {
        auto& saveInfo = gSettingsGroup.getSaveInfo();
        saveInfo.mSaveName = gSettingsGroup.getName();
        saveInfo.mBlocks = 1;
        saveInfo.mGameCode = 'GMSE';
        saveInfo.mCompany = 0x3034;  // '04'
        saveInfo.mBannerFmt = CARD_BANNER_CI;
        saveInfo.mBannerImage = GetResourceTextureHeader(gSaveBnr);
        saveInfo.mIconFmt = CARD_ICON_CI;
        saveInfo.mIconSpeed = CARD_SPEED_FAST;
        saveInfo.mIconCount = 2;
        saveInfo.mIconTable = GetResourceTextureHeader(gSaveIcon);
        saveInfo.mSaveGlobal = true;
    }

    // Register module
    BetterSMS::registerModule("Super Mario Eclipse", &sModuleInfo);

    // Register callbacks
    Application::registerContextCallback(11, directCharacterSelectMenu);

    Stage::registerUpdateCallback("__update_darkness", manageShineDarkness);

    Player::registerUpdateProcess("__update_yoshi_tounge", adjustYoshiTounge);
}

static void deinitModule() {
    OSReport("Deinitializing Module...\n");

    BetterSMS::deregisterModule("Super Mario Eclipse");

    // Cleanup callbacks
    Application::deregisterContextCallback(11);

    Stage::deregisterUpdateCallback("__update_darkness");

    Player::deregisterUpdateProcess("__update_yoshi_tounge");


}

// Definition block
KURIBO_MODULE_BEGIN("Super Mario Eclipse", "JoshuaMK", "v1.0") {
    // Set the load and unload callbacks to our registration functions
    KURIBO_EXECUTE_ON_LOAD{ initModule(); }
    KURIBO_EXECUTE_ON_UNLOAD{ deinitModule(); }
}
KURIBO_MODULE_END()