#include <OS.h>

void OSSetPeriodicAlarm(OSAlarm *alarm, OSTime start, OSTime period, OSAlarmHandler handler)
{
    u32 iStatus = OSDisableInterrupts();

    alarm->mStart = start;
    alarm->mPeriod = period;

    InsertAlarm(alarm, OSGetTime() - start, handler);

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