#pragma once

#include "libs/sAssert.hxx"
#include "libs/sBmg.hxx"
#include "libs/sLogging.hxx"
#include "libs/sMath.hxx"
#include "libs/sMemory.hxx"
#include "libs/sMusic.hxx"
#include "libs/sSecurity.hxx"
#include "libs/sString.hxx"
#include "libs/sTime.hxx"

#include "CheatHandler.hxx"
#include "Globals.hxx"
#include "Player.hxx"
#include "KeyCode.hxx"
#include "funcs.hxx"
#include "stage/FileUtils.hxx"

#include "params/MarioParams.hxx"
#include "params/StageParams.hxx"

#include "J2D/J2DOrthoGraph.hxx"
#include "JGeometry.hxx"
#include "MTX.h"
#include "OS.h"
#include "defines.h"
#include "macros.h"
#include "sms/SMS.hxx"
#include "sms/enemy/EnemyMario.hxx"
#include "sms/game/Conductor.hxx"
#include "sms/npc/BaseNPC.hxx"
#include "sms/object/ResetFruit.hxx"
#include "sms/screen/ShineFader.hxx"
#include "sms/sound/MSBGM.hxx"
#include "sms/talk/Talk2D2.hxx"
#include "types.h"

#ifndef KURIBO_NO_TYPES
#define KURIBO_NO_TYPES
#endif

#ifndef HW_DOL
#define HW_DOL
#endif

#include "common_sdk.h"

#define SME_MODULE_NAME "Eclipse"
#define SME_AUTHOR_NAME "JoshuaMK"

#if defined(SME_DEBUG) && !defined(SME_RELEASE)
#define SME_VERSION_TAG                                                        \
  "(DEBUG) " SME_VERSION "[" SME_STRINGIZE(SME_MAX_SHINES) " Shines]"
#else
#define SME_VERSION_TAG                                                        \
  "(RELEASE) " SME_VERSION "[" SME_STRINGIZE(SME_MAX_SHINES) " Shines]"
#endif

#if defined(SME_BUILD_KURIBO)
#define SME_PATCH_B(source, target) pp::PatchB(source, target)
#define SME_PATCH_BL(source, target) pp::PatchBL(source, target)
#define SME_WRITE_8(source, value) pp::Patch8(source, value)
#define SME_WRITE_16(source, value) pp::Patch16(source, value)
#define SME_WRITE_32(source, value) pp::Patch32(source, value)
#elif defined(SME_BUILD_KAMEK) || defined(SME_BUILD_KAMEK_INLINE)
#define SME_PATCH_B(source, target) kmBranch(source, target)
#define SME_PATCH_BL(source, target) kmCall(source, target)
#define SME_WRITE_8(source, value) kmWrite8(source, value)
#define SME_WRITE_16(source, value) kmWrite16(source, value)
#define SME_WRITE_32(source, value) kmWrite32(source, value)
#else
#error                                                                         \
    "Build type unspecified. Define either SME_BUILD_KAMEK or SME_BUILD_KAMEK_INLINE or SME_BUILD_KURIBO"
#endif

#ifndef __ppc__
#define __ppc__
#endif

/* CONFIGURATION DEFINES */


#ifndef SME_BUGFIXES
#define SME_BUGFIXES 1
#endif

#ifndef SME_CRASHFIXES
#define SME_CRASHFIXES 1
#endif

#ifndef SME_EXCEPTION_HANDLER
#define SME_EXCEPTION_HANDLER 1
#endif

#ifndef SME_PROTECT_MEM
#define SME_PROTECT_MEM 0
#endif

#ifndef SME_SLOT_B_SUPPORT
#define SME_SLOT_B_SUPPORT 1
#endif

#ifndef SME_EXTENDED_RENDER_DISTANCE
#define SME_EXTENDED_RENDER_DISTANCE 1
#endif

#ifndef SME_DEMO
#define SME_DEMO 1
#endif

#ifndef SME_MIRROR_SUNSHINE
#define SME_MIRROR_SUNSHINE 0
#endif

#ifndef SME_MULTIPLAYER
#define SME_MULTIPLAYER 0
#endif

#ifndef SME_WIDESCREEN
#define SME_WIDESCREEN 1
#endif

#ifndef SME_VARIABLE_FPS
#define SME_VARIABLE_FPS 0
#endif

#ifndef SME_CUSTOM_MUSIC
#define SME_CUSTOM_MUSIC 0
#endif
 
#ifndef SME_EXTRA_SHINES
#define SME_EXTRA_SHINES 1
#endif

#ifndef SME_EXTRA_OBJECTS
#define SME_EXTRA_OBJECTS 1
#endif

#ifndef SME_EXTRA_COLLISION
#define SME_EXTRA_COLLISION 1
#endif

#ifndef SME_VANILLA_MOVESET
#define SME_VANILLA_MOVESET 0
#endif

#if !SME_VANILLA_MOVESET

#ifndef SME_LONG_JUMP
#define SME_LONG_JUMP 1
#endif

#ifndef SME_MULTI_JUMP
#define SME_MULTI_JUMP 1
#endif

