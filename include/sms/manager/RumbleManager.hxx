#pragma once

#include "MTX.h"
#include "types.h"

class RumbleMgr {
public:
  RumbleMgr(bool p1, bool p2, bool p3, bool p4);

  void finishPause();
  void init();
  void reset();
  void setActive(bool active);
  void start(int, int, Vec *);
  void start(int, int, f32 *);
  void start(int, Vec *);
  void start(int, f32 *);
  void startPause();
  void stop(int);
  void stop();
  void update();

  f32 _0;
  f32 _4;
  u8 _8;
  u8 _9;
  u8 _A;
  u8 _B;
  u8 _C[0x1C - 0xC];
  u32 _1C;
  u32 _20;
  u32 _24;
  u32 _28;
  u32 *_2C;
};
