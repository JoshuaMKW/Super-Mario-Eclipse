#include "printf.h"

#include "JKR/JKRDecomp.hxx"
#include "JKR/JKRHeap.hxx"
#include "JKR/JKRMemArchive.hxx"
#include "sms/SMS.hxx"
#include "sms/camera/PolarSubCamera.hxx"

#include "Globals.hxx"
#include "Player.hxx"
#include "libs/sAssert.hxx"
#include "macros.h"
#include "stage/FileUtils.hxx"

using namespace SME;
using namespace Util::Math;

static u8 gJ3DBuffer[0x2000];
static JKRSolidHeap gJ3DHeap(&gJ3DBuffer, sizeof(gJ3DBuffer), nullptr, false);
static OSThread gCharacterSwapThread;
static Enum::Player gTargetCharacterID = Enum::Player::UNKNOWN;
static u8 gCharacterSwapStack[0x4000];
static bool gFadeInOut = true;
static bool gSwapSuccessful = false;
static u8 gHeapAllocState = 0;

bool Util::Mario::loadParams() {
  JKRMemArchive *marioVolumeData =
      static_cast<JKRMemArchive *>(JKRFileLoader::getVolume("mario"));

  u8 *params = static_cast<u8 *>(marioVolumeData->getResource("/params.szs"));

  void *allocation;
  if (params) {
    u32 filesize = marioVolumeData->getResSize(params);
    CompressionType compressionState = JKRDecomp::checkCompressed(params);

    switch (compressionState) {
    case CompressionType::SZS: {
      *(u32 *)0x8040E260 = params[1];
      allocation =
          JKRHeap::sCurrentHeap->alloc(params[1], 32, JKRHeap::sCurrentHeap);
      decompSZS_subroutine(params, static_cast<u8 *>(allocation));
      TGlobals::sPRMFile = allocation;
    }
    case CompressionType::SZP:
      TGlobals::sPRMFile = nullptr;
    case CompressionType::NONE: {
      allocation = JKRHeap::sCurrentHeap->alloc(filesize, 32);
      memcpy(allocation, params, filesize);
      TGlobals::sPRMFile = allocation;
    }
    }

    JKRMemArchive *oldParams =
        static_cast<JKRMemArchive *>(JKRFileLoader::getVolume("params"));

    oldParams->unmountFixed();
    oldParams->mountFixed(TGlobals::sPRMFile, JKRMemBreakFlag::UNK_0);

    return true;
  }
  return false;
}

bool Util::Mario::swapBinary(Enum::Player id) {
  if (id == Enum::Player::UNKNOWN) {
    id = Enum::Player::MARIO;
  }

  char buffer[32];
  snprintf(buffer, 32, "/data/chr%hhu.szs", id);

  if (DVDConvertPathToEntrynum(buffer) < 0) {
    return false;
  }

  JKRExpHeap *CharacterHeap = TGlobals::sCharacterHeap;
  JKRMemArchive *marioVolumeData =
      static_cast<JKRMemArchive *>(JKRFileLoader::getVolume("mario"));

#ifndef SME_DETACHED_HEAPS
  CharacterHeap->freeAll();
  gHeapAllocState = 0;
#else
  CharacterHeap->free(gpArcBufferMario);
#endif

  u32 *marioData = reinterpret_cast<u32 *>(Util::loadArchive(
      buffer, CharacterHeap,
      static_cast<JKRDvdRipper::EAllocDirection>(gHeapAllocState)));

  // Character failed to load / doesn't exist
  SME_ASSERT(marioData,
             "Tried to swap the player with data that doesn't exist!");

  gHeapAllocState ^= 1;
  gpArcBufferMario = marioData;

#ifndef SME_DETACHED_HEAPS
  CharacterHeap->alloc(CharacterHeap->getFreeSize(), 4);
#endif

  marioVolumeData->unmountFixed();
  marioVolumeData->mountFixed(gpArcBufferMario, JKRMemBreakFlag::UNK_0);

  return true;
}

