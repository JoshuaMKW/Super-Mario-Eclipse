#pragma once

#include "../JSU/JSUList.hxx"
#include "JKRAramBlock.hxx"
#include "JKRAramHeap.hxx"
#include "JKRDvdRipper.hxx"
#include "JKRThread.hxx"
#include "OS.h"
#include "types.h"


class JKRAram : public JKRThread {
public:
  JKRAram(size_t size, u32, s32);
  virtual ~JKRAram();

  virtual void run();

  static u32 *create(size_t size, u32, s32 streamPriority, s32 decompPriority);
  static s32 mainRamToAram(u8 *src, u32 dstAddr, size_t size,
                           JKRExpandSwitch decompress, size_t, JKRHeap *heap,
                           int);
  static s32 aramToMainRam(u32 srcAddr, u8 *dst, size_t size,
                           JKRExpandSwitch decompress, size_t maxSize,
                           JKRHeap *heap, int, u32 *stackSize);
  static s32 aramToMainRam(JKRAramBlock *, u8 *dst, size_t size, u32 srcAddr,
                           JKRExpandSwitch decompress, size_t maxSize,
                           JKRHeap *heap, int, u32 *stackSize);

  u32 _60;
  u32 _64;
  u32 _68;
  u32 _6C;
  u32 _70;
  u32 _74;
  JKRAramHeap *mAramHeap;
  void *_7C;

  static JSUPtrList sAramCommandList;
  static JKRAram *sAramObject;
  static u8 sMessageBuffer[12];
  static OSMessageQueue sMessageQueue;
};

void JKRDecompressFromAramToMainRam(u32 srcAddr, void *dst, u32, size_t size,
                                    u32);