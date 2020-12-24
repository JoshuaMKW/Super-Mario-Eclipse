#ifndef AI_H
#define AI_H

#include "types.h"

#ifdef __cplusplus
extern "C"
{
#endif

    typedef void (*AIDCallback)();

    AIDCallback AIRegisterDMACallback(AIDCallback cb);
    void AIInitDMA(u32 start, u32 len);
    void AIStartDMA();
    void AIResetStreamSampleCount();

    void AISetStreamPlayState(u32 state);
    u32 AIGetStreamPlayState();

    void AISetDSPSampleRate(u32 rate);
    u32 AIGetDSPSampleRate();

    void AISetStreamSampleRate(u32 rate);
    u32 AIGetStreamSampleRate();

    void AISetStreamVolLeft(u8 volume);
    u8 AIGetStreamVolLeft();

    void AISetStreamVolRight(u8 volume);
    u8 AIGetStreamVolRight();

    void AIInit(u8 *);

#ifdef __cplusplus
}
#endif

#endif