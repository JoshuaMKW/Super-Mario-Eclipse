#ifndef AR_H
#define AR_H

#include "types.h"

#ifdef __cplusplus
extern "C"
{
#endif

    typedef void (*ARCallback)();

    ARCallback ARRegisterDMACallback(ARCallback cb);
    void ARStartDMA(u32 type, u32 memAddr, u32 aramAddr, u32 len);

    u32 ARAlloc(u32 len);
    u32 ARInit(u32 *stackIndexAddr, u32 entryCount);

    u32 ARGetBaseAddress(); // 0x4000
    u32 ARGetSize();

#ifdef __cplusplus
}
#endif

#endif