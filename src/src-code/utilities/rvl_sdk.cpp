#include "../includes/eclipse.hxx"

void OSSetPeriodicAlarm(OSAlarm *alarm, OSTime start, OSTime period, OSAlarmHandler handler)
{
    alarm->mStart = start;
    alarm->mPeriod = period;
    alarm->mFire = start + alarm->mPeriod;
    alarm->mHandler = handler;
};

void OSDumpStopwatch(OSStopwatch *watch)
{
    OSReport("============================"
             "Stopwatch [%s]"
             "    Total: %llu us"
             "    Hits:  %u"
             "    Min:   %llu us"
             "    Max:   %llu us"
             "    Mean:  %llu us"
             "============================",
             watch->mName,
             watch->mTotal,
             watch->mHits,
             watch->mMin,
             watch->mMax,
             watch->mMean);
};