#pragma once

#include "math.h"
#include "types.h"

template <size_t _S, typename _T> class TRingBuffer {
public:
  TRingBuffer() = default;
  virtual ~TRingBuffer() = default;

  void push(_T *item) {
    mIndex = (mIndex + 1) % _S;
    mBuffer[mIndex] = item;
  }

  _T *pop() {
    _T *item = mBuffer[mIndex];
    mBuffer[mIndex] = nullptr;
    mIndex = mIndex > 0 ? mIndex - 1 : _S - (1 + mIndex);
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

private:
  _T *mBuffer[_S];
  int mIndex;
};