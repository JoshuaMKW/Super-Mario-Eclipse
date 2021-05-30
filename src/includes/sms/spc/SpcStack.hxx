#pragma once

#include "types.h"

// I've only seen this used as u32 and TSpcSlice
template <typename T> class TSpcStack {
public:
  size_t mMaxSize;
  size_t mCurSize;
  T *mStack;

  TSpcStack(size_t size) : mMaxSize(size), mCurSize(0) {
    mStack = new T[size];
  }
  
  virtual ~TSpcStack();

  void push(const T &slice);
  T pop() {
    mCurSize -= 1;
    return mStack[mCurSize + 1];
  };
};