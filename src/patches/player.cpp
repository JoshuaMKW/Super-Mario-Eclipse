#include "GX.h"
#include "OS.h"

#include "J2D/J2DPrint.hxx"
#include "JKR/JKRFileLoader.hxx"
#include "sms/GC2D/ConsoleStr.hxx"
#include "sms/actor/Mario.hxx"
#include "sms/mapobj/MapObjNormalLift.hxx"
#include "sms/mapobj/MapObjTree.hxx"

#include "SME.hxx"
#include "defines.h"
#include "macros.h"

#if SME_BUGFIXES

static u32 patchYStorage() {
  TMario *player;
  SME_FROM_GPR(31, player);

  if (player->mState != static_cast<u32>(TMario::State::IDLE))
    player->mSpeed.y = 0.0f;

  return 0;
}
SME_PATCH_BL(SME_PORT_REGION(0x802571F0, 0, 0, 0), patchYStorage);

static void patchRideMovementUpWarp(Mtx out, Vec *ride, Vec *pos) {
  TMario *player;
  SME_FROM_GPR(30, player);

  if (!(player->mState & static_cast<u32>(TMario::State::AIRBORN))) {
    PSMTXMultVec(out, ride, pos);
  }
}
// SME_PATCH_BL(SME_PORT_REGION(0x80250514, 0, 0, 0), patchRideMovementUpWarp);

#endif