#ifndef SME_HOVER_BURST
#define SME_HOVER_BURST 1
#endif

#ifndef SME_HOVER_SLIDE
#define SME_HOVER_SLIDE 1
#endif

#ifndef SME_ROCKET_DIVE
#define SME_ROCKET_DIVE 1
#endif

#ifndef SME_WATERBALLOONS
#define SME_WATERBALLOONS 1
#endif

#ifndef SME_ZHINE_BOSS
#define SME_ZHINE_BOSS 1
#endif

#else

#define SME_LONG_JUMP 0
#define SME_MULTI_JUMP 0
#define SME_HOVER_BURST 0
#define SME_HOVER_SLIDE 0
#define SME_ROCKET_DIVE 0

#endif

#ifndef SME_DYNAMIC_FALL_DAMAGE
#define SME_DYNAMIC_FALL_DAMAGE 1
#endif

#ifndef SME_NO_DOWNWARP
#define SME_NO_DOWNWARP 1
#endif

#ifndef SME_GRAB_NPCS
#define SME_GRAB_NPCS 1
#endif

#ifndef SME_SHADOW_MARIO_HEALTHBAR
#define SME_SHADOW_MARIO_HEALTHBAR 1
#endif

#ifndef SME_FLUDD_GOOP_MOD
#define SME_FLUDD_GOOP_MOD 1
#endif

#ifndef SME_GREEN_YOSHI
#define SME_GREEN_YOSHI 1
#endif

#ifndef SME_YOSHI_SAVE_NOZZLES
#define SME_YOSHI_SAVE_NOZZLES 1
#endif

#ifndef SME_YOSHI_EGG_FREE
#define SME_YOSHI_EGG_FREE 1
#endif

#ifndef SME_UNDERWATER_FRUIT
#define SME_UNDERWATER_FRUIT 1
#endif

#ifndef SME_USE_PS_MATH
#define SME_USE_PS_MATH 1
#endif

// init_mods.cpp
SME_EXTERN_C OSBootInfo BootInfo;

