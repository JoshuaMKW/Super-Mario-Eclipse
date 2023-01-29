#include <Dolphin/OS.h>
#include <JSystem/J2D/J2DOrthoGraph.hxx>
#include <JSystem/J2D/J2DTextBox.hxx>

#include <SMS/System/Application.hxx>

#include <BetterSMS/game.hxx>
#include <BetterSMS/module.hxx>
#include <BetterSMS/player.hxx>
#include <BetterSMS/stage.hxx>

#include "settings.hxx"

static BetterSMS::ModuleInfo sModuleInfo{ "Super Mario Eclipse", 1, 0, &gSettingsGroup };

static void initModule() {
    OSReport("Initializing Module...\n");

    // Register settings
    gSettingsGroup.addSetting(&gLongJumpMappingSetting);
    gSettingsGroup.addSetting(&gLongJumpSetting);
    gSettingsGroup.addSetting(&gBackFlipSetting);
    gSettingsGroup.addSetting(&gHoverBurstSetting);
    gSettingsGroup.addSetting(&gHoverSlideSetting);
    gSettingsGroup.addSetting(&gRocketDiveSetting);
    gSettingsGroup.addSetting(&gFastTurboSetting);
    gSettingsGroup.addSetting(&gFastDiveSetting);
    {
        auto& saveInfo = gSettingsGroup.getSaveInfo();
        saveInfo.mSaveName = gSettingsGroup.getName();
        saveInfo.mBlocks = 1;
        saveInfo.mGameCode = 'GMSB';
        saveInfo.mCompany = 0x3031;  // '01'
        saveInfo.mBannerFmt = CARD_BANNER_CI;
        saveInfo.mBannerImage = GetResourceTextureHeader(gSaveBnr);
        saveInfo.mIconFmt = CARD_ICON_CI;
        saveInfo.mIconSpeed = CARD_SPEED_FAST;
        saveInfo.mIconCount = 2;
        saveInfo.mIconTable = GetResourceTextureHeader(gSaveIcon);
        saveInfo.mSaveGlobal = false;
    }

    // Register module
    BetterSMS::registerModule("Better Sunshine Movement", &sModuleInfo);

    // Register callbacks
    Player::registerInitProcess("__init_fast_turbo", initFastTurbo);
    Player::registerUpdateProcess("__update_turbo_usage", updateTurboContext);
    Player::registerUpdateProcess("__update_hover_burst", checkSpamHover);
    Player::registerUpdateProcess("__update_rocket_dive", checkRocketNozzleDiveBlast);
    Player::registerUpdateProcess("__update_mario_crouch", checkForCrouch);
    Player::registerStateMachine(CrouchState, processCrouch);
}

static void deinitModule() {
    OSReport("Deinitializing Module...\n");

    // Cleanup callbacks
    Player::deregisterUpdateProcess("__update_mario_crouch");
    Player::deregisterStateMachine(CrouchState);
}

// Definition block
KURIBO_MODULE_BEGIN("OurModule", "JoshuaMK", "v1.0") {
    // Set the load and unload callbacks to our registration functions
    KURIBO_EXECUTE_ON_LOAD{ initModule(); }
    KURIBO_EXECUTE_ON_UNLOAD{ deinitModule(); }
}
KURIBO_MODULE_END()