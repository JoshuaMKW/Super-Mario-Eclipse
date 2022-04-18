#pragma once

#include "JPADefaultTexture.hxx"
#include "JPATexture.hxx"
#include "JKR/JKRHeap.hxx"


class JPATextureResource {
public:
  JPATextureResource(size_t capacity, JKRHeap *heap);

  void registration(u8 *data, JKRHeap *heap);

  JPADefaultTexture mDefaultTexture;
  size_t mTextureNum;
  size_t mTextureMax;
  JPATexture **mTextures;
};