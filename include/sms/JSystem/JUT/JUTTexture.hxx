#pragma once

#include "types.h"
#include "GX.h"

#include "JUTPalette.hxx"

class ResTIMG {};

class JUTTexture {
public:
  JUTTexture(int sX, int sY, GXTexFmt format);
  ~JUTTexture();

  void attachPalette(JUTPalette *palette);
  void initTexObj(GXTlut tlut);
  void initTexObj();
  void load(GXTexMapID id);
  void storeTIMG(ResTIMG *timg);

  u32 _00[0x54 / 4];
};