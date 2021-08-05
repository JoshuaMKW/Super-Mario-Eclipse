
#include "DVD.h"
#include "OS.h"
#include "printf.h"
#include "sms/JSystem/JSU/JSUStream.hxx"
#include "sms/sound/MSBGM.hxx"

#include "SME.hxx"
#include "sMusic.hxx"

using namespace SME::Util;
using namespace SME::Class;

constexpr size_t AudioPreparePreOffset = 0;

static void updaterLoop() {
  main__Q28JASystem10HardStreamFv();

  AudioStreamer *streamer = AudioStreamer::getInstance();
  streamer->update_();
}
SME_PATCH_BL(SME_PORT_REGION(0x80316034, 0, 0, 0), updaterLoop);

static void *mainLoop(void *param) {
  AudioStreamer *streamer = reinterpret_cast<AudioStreamer *>(param);
  AudioStreamer::AudioCommand command;
  OSMessage msg;

  while (true) {
    OSReceiveMessage(&streamer->mMessageQueue, &msg, OS_MESSAGE_BLOCK);
    command = *reinterpret_cast<AudioStreamer::AudioCommand *>(msg);
    switch (command) {
    case AudioStreamer::AudioCommand::PLAY:
      streamer->play_();
      break;
    case AudioStreamer::AudioCommand::PAUSE:
      streamer->pause_();
      break;
    case AudioStreamer::AudioCommand::STOP:
      streamer->stop_();
      break;
    case AudioStreamer::AudioCommand::SKIP:
      streamer->skip_();
      break;
    case AudioStreamer::AudioCommand::NEXT:
      streamer->next_();
      break;
    case AudioStreamer::AudioCommand::SEEK:
      streamer->seek_();
      break;
    default:
      break;
    }
  }
}

static void cbForStopStreamAtEndAsync_(s32 result, DVDCommandBlock *cmdblock);

static void cbForPrepareStreamAsync_(s32 result, DVDFileInfo *finfo) {
  AudioStreamer *streamer = AudioStreamer::getInstance();
  u16 volLR = streamer->getVolumeLR();

  // Init AI
  AISetStreamVolLeft(static_cast<u8>(volLR >> 8));
  AISetStreamVolRight(static_cast<u8>(volLR));
  AIResetStreamSampleCount();
  AISetStreamTrigger(0xBB80);
  AISetStreamPlayState(1);

  DVDStopStreamAtEndAsync(streamer->mAudioCommandBlock,
                          cbForStopStreamAtEndAsync_);
}

static void cbForStopStreamAtEndAsync_(s32 result, DVDCommandBlock *cmdblock) {
#if 0
  AudioStreamer *streamer = AudioStreamer::getInstance();

  if (streamer->isLooping()) {
    DVDPrepareStreamAsync(fileinfo, 0, 0, cbForPrepareStreamAsync_);
  }
#endif
}

static DVDFileInfo sAudioFInfo;
static DVDCommandBlock sAudioCmdBlock;

static AudioStreamer::AudioCommand sAudioCommand =
    AudioStreamer::AudioCommand::NONE;
AudioStreamer AudioStreamer::sInstance =
    AudioStreamer(mainLoop, 18, &sAudioFInfo, &sAudioCmdBlock);

void AudioStreamer::setVolumeLR(u8 left, u8 right) {
  mVolLeft = left;
  mVolRight = right;
  if (isPlaying()) {
    AISetStreamVolLeft(left);
    AISetStreamVolRight(right);
  }
}

bool AudioStreamer::queueAudio(AudioPacket &packet) {
  for (u32 i = 0; i < AudioQueueSize; ++i) {
    u32 index = (i + mAudioIndex) % AudioQueueSize;
    if (mAudioQueue[index] == nullptr) {
      mAudioQueue[index] = &packet;
      return true;
    }
  }
  SME_LOG("%s: Queue is full!\n", SME_FUNC_SIG);
  return false;
}

void AudioStreamer::play() {
  sAudioCommand = AudioCommand::PLAY;
  OSSendMessage(&mMessageQueue, &sAudioCommand, OS_MESSAGE_NOBLOCK);
}

void AudioStreamer::pause() {
  sAudioCommand = AudioCommand::PAUSE;
  OSSendMessage(&mMessageQueue, &sAudioCommand, OS_MESSAGE_NOBLOCK);
}

void AudioStreamer::stop() {
  sAudioCommand = AudioCommand::STOP;
  OSSendMessage(&mMessageQueue, &sAudioCommand, OS_MESSAGE_NOBLOCK);
}

void AudioStreamer::skip(f32 delay) {
  sAudioCommand = AudioCommand::SKIP;
  mDelayedTime = delay;
  OSSendMessage(&mMessageQueue, &sAudioCommand, OS_MESSAGE_NOBLOCK);
}

