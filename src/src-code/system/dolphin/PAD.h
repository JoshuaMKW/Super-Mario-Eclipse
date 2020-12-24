#ifndef PAD_H
#define PAD_H

#include "types.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define PAD_STOP_RUMBLE 0
#define PAD_START_RUMBLE 1
#define PAD_STOP_HARD 2

    typedef void (*PADSamplingCallback)();

    struct PADStatus
    {
        u16 button;      // _0
        u8 stickX;       // _2
        u8 stickY;       // _3
        u8 subStickX;    // c stick X (_4)
        u8 subStickY;    // c stick Y (_5)
        u8 triggerLeft;  // left trigger pressure (_6)
        u8 triggerRight; // right trigger pressure (_7)
        u8 analogA;      // _8
        u8 analogB;      // _9
        u8 curError;     // some odd number based on some constant (_10)
    };

    bool PADReset(u32 mask);
    bool PADRecalibrate(u32 mask);
    void PADClamp(PADStatus *padStat);
    bool PADInit();
    u32 PADRead(PADStatus *padStat);
    void PADControlMotor(u32 channel, u32 command);
    void PADSetAnalogMode(u32 mode);
    void PADSetSpec(u32 padModel);

#ifdef __cplusplus
}
#endif

#endif