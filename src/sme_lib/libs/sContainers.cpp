#include "libs/sContainer.hxx"
#include "obj/WaterBalloon.hxx"

template <> TRingBuffer<TWaterBalloon>::~TRingBuffer() {
  if (mGarbageCollect) {
    for (int i = 0; i < capacity(); ++i) {
      delete mBuffer[i];
    }
  }
  delete[] mBuffer;
}