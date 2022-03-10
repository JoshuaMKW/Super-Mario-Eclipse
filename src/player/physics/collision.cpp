#include "JGeometry.hxx"
#include "sms/actor/Mario.hxx"
#include "sms/sound/MSound.hxx"
#include "sms/sound/MSoundSESystem.hxx"

#include "MTX.h"
#include "SME.hxx"
#include "collision/WarpCollision.hxx"
#include "libs/sGeometry.hxx"

using namespace SME;
using namespace SME::Util::Math;

static SME_PURE_ASM void checkGrabHeight() {
  asm volatile("lfs 0, " SME_STRINGIZE(SME_PORT_REGION(
      -0xED4, -0x105c, 0, 0)) "(2)\n\t"
                        "lfs 4, 0x28(29)                     \n\t"
                        "fmuls 0, 0, 4                       \n\t"
                        "blr                                 \n\t");
}
SME_PATCH_BL(SME_PORT_REGION(0x80256D34, 0x8024EAC0, 0, 0), checkGrabHeight);

static SME_PURE_ASM void setCollisionHeight1() {
  asm volatile("lfs 1, " SME_STRINGIZE(
      SME_PORT_REGION(-0xEDC, -0x1064, 0, 0)) "(2)                  \n\t"
                                        "lfs 0, 0x28(22)            \n\t"
                                        "fmuls 1, 0, 1              \n\t"
                                        "blr                        \n\t");
}
SME_PATCH_BL(SME_PORT_REGION(0x8025696C, 0x8024E6F8, 0, 0), setCollisionHeight1);

static SME_PURE_ASM void setCollisionHeight2() {
  asm volatile("lfs 2, " SME_STRINGIZE(
      SME_PORT_REGION(-0xEDC, -0x1064, 0, 0)) "(2)                        \n\t"
                                        "lfs 0, 0x28(29)            \n\t"
                                        "fmuls 2, 0, 2              \n\t"
                                        "blr                        \n\t");
}
SME_PATCH_BL(SME_PORT_REGION(0x80256D14, 0x8024EAA0, 0, 0), setCollisionHeight2);

static SME_PURE_ASM void setCollisionHeight3() {
  asm volatile("lfs 0, " SME_STRINGIZE(
      SME_PORT_REGION(-0xEDC, -0x1064, 0, 0)) "(2)                        \n\t"
                                        "lfs 2, 0x28(30)            \n\t"
                                        "fmuls 0, 2, 0              \n\t"
                                        "blr                        \n\t");
}
SME_PATCH_BL(SME_PORT_REGION(0x802573FC, 0x8024F188, 0, 0), setCollisionHeight3);

static void setCollisionWidth() {
  TMario *player;
  SME_FROM_GPR(29, player);

  f32 width = 50.0f;

  Vec size;
  player->JSGGetScaling(&size);

  Class::TPlayerData *playerData = TGlobals::getPlayerData(player);
  if (playerData->isMario())
    width *= size.x;

  player->mCollisionXZSize = width;
}
SME_PATCH_BL(SME_PORT_REGION(0x802505F4, 0x80248380, 0, 0), setCollisionWidth);

static f32 manageGrabLength() {
  TMario *player;
  SME_FROM_GPR(29, player);

  Vec size;
  player->JSGGetScaling(&size);

  return 60.0f * size.z;
}
SME_PATCH_BL(SME_PORT_REGION(0x80256CE8, 0x8024EA74, 0, 0), manageGrabLength);
SME_WRITE_32(SME_PORT_REGION(0x80256CFC, 0x8024EA88, 0, 0), 0xEC01283C);
SME_WRITE_32(SME_PORT_REGION(0x80256D04, 0x8024EA90, 0, 0), 0xC05E003C);
SME_WRITE_32(SME_PORT_REGION(0x80256D0C, 0x8024EA98, 0, 0), 0xEC0100BC);

static f32 setBounceYSpeed() {
  TMario *player;
  SME_FROM_GPR(30, player);

  Vec size;
  player->JSGGetScaling(&size);

  return 130.0f * size.y;
}
SME_PATCH_BL(SME_PORT_REGION(0x80254720, 0x8024C4AC, 0, 0), setBounceYSpeed);
SME_WRITE_32(SME_PORT_REGION(0x80254724, 0x8024C4B0, 0, 0), 0xD01E00A8);