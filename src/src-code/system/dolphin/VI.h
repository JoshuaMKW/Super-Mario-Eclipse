#ifndef VI_H
#define VI_H

#include "types.h"

#ifdef __cplusplus
extern "C"
{
#endif

    // this is in the global stack, at (SDA_ADDR - 0x7138)
    typedef void (*VIRetraceCallback)(u32 count);

    void VIInit();
    void VIFlush();
    void VIWaitForRetrace();
    void VISetBlack(bool);
    u32 VIGetRetraceCount();

    void VISetNextFrameBuffer(void *frameBuffer);

    VIRetraceCallback VISetPreRetraceCallback(VIRetraceCallback cb);
    VIRetraceCallback VISetPostRetraceCallback(VIRetraceCallback cb);

    u32 VIGetNextField();
    u32 VIGetCurrentLine();
    u32 VIGetDTVStatus();
    u32 VIGetTvFormat();

#ifdef __cplusplus
}
#endif

#endif