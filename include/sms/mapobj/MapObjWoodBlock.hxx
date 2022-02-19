#pragma once

#include "JSU/JSUMemoryStream.hxx"
#include "MapObjNormalLift.hxx"
#include "types.h"

union U32SplitU16 {
  struct {
    u16 upper;
    u16 lower;
  } u16;
  u32 u32;
};

class TWoodBlock : public TNormalLift {
public:
  TWoodBlock(const char *);
  virtual ~TWoodBlock();

  virtual void load(JSUMemoryInputStream &);
  virtual bool calcRecycle() override;

  u32 _15C;
  u32 _160;
  U32SplitU16 _164;
  U32SplitU16 _168;
};