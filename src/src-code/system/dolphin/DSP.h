#ifndef DSP_H
#define DSP_H

#include "types.h"

#ifdef __cplusplus
extern "C"
{
#endif

    u32 DSPCheckMailToDSP();
    u32 DSPCheckMailFromDSP();

    u32 DSPReadMailFromDSP();
    void DSPSendMailToDSP(u32 mail);

    void DSPAssertInt();
    void DSPInit();

#ifdef __cplusplus
}
#endif

#endif