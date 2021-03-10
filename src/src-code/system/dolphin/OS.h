#ifndef OS_H
#define OS_H

#include "types.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define OS_BUS_CLOCK *(u32 *)0x800000F8
#define OS_CORE_CLOCK *(u32 *)0x800000FC
#define OS_TIMER_CLOCK OS_BUS_CLOCK / 4

#define OSDiffTick(tick1, tick0) ((tick1) - (tick0))
#define OSTicksToCycles(ticks) (((ticks) * ((OS_CORE_CLOCK * 2) / OS_TIMER_CLOCK)) / 2)
#define OSTicksToSeconds(ticks) ((ticks) / OS_TIMER_CLOCK)
#define OSTicksToMilliseconds(ticks) ((ticks) / (OS_TIMER_CLOCK / 1000u))
#define OSTicksToMicroseconds(ticks) (((ticks)*8u) / (OS_TIMER_CLOCK / 125000u))
#define OSTicksToNanoseconds(ticks) (((ticks)*8000u) / (OS_TIMER_CLOCK / 125000u))
#define OSSecondsToTicks(sec) ((sec)*OS_TIMER_CLOCK)
#define OSMillisecondsToTicks(msec) (((msec)*OS_TIMER_CLOCK) / 1000u)
#define OSMicrosecondsToTicks(usec) (((usec)*OS_TIMER_CLOCK) / 125000u)
#define OSNanosecondsToTicks(nsec) (((nsec) * (OS_TIMER_CLOCK) / 125000u) / 8000u)

#define OSRoundUp32B(x) (((u32)(x) + 32 - 1) & ~(32 - 1))
#define OSRoundDown32B(x) (((u32)(x)) & ~(32 - 1))

#define OS_CONSOLE_TYPE *(u32 *)0x8000002C

#define OS_CONSOLE_RETAIL1 0x00000001
#define OS_CONSOLE_HW2 0x00000002
#define OS_CONSOLE_PRODUCTION 0x00000003
#define OS_CONSOLE_RESERVED 0x00000004
#define OS_CONSOLE_DEV_MAC 0x10000000
#define OS_CONSOLE_DEV_PC 0x10000001
#define OS_CONSOLE_DEV_ARTHUR 0x10000002
#define OS_CONSOLE_DEV_MINNOW 0x10000003
#define OS_CONSOLE_DEV_KIT1 0x10000004
#define OS_CONSOLE_DEV_KIT2 0x10000005
#define OS_CONSOLE_DEV_KIT3 0x10000006
#define OS_CONSOLE_DEV_RESERVED 0x10000007
#define OS_CONSOLE_TDEV_HW2 0x10000005
#define OS_CONSOLE_TDEV_NEWEST 0x10000006
#define OS_CONSOLE_TDEV_RESERVED 0x20000007

