#pragma once

#include "types.h"

class J3DModel;
class J3DModelData;

enum J3DDeformAttachFlag {
	UNK_0,
	UNK_1
};

class J3DSkinDeform {
public:
  J3DSkinDeform();

  virtual void deform(J3DModel *);
  virtual ~J3DSkinDeform();

  void calcNrmMtx(J3DModel *);
  void initMtxIndexArray(J3DModelData *);

  u32 _4;
  u32 _8;
  u32 _C;
  u32 _10;
};