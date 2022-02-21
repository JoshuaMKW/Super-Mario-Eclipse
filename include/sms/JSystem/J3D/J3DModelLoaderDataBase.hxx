#pragma once

#include "types.h"

#include "J3DMaterial.hxx"
#include "J3DModel.hxx"

class J3DModelLoaderDataBase {
public:
  static J3DModelData *load(const void *, u32);
  static J3DMaterialTable *loadMaterialTable(const void *);
};