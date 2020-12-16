#ifndef __KAMEK_BASE_RVL_SDK_H
#define __KAMEK_BASE_RVL_SDK_H

#ifdef __cplusplus
extern "C"
{
#endif
	/* OS Module */
	typedef u32 OSTick;
	typedef u64 OSTime;
	typedef s32 OSPriority;
	typedef void *OSAlarmHandler;

	OSTime OSGetTime();
	OSTick OSGetTick();
	#define OSTicksToSeconds(ticks) ((OSTime)(ticks) / *(OSTime *)0x800000F8)
	#define OSTicksToMilliseconds(ticks) (((OSTime)(ticks)*1000ull) / *(OSTime *)0x800000F8)
	#define OSTicksToMicroseconds(ticks) (((OSTime)(ticks)*1000000ull) / *(OSTime *)0x800000F8)
	#define OSTicksToNanoseconds(ticks) (((OSTime)(ticks)*32000ull) / (*(OSTime *)0x800000F8 / 31250ull))
	#define OSSecondsToTicks(sec) ((u32)(sec) * *(OSTime *)0x800000F8)
	#define OSMillisecondsToTicks(msec) (((u32)(msec) * *(OSTime *)0x800000F8) / 1000ull)
	#define OSMicrosecondsToTicks(usec) (((u32)(usec) * *(OSTime *)0x800000F8) / 1000000ull)
	#define OSNanosecondsToTicks(nsec) (((u32)(nsec) * (*(OSTime *)0x800000F8) / 31250ull) / 32000ull)

	u32 OSGetConsoleType();
	void *OSGetArenaHi();
	void *OSGetArenaLo();

	void OSSetArenaHi(void *ptr);
	void OSSetArenaLo(void *ptr);

	void OSProtectRange(u32 chan, void *addr, u32 size, u32 access);

	typedef struct
	{
		u64 tag;

		u32 mGPR[32];
		u32 mCR;
		u32 mLR;
		u32 mCTR;
		u32 mXER;
		u32 mSRR0;
		u32 mSRR1;
		u32 mDSISR;
		u32 mDAR;
		u8 _00[0xC];
		u32 mFPSCR;
		double mFPR[32];
		u16 mSpinLockCount;
		u16 mState;
		u32 mGQR[8];
		u8 _01[4];
		double mPSF[32];
		OSTime mCoretime[3];
		OSTime mStarttime;
		u32 mError;
		u8 _02[4];
		u32 mPMC1;
		u32 mPMC2;
		u32 mPMC3;
		u32 mPMC4;
		u32 mMMCR0;
		u32 mMMCR1;
	} OSContext;

	typedef struct
	{
		u8 _data[0x310];
	} OSThread;

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
	};
	typedef OSAlarm OSAlarm;

	typedef struct
	{
		char *mName;
		u32 _04;
		OSTime mTotal;
		s32 mHits;
		u32 _14;
		OSTime mMin;
		OSTime mMax;
		OSTime mMean;
		u32 mHasStarted; // BOOL
	} OSStopwatch;

	void OSInit();
	void OSReport(const char *format, ...);
	void OSFatal(u32 *fg, u32 *bg, const char *message);
	void OSExceptionInit();
	void OSExceptionVector(u32 unk);
	void OSDefaultExceptionHandler();
	void OSInitAlarm();
	void OSInitAlloc();
	void OSCreateAlarm(void *alarm);
	void OSSetAlarm(OSAlarm *alarm, OSTime tick, OSAlarmHandler handler);
	void OSSetPeriodicAlarm(OSAlarm *alarm, OSTime start, OSTime period, OSAlarmHandler handler);
	void OSCancelAlarm();

	bool OSCreateThread(OSThread *thread, void *func, void *param, void *stackBase, u32 stackSize, OSPriority priority, u16 attribute);
	s32 OSResumeThread(OSThread *thread);
	void OSYieldThread();
	bool OSIsThreadTerminated(OSThread *thread);

	void OSInitStopwatch(OSStopwatch *watch, char *name);
	void OSStartStopwatch(OSStopwatch *watch);
	OSTime OSCheckStopwatch(OSStopwatch *watch);
	void OSStopStopwatch(OSStopwatch *watch);
	void OSDumpStopwatch(OSStopwatch *watch);

	void *OSCreateHeap(void *start, void *end);
	void OSDestroyHeap(u32 index);
	u32 OSCheckHeap(u32 index);
	void *OSAllocFromHeap(u32 size, u32 align);
	void OSFreeToHeap(void *heap);
	void OSDumpHeap(void *heap);
	u32 OSReferentSize(void *unk);


	typedef struct
	{
		int sec, min, hour, mday, mon, year, wday, yday, msec, usec;
	} OSCalendarTime;
	void OSTicksToCalendarTime(u64 time, OSCalendarTime *result);

	/* MTX Module */
	typedef struct
	{
		f32 x, y;
	} Vec2;
	typedef struct
	{
		f32 x, y, z;
	} Vec;
	typedef struct
	{
		s16 x, y, z;
	} S16Vec;
	typedef f32 Mtx[3][4];
	typedef f32 Mtx44[4][4];
	typedef struct
	{
		f32 x, y, z, w;
	} Quaternion;

	void PSMTXIdentity(Mtx matrix);
	void PSMTXCopy(const Mtx source, Mtx dest);
	void PSMTXConcat(const Mtx sourceA, const Mtx sourceB, Mtx dest);
	void PSMTXConcatArray(const Mtx sourceA, const Mtx *sourcesB, Mtx *destsB, int count);
	u32 PSMTXInverse(const Mtx source, Mtx dest);
	u32 PSMTXInvXpose(const Mtx source, Mtx dest);
	void PSMTXRotRad(Mtx matrix, u8 axis, f32 radians);
	void PSMTXRotTrig(Mtx matrix, u8 axis, f32 sin, f32 cos);
	void PSMTXRotAxisRad(Mtx matrix, Vec *axis, f32 radians);
	void PSMTXTrans(Mtx matrix, f32 x, f32 y, f32 z);
	void PSMTXTransApply(const Mtx source, Mtx dest, f32 x, f32 y, f32 z);
	void PSMTXScale(Mtx matrix, f32 x, f32 y, f32 z);
	void PSMTXScaleApply(const Mtx source, Mtx dest, f32 x, f32 y, f32 z);
	void PSMTXQuat(Mtx dest, const Quaternion *quat);
	void C_MTXLookAt(Mtx dest, const Vec *cameraPos, const Vec *cameraUp, const Vec *target);
	void C_MTXLightFrustum(Mtx dest, f32 top, f32 bottom, f32 left, f32 right, f32 near, f32 scaleS, f32 scaleT, f32 transS, f32 transT);
	void C_MTXLightPerspective(Mtx dest, f32 fovy, f32 aspect, f32 scaleS, f32 scaleT, f32 transS, f32 transT);
	void C_MTXLightOrtho(Mtx dest, f32 top, f32 bottom, f32 left, f32 right, f32 scaleS, f32 scaleT, f32 transS, f32 transT);
	void PSMTXMultVec(const Mtx matrix, const Vec *source, Vec *dest);
	void C_MTXFrustum(Mtx44 dest, f32 top, f32 bottom, f32 left, f32 right, f32 near, f32 far);
	void C_MTXPerspective(Mtx44 dest, f32 fovy, f32 aspect, f32 near, f32 far);
	void C_MTXOrtho(Mtx44 dest, f32 top, f32 bottom, f32 left, f32 right, f32 near, f32 far);
	void PSVECAdd(const Vec *sourceA, const Vec *sourceB, Vec *dest);
	void PSVECSubtract(const Vec *sourceA, const Vec *sourceB, Vec *dest);
	void PSVECScale(const Vec *source, Vec *dest, f32 scale);
	void PSVECNormalize(const Vec *source, Vec *dest);
	f32 PSVECMag(const Vec *vec);
	f32 PSVECDotProduct(const Vec *sourceA, const Vec *sourceB);
	void PSVECCrossProduct(const Vec *sourceA, const Vec *sourceB, Vec *dest);
	void C_VECHalfAngle(const Vec *sourceA, const Vec *sourceB, Vec *dest);
	f32 PSVECSquareDistance(const Vec *sourceA, const Vec *sourceB);
	void C_QUATMtx(Quaternion *dest, const Mtx source);
	void C_QUATSlerp(const Quaternion *sourceA, const Quaternion *sourceB, Quaternion *dest, f32 value);

