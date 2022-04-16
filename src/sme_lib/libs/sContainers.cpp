#include "libs/sContainer.hxx"
#include "obj/WaterBalloon.hxx"
#include "sms/enemy/BossGesso.hxx"

template <> TRingBuffer<TWaterBalloon>::~TRingBuffer() {
  if (mGarbageCollect) {
    for (int i = 0; i < capacity(); ++i) {
      delete mBuffer[i];
    }
  }
  delete[] mBuffer;
}

template <> TRingBuffer<TBGPolDrop>::~TRingBuffer() {
  if (mGarbageCollect) {
    for (int i = 0; i < capacity(); ++i) {
      delete mBuffer[i];
    }
  }
  delete[] mBuffer;
}