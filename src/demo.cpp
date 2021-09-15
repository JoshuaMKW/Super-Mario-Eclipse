#include "SME.hxx"
#include "types.h"

using namespace SME;
using namespace SME::Class;

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