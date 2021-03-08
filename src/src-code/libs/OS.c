#include "OS.h"
#include "math.h"

OSBootInfo BootInfo;
u32 BI2DebugFlag;
u32 BI2DebugFlagHolder;
u32 AreWeInitialized;
void *OSExceptionTable;

void OSSetPeriodicAlarm(OSAlarm *alarm, OSTime start, OSTime period, OSAlarmHandler handler)
{
    u32 iStatus = OSDisableInterrupts();

    alarm->mStart = start;
    alarm->mPeriod = period;

    InsertAlarm(alarm, start - OSGetTime(), handler);

    OSRestoreInterrupts(iStatus);
};

void OSDumpStopwatch(OSStopwatch *watch)
{
    OSReport("============================\n"
             "Stopwatch [%s]\n"
             "    Total: %llu us\n"
             "    Hits:  %u\n"
             "    Min:   %llu us\n"
             "    Max:   %llu us\n"
             "    Mean:  %llu us\n"
             "============================\n",
             watch->mName,
             watch->mTotal,
             watch->mHits,
             watch->mMin,
             watch->mMax,
             watch->mTotal / watch->mHits);
};