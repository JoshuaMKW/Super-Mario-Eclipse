#pragma once

#include "AI.h"
#include "DVD.h"
#include "OS.h"
#include "sms/JSystem/JSU/JSUStream.hxx"
#include "sms/JSystem/JKR/JKRHeap.hxx"
#include "sms/sound/MSBGM.hxx"

using namespace SME::Util;

constexpr size_t AudioQueueSize = 4;
constexpr size_t AudioStackSize = 0x4000;

namespace SME::Class {

class AudioStreamer {
public:
  enum class AudioCommand {
    NONE,
    PLAY,
    PAUSE,
    STOP,
    SKIP,
    NEXT,
    SEEK,
  };

  struct AudioPacket {
    friend class AudioStreamer;

    union Identifier {
      u32 as_u32;
      char *as_string;
    };

    AudioPacket(u32 id) : mLoopStart(-1), mLoopEnd(-1) {
      mIdentifier.as_u32 = id;
    }

    AudioPacket(char *file) : mLoopStart(-1), mLoopEnd(-1) {
      mIdentifier.as_string = file;
    }

    bool exec(DVDFileInfo *handle);
    void setLoopPoint(s32 start, size_t length);
    void setLoopPoint(f64 start, f64 length);

    Identifier mIdentifier;

  private:
    s32 mLoopStart;
    s32 mLoopEnd;
  };

public:
  AudioStreamer(void *(*mainLoop)(void *), OSPriority priority, DVDFileInfo *fInfo,
                DVDCommandBlock *cb)
      : mAudioHandle(fInfo), mAudioCommandBlock(cb), mAudioIndex(0),
        mDelayedTime(0.0f), _mWhere(0), _mWhence(JSUStreamSeekFrom::BEGIN),
        mIsPlaying(false), mIsPaused(false), mIsLooping(false), mVolLeft(0x7F),
        mVolRight(0x7F) {
    mAudioStack =
        static_cast<u8 *>(JKRHeap::sRootHeap->alloc(AudioStackSize, 32));
    OSInitMessageQueue(&mMessageQueue, mMessageList, sizeof(mMessageList));
    OSCreateThread(&mMainThread, mainLoop, this, mAudioStack + AudioStackSize,
                   AudioStackSize, priority, OS_THREAD_ATTR_DETACH);
    OSResumeThread(&mMainThread);
  }

  ~AudioStreamer() {
    JKRHeap::sRootHeap->free(mAudioStack);
    OSCancelThread(&mMainThread);
  }

  static AudioStreamer *getInstance() { return &sInstance; }
  AudioPacket *getCurrentAudio() const { return mAudioQueue[mAudioIndex]; }
  u16 getVolumeLR() const { return (mVolLeft << 8) | mVolRight; }

  void setLooping(bool loop) { mIsLooping = loop; }

  bool isPlaying() const { return mIsPlaying; }
  bool isPaused() const { return mIsPaused; }
  bool isLooping() const { return mIsLooping; }

  void setVolumeLR(u8 left, u8 right);

  bool queueAudio(AudioPacket &packet);
  void play();
  void pause();
  void stop();
  void skip(f32 delay);
  void next(f32 delay);
  void seek(s32 where, JSUStreamSeekFrom whence);
  void seek(f64 seconds, JSUStreamSeekFrom whence);

  bool play_();
  bool pause_();
  bool stop_();
  bool skip_();
  void next_();
  bool seek_();
  void update_();

  OSThread mMainThread;
  OSMessageQueue mMessageQueue;
  OSMessage mMessageList[16];
  DVDFileInfo *mAudioHandle;
  DVDCommandBlock *mAudioCommandBlock;
  u8 *mAudioStack;

private:
  AudioPacket *mAudioQueue[AudioQueueSize];
  s32 mAudioIndex;
  f32 mDelayedTime;

  s32 _mWhere;
  JSUStreamSeekFrom _mWhence;

  bool mIsPlaying;
  bool mIsPaused;
  bool mIsLooping;
  u8 mVolLeft;
  u8 mVolRight;

  static AudioStreamer sInstance;
};

} // namespace SME::Class

namespace SME::Util::Music {

bool isValidBGM(u32 id);
bool isValidBGM(MSStageInfo id);

} // namespace SME::Util::Music