#define MTXIdentity PSMTXIdentity
#define MTXCopy PSMTXCopy
#define MTXConcat PSMTXConcat
#define MTXConcatArray PSMTXConcatArray
#define MTXInverse PSMTXInverse
#define MTXInvXpose PSMTXInvXpose
#define MTXRotRad PSMTXRotRad
#define MTXRotTrig PSMTXRotTrig
#define MTXRotAxisRad PSMTXRotAxisRad
#define MTXTrans PSMTXTrans
#define MTXTransApply PSMTXTransApply
#define MTXScale PSMTXScale
#define MTXScaleApply PSMTXScaleApply
#define MTXQuat PSMTXQuat
#define MTXLookAt C_MTXLookAt
#define MTXLightFrustum C_MTXLightFrustum
#define MTXLightPerspective C_MTXLightPerspective
#define MTXLightOrtho C_MTXLightOrtho
#define MTXMultVec PSMTXMultVec
#define MTXFrustum C_MTXFrustum
#define MTXPerspective C_MTXPerspective
#define MTXOrtho C_MTXOrtho
#define VECAdd PSVECAdd
#define VECSubtract PSVECSubtract
#define VECScale PSVECScale
#define VECNormalize PSVECNormalize
#define VECMag PSVECMag
#define VECDotProduct PSVECDotProduct
#define VECCrossProduct PSVECCrossProduct
#define VECHalfAngle C_VECHalfAngle
#define VECSquareDistance PSVECSquareDistance
#define QUATMtx C_QUATMtx
#define QUATSlerp C_QUATSlerp

	/* PPC */
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

	u32 DVDConvertPathToEntrynum(const char *pth);
	void *DVDOpen(const char *pth, void *handle);
	void DVDClose(void *handle);
	u32 DVDReadPrio(void *handle, void *dst, s32 length, s32 offset, s32 prio);

	// TODO: GX, CX, IOS ... and then of course NW4R

#ifdef __cplusplus
}
#endif

#endif