void AudioStreamer::next(f32 delay) {
  sAudioCommand = AudioCommand::NEXT;
  mDelayedTime = delay;
  OSSendMessage(&mMessageQueue, &sAudioCommand, OS_MESSAGE_NOBLOCK);
}

void AudioStreamer::seek(s32 where, JSUStreamSeekFrom whence) {
  sAudioCommand = AudioCommand::SEEK;
  _mWhere = where;
  _mWhence = whence;
  OSSendMessage(&mMessageQueue, &sAudioCommand, OS_MESSAGE_NOBLOCK);
}

void AudioStreamer::seek(f64 seconds, JSUStreamSeekFrom whence) {
  sAudioCommand = AudioCommand::SEEK;
  _mWhere =
      32768 *
      (seconds / OSTicksToSeconds(
                     23465670)); // magic number using mean of data go brrrrrr
  _mWhence = whence;
  OSSendMessage(&mMessageQueue, &sAudioCommand, OS_MESSAGE_NOBLOCK);
}

bool AudioStreamer::play_() {
  AISetStreamVolLeft(mVolLeft);
  AISetStreamVolRight(mVolRight);

  if (isPlaying()) {
    if (isPaused()) {
      AISetStreamPlayState(1);
      mIsPaused = false;
      return true;
    } else {
      SME_LOG("%s: Already playing audio!\n", SME_FUNC_SIG);
      return false;
    }
  }

  AudioPacket *packet = getCurrentAudio();
  if (packet) {
    mIsPlaying = packet->exec(mAudioHandle);
    return mIsPlaying;
  }

  SME_LOG("%s: No audio queued to play!\n", SME_FUNC_SIG);
  return false;
}

bool AudioStreamer::pause_() {
  if (!mIsPlaying)
    return false;

  AISetStreamVolLeft(0);
  AISetStreamVolRight(0);
  AISetStreamPlayState(0);
  mIsPaused = true;
  return true;
}

bool AudioStreamer::stop_() {
  if (!mIsPlaying)
    return false;

  DVDCancelStreamAsync(mAudioCommandBlock, 0);
  mIsPaused = false;
  mIsPlaying = false;
  return true;
}

bool AudioStreamer::skip_() {
  next_();
  return play_();
}

void AudioStreamer::next_() {
  const f32 waitTime = mDelayedTime;
  OSTime startTime = OSGetTime();
  while (OSTicksToSeconds(static_cast<f32>(OSGetTime() - startTime)) <
         waitTime) {
    asm volatile("");
  }
  stop_();

  // clang-format off
  SME_ATOMIC_CODE (
    mAudioQueue[mAudioIndex] = nullptr;
    mAudioIndex = (mAudioIndex + 1) % AudioQueueSize;
  )
  // clang-format on
}

bool AudioStreamer::seek_() {
  AudioPacket *packet = getCurrentAudio();

  const u32 streamSize = mAudioHandle->mLen;
  const u32 streamStart = mAudioHandle->mStart;

  s32 streamPos = 0;
  switch (_mWhence) {
  case JSUStreamSeekFrom::BEGIN:
    streamPos = _mWhere;
    break;
  case JSUStreamSeekFrom::CURRENT:
    streamPos = (DVDGetStreamPlayAddr(mAudioCommandBlock) - streamStart) + _mWhere;
    break;
  case JSUStreamSeekFrom::END:
    streamPos = streamSize - _mWhere;
    break;
  }

  if (streamPos > streamSize) {
    skip_();
  } else if (streamPos < 0) {
    streamPos = 0;
  }
  streamPos += streamStart;

  DVDPrepareStreamAsync(mAudioHandle, packet->mLoopEnd - packet->mLoopStart,
                        packet->mLoopStart, cbForPrepareStreamAsync_);
}

static u32 _sLastOfs = 0;
static OSStopwatch _sStopWatch;
static bool _sSwInit = false;
static bool _sSwStarted = false;

