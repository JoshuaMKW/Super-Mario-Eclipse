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

BETTER_SMS_FOR_CALLBACK void initEclipseData(TMario *player, bool isMario) {
    BetterSMS::Player::registerData(player, SME::Player::data_key, new SME::Player::PlayerState());

    if (gLivesSetting.getBool()) {
        TFlagManager::smInstance->setFlag(0x20001, 99);
    }
}