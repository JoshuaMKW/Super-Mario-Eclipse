#include "MTX.h"
#include "math.h"
#include "types.h"

#include "sms/actor/Mario.hxx"
#include "sms/enemy/EnemyMario.hxx"
#include "sms/npc/BaseNPC.hxx"

#include "SME.hxx"

using namespace SME;

#if SME_SHADOW_MARIO_HEALTHBAR

// ------------ //
// Shadow Mario //
// ------------ //

static JUtility::TColor getEMarioHealthBarRGBA(TEnemyMario *eMario) {
  JUtility::TColor color;
  const s16 maxHealth = ((s16 *)eMario->mEnemyManager)[0x40 / 2];

  color.set(0xFF, 0x00, 0x00, 0xFF);
  color.g = Util::Math::lerp<u8>(0, 255,
                                 static_cast<float>(eMario->mEnemyHealth) /
                                     static_cast<float>(maxHealth));
  color.r -= color.g;

  return color;
}

// extern -> SME.cpp
// 0x8003FDAC
static void manageEMarioHealthWrapper(TEnemyMario *eMario, Mtx *posMtx) {
  *(JUtility::TColor *)0x8040FA90 = getEMarioHealthBarRGBA(eMario);
  drawHPMeter__11TEnemyMarioFPA4_f(eMario, posMtx);
}
SME_WRITE_32(SME_PORT_REGION(0x8003FD94, 0, 0, 0), 0x60000000);
SME_PATCH_BL(SME_PORT_REGION(0x8003FDAC, 0, 0, 0), manageEMarioHealthWrapper);

#endif