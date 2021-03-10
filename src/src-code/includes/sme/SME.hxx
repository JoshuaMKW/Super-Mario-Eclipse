#include "types.h"

#include "GlobalSMEDataStruct.hxx"
#include "CheatHandler.hxx"
#include "stage/FileUtils.hxx"
#include "funcs.hxx"

#include "libs/sMath.hxx"
#include "libs/sMemory.hxx"
#include "libs/sString.hxx"

#include "params/MarioParams.hxx"
#include "params/StageParams.hxx"

#pragma once

#include "OS.h"
#include "sms/SMS.hxx"

#ifndef SME_MAX_SHINES
#define SME_MAX_SHINES 120
#endif

#ifndef KURIBO_NO_TYPES
#define KURIBO_NO_TYPES
#endif

#define STRINGIZE(x) STRINGIZE_(x)
#define STRINGIZE_(x) #x

#define SME_FROM_GPR(reg, var) asm volatile ("mr %0, " #reg : "=r"(var))
#define SME_TO_GPR(reg, var)   asm volatile ("mr " #reg ", %0" : : "r"(var))
#define SME_FROM_FPR(reg, var) asm volatile ("fmr %0, " #reg : "=r"(var))
#define SME_TO_FPR(reg, var)   asm volatile ("fmr " #reg ", %0" : : "r"(var))


/* Misc compiler info */
#define SME_VERSION "v0.4a"

#ifdef __VERSION__
#define SME_CC_VERSION __VERSION__
#elif defined(__CWCC__)
#define SME_CC_VERSION "CWCC " STRINGIZE(__CWCC__)
#else
#define SME_CC_VERSION "Unknown"
#endif

#ifdef __cplusplus
#if __cplusplus >= 201103L
#define offsetof(t, d) __builtin_offsetof(t, d)
#else
#define offsetof(t, d) ((size_t)&(((t *)0)->d))
#endif
#define SME_EXTERN_C extern "C"
#else
#define SME_EXTERN_C extern
#endif

// init_mods.cpp
extern GlobalSMEDataStruct gInfo;
extern TCheatHandler gDebugModeHandler;

SME_EXTERN_C OSBootInfo BootInfo;
inline bool isGameEmulated() { return BootInfo.mConsoleType == OS_CONSOLE_DEV_KIT3; }