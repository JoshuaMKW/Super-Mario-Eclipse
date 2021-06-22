#include "OS.h"
#include "math.h"

#include "macros.h"

OSBootInfo BootInfo;
u32 BI2DebugFlag;
u32 BI2DebugFlagHolder;
bool AreWeInitialized;
void *OSExceptionTable;

void OSSetPeriodicAlarm(OSAlarm *alarm, OSTime start, OSTime period,
                        OSAlarmHandler handler) {
  const u32 iStatus = OSDisableInterrupts();

  alarm->mStart = start;
  alarm->mPeriod = period;

  InsertAlarm(alarm, start - OSGetTime(), handler);

  OSRestoreInterrupts(iStatus);
}

void OSDumpStopwatch(OSStopwatch *watch) {
  OSReport("============================\n"
           "Stopwatch [%s]\n"
           "    Total: %llu us\n"
           "    Hits:  %u\n"
           "    Min:   %llu us\n"
           "    Max:   %llu us\n"
           "    Mean:  %llu us\n"
           "============================\n",
           watch->mName, watch->mTotal, watch->mHits, watch->mMin, watch->mMax,
           watch->mTotal / watch->mHits);
}

SME_PURE_ASM void DCDisable() {
  __asm__ volatile("sync                           \n\t"
                   "mfspr      3, 1008             \n\t"
                   "rlwinm     3, 3, 0, 18, 16     \n\t"
                   "mtspr      1008, 3             \n\t"
                   "blr                            \n\t");
}

SME_PURE_ASM void ICDisable() {
  __asm__ volatile("sync                           \n\t"
                   "mfspr      3, 1008             \n\t"
                   "rlwinm     3, 3, 0, 17, 15     \n\t"
                   "mtspr      1008, 3             \n\t"
                   "blr                            \n\t");
}