#include "MTX.h"
#include "math.h"
#include "types.h"

#include "sms/actor/Mario.hxx"
#include "sms/enemy/EnemyMario.hxx"
#include "sms/npc/BaseNPC.hxx"

#include "SME.hxx"

using namespace SME;

static u32 updateContexts(TMario *player) {
    extern void updateCollisionContext(TMario *player);
    extern void updateClimbContext(TMario *player);

    updateCollisionContext(player);
    updateClimbContext(player);
}

static void addVelocity(TMario *player, f32 velocity) {
  Class::TPlayerData *playerData = TGlobals::getPlayerData(player);

  if (!onYoshi__6TMarioCFv(player)) {
    player->mForwardSpeed =
        Min(player->mForwardSpeed + velocity,
            (playerData->mMaxAddVelocity *
             playerData->getParams()->mSpeedMultiplier.get()) *
                playerData->mSlideSpeedMultiplier);
  } else {
    player->mForwardSpeed =
        Min(player->mForwardSpeed + velocity,
            playerData->mMaxAddVelocity * playerData->mSlideSpeedMultiplier);
  }
}
SME_PATCH_BL(SME_PORT_REGION(0x802558A4, 0, 0, 0), addVelocity);

static void rescaleHeldObj(Mtx holderMatrix, Mtx destMatrix) {
  TMapObjBase *heldObj;
  SME_FROM_GPR(31, heldObj);

  Vec holderSize;
  heldObj->mHolder->JSGGetScaling(&holderSize);

  PSMTXCopy(holderMatrix, destMatrix);
  PSMTXScaleApply(destMatrix, destMatrix, 1 / holderSize.x, 1 / holderSize.y,
                  1 / holderSize.z);
}
SME_PATCH_BL(SME_PORT_REGION(0x801E4118, 0, 0, 0), rescaleHeldObj);