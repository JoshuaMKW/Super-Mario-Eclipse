#ifndef PPCARCH_H
#define PPCARCH_H

#include "types.h"

#ifdef __cplusplus
extern "C"
{
#endif

    u32 PPCMfmsr();
    void PPCMtmsr(u32 state);
    u32 PPCMfhid0();
    void PPCMthid0(u32 state);
    u32 PPCMfl2cr();
    void PPCMtl2cr(u32 state);
    void PPCMtdec(u32 state);
    void PPCSync();
    void PPCHalt();
    u32 PPCMfhid2();
    void PPCMthid2(u32 state);
    void PPCMtwpar(u32 state);
    void PPCDisableSpeculation();
    void PPCSetFpNonIEEEMode();

#ifdef __cplusplus
}
#endif

#endif