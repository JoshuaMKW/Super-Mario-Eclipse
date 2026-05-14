#include <SMS/Manager/FlagManager.hxx>
#include <SMS/System/Application.hxx>
#include <SMS/assert.h>
#include <SMS/macros.h>

#include <BetterSMS/libs/global_vector.hxx>
#include <BetterSMS/module.hxx>
#include <BetterSMS/player.hxx>
#include <BetterSMS/stage.hxx>

#include "globals.hxx"
#include "player.hxx"

extern Settings::SwitchSetting gLivesSetting;
extern Settings::SwitchSetting gSunglassSetting;

BETTER_SMS_FOR_CALLBACK void initEclipseData(TMario *player, bool isMario) {
    auto *player_state = new SME::Player::PlayerState();

    BetterSMS::Player::registerData(player, SME::Player::data_key, player_state);

    if (gLivesSetting.getBool()) {
        TFlagManager::smInstance->setFlag(0x20001, 99);
    }

    if (gSunglassSetting.getBool()) {
        if (TFlagManager::smInstance->getFlag(0x3001B) != 0) {
            /*player->wearGlass();*/
            ((u16 *)player->mCap)[2] |= 4;

            if (TFlagManager::smInstance->getShineFlag(119)) {
                player->mAttributes.mIsShineShirt = true;
            }
        }
    }
}

static void hookForSunglassWear(TMario *player) {
    player->emitGetEffect();
    TFlagManager::smInstance->setFlag(0x3001B, 1);
}
SMS_PATCH_BL(SMS_PORT_REGION(0x80247FC8, 0, 0, 0), hookForSunglassWear);

static void hookForSunglassUnwear(TMario *player) {
    player->emitGetEffect();
    TFlagManager::smInstance->setFlag(0x3001B, 0);
}
SMS_PATCH_BL(SMS_PORT_REGION(0x80247F7C, 0, 0, 0), hookForSunglassUnwear);

// Sunglasses no longer fade the screen
SMS_WRITE_32(SMS_PORT_REGION(0x8028ADE8, 0, 0, 0), 0x60000000);
SMS_WRITE_32(SMS_PORT_REGION(0x8028AE24, 0, 0, 0), 0x60000000);

//SMS_WRITE_32(0x8028900c, 0x60000000);
