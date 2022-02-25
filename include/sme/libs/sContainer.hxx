#pragma once

#include "math.h"
#include "types.h"
#include "sLogging.hxx"

#if 0
template <size_t _S, typename _T> class TRingBuffer {
public:
  TRingBuffer() = default;
  virtual ~TRingBuffer() = default;

  void push(_T *item) {
    if (mBuffer[mIndex]) {
      SME_DEBUG_LOG("[RingBuffer] Deleting item %d\n", mIndex);
      delete mBuffer[mIndex];
    }
    mBuffer[mIndex] = item;
    mIndex = (mIndex + 1) % _S;
  }

  _T *pop() {
    _T *item = mBuffer[mIndex];
    mBuffer[mIndex] = nullptr;
    mIndex = mIndex > 0 ? mIndex - 1 : _S - (1 + mIndex);
    return item;
  }

  _T *current() const {
    return mBuffer[mIndex];
  }

  _T *at(int index, bool absolute = true) {
    if (absolute) {
      return mBuffer[index % _S];
    }
    return mBuffer[(mIndex + index) % _S];
  }

  _T *next() {
    mIndex = (mIndex + 1) % _S;
    _T *item = mBuffer[mIndex];
    return item;
  }

  _T *prev() {
    mIndex = mIndex > 0 ? mIndex - 1 : _S - (1 + mIndex);
    _T *item = mBuffer[mIndex];
    return item;
  }

  bool contains(_T *item) const {
    for (s32 i = 0; i < _S; ++i) {
      if (mBuffer[i] == item) {
        return true;
      }
    }
    return false;
  }

  constexpr size_t capacity() {
    return _S;
  }

private:
  _T *mBuffer[_S];
  int mIndex;
};
#else

template <typename _T> class TRingBuffer {
public:
  TRingBuffer(size_t capacity) : mCapacity(capacity), mIndex(0) {
    mBuffer = new _T[capacity];
  }
  virtual ~TRingBuffer() {
    delete[] mBuffer;
  }

  void push(_T *item) {
    if (mBuffer[mIndex]) {
      SME_DEBUG_LOG("[RingBuffer] Deleting item %d\n", mIndex);
      delete mBuffer[mIndex];
    }
    mBuffer[mIndex] = item;
    mIndex = (mIndex + 1) % mCapacity;
  }

  _T *pop() {
    _T *item = mBuffer[mIndex];
    mBuffer[mIndex] = nullptr;
    mIndex = mIndex > 0 ? mIndex - 1 : mCapacity - (1 + mIndex);
    return item;
  }

  _T *current() const {
    return mBuffer[mIndex];
  }

  _T *at(int index, bool absolute = true) {
    if (absolute) {
      return mBuffer[index % mCapacity];
    }
    return mBuffer[(mIndex + index) % mCapacity];
  }

  _T *next() {
    mIndex = (mIndex + 1) % mCapacity;
    _T *item = mBuffer[mIndex];
    return item;
  }

  _T *prev() {
    mIndex = mIndex > 0 ? mIndex - 1 : mCapacity - (1 + mIndex);
    _T *item = mBuffer[mIndex];
    return item;
  }

  bool contains(_T *item) const {
    for (s32 i = 0; i < mCapacity; ++i) {
      if (mBuffer[i] == item) {
        return true;
      }
    }
    return false;
  }

  constexpr size_t capacity() {
    return mCapacity;
  }

private:
  _T **mBuffer;
  size_t mCapacity;
  int mIndex;
};
#endif