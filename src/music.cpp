#include "printf.h"
#include "sms/sound/MSBGM.hxx"

#include "SME.hxx"

using namespace SME;
using namespace SME::Class;

// 0x802BB89C
static void initExMusic(MSStageInfo bgm) {
  TStageParams *config = TStageParams::sStageConfig;
  if (!isGameEmulated()) {
    if (config->mMusicEnabled.get())
      MSBgm::startBGM(bgm);
    return;
  }

  if (config->mMusicSetCustom.get()) {
    gStageBGM = 0x80010000 | config->mMusicID.get();
  }

  gAudioSpeed = config->mMusicSpeed.get();
  gAudioPitch = config->mMusicPitch.get();
  gAudioVolume = Max(Min(config->mMusicVolume.get(), 1), 0);

  if (!config->mMusicEnabled.get())
    return;

  if (!config->mIsExStage.get()) {
    MSBgm::startBGM(bgm);
    return;
  }

  AudioStreamer *streamer = AudioStreamer::getInstance();

  AudioStreamer::AudioPacket packet(gStageBGM & 0x3FF);
  streamer->queueAudio(packet);

  streamer->setLooping(true);

  if (streamer->isPlaying()) {
    streamer->skip(0.0f);
  }

  streamer->setVolumeLR(AudioVolumeDefault, AudioVolumeDefault);
  streamer->play();

  return;
}
SME_PATCH_BL(SME_PORT_REGION(0x802BB89C, 0, 0, 0), initExMusic);

// 0x802983F0
// 0x80298420
// 0x802984D0
static void initStageMusic() {
  TStageParams *config = TStageParams::sStageConfig;
  if (!isGameEmulated()) {
    if (config->mMusicEnabled.get())
      MSBgm::startBGM(gStageBGM);
    return;
  }

  if (config->mMusicSetCustom.get()) {
    gStageBGM = 0x80010000 | config->mMusicID.get();
  }

  gAudioSpeed = config->mMusicSpeed.get();
  gAudioPitch = config->mMusicPitch.get();
  gAudioVolume = Max(Min(config->mMusicVolume.get(), 1), 0);

  if (!config->mMusicEnabled.get())
    return;
    
  if ((gStageBGM & 0xFC00) == 0) {
    startStageBGM__10MSMainProcFUcUc(gpMarDirector->mAreaID, gpMarDirector->mEpisodeID);
    return;
  }

  if (config->mIsExStage.get())
    return;

  AudioStreamer *streamer = AudioStreamer::getInstance();

  AudioStreamer::AudioPacket packet(gStageBGM & 0x3FF);
  streamer->queueAudio(packet);

  streamer->setLooping(true);

  if (streamer->isPlaying()) {
    streamer->skip(0.0f);
  }

  streamer->setVolumeLR(AudioVolumeDefault, AudioVolumeDefault);
  streamer->play();

  return;
}
SME_PATCH_BL(SME_PORT_REGION(0x802983F0, 0, 0, 0), initStageMusic);
SME_PATCH_BL(SME_PORT_REGION(0x80298420, 0, 0, 0), initStageMusic);
SME_PATCH_BL(SME_PORT_REGION(0x802984D0, 0, 0, 0), initStageMusic);

// 0x802A670C
static void stopMusicOnStageExit(TMarioGamePad *gamepad) {
  if (TFlagManager::smInstance->getBool(0x90001) == true) {
    TGlobals::setFrameRate(60.0f);
  } else {
    TGlobals::setFrameRate(30.0f);
  }

  if (!isGameEmulated()) {
    reset__9RumbleMgrFv(gamepad);
    return;
  }
  
  AudioStreamer *streamer = AudioStreamer::getInstance();
  streamer->next(0.2f);

  reset__9RumbleMgrFv(gamepad);
}
SME_PATCH_BL(SME_PORT_REGION(0x802A670C, 0, 0, 0), stopMusicOnStageExit);

// 0x80297B7C
static void stopMusicOnShineGet(u32 musicID) {
  if (!isGameEmulated()) {
    MSBgm::startBGM(musicID);
    return;
  }

  AudioStreamer *streamer = AudioStreamer::getInstance();

  if ((gpMarDirector->mCollectedShine->mType & 0x10) == 0 &&
      streamer->isPlaying()) {
    streamer->stop(0.2f);
  }

  MSBgm::startBGM(musicID);
}
SME_PATCH_BL(SME_PORT_REGION(0x80297B7C, 0, 0, 0), stopMusicOnShineGet);

// 0x8024FAB8
static void stopMusicOnManholeEnter(u32 musicID) {
  if (!isGameEmulated()) {
    MSBgm::startBGM(musicID);
    return;
  }
  
  AudioStreamer *streamer = AudioStreamer::getInstance();
  if (streamer->isPlaying())
    streamer->pause(0.2f);

  MSBgm::startBGM(musicID);
}
SME_PATCH_BL(SME_PORT_REGION(0x8024FAB8, 0, 0, 0), stopMusicOnManholeEnter);

// 0x8024FB0C
static void stopMusicOnManholeExit(u32 musicID, u32 unk_0) {
  if (!isGameEmulated()) {
    MSBgm::stopBGM(musicID, unk_0);
    return;
  }

  AudioStreamer *streamer = AudioStreamer::getInstance();
  if (streamer->isPaused())
    streamer->play();

  MSBgm::stopBGM(musicID, unk_0);
}
SME_PATCH_BL(SME_PORT_REGION(0x8024FB0C, 0, 0, 0), stopMusicOnManholeExit);

// 0x802981A8
static void stopMusicBeforeShineCamera(CPolarSubCamera *cam, const char *demo,
                                       const JGeometry::TVec3<f32> *pos,
                                       s32 unk_0, f32 unk_1, bool unk_2) {
  cam->startDemoCamera(demo, pos, unk_0, unk_1, unk_2);

  if (!isGameEmulated())
    return;

  AudioStreamer *streamer = AudioStreamer::getInstance();
  if (streamer->isPlaying())
    streamer->pause(0.2f);
}
SME_PATCH_BL(SME_PORT_REGION(0x802981A8, 0, 0, 0), stopMusicBeforeShineCamera);

// 0x80297FD4
static void startMusicAfterShineCamera(CPolarSubCamera *cam) {
  cam->endDemoCamera();

  if (!isGameEmulated())
    return;

  AudioStreamer *streamer = AudioStreamer::getInstance();
  if (streamer->isPaused())
    streamer->play();
}
SME_PATCH_BL(SME_PORT_REGION(0x80297FD4, 0, 0, 0), startMusicAfterShineCamera);