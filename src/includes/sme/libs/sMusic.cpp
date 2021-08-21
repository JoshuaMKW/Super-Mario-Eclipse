
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
    command = static_cast<AudioStreamer::AudioCommand>(msg);
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

static void volumeAlarm(OSAlarm *alarm, OSContext *context) {
  AudioStreamer *streamer = AudioStreamer::getInstance();
  streamer->fadeAudio_();
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
  if (mVolLeft != left) {
    mVolLeft = left;
    AISetStreamVolLeft(left);
  }
  if (mVolRight != right) {
    mVolRight = right;
    AISetStreamVolRight(right);
  }
}

AudioStreamer::AudioStreamer(void *(*mainLoop)(void *), OSPriority priority,
                             DVDFileInfo *fInfo, DVDCommandBlock *cb)
    : mAudioHandle(fInfo), mAudioCommandBlock(cb), mAudioIndex(0),
      mDelayedTime(0.0f), mFadeTime(0.0f), _mWhere(0),
      _mWhence(JSUStreamSeekFrom::BEGIN), mIsPlaying(false), mIsPaused(false),
      mIsLooping(false), mVolLeft(0x7F), mVolRight(0x7F), mTargetVolume(0x7F),
      mPreservedVolLeft(0x7F), mPreservedVolRight(0x7F) {
  mAudioStack =
      static_cast<u8 *>(JKRHeap::sRootHeap->alloc(AudioStackSize, 32));
  OSInitMessageQueue(&mMessageQueue, mMessageList, AudioMessageQueueSize);
  OSCreateAlarm(&mVolumeFadeAlarm);
  OSSetPeriodicAlarm(&mVolumeFadeAlarm, OSGetTime(), OSMillisecondsToTicks(1),
                     volumeAlarm);
  OSCreateThread(&mMainThread, mainLoop, this, mAudioStack + AudioStackSize,
                 AudioStackSize, priority, OS_THREAD_ATTR_DETACH);
  OSResumeThread(&mMainThread);
}

AudioStreamer::~AudioStreamer() {
  JKRHeap::sRootHeap->free(mAudioStack);
  OSCancelAlarm(&mVolumeFadeAlarm);
  OSCancelThread(&mMainThread);
}

