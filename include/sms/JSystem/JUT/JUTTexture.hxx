#pragma once

#include "types.h"

class ResTIMG {};

class JUTTexture {
public:
  JUTTexture() {}

  void storeTIMG(ResTIMG *timg);

  u32 _00[0x54 / 4];
};