#define OS_TVMODE_NTSC 0
#define OS_TVMODE_PAL 1
#define OS_TVMODE_DEBUG 2
#define OS_TVMODE_DEBUGPAL 3
#define OS_TVMODE_MPAL 4
#define OS_TVMODE_PAL60 5

    typedef u32 OSTick;
    typedef u64 OSTime;
    typedef s32 OSPriority; // 0 is highest priority, 31 is lowest
    typedef u32 OSHeapHandle;
    typedef void *OSMessage;

    typedef struct OSAlarm OSAlarm;
    typedef struct OSContext OSContext;
    typedef struct OSMutex OSMutex;
    typedef struct OSThread OSThread;

    typedef struct OSCalendarTime
    {
        u32 sec, min, hour, mday, mon, year, wday, yday, msec, usec;
    } OSCalendarTime;

    typedef void (*OSAlarmHandler)(OSAlarm *, OSContext *);

    typedef struct OSThreadQueue
    {
        OSThread *mThreadHead; // _0
        OSThread *mThreadTail; // _4
    } OSThreadQueue;

    typedef struct OSThreadLink
    {
        OSThread *mNextThread; // _0
        OSThread *mPrevThread; // _4
    } OSThreadLink;

    typedef struct OSMutexQueue
    {
        OSMutex *mMutexHead; // _0
        OSMutex *mMutexTail; // _4
    } OSMutexQueue;

    typedef struct OSMutexLink
    {
        OSMutex *mNextMutex; // _0
        OSMutex *mPrevMutex; // _4
    } OSMutexLink;

    typedef struct OSMessageQueue
    {
        OSThreadQueue mSentQueue;     // _0
        OSThreadQueue mRecievedQueue; // _8
        OSMessage *mMessageArray;     // _10
        s32 mMessageCount;            // _14
        s32 mFirstIndex;              // _18
        s32 mUsedCount;               // _1C
    } OSMessageQueue;

    typedef struct OSContext
    {
        u32 mGPR[32];   // general purpose registers r0-r31
        u32 mCR;        // control register
        u32 mLR;        // link register
        u32 mCTR;       // counter register
        u32 mXER;       // condition indicator register
        f64 mFPR[32];   // f32ing point registers f0-f31
        u32 mFPSCR_Pad; // f32ing point status -- control
        u32 mFPSCR;     // f32ing point status -- control
        u32 mSRR0;      // save/store register 0
        u32 mSRR1;      // save/store register 1
        u16 mMode;
        u16 mState;
        u32 mGQR[8];
        f64 mPSF[32];
    } OSContext;

    typedef struct OSThread
    {
        OSContext mRegisterContext;
        u16 mThreadState;
        u16 mThreadAttribute;
        s32 mWillSuspend;
        OSPriority mPriority;
        OSPriority mBaseScheduler;
        void *mExitVal;
        OSThreadQueue *mThreadQueue;
        OSThreadLink mQueueLink;
        OSThreadQueue mQueueJoin;
        OSMutex *mMutex;
        OSMutexQueue mOwnedMutex;
        OSThreadLink mLinkActive;
        u8 *mStackBase;
        u32 *mStackEnd;
    } OSThread;

    typedef struct OSMutex
    {
        OSThreadQueue mQueue; // 0
        OSThread *mThread;    // _8
        s32 mLockCount;       // _C
        OSMutexLink mLink;    // _10
    } OSMutex;

    typedef struct OSCond
    {
        OSThreadQueue mThreadQueue;
    } OSCond;

    typedef struct OSAlarm
    {
        OSAlarmHandler mHandler;
        u32 mTag;
        OSTime mFire;
        OSAlarm *mPrev;
        OSAlarm *mNext;
        OSTime mPeriod;
        OSTime mStart;
        void *mQueue;
    } OSAlarm;

    typedef struct OSStopwatch
    {
        char *mName;
        OSTime mTotal;
        s32 mHits;
        OSTime mMin;
        OSTime mMax;
        OSTime mLast;
        BOOL mActive;
    } OSStopwatch;

    typedef struct OSBootInfo
    {
        char mGameCode[4];
        char mMakerCode[2];
        u8 mDiskID;
        u8 mDiskVersion;
        u8 mStreamingEnabled;
        u8 mStreamBufSize;
        u8 _0A[0xF];
        u32 mDVDMagic;
        u32 mBootMagic;
        u32 mVersion;
        u32 mPhysicalMemSize;
        u32 mConsoleType;
        void *mArenaLo;
        void *mArenaHi;
        void *mFSTTable;
        u32 mFSTMaxLength;
        u32 mDebuggerPresent;
        u32 mDebuggerMask;
        u32 mExceptionHook;
        u32 _mTempLR;
    } OSBootInfo;

    void OSInit();
    void OSExceptionInit();

    OSTime OSGetTime();
    OSTick OSGetTick();
    OSTime __OSGetSystemTime();
    void OSTicksToCalendarTime(OSTime time, OSCalendarTime *result);

    void OSReport(const char *msg, ...);
    void OSPanic(u8 *, u32, u8 *, ...);
    void OSFatal(u32 *, u32 *, const u8 *);
    void OSResetSystem(u32, u32, BOOL);
    u32 OSGetResetCode();
    u32 OSGetConsoleType();

    BOOL OSDisableInterrupts();
    void OSEnableInterrupts();
    void OSRestoreInterrupts(BOOL enable);
    void OSExceptionInit();
    void OSExceptionVector(u32 unk);
    void OSDefaultExceptionHandler();

    void OSInitAlarm();
    void OSCreateAlarm(void *alarm);
    void OSSetAlarm(OSAlarm *alarm, OSTime tick, OSAlarmHandler handler);
    void OSSetPeriodicAlarm(OSAlarm *alarm, OSTime start, OSTime period, OSAlarmHandler handler);
    void InsertAlarm(OSAlarm *alarm, OSTime time, OSAlarmHandler handler);
    void OSCancelAlarm(OSAlarm *alarm);

    void OSYieldThread();
    BOOL OSCreateThread(OSThread *thread, void *(*funcToThread)(void *), void *parameter, void *stack, u32 stackSize, OSPriority priority, u16 attributes);
    void OSExitThread(void *);
    void OSCancelThread(OSThread *thread);
    BOOL OSJoinThread(OSThread *thread, void **);
    void OSDetachThread(OSThread *thread);
    s32 OSResumeThread(OSThread *thread);
    s32 OSSuspendThread(OSThread *thread);
    void OSSleepThread(OSThreadQueue *queue);
    void OSWakeupThread(OSThreadQueue *queue);
    OSPriority OSGetThreadPriority(OSThread *thread);

    void OSInitStopwatch(OSStopwatch *watch, const char *name);
    void OSStartStopwatch(OSStopwatch *watch);
    OSTime OSCheckStopwatch(OSStopwatch *watch);
    void OSStopStopwatch(OSStopwatch *watch);
    void OSDumpStopwatch(OSStopwatch *watch);

    void OSInitAlloc();
    void *OSGetArenaHi();
    void *OSGetArenaLo();
    void OSSetArenaHi(void *ptr);
    void OSSetArenaLo(void *ptr);
    void OSProtectRange(u32 chan, void *addr, size_t size, u32 access);

    void *OSCreateHeap(void *start, void *end);
    void OSDestroyHeap(u32 index);
    u32 OSCheckHeap(u32 index);
    void *OSAllocFromHeap(size_t size, u32 align);
    void OSFreeToHeap(void *heap);
    void OSDumpHeap(void *heap);
    u32 OSReferentSize(void *unk);

    void DCEnable();
    void DCDisable();
    void DCInvalidateRange(void *ptr, size_t size);
    void DCFlushRange(void *ptr, size_t size);
    void DCStoreRange(void *ptr, size_t size);
    void DCFlushRangeNoSync(void *ptr, size_t size);
    void DCZeroRange(void *ptr, size_t size);

    void ICEnable();
    void ICDisable();
    void ICInvalidateRange(void *ptr, size_t size);
    void ICFlashInvalidate();

    void LCEnable();
    void LCDisable();
    void LCStoreBlocks(void *, void *, u32);
    void LCStoreData(void *, void *, u32);
    void LCQueueWait(u32 len);

    void L2GlobalInvalidate();

    void OSInitMutex(OSMutex *mutex);
    void OSLockMutex(OSMutex *mutex);
    void OSUnlockMutex(OSMutex *mutex);
    BOOL OSTryLockMutex(OSMutex *mutex); // false if failed
    void OSInitCond(OSCond *condition);
    void OSWaitCond(OSCond *condition, OSMutex *mutex);
    void OSSignalCond(OSCond *condition);

#ifdef __cplusplus
}
#endif

#endif