void AudioStreamer::setVolumeFadeTo(u8 volume, f32 seconds) {
  mTargetVolume = volume;
  mFadeTime = seconds;
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

static OSTime sStartTime;
static bool _sHasFadeStarted = false;

void AudioStreamer::fadeAudio_() {
  AudioStreamer *streamer = AudioStreamer::getInstance();

  u16 volume = streamer->getVolumeLR();
  if (u8(volume >> 8) == streamer->mTargetVolume &&
      u8(volume) == streamer->mTargetVolume) {
    sStartTime = 0;
    _sHasFadeStarted = false;
    return;
  }

  if (!_sHasFadeStarted) {
    sStartTime = OSGetTime();
    _sHasFadeStarted = true;
  }

  const OSTime now = OSGetTime();

  f32 curTime = f32(OSTicksToMilliseconds(now - sStartTime)) / 1000.0f;

  SME_DEBUG_LOG("ticks = %llu; curTime = %.04f\n",
                now - sStartTime, curTime);

  u8 volL = Math::lerp<u8>(u8(volume >> 8), streamer->mTargetVolume,
                           curTime / streamer->mFadeTime);
  u8 volR = Math::lerp<u8>(u8(volume), streamer->mTargetVolume,
                           curTime / streamer->mFadeTime);

  if (volL == u8(volume >> 8) && volR == u8(volume)) {
    return;
  }

  setVolumeLR(volL, volR);
}

void AudioStreamer::play() {
  sAudioCommand = AudioCommand::PLAY;
  OSSendMessage(&mMessageQueue, static_cast<u32>(sAudioCommand),
                OS_MESSAGE_NOBLOCK);
}

void AudioStreamer::pause(f32 fadeTime) {
  sAudioCommand = AudioCommand::PAUSE;
  mDelayedTime = fadeTime;
  OSSendMessage(&mMessageQueue, static_cast<u32>(sAudioCommand),
                OS_MESSAGE_NOBLOCK);
}

void AudioStreamer::stop(f32 fadeTime) {
  sAudioCommand = AudioCommand::STOP;
  mDelayedTime = fadeTime;
  OSSendMessage(&mMessageQueue, static_cast<u32>(sAudioCommand),
                OS_MESSAGE_NOBLOCK);
}

void AudioStreamer::skip(f32 fadeTime) {
  sAudioCommand = AudioCommand::SKIP;
  mDelayedTime = fadeTime;
  OSSendMessage(&mMessageQueue, static_cast<u32>(sAudioCommand),
                OS_MESSAGE_NOBLOCK);
}

void AudioStreamer::next(f32 fadeTime) {
  sAudioCommand = AudioCommand::NEXT;
  mDelayedTime = fadeTime;
  OSSendMessage(&mMessageQueue, static_cast<u32>(sAudioCommand),
                OS_MESSAGE_NOBLOCK);
}

void AudioStreamer::seek(s32 where, JSUStreamSeekFrom whence) {
  sAudioCommand = AudioCommand::SEEK;
  _mWhere = where;
  _mWhence = whence;
  OSSendMessage(&mMessageQueue, static_cast<u32>(sAudioCommand),
                OS_MESSAGE_NOBLOCK);
}

void AudioStreamer::seek(f64 seconds, JSUStreamSeekFrom whence) {
  sAudioCommand = AudioCommand::SEEK;
  _mWhere =
      32768 *
      (seconds / OSTicksToSeconds(
                     23465670)); // magic number using mean of data go brrrrrr
  _mWhence = whence;
  OSSendMessage(&mMessageQueue, static_cast<u32>(sAudioCommand),
                OS_MESSAGE_NOBLOCK);
}

bool AudioStreamer::play_() {
  if (isPlaying()) {
    if (isPaused()) {
      AISetStreamPlayState(1);

      setVolumeFadeTo((mPreservedVolLeft + mPreservedVolRight) / 2, mFadeTime);

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

  mFadeTime = mDelayedTime;
  mPreservedVolLeft = mVolLeft;
  mPreservedVolRight = mVolRight;
  setVolumeFadeTo(0, mFadeTime);

  mIsPaused = true;
  return true;
}

bool AudioStreamer::stop_() {
  if (!mIsPlaying)
    return false;

  mFadeTime = mDelayedTime;
  mPreservedVolLeft = mVolLeft;
  mPreservedVolRight = mVolRight;
  setVolumeFadeTo(0, mFadeTime);

  mIsPaused = false;
  mIsPlaying = false;
  return true;
}

bool AudioStreamer::skip_() {
  next_();
  return play_();
}

void AudioStreamer::next_() {
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
    streamPos =
        (DVDGetStreamPlayAddr(mAudioCommandBlock) - streamStart) + _mWhere;
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

  return DVDPrepareStreamAsync(mAudioHandle,
                               packet->mLoopEnd - packet->mLoopStart,
                               packet->mLoopStart, cbForPrepareStreamAsync_);
}

static u32 _sLastOfs = 0;
static u8 _sLastVol = 0xFF;
static bool _startPaused = false;

void AudioStreamer::update_() {
  const u8 vol = (mVolLeft + mVolRight) / 2;
  if (_sLastVol > 0) {
    if (mIsPaused && vol == 0) {
      if (!mIsPlaying) {
        DVDCancelStreamAsync(mAudioCommandBlock, nullptr);
      }
      AISetStreamPlayState(false);
    } else if (!mIsPaused && !mIsPlaying && vol == 0) {
      DVDCancelStreamAsync(mAudioCommandBlock, nullptr);
      AISetStreamPlayState(false);
    }
  }
  _sLastVol = vol;

  if (!isPlaying())
    return;

  AudioPacket *packet = getCurrentAudio();
  if (!packet)
    return;

  if (!_startPaused && gpMarDirector->mCurState == TMarDirector::PAUSE_MENU) {
    pause(0.7f);
    _startPaused = true;
  } else if (_startPaused &&
             gpMarDirector->mCurState != TMarDirector::PAUSE_MENU) {
    play();
    _startPaused = false;
  }

  const u32 streamSize = mAudioHandle->mLen;
  const u32 streamStart = mAudioHandle->mStart;
  const u32 streamCur = DVDGetStreamPlayAddr(mAudioCommandBlock);

  const bool shouldLoop =
      (packet->mLoopEnd != 0xFFFFFFFF && packet->mLoopStart != 0xFFFFFFFF);

  if (streamCur != _sLastOfs) {
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
        play();
      }
    } else if (streamCur == streamStart && _sLastOfs > streamStart) {
      next(0.0f);
      play();
    }

    _sLastOfs = streamCur;
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
  switch (id & 0x3FF) {
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
    return true;
  default:
    return false;
  }
}

bool Music::isWeakBGM(MSStageInfo id) {
  switch (id) {
  case BGM_UNDERGROUND:
  case BGM_SHINE_APPEAR:
    return true;
  default:
    return false;
  }
}

bool Music::isWeakBGM(u32 id) {
  switch (id & 0x3FF) {
  case BGM_UNDERGROUND & 0xFF:
  case BGM_SHINE_APPEAR & 0xFF:
    return true;
  default:
    return false;
  }
}