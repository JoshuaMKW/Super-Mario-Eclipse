#include "printf.h"

#include "sms/JSystem/JKR/JKRDecomp.hxx"
#include "sms/JSystem/JKR/JKRHeap.hxx"
#include "sms/JSystem/JKR/JKRMemArchive.hxx"
#include "sms/SMS.hxx"

#include "Globals.hxx"
#include "Player.hxx"
#include "stage/FileUtils.hxx"
#include "libs/sAssert.hxx"

using namespace SME;

static OSThread gCharacterSwapThread;
static TMario *gPlayerToSwap;
static Enum::Player gTargetCharacterID;
static u8 gCharacterSwapStack[0x1000];
static bool gFadeInOut = true;
static bool gSwapSuccessful = false;

bool Util::Mario::loadParams() {
  JKRMemArchive *marioVolumeData =
      static_cast<JKRMemArchive *>(JKRFileLoader::getVolume("mario"));

  u8 *params = marioVolumeData->getResource("/params.szs");

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
  char buffer[32];
  sprintf(buffer, "/data/chr%hhu.szs", id);

  if (DVDConvertPathToEntrynum(buffer) < 0) {
    return false;
  }

  JKRExpHeap *CharacterHeap = TGlobals::sCharacterHeap;
  JKRMemArchive *marioVolumeData =
      static_cast<JKRMemArchive *>(JKRFileLoader::getVolume("mario"));

  const u32 *marioData =
      reinterpret_cast<u32 *>(SME::Util::loadArchive(buffer, CharacterHeap));

  if (!marioData) {
    return false;
  }

  CharacterHeap->free(gpArcBufferMario);
  *(u32 *)gpArcBufferMario = (u32)marioData;

  marioVolumeData->unmountFixed();
  marioVolumeData->mountFixed((void *)*(u32 *)gpArcBufferMario,
                              JKRMemBreakFlag::UNK_0);

  return true;
}

static void *t_swapCharacter(void *context) {
  gSwapSuccessful = false;
  
  char buffer[32];
  sprintf(buffer, "/data/chr%hhu.szs", gTargetCharacterID);

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
    marioData =
      reinterpret_cast<u32 *>(SME::Util::loadArchive(buffer, CharacterHeap));

    // Threaded loop to wait on fade to complete
    while (gpApplication.mFader->mFadeStatus != TSMSFader::FADE_ON) {
      asm volatile("");
    }
  } else {
    marioData =
      reinterpret_cast<u32 *>(SME::Util::loadArchive(buffer, CharacterHeap));
  }

  // Character failed to load / doesn't exist
  SME_ASSERT(marioData, "Tried to swap the player with data that doesn't exist!");

  // Make the player inactive
  gPlayerToSwap->mAttributes.mIsInactive = true;

  // Free buffer and swap pointers
  CharacterHeap->free(gpArcBufferMario);
  gpArcBufferMario = marioData;

  // Refresh mounted handle
  marioVolumeData->unmountFixed();
  marioVolumeData->mountFixed((void *)*(u32 *)gpArcBufferMario,
                              JKRMemBreakFlag::UNK_0);

  // -- Update player data -- //
  {
    const s16 health = gPlayerToSwap->mHealth;
    Vec position;
    Vec rotation;

    gPlayerToSwap->JSGGetTranslation(&position);
    gPlayerToSwap->JSGGetRotation(&rotation);

    // Fresh player data
    TMario *newPlayer = new TMario();

    // Prevent other threads from interrupting
    const bool interruptStatus = OSDisableInterrupts();

    if (gpMarioAddress == gPlayerToSwap) {
      gpMarioAddress = newPlayer;
      SMS_SetMarioAccessParams__Fv();
    }

    newPlayer->mAttributes.mIsInactive = false;
    newPlayer->JSGSetTranslation(position);
    newPlayer->JSGSetRotation(rotation);

    for (u32 i = 0; i < TGlobals::getMaxPlayers(); ++i) {
      if (TGlobals::getPlayerByIndex(i) == gPlayerToSwap) {
        TGlobals::setPlayerByIndex(i, newPlayer);
      }
    }

    // Restore multi-threaded nature
    OSRestoreInterrupts(interruptStatus);
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
  gPlayerToSwap = player;
  gTargetCharacterID = id;
  gFadeInOut = fadeInOut;

  if (OSIsThreadTerminated(&gCharacterSwapThread)) {
    //fix stack bug, gets overwritten due to obnoxious pointer
    OSCreateThread(&gCharacterSwapThread, &t_swapCharacter, nullptr,
                   &gCharacterSwapStack + sizeof(gCharacterSwapStack),
                   sizeof(gCharacterSwapStack), 18, 0);
    OSResumeThread(&gCharacterSwapThread);
  }
}

Enum::Player Util::Mario::getPlayerIDFromInput(u32 input) {
  switch (input) {
  case 0:
    return Enum::Player::MARIO;
  case TMarioGamePad::Buttons::DPAD_UP:
    return Enum::Player::LUIGI;
  case TMarioGamePad::Buttons::DPAD_DOWN:
    return Enum::Player::IL_PIANTISSIMO;
  case TMarioGamePad::Buttons::DPAD_LEFT:
    return Enum::Player::SHADOW_MARIO;
  case TMarioGamePad::Buttons::DPAD_RIGHT:
    return Enum::Player::DRY_BONES;
  default:
    return Enum::Player::UNKNOWN;
  }
}

Enum::Player getPlayerIDFromInt(u8 id) { return static_cast<Enum::Player>(id); }