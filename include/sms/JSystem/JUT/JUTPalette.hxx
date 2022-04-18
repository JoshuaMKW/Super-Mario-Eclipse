#pragma once

#include "GX.h"

struct ResTLUT {
  u8 mTlutFmt;
  u8 mTransparency;
  u16 mLutSize;
  void *mLut __attribute__((aligned(32)));
};

enum JUTTransparency {
    OPAQUE,
    TRANSPARENT
};

class JUTPalette {
public:
    JUTPalette(GXTlut tlut, GXTlutFmt fmt, JUTTransparency transparency, u16 lutSize, void *lut);
    ~JUTPalette();

    void load();
    void storeTLUT(GXTlut tlut, ResTLUT *resource);

    GXTlutObj mTlutObj;
    GXTlut mTlut;
    GXTlutFmt mTlutFmt;
    void *mLut;
    u16 mLutSize;
    JUTTransparency mTransparency;
};