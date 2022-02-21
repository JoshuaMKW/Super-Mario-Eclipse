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

/* extern to handlers.cpp */

static bool sIsWiping = false;

SME_NO_INLINE void warpToLinkedCol(TMario *player, Enum::WarpKind kind,
                                          bool isInstantlyActivated) {
  constexpr s32 DisableMovementTime = 80;
  constexpr s32 TeleportTime = 140;
  constexpr s32 EnableMovementTime = 60;
  constexpr f32 WipeKindInDelay = 1.0f;

  SME::Class::TPlayerData *playerData = SME::TGlobals::getPlayerData(player);

  TBGCheckData *linkedCol = SME::TGlobals::sWarpColArray->resolveCollisionWarp(
      player->mFloorTriangle);

  const f32 speed = PSVECMag(reinterpret_cast<Vec *>(&player->mSpeed));

  if (playerData->mCollisionFlags.mIsWarpActive) {
    switch (playerData->mCollisionFlags.mWarpingType) {
    case Enum::WarpKind::SPARKLES: {
      if (playerData->mCollisionTimer > EnableMovementTime) {
        playerData->mCollisionFlags.mIsDisableInput = false;
        playerData->mCollisionFlags.mIsWarpActive = false;
        player->mController->State.mReadInput = true;
        playerData->mCollisionTimer = 0;
      } else {
        playerData->mCollisionTimer += 1;
      }
      break;
    }
    case Enum::WarpKind::WIPE: {
      if (gpApplication.mFader->mFadeStatus == TSMSFader::FADE_OFF) {
        playerData->mCollisionFlags.mIsDisableInput = false;
        playerData->mCollisionFlags.mIsWarpActive = false;
        player->mController->State.mReadInput = true;
        playerData->mCollisionTimer = 0;
      } else {
        playerData->mCollisionTimer += 1;
      }
      break;
    }
    case Enum::WarpKind::INSTANT:
    default:
      playerData->mCollisionFlags.mIsDisableInput = false;
      playerData->mCollisionFlags.mIsWarpActive = false;
    }
  } else {
    if (!linkedCol) {
      playerData->mCollisionTimer = 0;
      return;
    }

    switch (kind) {
    case Enum::WarpKind::SPARKLES: {
      size_t timeCut = 0;
      if (isInstantlyActivated) {
        timeCut = DisableMovementTime;
      } else if (speed > 1.0f) {
        playerData->mCollisionTimer = 0;
        return;
      }

      if (!playerData->mCollisionFlags.mIsFaceUsed) {
        if (playerData->mCollisionTimer >= TeleportTime - timeCut) {
          Util::Mario::warpToCollisionFace(player, linkedCol, false);

          playerData->mCollisionFlags.mWarpingType = kind;
          playerData->mCollisionFlags.mIsFaceUsed = true;
          playerData->mCollisionFlags.mIsWarpActive = true;
          playerData->mCollisionFlags.mIsWarpUsed = true;
          playerData->mCollisionTimer = 0;
          startSoundActor__6TMarioFUl(player,
                                      static_cast<u32>(TMario::Voice::JUMP));
        } else if (playerData->mCollisionTimer >=
                   DisableMovementTime - timeCut) {
          if (!playerData->mCollisionFlags.mIsDisableInput) {
            emitGetEffect__6TMarioFv(player);
          }
          playerData->mCollisionFlags.mIsDisableInput = true;
          player->mController->State.mReadInput = false;
        }
      }
      playerData->mCollisionTimer += 1;
      return;
    }
    case Enum::WarpKind::WIPE: {
      size_t timeCut = 0;
      if (isInstantlyActivated) {
        timeCut = DisableMovementTime;
      } else if (speed > 1.0f) {
        playerData->mCollisionTimer = 0;
        return;
      }

      if (!playerData->mCollisionFlags.mIsFaceUsed) {
        if (gpApplication.mFader->mFadeStatus == TSMSFader::FADE_ON) {
          Util::Mario::warpToCollisionFace(player, linkedCol, false);

          playerData->mCollisionFlags.mWarpingType = kind;
          playerData->mCollisionFlags.mIsFaceUsed = true;
          playerData->mCollisionFlags.mIsWarpActive = true;
          playerData->mCollisionFlags.mIsWarpUsed = true;
          playerData->mCollisionTimer = 0;
          sIsWiping = false;

          gpApplication.mFader->startWipe(
              TSMSFader::WipeRequest::FADE_CIRCLE_IN, 1.0f, WipeKindInDelay);
        } else if (playerData->mCollisionTimer >=
                   DisableMovementTime - timeCut) {
          playerData->mCollisionFlags.mIsDisableInput = true;
          player->mController->State.mReadInput = false;
          if (gpApplication.mFader->mFadeStatus == TSMSFader::FADE_OFF &&
              !sIsWiping) {
            gpApplication.mFader->startWipe(
                TSMSFader::WipeRequest::FADE_SPIRAL_OUT, 1.0f, 0.0f);
            MSoundSESystem::MSoundSE::startSoundSystemSE(0x4859, 0, nullptr, 0);
            sIsWiping = true;
          }
        }
      }
      playerData->mCollisionTimer += 1;
      return;
    }
    case Enum::WarpKind::INSTANT:
    default: {
      if (!playerData->mCollisionFlags.mIsFaceUsed) {
        Util::Mario::warpToCollisionFace(player, linkedCol, false);

        playerData->mCollisionFlags.mWarpingType = kind;
        playerData->mCollisionFlags.mIsFaceUsed = true;
        playerData->mCollisionFlags.mIsWarpUsed = true;
        playerData->mCollisionTimer = 0;
      }
      return;
    }
    }
  }
}

void warpToLinkedColPreserve(TMario *player, bool fluid) {
  SME::Class::TPlayerData *playerData = SME::TGlobals::getPlayerData(player);

  TBGCheckData *linkedCol = SME::TGlobals::sWarpColArray->resolveCollisionWarp(
      player->mFloorTriangle);

  if (!linkedCol)
    return;

  if (!playerData->mCollisionFlags.mIsFaceUsed) {
    Util::Mario::warpToCollisionFace(player, linkedCol, true);
  } else {
    playerData->mCollisionFlags.mIsFaceUsed =
        (!(player->mController->mButtons.mFrameInput &
           TMarioGamePad::EButtons::DPAD_DOWN) &&
         !fluid);
  }
}

#endif