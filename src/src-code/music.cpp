#include "printf.h"
#include "sms/sound/MSBGM.hxx"

#include "SME.hxx"

// 0x80016998
u32 SME::Patch::Music::setIsValid(MSStageInfo musicID) {
  gInfo.Context.mIsAudioStreamAllowed = SME::Util::Music::isValidBGM(musicID);
  return musicID & 0x3FF;
}

// 0x80016ABC
void SME::Patch::Music::initMusic() {
  if (SMEFile::mStageConfig.FileHeader.mMAGIC != SMEFile::MAGIC)
    return;

  DVDFileInfo *handle = (DVDFileInfo *)0x803FDB7C;

  if (!gInfo.Context.mIsAudioStreaming &&
      (SMEFile::mStageConfig.Music.mMusicID & 0x400))
    startStreamedBGM((MSStageInfo)SMEFile::mStageConfig.Music.mMusicID, true);

  if (gInfo.Context.mIsAudioStreaming && !gInfo.Context.mIsAudioStreamAllowed) {
    DVDCancelStreamAsync(&handle->mCmdBlock, 0);
    gInfo.Context.mIsAudioStreaming = false;
  }
}

// 0x80016948
void SME::Patch::Music::stopMusicOnStop() {
  DVDFileInfo *handle = (DVDFileInfo *)0x803FDB7C;

  if (gInfo.Context.mIsAudioStreaming) {
    DVDCancelStreamAsync(&handle->mCmdBlock, 0);
    gInfo.Context.mIsAudioStreaming = false;
  }
}

// 0x802A670C
void SME::Patch::Music::stopMusicOnStageExit(TMarioGamePad *gamepad) {
  DVDFileInfo *handle = (DVDFileInfo *)0x803FDB7C;

  if (gInfo.Context.mIsAudioStreaming) {
    DVDCancelStreamAsync(&handle->mCmdBlock, 0);
    gInfo.Context.mIsAudioStreaming = false;
  }
  reset__9RumbleMgrFv(gamepad);
}