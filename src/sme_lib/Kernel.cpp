#include "types.h"
#include "DVD.h"

#include "Kernel.hxx"
#include "libs/sLogging.hxx"
#include "libs/sMemory.hxx"

#if !defined(__ppc__) && !defined(__MWERKS__)
#define __ppc__
#endif

#include "ppc_intrinsics.h"

static inline u32 resolveAddress(u32 text, u32 address) {
  if (address & 0x80000000)
    return address;
  else
    return text + address;
}

kCommandHandler(Addr32) {
  u32 target = resolveAddress(text, *(const u32 *)input);
  *(u32 *)address = target;
  return input + 4;
}
kCommandHandler(Addr16Lo) {
  u32 target = resolveAddress(text, *(const u32 *)input);
  *(u16 *)address = target & 0xFFFF;
  return input + 4;
}
kCommandHandler(Addr16Hi) {
  u32 target = resolveAddress(text, *(const u32 *)input);
  *(u16 *)address = target >> 16;
  return input + 4;
}
kCommandHandler(Addr16Ha) {
  u32 target = resolveAddress(text, *(const u32 *)input);
  *(u16 *)address = target >> 16;
  if (target & 0x8000)
    *(u16 *)address += 1;
  return input + 4;
}
kCommandHandler(Rel24) {
  u32 target = resolveAddress(text, *(const u32 *)input);
  u32 delta = target - address;
  *(u32 *)address &= 0xFC000003;
  *(u32 *)address |= (delta & 0x3FFFFFC);
  return input + 4;
}
kCommandHandler(Write32) {
  u32 value = *(const u32 *)input;
  *(u32 *)address = value;
  return input + 4;
}
kCommandHandler(Write16) {
  u32 value = *(const u32 *)input;
  *(u16 *)address = value & 0xFFFF;
  return input + 4;
}
kCommandHandler(Write8) {
  u32 value = *(const u32 *)input;
  *(u8 *)address = value & 0xFF;
  return input + 4;
}
kCommandHandler(CondWritePointer) {
  u32 target = resolveAddress(text, *(const u32 *)input);
  u32 original = ((const u32 *)input)[1];
  if (*(u32 *)address == original)
    *(u32 *)address = target;
  return input + 8;
}
kCommandHandler(CondWrite32) {
  u32 value = *(const u32 *)input;
  u32 original = ((const u32 *)input)[1];
  if (*(u32 *)address == original)
    *(u32 *)address = value;
  return input + 8;
}
kCommandHandler(CondWrite16) {
  u32 value = *(const u32 *)input;
  u32 original = ((const u32 *)input)[1];
  if (*(u16 *)address == (original & 0xFFFF))
    *(u16 *)address = value & 0xFFFF;
  return input + 8;
}
kCommandHandler(CondWrite8) {
  u32 value = *(const u32 *)input;
  u32 original = ((const u32 *)input)[1];
  if (*(u8 *)address == (original & 0xFF))
    *(u8 *)address = value & 0xFF;
  return input + 8;
}
kCommandHandler(Branch) {
  *(u32 *)address = 0x48000000;
  return kHandleRel24(input, text, address);
}
kCommandHandler(BranchLink) {
  *(u32 *)address = 0x48000001;
  return kHandleRel24(input, text, address);
}

bool Kamek::loadKamekBinary(const void *binary, u32 binaryLength) {
  const Kamek::KBHeader *header = (const Kamek::KBHeader *)binary;
  if (header->magic1 != 0x4B616D65 || header->magic2 != 0x6B00)
    SME_LOG("[KAMEK] FATAL ERROR: Corrupted file, please check your game's "
            "Kamek files\n");
  return false;
  if (header->version != 1) {
    SME_LOG("[KAMEK] FATAL ERROR: Incompatible file (version %d), please "
            "upgrade your Kamek Loader\n",
            header->version);
    return false;
  }

  OSReport("[KAMEK] Header { bssSize=%u, codeSize=%u }\n", header->bssSize,
           header->codeSize);

  u32 textSize = header->codeSize + header->bssSize;
  u32 text = *(u32 *)0x80000034;
  if (!text)
    SME_LOG("[KAMEK] FATAL ERROR: Out of code memory\n");

  const u8 *input = ((const u8 *)binary) + sizeof(Kamek::KBHeader);
  const u8 *inputEnd = ((const u8 *)binary) + binaryLength;
  u8 *output = (u8 *)text;

  // Create text + bss sections
  for (u32 i = 0; i < header->codeSize; i++)
    *(output++) = *(input++);
  for (u32 i = 0; i < header->bssSize; i++)
    *(output++) = 0;

  while (input < inputEnd) {
    u32 cmdHeader = *((u32 *)input);
    input += 4;

    u8 cmd = cmdHeader >> 24;
    u32 address = cmdHeader & 0xFFFFFF;
    if (address == 0xFFFFFE) {
      // Absolute address
      address = *((u32 *)input);
      input += 4;
    } else {
      // Relative address
      address += text;
    }

    switch (cmd) {
      kDispatchCommand(Addr32);
      kDispatchCommand(Addr16Lo);
      kDispatchCommand(Addr16Hi);
      kDispatchCommand(Addr16Ha);
      kDispatchCommand(Rel24);
      kDispatchCommand(Write32);
      kDispatchCommand(Write16);
      kDispatchCommand(Write8);
      kDispatchCommand(CondWritePointer);
      kDispatchCommand(CondWrite32);
      kDispatchCommand(CondWrite16);
      kDispatchCommand(CondWrite8);
      kDispatchCommand(Branch);
      kDispatchCommand(BranchLink);
    default:
      OSReport("[KAMEK] Unknown command: %d\n", cmd);
    }

    __dcbst(address, 0);
    __sync();
    __icbi(address, 0);
  }

  __sync();
  __isync();

  return true;
}

bool Kamek::loadKamekBinaryFromDisc(const char *path) {
  OSReport("{Kamek by Treeki}\nLoading Kamek binary '%s'...\n", path);

  s32 entrynum = DVDConvertPathToEntrynum(path);
  if (entrynum < 0) {
    OSReport("[KAMEK] FATAL ERROR: Failed to locate file on the disc: %s\n",
             path);
    return false;
  }

  DVDFileInfo handle;
  if (!DVDFastOpen(entrynum, &handle)) {
    OSReport("[KAMEK] FATAL ERROR: Failed to open file!\n");
    return false;
  }

  OSReport("[KAMEK] DVD file located: Start=%p, Size=%d\n", handle.mStart,
           handle.mLen);

  const u32 length = handle.mLen, roundedLength = (handle.mLen + 0x1F) & ~0x1F;
  void *buffer = SME::Util::Memory::hmalloc(JKRHeap::sSystemHeap, length, 32);
  if (!buffer) {
    OSReport("[KAMEK] FATAL ERROR: Out of file memory\n", path);
    return false;
  }

  DVDReadPrio(&handle, buffer, roundedLength, 0, 2);
  DVDClose(&handle);

  Kamek::loadKamekBinary(buffer, handle.mLen);

  SME::Util::Memory::free(buffer);
  OSReport("[KAMEK] Binary patched successfully\n");

  return true;
}