namespace SME {

#ifdef SME_IGNORE_CONSOLE
constexpr bool isGameEmulated() {
  return true;
}
#else
inline bool isGameEmulated() {
  return BootInfo.mConsoleType == OS_CONSOLE_DEV_KIT3;
}
#endif

namespace Util {

namespace SMS {

bool isExMap();
bool isMultiplayerMap();
bool isDivingMap();
bool isOptionMap();

} // namespace SMS

namespace Yoshi {

bool isMounted(TYoshi *yoshi);
bool isMounted(TMario *mario);
bool isGreenYoshi(TYoshi *yoshi);
bool isGreenYoshi(TMario *mario);
bool isGreenYoshiMounted(TYoshi *yoshi);
bool isGreenYoshiMounted(TMario *mario);
bool isGreenYoshiAscendingWater(TMario *mario);

} // namespace Yoshi

} // namespace Util

namespace Patch {

namespace Camera {

void modifyCameraRangeToSize(f32 *params, f32 *saveParams);

}

namespace Cheat {

void drawCheatText();
void *handleDebugCheat(void *GCLogoDir);

} // namespace Cheat

namespace CKit {

void onSetup(TMarDirector *director);
s32 onUpdate(void *director);
void onDraw2D(J2DOrthoGraph *graph);
bool manageLightSize();
void formatTalkMessage(Talk2D2 *talker, char *msgfield, u32 *entrydata);
void realTimeCustomAttrsHandler(TMario *player);

} // namespace CKit

namespace Collision {

void checkIsGlideBounce(TMario *player);
u16 checkIsRestoreTypeNoFallDamage(TBGCheckData *floor);
u32 updateCollisionContext(TMario *player);

} // namespace Collision

namespace Debug {

void xyzModifierMario(TMario *player);
void updateDebugCollision(TMario *player);

} // namespace Debug

namespace Fixes {

void shadowCrashPatch();
u32 patchYStorage();

} // namespace Fixes

namespace Flag {

TCardBookmarkInfo *setFileCompleteBool(TCardManager *cardManager);
void resetStage(TFlagManager *flagManager);

} // namespace Flag

namespace Fludd {

bool isPumpOK(TMarioAnimeData *animeData);
bool hasWaterCardOpen();
bool canCollectFluddItem(TMario *player);
void sprayGoopMap(TPollutionManager *gpPollutionManager, f32 x, f32 y, f32 z,
                  f32 r);
bool canCleanSeals(TModelWaterManager *gpWaterManager);
TWaterGun *bindFluddtojoint();
void checkExecWaterGun(TWaterGun *fludd);
void killTriggerNozzle();
bool checkAirNozzle();

} // namespace Fludd

namespace Fruit {

bool canFruitDieWater(TResetFruit *fruit);
f32 chooseGrabDistancing(M3UModelMario *model);
bool isGrabWaitOver(TMario *player);

} // namespace Fruit

namespace Init {

void initCodeProtection();
JKRExpHeap *createGlobalHeaps(void *newHeap, size_t size, JKRHeap *rootHeap,
                              bool unk_1);
s32 setupMarioDatas(char *filepath);
u32 *initFirstModel(const char *path, u32 unk_1, u32 unk_2, u32 unk_3, JKRHeap *heap,
                    JKRDvdRipper::EAllocDirection direction, u32 unk_5,
                    u32 unk_6);
TMarDirector *initFileMods();
void initShineShadow();
void initSoundBank(u8 areaID, u8 episodeID);
TMario *fromMarioInit(TMario *player);
bool fromShadowMarioInit();
void initYoshi(MAnmSound *anmSound, void *r4, u32 r5, f32 f1);
void initCardColors();
u32 initCollisionWarpLinks(const char *name);
void createUIHeap(u32 size, s32 alignment);
u32 initHUDElements(char *filepath);
JKRMemArchive *switchHUDOnStageLoad(char *curArchive, u32 *gameUI);
JKRHeap *useCustomHUDHeap(u32 size, s32 alignment);

} // namespace Init

namespace Mario {

void addVelocity(TMario *player, f32 velocity);
u32 updateContexts(TMario *player);
u32 carryOrTalkNPC(TBaseNPC *npc);
bool canGrabAtNPC();
bool canCarryNPC();
TMario *scaleNPCThrowLength(TMario *player, float *params);
u32 scaleNPCThrowHeight(u32 _r3, f32 z, f32 y);
void scaleNPCTalkRadius();
f32 getTreeClimbMinFall();
void getTreeClimbMaxFall();
bool scaleTreeSlideSpeed(f32 _f1, f32 _f2);
void getClimbingAnimSpd(TMario *player, TMario::Animation anim, f32 speed);
void scaleHangSpeed(TMario *player);
void checkGraffitiAffected(TMario *player);
void rescaleHeldObj(Mtx holderMatrix, Mtx destMatrix);
void checkYSpdForTerminalVelocity();
void normJumpMultiplier();
f32 checkGroundSpeedLimit();
void checkJumpSpeedLimit(f32 speed);
TMario *checkJumpSpeedMulti(TMario *player, f32 factor, f32 max);
void manageEMarioHealthWrapper(TEnemyMario *eMario, Mtx *posMtx);

} // namespace Mario

namespace Multiplayer {

void draw3DOverhaul(J3DDrawBuffer *drawBuffer);
TMario *makeMarios();
void loadMarioTrickyOverhaul(TMario *player, JSUMemoryInputStream *stream);
void performMarioTrickyOverhaul(TMario *player, JSUMemoryInputStream *stream);
void setMarioOverhaul(TMarDirector *director);

} // namespace Multiplayer

namespace Shine {

void manageShineVanish(TVec3f *marioPos);
void isKillEnemiesShine(TConductor *gpConductor,
                        TVec3f *playerCoordinates, f32 range);
void checkBootOut(TMarDirector *gpMarDirector, u8 curState);
u32 extendShineIDLogic(TFlagManager *flagManager, u32 flagID);
void shineObjectStringMod(JSUInputStream *stream, u8 *dst, u32 size);
void shineFlagSetter(TFlagManager *flagManager, u32 flag, s32 val);
u32 shineFlagGetter(TFlagManager *flagManager, u32 flag);
u32 shineSetClamper(TFlagManager *flagManager, u32 flag);
u32 shineGetClamper(TFlagManager *flagManager, u32 flag);
void extendFlagManagerLoad(JSUMemoryInputStream &stream);
void extendFlagManagerSave(JSUMemoryOutputStream &stream);
void thinkSetBootFlag(TShineFader *shineFader, u32 unk_1, u32 unk_2);
u32 loadAfterMaskState();
void setKillState();
void thinkCloseCamera();
void animationFreezeCheck();

} // namespace Shine

namespace Stage {

void setStageOnExit(TGameSequence *gpSequence, s8 stage, s8 episode);
void startEpisodeSelect(void *selectMenu);

} // namespace Stage

namespace Spc {

void initCustomFunctions(TSpcBinary *spcBinary, const char *symbol,
                         u32 address);

}

namespace Yoshi {

bool isYoshiDie(TMario *player);
bool isYoshiEggNeedFruit(THitActor *gpFruit);
u8 isYoshiEggFree(TEggYoshi *gpEgg, THitActor *gpFruit);
bool isYoshiMaintainFluddModel();
void maybeYoshiDrown(TYoshi *yoshi);
bool canMountYoshi();
f32 getYoshiYPos_(TYoshi *yoshi);
void fixYoshiJuiceDecrement();
void canYoshiSpray(TWaterGun *gpWaterGun);
u32 calcYoshiSwimVelocity(TMario *player, u32 arg1);
u32 isYoshiWaterFlutter();
u32 isYoshiValidWaterFlutter(s32 anmIdx, u32 unk1, TMario *player);
bool isYoshiValidDrip(TYoshi *yoshi);
void initFreeEggCard(J3DFrameCtrl *frameCtrl);
u32 checkFreeEggCard(J3DFrameCtrl *frameCtrl);
void saveNozzles(TYoshi *yoshi);
void restoreNozzles(TMario *player);

} // namespace Yoshi

} // namespace Patch

} // namespace SME