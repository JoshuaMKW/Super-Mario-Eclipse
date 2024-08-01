#include <SMS/Player/MarioDraw.hxx>

#include <BetterSMS/module.hxx>
#include <BetterSMS/player.hxx>

constexpr int PoundJumpAnimationID = 112;
constexpr int CrouchAnimationID = 113;

BETTER_SMS_FOR_CALLBACK void initializePoundJumpAnimation(TApplication *app) {
    Player::addAnimationDataEx(PoundJumpAnimationID, "hipjump", true, true);
}