void AudioStreamer::update_() {
  if (!_sSwInit) {
    OSInitStopwatch(&_sStopWatch, "AudioStreamer");
    _sSwInit = true;
  }

  if (!isPlaying())
    return;

  const u32 streamSize = mAudioHandle->mLen;
  const u32 streamStart = mAudioHandle->mStart;
  const u32 streamCur = DVDGetStreamPlayAddr(mAudioCommandBlock);

  AudioPacket *packet = getCurrentAudio();
  const bool shouldLoop =
      (packet->mLoopEnd != 0xFFFFFFFF && packet->mLoopStart != 0xFFFFFFFF);

  if (streamCur != _sLastOfs) {
    OSStopStopwatch(&_sStopWatch);
    OSDumpStopwatch(&_sStopWatch);
    _sSwStarted = false;

    if (gpApplication.mGamePad1->mButtons.mInput ==
        (TMarioGamePad::Y | TMarioGamePad::X | TMarioGamePad::Z)) {
      SME_DEBUG_LOG(
          "%s: {\n  streamSize = %lu\n  streamStart = %lu\n  streamCur "
          "= %lu\n  isLooping = %lu\n  isPlaying = %lu\n}\n",
          SME_FUNC_SIG, streamSize, streamStart, streamCur, isLooping(),
          isPlaying());
    }

    if (isLooping()) {
      if (shouldLoop && (streamCur - streamStart) >=
                            Max(packet->mLoopEnd - AudioPreparePreOffset, 0)) {
        DVDPrepareStreamAsync(mAudioHandle,
                              packet->mLoopEnd - packet->mLoopStart,
                              packet->mLoopStart, cbForPrepareStreamAsync_);
      } else if (streamSize - (streamCur - streamStart) <=
                     AudioPreparePreOffset ||
                 (streamCur == streamStart && _sLastOfs > streamStart)) {
        mDelayedTime = 0.0f;
        mIsPlaying = false;
        play_();
      }
    } else if (streamCur == streamStart && _sLastOfs > streamStart) {
      mDelayedTime = 0.0f;
      next_();
      play_();
    }

    _sLastOfs = streamCur;
  } else if (!_sSwStarted) {
    OSStartStopwatch(&_sStopWatch);
    _sSwStarted = true;
  }
}

bool AudioStreamer::AudioPacket::exec(DVDFileInfo *handle) {
  char buffer[0x40];

  if (mIdentifier.as_u32 == 0xFFFFFFFF)
    return false;

  if (mIdentifier.as_u32 & 0x80000000)
    sprintf(buffer, "/AudioRes/Streams/Music/%s.adp", mIdentifier.as_string);
  else
    sprintf(buffer, "/AudioRes/Streams/Music/%lu.adp", mIdentifier.as_u32);

  if (!DVDOpen(buffer, handle))
    return false;

  DVDPrepareStreamAsync(handle, 0, 0, cbForPrepareStreamAsync_);

  return true;
}

void AudioStreamer::AudioPacket::setLoopPoint(s32 start, size_t length) {
  mLoopStart = start;
  mLoopEnd = start + length;
}

void AudioStreamer::AudioPacket::setLoopPoint(f64 start, f64 length) {
  mLoopStart = start;
  mLoopEnd = start + length;
}

bool Music::isValidBGM(MSStageInfo id) {
  switch (id) {
  case BGM_AIRPORT:
  case BGM_BIANCO:
  case BGM_CASINO:
  case BGM_CORONA:
  case BGM_DELFINO:
  case BGM_DOLPIC:
  case BGM_EVENT:
  case BGM_EXTRA:
  case BGM_MAMMA:
  case BGM_MARE_SEA:
  case BGM_MAREVILLAGE:
  case BGM_MERRY_GO_ROUND:
  case BGM_MONTE_NIGHT:
  case BGM_MONTE_ONSEN:
  case BGM_MONTE_RESCUE:
  case BGM_MONTEVILLAGE:
  case BGM_PINNAPACO:
  case BGM_PINNAPACO_SEA:
  case BGM_RICCO:
  case BGM_SHILENA:
  case BGM_SKY_AND_SEA:
    return true;
  default:
    return false;
  }
}

bool Music::isValidBGM(u32 id) {
#if 0
  return true;
#else
  switch (id & 0x3FF) {
#if 0
  case 0x1A:
  case 0x02:
  case 0x1E:
  case 0x15:
  case 0x13:
  case 0x01:
  case 0x1F:
  case 0x12:
  case 0x03:
  case 0x06:
  case 0x14:
  case 0x23:
  case 0x1D:
  case 0x18:
  case 0x22:
  case 0x07:
  case 0x05:
  case 0x04:
  case 0x09:
  case 0x08:
  case 0x21:
#else
  case BGM_AIRPORT & 0xFF:
  case BGM_BIANCO & 0xFF:
  case BGM_CASINO & 0xFF:
  case BGM_CORONA & 0xFF:
  case BGM_DELFINO & 0xFF:
  case BGM_DOLPIC & 0xFF:
  case BGM_EVENT & 0xFF:
  case BGM_EXTRA & 0xFF:
  case BGM_MAMMA & 0xFF:
  case BGM_MARE_SEA & 0xFF:
  case BGM_MAREVILLAGE & 0xFF:
  case BGM_MERRY_GO_ROUND & 0xFF:
  case BGM_MONTE_NIGHT & 0xFF:
  case BGM_MONTE_ONSEN & 0xFF:
  case BGM_MONTE_RESCUE & 0xFF:
  case BGM_MONTEVILLAGE & 0xFF:
  case BGM_PINNAPACO & 0xFF:
  case BGM_PINNAPACO_SEA & 0xFF:
  case BGM_RICCO & 0xFF:
  case BGM_SHILENA & 0xFF:
  case BGM_SKY_AND_SEA & 0xFF:
#endif
    return true;
  default:
    return false;
  }
#endif
}