static void *t_swapCharacter(void *param) {
  TMario *player = reinterpret_cast<TMario *>(param);
  gSwapSuccessful = false;

  char buffer[32];
  snprintf(buffer, 32, "/data/chr%hhu.szs", gTargetCharacterID);

  // Player doesn't exist
  if (DVDConvertPathToEntrynum(buffer) < 0) {
    return &gSwapSuccessful;
  }

  JKRExpHeap *CharacterHeap = TGlobals::sCharacterHeap;
  JKRMemArchive *marioVolumeData =
      static_cast<JKRMemArchive *>(JKRFileLoader::getVolume("mario"));

  // -- Start screen wipe and async load the new character binary -- //
  u32 *marioData;
  if (gFadeInOut) {
    gpApplication.mFader->startWipe(TSMSFader::WipeRequest::FADE_CIRCLE_OUT,
                                    1.0f, 0.0f);
    CharacterHeap->freeAll();
    marioData = reinterpret_cast<u32 *>(Util::loadArchive(
        buffer, CharacterHeap,
        static_cast<JKRDvdRipper::EAllocDirection>(gHeapAllocState)));

    // Threaded loop to wait on fade to complete
    while (gpApplication.mFader->mFadeStatus != TSMSFader::FADE_ON) {
      asm volatile("");
    }
  } else {
    CharacterHeap->freeAll();
    marioData = reinterpret_cast<u32 *>(Util::loadArchive(
        buffer, CharacterHeap,
        static_cast<JKRDvdRipper::EAllocDirection>(gHeapAllocState)));
  }

  // Character failed to load / doesn't exist
  SME_ASSERT(marioData,
             "Tried to swap the player with data that doesn't exist!");

  // Toggle heap direction to keep heap from fragmenting
  gHeapAllocState ^= 1;

  // Make the player inactive
  player->mAttributes.mIsPerforming = true;

  // Free buffer and swap pointers
  gpArcBufferMario = marioData;

  // Refresh mounted handle
  marioVolumeData->unmountFixed();
  marioVolumeData->mountFixed(gpArcBufferMario, JKRMemBreakFlag::UNK_0);

  // -- Update player data -- //
  {
    const s16 health = player->mHealth;
    Vec position;
    Vec rotation;

    player->JSGGetTranslation(&position);
    player->JSGGetRotation(&rotation);

    // Prevent other threads from interrupting
    SME_ATOMIC_CODE(JKRHeap *currentHeap = gJ3DHeap.becomeCurrentHeap();
                    gJ3DHeap.freeAll();

                    player->initValues(); player->loadAfter();

                    currentHeap->becomeCurrentHeap();)

    player->mAttributes.mIsPerforming = false;
    player->JSGSetTranslation(position);
    player->JSGSetRotation(rotation);
  }

  if (gFadeInOut) {
    gpApplication.mFader->startWipe(TSMSFader::WipeRequest::FADE_CIRCLE_IN,
                                    1.0f, 0.0f);

    while (gpApplication.mFader->mFadeStatus != TSMSFader::FADE_OFF) {
      asm volatile("");
    }
  }

  gSwapSuccessful = true;
  return &gSwapSuccessful;
}

void Util::Mario::switchCharacter(TMario *player, Enum::Player id,
                                  bool fadeInOut) {
  gTargetCharacterID = id;
  gFadeInOut = fadeInOut;

  if (OSIsThreadTerminated(&gCharacterSwapThread)) {
    // fix stack bug, gets overwritten due to obnoxious pointer
    OSCreateThread(&gCharacterSwapThread, &t_swapCharacter, player,
                   (u8 *)(&gCharacterSwapStack) + sizeof(gCharacterSwapStack),
                   sizeof(gCharacterSwapStack), 18, 0);
    OSResumeThread(&gCharacterSwapThread);
  }
}

Enum::Player Util::Mario::getPlayerIDFromInput(u32 input) {
  switch (input) {
  case 0:
    return Enum::Player::MARIO;
  case TMarioGamePad::EButtons::DPAD_UP:
    return Enum::Player::LUIGI;
  case TMarioGamePad::EButtons::DPAD_DOWN:
    return Enum::Player::IL_PIANTISSIMO;
  case TMarioGamePad::EButtons::DPAD_LEFT:
    return Enum::Player::SHADOW_MARIO;
  case TMarioGamePad::EButtons::DPAD_RIGHT:
    return Enum::Player::DRY_BONES;
  default:
    return Enum::Player::UNKNOWN;
  }
}

