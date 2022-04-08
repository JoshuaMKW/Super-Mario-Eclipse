#pragma once

#include "types.h"
#include "JGeometry.hxx"

enum EnumCamShakeMode {
  UnknownShake3 = 0x3,
  UnknownShake5 = 0x5,
  UnknownShake8 = 0x8,
  UnknownShake9 = 0x9,
  UnknownShake11 = 0x11,
  UnknownShake18 = 0x18,
  UnknownShake27 = 0x27
};

class TCameraShake {
public:
  TCameraShake();
    
  void execShake(const TVec3f &, TVec3f *, TVec3f *);
  void *getUseShakeData();
  void keepShake(EnumCamShakeMode mode, f32 strength);
  void startShake(EnumCamShakeMode mode, f32 strength);

  u16 _0;
  u16 _2;
  u32 _4;
  u8 _8;
  u8 _9;
  u16 _A;
  u16 _C;
  u16 _E; // padding?
  f32 _10;
  f32 _14;
  u16 _18;
  u16 _1A; // padding?
  f32 _1C;
  f32 _20;
  u16 _24;
  u16 _26; // padding?
  f32 _28;
  f32 _2C;
  u16 _30;
  u16 _32; // padding?
  u8 _34[0x6A8 - 0x34];
};