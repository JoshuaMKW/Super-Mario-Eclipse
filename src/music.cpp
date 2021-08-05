#include "printf.h"
#include "sms/sound/MSBGM.hxx"

#include "SME.hxx"

using namespace SME;
using namespace SME::Class;

#if 1

AudioStreamer::AudioPacket packet(0xFFFFFFFF);

// 0x80016998
u32 SME::Patch::Music::setIsValid(u32 musicID) {
  TGlobals::sIsAudioStreamAllowed =
      SME::Util::Music::isValidBGM(musicID) && musicID == gStageBGM &&
      TStageParams::sStageConfig->mMusicSetCustom.get();

  return musicID & 0x3FF;
}

// 0x80016ABC
JAISound *SME::Patch::Music::initMusic(JAISound *jai) {
  AudioStreamer *streamer = AudioStreamer::getInstance();
  streamer->setLooping(true);

  if (TGlobals::sIsAudioStreamAllowed) {
    packet.mIdentifier.as_u32 = TStageParams::sStageConfig->mMusicID.get() & 0x3FF;
    streamer->queueAudio(packet);
    if (streamer->isPlaying()) {
      streamer->skip(0.0f);
    }
    streamer->play();
    MSBgm::stopBGM(gStageBGM, 32);
  } else if (streamer->isPlaying()) {
    streamer->stop();
  }

  return jai;
}

// 0x80016948
void SME::Patch::Music::stopMusicOnStop() {
  AudioStreamer *streamer = AudioStreamer::getInstance();

  if (streamer->isPlaying())
    streamer->next(0.0f);
}

// 0x802A670C
void SME::Patch::Music::stopMusicOnStageExit(TMarioGamePad *gamepad) {
  AudioStreamer *streamer = AudioStreamer::getInstance();

  if (streamer->isPlaying())
    streamer->next(0.0f);
  reset__9RumbleMgrFv(gamepad);
}

#endif