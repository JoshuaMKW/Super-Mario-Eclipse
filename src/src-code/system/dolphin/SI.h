#ifndef SI_H
#define SI_H

#include "types.h"

#ifdef __cplusplus
extern "C"
{
    bool SIBusy();
    bool SIIsChanBusy(u32 chanID);
    void SIInterruptHandler(u32, u32);
    bool SIEnablePollingInterrupt(u32);
    bool SIRegisterPollingHandler(u32);
    bool SIUnregisterPollingHandler(u32);
    u32 SIInit();
    u32 SIGetStatus(u32);
    void SISetCommand(u32 cmd);
    void SITransferCommands();
    u32 SISetXY(u16, u8);
    u32 SIEnablePolling(u32);
    u32 SIDisablePolling(u32);
    bool SIGetResponseRaw(u32);
    u32 SIGetResponse(u32, u32);

    void CompleteTransfer();
}
#endif

#endif