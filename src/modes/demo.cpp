#include "SME.hxx"
#include "types.h"

using namespace SME;
using namespace SME::Class;

#ifdef SME_DEMO

static constexpr u32 SwapColType = 16420;
static constexpr u32 DemoStage = 0x205;
bool gIsSwappingWarp = false;

static bool active = false;
void demoHandler(TMario *player) {
  if (!gpMarDirector)
    return;

  if ((u32)player->mFloorTriangle < 0x80000000 || (u32)player->mFloorTriangle > 0x817FFFFC)
    return;

#if 1
  TPlayerData *playerData = TGlobals::getPlayerData(player);
  if (player->mFloorTriangle->mCollisionType == SwapColType &&
      !playerData->mCollisionFlags.mIsAirborn &&
      !playerData->mCollisionFlags.mIsCollisionUsed) {
    gpMarDirector->setNextStage(DemoStage, player);
    playerData->mCollisionFlags.mIsCollisionUsed = true;
    playerData->mCollisionFlags.mIsFaceUsed = true;
    gIsSwappingWarp = true;
  }
#else
  TPlayerData *playerData = TGlobals::getPlayerData(player);
  if (player->mController->mButtons.mInput ==
      (TMarioGamePad::X | TMarioGamePad::Z)) {
    gpMarDirector->setNextStage(DemoStage, nullptr /*player*/);
    active = true;
  } else {
    active = false;
  }
#endif
}

// To Plaza From Death
SME_WRITE_32(SME_PORT_REGION(0x802997A4, 0, 0, 0), 0x38000001);
SME_WRITE_32(SME_PORT_REGION(0x802997AC, 0, 0, 0), 0x38000005);
SME_WRITE_32(SME_PORT_REGION(0x802997BC, 0, 0, 0), 0x38000001);
SME_WRITE_32(SME_PORT_REGION(0x802997C4, 0, 0, 0), 0x38600005);

// To Plaza From Exit Area
SME_WRITE_32(SME_PORT_REGION(0x802991E0, 0, 0, 0), 0x38000001);
SME_WRITE_32(SME_PORT_REGION(0x802991E8, 0, 0, 0), 0x38000005);
SME_WRITE_32(SME_PORT_REGION(0x802991F8, 0, 0, 0), 0x38000001);
SME_WRITE_32(SME_PORT_REGION(0x80299200, 0, 0, 0), 0x38600005);

SME_WRITE_32(SME_PORT_REGION(0x80164E30, 0, 0, 0), 0x38800205);
SME_WRITE_32(SME_PORT_REGION(0x80164E44, 0, 0, 0), 0x38800205);

// To Plaza From Game Over
SME_WRITE_32(SME_PORT_REGION(0x80299314, 0, 0, 0), 0x38000001);
SME_WRITE_32(SME_PORT_REGION(0x8029931C, 0, 0, 0), 0x38600005);

// Remove subtitles
SME_WRITE_32(SME_PORT_REGION(0x80183060, 0, 0, 0), 0x4E800020);

// Remove Ice Fog
SME_WRITE_32(SME_PORT_REGION(0x801C377C, 0, 0, 0), 0x38600000);
SME_WRITE_32(SME_PORT_REGION(0x801C37B4, 0, 0, 0), 0x38600000);

// Monument Tower
SME_WRITE_32(SME_PORT_REGION(0x801E7FF4, 0, 0, 0), 0x38600001);

// Bell Tower
SME_WRITE_32(SME_PORT_REGION(0x801E7960, 0, 0, 0), 0x38600001);

// Bell Tower 2
SME_WRITE_32(SME_PORT_REGION(0x801E793C, 0, 0, 0), 0x38600001);

#endif