Enum::Player Util::Mario::getPlayerIDFromInt(u8 id) {
  return static_cast<Enum::Player>(id);
}

void Util::Mario::warpToCollisionFace(TMario *player, TBGCheckData *colTriangle,
                                      bool isFluid) {
  constexpr s32 DisableMovementTime = 80;
  constexpr s32 TeleportTime = 140;
  constexpr s32 EnableMovementTime = 60;
  constexpr f32 WipeKindInDelay = 1.0f;

  if (!player)
    return;

  Class::TPlayerData *playerData = TGlobals::getPlayerData(player);

  Class::TVectorTriangle vectorTri(colTriangle->mVertexA, colTriangle->mVertexB,
                                   colTriangle->mVertexC);

  if (!isFluid) {
    TVec3f playerPos;
    TVec3f cameraPos;
    player->JSGGetTranslation(reinterpret_cast<Vec *>(&playerPos));
    gpCamera->JSGGetViewPosition(reinterpret_cast<Vec *>(&cameraPos));

    vectorTri.center(playerPos);

    player->mFloorTriangle = colTriangle;
    player->mFloorTriangleWater = colTriangle;
    player->mFloorBelow = playerPos.y;
    // playerData->mPrevCollisionFloor = colTriangle;

    {
      f32 x = lerp<f32>(cameraPos.x, playerPos.x, 0.9375f);
      f32 y = playerPos.y + 300.0f;
      f32 z = lerp<f32>(cameraPos.z, playerPos.z, 0.9375f);
      cameraPos.set(x, y, z);
    }

    player->JSGSetTranslation(reinterpret_cast<Vec &>(playerPos));
    gpCamera->JSGSetViewPosition(reinterpret_cast<Vec &>(cameraPos));
    gpCamera->JSGSetViewTargetPosition(reinterpret_cast<Vec &>(playerPos));
  } else {
    TVec3f playerPos;
    TVec3f cameraPos;
    player->JSGGetTranslation(reinterpret_cast<Vec *>(&playerPos));
    gpCamera->JSGGetViewPosition(reinterpret_cast<Vec *>(&cameraPos));

    player->mFloorTriangle = colTriangle;
    player->mFloorTriangleWater = colTriangle;
    playerData->mCollisionFlags.mIsFaceUsed = true;

    vectorTri.center(playerPos);

    {
      f32 x = lerp<f32>(cameraPos.x, playerPos.x, 0.9375f);
      f32 y = playerPos.y + 300.0f;
      f32 z = lerp<f32>(cameraPos.z, playerPos.z, 0.9375f);
      cameraPos.set(x, y, z);
    }

    TVec3f *colNormal = colTriangle->getNormal();

    const f32 magnitude = PSVECMag(reinterpret_cast<Vec *>(&player->mSpeed));

    player->mAngle.y =
        static_cast<u16>(Vector3::getNormalAngle(*colNormal)) * 182;
    setPlayerVelocity__6TMarioFf(player, magnitude * colNormal->x +
                                             magnitude * colNormal->z);
    player->mSpeed.y = magnitude * colNormal->y;

    playerPos.add(TVec3f{
        40.0f * colNormal->x, 40.0f * colNormal->y, 40.0f * colNormal->z});

    player->JSGSetTranslation(reinterpret_cast<Vec &>(playerPos));
    gpCamera->JSGSetViewPosition(reinterpret_cast<Vec &>(cameraPos));
    gpCamera->JSGSetViewTargetPosition(reinterpret_cast<Vec &>(playerPos));

    changePlayerStatus__6TMarioFUlUlb(
        player, static_cast<u32>(TMario::State::FALL), 0, 0);
  }
}

void Util::Mario::rotatePlayerRelativeToCamera(TMario *player,
                                               CPolarSubCamera *camera,
                                               Vec2 dir, f32 lerp) {
  player->mAngle.y = Math::lerp<f32>(
      player->mAngle.y,
      camera->mHorizontalAngle +
          s16(Util::Math::radiansToAngle(atan2f(dir.x, -dir.y)) * 182),
      lerp);
}