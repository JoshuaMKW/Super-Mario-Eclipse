#include <Dolphin/types.h>

#include <BetterSMS/module.hxx>
#include <BetterSMS/player.hxx>

using namespace BetterSMS;

static constexpr u32 SwapColType = 16420;
static constexpr u32 DemoStage   = 0x205;
bool gIsSwappingWarp             = false;

static bool active = false;
// void demoHandler(TMario *player) {
//   if (!gpMarDirector)
//     return;
//
//   if ((u32)player->mFloorTriangle < 0x80000000 || (u32)player->mFloorTriangle > 0x817FFFFC)
//     return;
//
// #if 1
//   auto *playerData = Player::getData(player);
//   if (player->mFloorTriangle->mType == SwapColType &&
//       !playerData->mCollisionFlags.mIsAirborn &&
//       !playerData->mCollisionFlags.mIsCollisionUsed) {
//     gpMarDirector->setNextStage(DemoStage, player);
//     playerData->mCollisionFlags.mIsCollisionUsed = true;
//     playerData->mCollisionFlags.mIsFaceUsed = true;
//     gIsSwappingWarp = true;
//   }
// #else
//   TPlayerData *playerData = TGlobals::getPlayerData(player);
//   if (player->mController->mButtons.mInput ==
//       (TMarioGamePad::X | TMarioGamePad::Z)) {
//     gpMarDirector->setNextStage(DemoStage, nullptr /*player*/);
//     active = true;
//   } else {
//     active = false;
//   }
// #endif
// }

//// To Plaza From Death
// SMS_WRITE_32(SMS_PORT_REGION(0x802997A4, 0x8029163C, 0, 0), 0x38000001);
// SMS_WRITE_32(SMS_PORT_REGION(0x802997AC, 0x80291644, 0, 0), 0x38000005);
// SMS_WRITE_32(SMS_PORT_REGION(0x802997BC, 0x80291654, 0, 0), 0x38000001);
// SMS_WRITE_32(SMS_PORT_REGION(0x802997C4, 0x8029165C, 0, 0), 0x38600005);
//
//// To Plaza From Exit Area
// SMS_WRITE_32(SMS_PORT_REGION(0x802991E0, 0x80291078, 0, 0), 0x38000001);
// SMS_WRITE_32(SMS_PORT_REGION(0x802991E8, 0x80291080, 0, 0), 0x38000005);
// SMS_WRITE_32(SMS_PORT_REGION(0x802991F8, 0x80291090, 0, 0), 0x38000001);
// SMS_WRITE_32(SMS_PORT_REGION(0x80299200, 0x80291098, 0, 0), 0x38600005);
//
// SMS_WRITE_32(SMS_PORT_REGION(0x80164E30, 0x80159EA8, 0, 0), 0x38800205);
// SMS_WRITE_32(SMS_PORT_REGION(0x80164E44, 0x80159EBC, 0, 0), 0x38800205);
//
//// To Plaza From Game Over
// SMS_WRITE_32(SMS_PORT_REGION(0x80299314, 0x802911AC, 0, 0), 0x38000001);
// SMS_WRITE_32(SMS_PORT_REGION(0x8029931C, 0x802911AC, 0, 0), 0x38600005);
//
//// Remove subtitles
// SMS_WRITE_32(SMS_PORT_REGION(0x80183060, 0x801799FC, 0, 0), 0x4E800020);
//
//// Remove Ice Fog
// SMS_WRITE_32(SMS_PORT_REGION(0x801C377C, 0x801BB634, 0, 0), 0x38600000);
// SMS_WRITE_32(SMS_PORT_REGION(0x801C37B4, 0x801BB66C, 0, 0), 0x38600000);
//
//// Monument Tower
// SMS_WRITE_32(SMS_PORT_REGION(0x801E7FF4, 0x801DFECC, 0, 0), 0x38600001);
//
//// Bell Tower
// SMS_WRITE_32(SMS_PORT_REGION(0x801E7960, 0x801DF838, 0, 0), 0x38600001);
//
//// Bell Tower 2
SMS_WRITE_32(SMS_PORT_REGION(0x801E793C, 0x801DF814, 0, 0), 0x38600001);