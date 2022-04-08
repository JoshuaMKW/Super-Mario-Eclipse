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

static void patchWaterDownWarp(f32 y) {
  TMario *player;
  SME_FROM_GPR(31, player);

  if (player->mFloorTriangleWater == player->mFloorTriangle)
    changePlayerStatus__6TMarioFUlUlb(player, TMario::STATE_FALL, 0, false);
  else
    player->mPosition.y = y;
}
SME_PATCH_BL(SME_PORT_REGION(0x80272710, 0x8026A49C, 0, 0), patchWaterDownWarp);

static bool canDiePlane(f32 floorY) {
  TMario *player;
  SME_FROM_GPR(31, player);

  Vec playerPos;
  player->JSGGetTranslation(&playerPos);

  return (floorY > playerPos.y) && !player->mAttributes.mIsGameOver;
}
SME_PATCH_BL(SME_PORT_REGION(0x8024FB54, 0x802478E4, 0, 0), canDiePlane);
SME_WRITE_32(SME_PORT_REGION(0x8024FB58, 0x802478E8, 0, 0), 0x2C030000);
SME_WRITE_32(SME_PORT_REGION(0x8024FB5C, 0x802478EC, 0, 0), 0x41820084);

static f32 enhanceWaterCheck(f32 x, f32 y, f32 z, TMario *player) {
  SME_FROM_GPR(29, player);

  const TBGCheckData **tri =
      const_cast<const TBGCheckData **>(&player->mFloorTriangleWater);

  const TMapCollisionData *mapCol = gpMapCollisionData;
  {
    f32 yPos = mapCol->checkGround(x, player->mCeilingAbove - 10.0f, z, 0, tri);
    if (*tri && (*tri)->mCollisionType > 255 && (*tri)->mCollisionType < 260)
      return yPos;
  }

  return mapCol->checkGround(x, y, z, 0, tri);
}
SME_PATCH_BL(SME_PORT_REGION(0x8024F12C, 0x80246EB8, 0, 0), enhanceWaterCheck);

#endif