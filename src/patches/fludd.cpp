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

static SME_PURE_ASM bool makeWaterHitCheckForDeath(TBGCheckData *col) {
  // clang-format off
  asm volatile (
    "lhz 0, 0 (3)             \n\t"
    "cmpwi 0, 2048            \n\t"
    "bne .makeWaterCheck_tmp0 \n\t"
    "li 0, 1025               \n\t"
    ".makeWaterCheck_tmp0:    \n\t"
    SME_PORT_REGION (
      "lis 12, 0x8018           \n\t"
      "ori 12, 12, 0xC36C       \n\t",

      "lis 12, 0x8018           \n\t"
      "ori 12, 12, 0x4bf4       \n\t",

      "lis 12, 0           \n\t"
      "ori 12, 12, 0       \n\t",

      "lis 12, 0           \n\t"
      "ori 12, 12, 0       \n\t"
    )
    "mtctr 12                 \n\t"
    "bctr                     \n\t"
  );
  // clang-format on
}
SME_PATCH_B(SME_PORT_REGION(0x8018C368, 0x80184BF0, 0, 0), makeWaterHitCheckForDeath);

static void normalizeHoverSlopeSpeed(f32 floorPos) {
  TMario *player;
  SME_FROM_GPR(22, player);

  player->mPosition.y = floorPos;

  if (!(player->mState == static_cast<u32>(TMario::State::HOVER)))
    return;

  const f32 playerRotY = f32(player->mAngle.y) / 182.0f;
  const Vec playerForward = {sinf(Math::angleToRadians(-playerRotY)), 0.0f,
                             cosf(Math::angleToRadians(playerRotY))};
  const Vec up = {0.0f, 1.0f, 0.0f};

  Vec floorNormal;
  PSVECNormalize(reinterpret_cast<Vec *>(player->mFloorTriangle->getNormal()),
                 &floorNormal);

  const f32 slopeStrength = PSVECDotProduct(&up, &floorNormal);
  if (slopeStrength < 0.0f)
    return;

  const f32 lookAtRatio =
      Math::Vector3::lookAtRatio(playerForward, floorNormal);

  player->mForwardSpeed =
      Min(player->mForwardSpeed,
          10.0f * Math::clamp(Math::scaleLinearAtAnchor(slopeStrength,
                                                        lookAtRatio, 1.0f),
                              0.0f, 1.0f));
}
SME_PATCH_BL(SME_PORT_REGION(0x802568F0, 0x8024E67C, 0, 0), normalizeHoverSlopeSpeed);

#endif