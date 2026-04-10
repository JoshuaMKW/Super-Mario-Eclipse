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
    auto *player_state = new SME::Player::PlayerState();

    if (SME::TGlobals::getCharacterIDFromPlayer(player) == SME::CharacterID::SHADOW_MARIO) {
        player_state->mPortals[0] = new TEMarioPortal("__PortalBegin");
        player_state->mPortals[1] = new TEMarioPortal("__PortalEnd");

        //player_state->mPortals[0]->init();
        //player_state->mPortals[1]->init();

        player_state->mPortals[0]->initAndRegister("emario_portal");
        player_state->mPortals[1]->initAndRegister("emario_portal");
    }

    BetterSMS::Player::registerData(player, SME::Player::data_key, player_state);

    if (gLivesSetting.getBool()) {
        TFlagManager::smInstance->setFlag(0x20001, 99);
    }
}