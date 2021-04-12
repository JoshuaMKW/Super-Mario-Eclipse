#ifndef SI_H
#define SI_H

#include "types.h"

#ifdef __cplusplus
extern "C"
{
#define SI_CONTROLLER_GBA 0x0004
#define SI_CONTROLLER_GBA_NA 0x0800
#define SI_CONTROLLER_GC_STANDARD 0x0900
#define SI_CONTROLLER_GC_KEYBOARD 0x0820
#define SI_CONTROLLER_GC_DKONGAS 0x0900

    u32 SIInit();
    void SIInterruptHandler(u32, u32);
    bool SIEnablePollingInterrupt(u32);
    bool SIRegisterPollingHandler(u32);
    bool SIUnregisterPollingHandler(u32);
    void SITransferCommands();
    u32 SISetXY(u16, u8);
    void SISetSamplingRate(u32 rate);
    void SISetCommand(u32 cmd);
    u32 SIEnablePolling(u32 chanID);
    u32 SIDisablePolling(u32 chanID);
    BOOL SIGetResponseRaw(u32 chanID);
    u32 SIGetResponse(u32 chanID, u32);
    u32 SIGetStatus(u32 chanID);
    u32 SIGetType(u32 chanID);
    u32 SIGetTypeAsync(u32 chanID, u32);
    BOOL SIBusy();
    BOOL SIIsChanBusy(u32 chanID);
    void CompleteTransfer();
}
#endif

#endif