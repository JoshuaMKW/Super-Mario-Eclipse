#pragma once

#include "types.h"

union J3DGXColorS10 {
  struct {
    s16 r;
    s16 g;
    s16 b;
    s16 a;
  } rgba;
  u64 value;
};


union J3DGXColor {
  struct {
    u8 r;
    u8 g;
    u8 b;
    u8 a;
  } rgba;
  u32 value;
};

class J3DColorBlock {
public:
  virtual void reset(J3DColorBlock *);

  virtual ~J3DColorBlock();
};