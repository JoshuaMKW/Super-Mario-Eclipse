#pragma once

#include "types.h"
#include "JKR/JKRHeap.hxx"

class JPADataBlock {
public:
  JPADataBlock(u8 *data, JKRHeap *heap);
  virtual ~JPADataBlock();

  void *mData;
};