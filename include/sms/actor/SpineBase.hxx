#pragma once

#include "LiveActor.hxx"

#include "types.h"

template <typename T> class TSpineBase;

template <typename T> class TSolidStack {
  size_t mStackCapacity;
  s32 mStackIndex;
  T *mStackItems;

public:
  inline TSolidStack(size_t capacity) : mStackCapacity(capacity) {
    mStackItems = new T[capacity];
  }
  virtual ~TSolidStack();

  inline void push(T item) {
    if (mStackIndex < mStackCapacity) {
      mStackItems[mStackIndex++] = item;
    }
  }

  inline T pop() {
    if (mStackIndex > 0) {
      return mStackItems[--mStackIndex];
    }
  }

  size_t capacity() const { return mStackCapacity; };
  s32 depth() const { return mStackIndex; }
};

template <typename T> class TNerveBase {
public:
  virtual ~TNerveBase();
  virtual bool execute(TSpineBase<T> *mSpineBase) const;
};

template <typename T> class TSpineBase {
public:
  T *mTarget;                                     // 0x0000
  TSolidStack<const TNerveBase<T> *> mNerveStack; // 0x0004
  const TNerveBase<T> *mNerveCurrent;             // 0x0014
  const TNerveBase<T> *mNerveCopy;                // 0x0018
  const TNerveBase<T> *mNervePrevious;            // 0x001C
  s32 mNerveTimer;                                // 0x0020

  virtual ~TSpineBase();

  virtual void update();

  const TNerveBase<T> *getLatestNerve();
  void pushNerve(const TNerveBase<T> *nerve);
  void setNerve(const TNerveBase<T> *nerve) {
    if (mNerveCurrent)
      mNervePrevious = mNerveCurrent;
    mNerveTimer = 0;
    mNerveCurrent = nerve;
  }
};