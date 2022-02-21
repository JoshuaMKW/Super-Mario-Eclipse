#ifndef AI_H
#define AI_H

#include "types.h"

#ifdef __cplusplus
extern "C"
{
#endif

    typedef void (*AIDCallback)();
    typedef void (*AISCallback)(u32);

    AIDCallback AIRegisterDMACallback(AIDCallback cb);
    AISCallback AIRegisterStreamCallback(AISCallback cb);

    void AIInitDMA(u32 start, u32 len);
    void AIStartDMA();
    void AIStopDMA();

    void AISetStreamPlayState(u32 state);
    u32 AIGetStreamPlayState();

    u16 AIGetDMABytesLeft();
    bool AIGetDMAEnableFlag();
    u16 AIGetDMALength();
    u16 AIGetDMAStartAddr();

    void AISetDSPSampleRate(u32 rate);
    u32 AIGetDSPSampleRate();

    void AISetStreamSampleRate(u32 rate);
    u32 AIGetStreamSampleRate();

    void AIResetStreamSampleCount();
    u32 AIGetStreamSampleCount();

    void AISetStreamTrigger(u32 trigger);
    u32 AIGetStreamTrigger();

    void AISetStreamVolLeft(u8 volume);
    u8 AIGetStreamVolLeft();

    void AISetStreamVolRight(u8 volume);
    u8 AIGetStreamVolRight();

    void AIInit(u8 *);
    void AIReset();
    bool AICheckInit();

#ifdef __cplusplus
}
#endif

#endif