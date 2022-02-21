#include "JGeometry.hxx"
#include "sms/actor/Mario.hxx"
#include "sms/sound/MSound.hxx"
#include "sms/sound/MSoundSESystem.hxx"

#include "MTX.h"
#include "SME.hxx"
#include "collision/WarpCollision.hxx"
#include "libs/sGeometry.hxx"

#if SME_EXTRA_COLLISION

using namespace SME;
using namespace SME::Util::Math;

/* extern to handlers.cpp */

void setGravityCol(TMario *player) {
  player->mJumpParams.mGravity.set(
      static_cast<f32>(player->mFloorTriangle->mValue4) / 100.0f);
}

void antiGravityCol(TMario *player) {
  Vec position;
  player->JSGGetTranslation(&position);

  if ((position.y - player->mFloorTriangle->mMaxHeight) >
          player->mFloorTriangle->mValue4 &&
      player->mFloorTriangle->mValue4 != 0)
    return;

  player->mSpeed.y = 10.0f;
  if ((player->mState & static_cast<u32>(TMario::State::AIRBORN)) == false) {
    position.y += 1.0f;
    player->JSGSetTranslation(position);
    changePlayerStatus__6TMarioFUlUlb(
        player, static_cast<u32>(TMario::State::FALL), 0, 0);
  }
  if (player->mState == static_cast<u32>(TMario::State::FALL))
    player->mSubStateTimer = 0;
}

#endif