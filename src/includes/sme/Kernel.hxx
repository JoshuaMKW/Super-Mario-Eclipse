#pragma once

#include "types.h"


namespace Kamek {

struct KBHeader {
	u32 magic1;
	u16 magic2;
	u16 version;
	u32 bssSize;
	u32 codeSize;
};

enum KCommandType {
    kAddr32 = 1,
    kAddr16Lo = 4,
    kAddr16Hi = 5,
    kAddr16Ha = 6,
    kRel24 = 10,
    kWrite32 = 32,
    kWrite16 = 33,
    kWrite8 = 34,
    kCondWritePointer = 35,
    kCondWrite32 = 36,
    kCondWrite16 = 37,
    kCondWrite8 = 38,
    kBranch = 64,
    kBranchLink = 65
};

bool loadKamekBinary(const void *binary, u32 binaryLength);
bool loadKamekBinaryFromDisc(const char *path);

}

#define kCommandHandler(name) \
	static inline const u8 *kHandle##name(const u8 *input, u32 text, u32 address)
#define kDispatchCommand(name) \
	case static_cast<u8>(Kamek::KCommandType::k##name): input = kHandle##name(input, text, address); break