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

using namespace SME::Util::Math;

static u32 patchYStorage() {
  TMario *player;
  SME_FROM_GPR(31, player);

  if (player->mState != static_cast<u32>(TMario::STATE_IDLE))
    player->mSpeed.y = 0.0f;

  return 0;
}
SME_PATCH_BL(SME_PORT_REGION(0x802571F0, 0x8024EF7C, 0, 0), patchYStorage);

static void patchRideMovementUpWarp(Mtx out, Vec *ride, Vec *pos) {
  TMario *player;
  SME_FROM_GPR(30, player);

  if (!(player->mState & static_cast<u32>(TMario::STATE_AIRBORN))) {
    PSMTXMultVec(out, ride, pos);
  }
}
SME_PATCH_BL(SME_PORT_REGION(0x80250514, 0x802482A0, 0, 0), patchRideMovementUpWarp);

static void patchRoofCollisionSpeed(TMario *player, f32 _speed) {
  TBGCheckData *roof = player->mRoofTriangle;
  if (!roof) {
    setPlayerVelocity__6TMarioFf(player, _speed);
    return;
  }

  TVec3f down(0.0f, -1.0f, 0.0f);

  TVec3f nroofvec;
  Vector3::normalized(*roof->getNormal(), nroofvec);

  const f32 ratio = Vector3::angleBetween(nroofvec, down);
  setPlayerVelocity__6TMarioFf(player, lerp(_speed, player->mForwardSpeed, ratio));
}
SME_PATCH_BL(SME_PORT_REGION(0x802569bc, 0x8024E748, 0, 0), patchRoofCollisionSpeed);

#endif