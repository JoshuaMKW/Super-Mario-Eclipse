
#include "GlobalSMEDataStruct.hxx"
#include "CheatHandler.hxx"
#include "stage/FileUtils.hxx"
#include "funcs.hxx"

#include "libs/sAssert.hxx"
#include "libs/sLogging.hxx"
#include "libs/sMath.hxx"
#include "libs/sMemory.hxx"
#include "libs/sString.hxx"

#include "params/MarioParams.hxx"
#include "params/StageParams.hxx"

#pragma once

#include "types.h"
#include "OS.h"
#include "sms/SMS.hxx"
#include "sms/JSystem/JGeometry.hxx"
#include "sms/game/Conductor.hxx"
#include "sms/sound/MSBGM.hxx"

#ifndef SME_MAX_SHINES
#define SME_MAX_SHINES 120
#else
#if SME_MAX_SHINES < 0
#undef SME_MAX_SHINES
#define SME_MAX_SHINES 0
#elif SME_MAX_SHINES > 999
#undef SME_MAX_SHINES
#define SME_MAX_SHINES 999
#endif
#endif

#ifndef KURIBO_NO_TYPES
#define KURIBO_NO_TYPES
#endif

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

namespace SME
{

inline bool isGameEmulated() { return BootInfo.mConsoleType == OS_CONSOLE_DEV_KIT3; }

namespace Util
{
}

namespace Patch
{

namespace Init
{
    //fill out
}

namespace Music
{
u32 setIsValid(MSStageInfo musicID);
void initMusic();
void stopMusicOnStop();
void stopMusicOnStageExit(TMarioGamePad *gamepad);
}

namespace Shine
{
void manageShineVanish(JGeometry::TVec3<f32> *marioPos);
void isKillEnemiesShine(TConductor *gpConductor, JGeometry::TVec3<f32> *playerCoordinates, f32 range);
static void restoreMario(TMarDirector *gpMarDirector, u32 curState);
void checkBootOut(TMarDirector *gpMarDirector, u32 curState);
u32 extendShineIDLogic(TFlagManager *flagManager, u32 flagID);
void shineObjectStringMod(JSUInputStream *stream, u8 *dst, u32 size);
void shineFlagSetter(TFlagManager *flagManager, u32 flag, s32 val);
u32 shineFlagGetter(TFlagManager *flagManager, u32 flag);
u32 shineSetClamper(TFlagManager *flagManager, u32 flag);
u32 shineGetClamper(TFlagManager *flagManager, u32 flag);
void extendFlagManagerLoad(JSUMemoryInputStream &stream);
void extendFlagManagerSave(JSUMemoryOutputStream &stream);
void thinkSetBootFlag(void *shineFader, u32 unk_1, u32 unk_2);
u32 loadAfterMaskState();
void setKillState();
}

namespace Stage
{
void setStageOnExit(TGameSequence *gpSequence, s8 stage, s8 episode);
void startEpisodeSelect(void *selectMenu);
}

namespace Yoshi
{
bool isYoshiDie(TMario *player) { return !player->mYoshi->isGreenYoshi(); }
bool isYoshiEggNeedFruit(THitActor *gpFruit);
u8 isYoshiEggFree(TEggYoshi *gpEgg, THitActor *gpFruit);
bool isYoshiMaintainFluddModel();
void maybeYoshiDrown(TYoshi *yoshi);
bool canMountYoshi(u32 state);
f32 getYoshiYPos_(TYoshi *yoshi);
void fixYoshiJuiceDecrement();
void canYoshiSpray(TWaterGun *gpWaterGun);
u32 calcYoshiSwimVelocity(TMario *player, u32 arg1);
u32 isYoshiWaterFlutter();
u32 isYoshiValidWaterFlutter(s32 anmIdx, u32 unk1, TMario *player);
bool isYoshiValidDrip(TYoshi *yoshi);
void initFreeEggCard(MActorAnmBck *bckData);
u32 checkFreeEggCard(MActorAnmBck *bckData);
void saveNozzles(TYoshi *yoshi);
void restoreNozzles(TMario *player);
}
}
}