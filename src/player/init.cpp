#include <SMS/System/Application.hxx>
#include <SMS/assert.h>
#include <SMS/macros.h>

#include <BetterSMS/module.hxx>
#include <BetterSMS/player.hxx>
#include <BetterSMS/stage.hxx>
#include <BetterSMS/libs/global_vector.hxx>

#include "globals.hxx"
#include "player.hxx"

BETTER_SMS_FOR_CALLBACK void initEclipseData(TMario* player, bool isMario) {
    BetterSMS::Player::registerData(player, SME::Player::data_key, new SME::Player::PlayerState());
}