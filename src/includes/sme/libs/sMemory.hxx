#include "mem.h"

#pragma once

#include "OS.h"
#include "JKR/JKRHeap.hxx"
#include "types.h"

namespace SME::Util::Cache {

inline void flush(void *addr, size_t size) {
  DCFlushRange(addr, size);
  ICInvalidateRange(addr, size);
}

inline void flash() { ICFlashInvalidate(); }

inline void store(void *addr, size_t size) {
  DCStoreRange(addr, size);
  ICInvalidateRange(addr, size);
}

inline void zero(void *addr, size_t size) { DCZeroRange(addr, size); }

inline void enable() {
  DCEnable();
  ICEnable();
}

inline void disable() {
  DCDisable();
  ICDisable();
}

} // namespace SME::Util::Cache

namespace SME::Util::Memory {

inline void *malloc(const size_t size, const size_t alignment) {
  return JKRHeap::sCurrentHeap->alloc(size, alignment);
}

inline void *hmalloc(JKRHeap *heap, const size_t size, const size_t alignment) {
  return JKRHeap::alloc(size, alignment, heap);
}

inline void *calloc(const size_t size, const size_t alignment) {
  void *obj = malloc(size, alignment);
  memset(obj, 0, size);
  return obj;
}

inline void *hcalloc(JKRHeap *heap, const size_t size, const size_t alignment) {
  void *obj = hmalloc(heap, size, alignment);
  memset(obj, 0, size);
  return obj;
}

inline void free(const void *ptr) { delete (u8 *)ptr; }

namespace PPC {

inline u32 *getBranchDest(u32 *bAddr) {
  s32 offset;
  u32 instr = *bAddr;

  if (instr & 0x2000000)
    offset = (instr & 0x3FFFFFD) - 0x4000000;
  else
    offset = instr & 0x3FFFFFD;
  return static_cast<u32 *>(bAddr + (offset / 4));
}

template <typename T> inline void write(T *ptr, T value) {
  *ptr = value;
  Cache::flush(ptr, sizeof(T));
}

} // namespace PPC

namespace Processor {}

namespace Protection {

enum ACCESS { DENIED, READ, WRITE, READWRITE };

class MemoryMap {
  u16 _mStart;
  u16 _mEnd;
  ACCESS mPermission;
  u8 mIndex;

public:
  MemoryMap() {}
  MemoryMap(u8 index);
  ~MemoryMap();

  void *end() const { return (void *)((this->_mEnd << 10) | 0x80000000); }
  u8 index() const { return this->mIndex; }
  ACCESS permission() const { return this->mPermission; }
  s32 size() const { return (this->_mEnd - this->_mStart) << 10; }
  void *start() const { return (void *)((this->_mStart << 10) | 0x80000000); }
  void setStart(const u32 addr) { this->_mStart = (addr & 0x1FFFFFF) >> 10; }
  void setEnd(const u32 addr) { this->_mEnd = (addr & 0x1FFFFFF) >> 10; }
  void setPermission(ACCESS perm) { this->mPermission = perm; }
  void setIndex(u8 index) { this->mIndex = index; }

  void activate();
  void deactivate();

  static const u32 MARR0 = 0xCC004000;
  static const u32 MARR1 = 0xCC004004;
  static const u32 MARR2 = 0xCC004008;
  static const u32 MARR3 = 0xCC00400C;
  static const u32 MARRCTRL = 0xCC004010;
  static const u32 MARRENBL = 0xCC00401C;
};

} // namespace Protection

} // namespace SME::Util::Memory
