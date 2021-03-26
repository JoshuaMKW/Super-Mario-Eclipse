#include "printf.h"
#include "sms/sound/MSBGM.hxx"

#include "SME.hxx"

static bool startStreamedBGM(MSStageInfo musicID, bool loopMusic) {
  char buffer[0x20];
  DVDFileInfo *handle = (DVDFileInfo *)0x803FDB7C;

  sprintf(buffer, "/AudioRes/Streams/Music/%lu.adp", (u32)(musicID & 0x3FF));

  if (!DVDOpen(buffer, handle))
    return false;

  MSBgm::stopBGM(gStageBGM, 32);
  DVDPrepareStreamAsync(handle, 0, 0, (DVDCallback)0x803184E4);

  if (!loopMusic)
    DVDStopStreamAtEndAsync(&handle->mCmdBlock, 0);
  return true;
}

static bool startStreamedSFX(u32 sfxID) {
  char buffer[0x20];
  DVDFileInfo *handle = (DVDFileInfo *)0x803FDB7C;

  sprintf(buffer, "/AudioRes/Streams/SFX/%lu.adp", (sfxID & 0x3FF));

  if (!DVDOpen(buffer, handle))
    return false;

  DVDPrepareStreamAsync(handle, 0, 0, (DVDCallback)0x803184E4);
  DVDStopStreamAtEndAsync(&handle->mCmdBlock, 0);

  return true;
}

// 0x80016998
u32 SME::Patch::Music::setIsValid(MSStageInfo musicID) {
  SME::TGlobals::sGlobals.Context.mIsAudioStreamAllowed = SME::Util::Music::isValidBGM(musicID);
  return musicID & 0x3FF;
}

// 0x80016ABC
void SME::Patch::Music::initMusic() {
  if (SMEFile::mStageConfig.FileHeader.mMAGIC != SMEFile::MAGIC)
    return;

  DVDFileInfo *handle = (DVDFileInfo *)0x803FDB7C;

  if (!SME::TGlobals::sGlobals.Context.mIsAudioStreaming &&
      (SMEFile::mStageConfig.Music.mMusicID & 0x400))
    startStreamedBGM((MSStageInfo)SMEFile::mStageConfig.Music.mMusicID, true);

  if (SME::TGlobals::sGlobals.Context.mIsAudioStreaming && !SME::TGlobals::sGlobals.Context.mIsAudioStreamAllowed) {
    DVDCancelStreamAsync(&handle->mCmdBlock, 0);
    SME::TGlobals::sGlobals.Context.mIsAudioStreaming = false;
  }
}

// 0x80016948
void SME::Patch::Music::stopMusicOnStop() {
  DVDFileInfo *handle = (DVDFileInfo *)0x803FDB7C;

  if (SME::TGlobals::sGlobals.Context.mIsAudioStreaming) {
    DVDCancelStreamAsync(&handle->mCmdBlock, 0);
    SME::TGlobals::sGlobals.Context.mIsAudioStreaming = false;
  }
}

// 0x802A670C
void SME::Patch::Music::stopMusicOnStageExit(TMarioGamePad *gamepad) {
  DVDFileInfo *handle = (DVDFileInfo *)0x803FDB7C;

  if (SME::TGlobals::sGlobals.Context.mIsAudioStreaming) {
    DVDCancelStreamAsync(&handle->mCmdBlock, 0);
    SME::TGlobals::sGlobals.Context.mIsAudioStreaming = false;
  }
  reset__9RumbleMgrFv(gamepad);
}