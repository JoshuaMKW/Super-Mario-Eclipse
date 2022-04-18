#pragma once

#include "JKR/JKRHeap.hxx"
#include "JPABaseEmitter.hxx"
#include "JPATextureResource.hxx"

struct JPAEmitterData {
  size_t mResourceNum;
  size_t mResourceMax;
  JPABaseEmitter **mEmitters;
};

class JPAResourceManager {
public:
  JPAResourceManager(size_t capacity, u32, JKRHeap *heap);

  void load(const char *filepath, u16);
  void load(const void *data, u16);

  JKRHeap *mHeap;
  JPAEmitterData *mResource;
  JPATextureResource *mTextureResource;
};