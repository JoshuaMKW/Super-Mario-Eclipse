#include <SMS/Player/MarioDraw.hxx>

#include <BetterSMS/module.hxx>
#include <BetterSMS/player.hxx>

constexpr int PoundJumpAnimationID = 112;

BETTER_SMS_FOR_CALLBACK void initializePoundJumpAnimation(TApplication *app) {
    bool success = Player::addAnimationDataEx(PoundJumpAnimationID, "hipjump", true, true);
}