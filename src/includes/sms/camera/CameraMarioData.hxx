#pragma once

#include "types.h"

class TCameraMarioData {
public:
  void calcAndSetMarioData();
  bool isMarioClimb(u32) const;
  bool isMarioGoDown() const;
  bool isMarioIndoor() const;
  bool isMarioLeanMirror() const;
  bool isMarioRocketing() const;
  bool isMarioSlider() const;

  f32 _0;
  f32 _4;
  f32 _8;
  f32 _C;
  f32 _10;
  u32 _14;
  u32 _18;
  f32 _1C;
};