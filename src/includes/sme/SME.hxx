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
#include "funcs.hxx"
#include "stage/FileUtils.hxx"

#include "params/MarioParams.hxx"
#include "params/StageParams.hxx"

#include "OS.h"
#include "defines.h"
#include "macros.h"
#include "sms/JSystem/J2D/J2DOrthoGraph.hxx"
#include "sms/JSystem/JGeometry.hxx"
#include "sms/SMS.hxx"
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

#ifndef __ppc__
#define __ppc__
#endif

#define SME_MODULE_NAME "Eclipse"
#define SME_AUTHOR_NAME "JoshuaMK"
#define SME_VERSION_TAG __VERSION__ "[" SME_STRINGIZE(SME_MAX_SHINES) " Shines]"

// init_mods.cpp
SME_EXTERN_C OSBootInfo BootInfo;

namespace SME {

inline bool isGameEmulated() {
  return BootInfo.mConsoleType == OS_CONSOLE_DEV_KIT3;
}

namespace Util {

namespace SMS {

bool isExMap();
bool isMultiplayerMap();
bool isDivingMap();
bool isOptionMap();

} // namespace SMS

} // namespace Util

namespace Patch {

namespace Camera {

void modifyCameraRangeToSize(f32 *params, f32 *saveParams);

}

namespace Card {

s32 mountCard(TCardManager *cardManager, bool r4);
s32 probeCard(TCardManager *cardManager);

} // namespace Card

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
u16 masterGroundCollisionHandler(TBGCheckData *colTriangle);
u32 masterAllCollisionHandler(TMario *player);

} // namespace Collision

namespace Debug {

bool xyzModifierMario(TMario *player);

} // namespace Debug

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
bool canCleanSeals(TWaterManager *gpWaterManager);
TWaterGun *bindFluddtojoint();
void checkExecWaterGun(TWaterGun *fludd);
void killTriggerNozzle();
void spamHoverWrapper(TNozzleTrigger *nozzle, u32 r4, TWaterEmitInfo *emitInfo);
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
u32 *initFirstModel(char *path, u32 unk_1, u32 unk_2, u32 unk_3, JKRHeap *heap,
                    JKRDvdRipper::EAllocDirection direction, u32 unk_5,
                    u32 unk_6);
TMarDirector *initFileMods();
void initShineShadow();
void initSoundBank(u8 areaID, u8 episodeID);
void initMusicTrack();
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

u32 updateContexts(TMario *player);
u32 carryOrTalkNPC(TBaseNPC *npc);
bool canGrabAtNPC();
bool canCarryNPC();

} // namespace Mario

namespace Multiplayer {

void draw3DOverhaul(J3DDrawBuffer *drawBuffer);
TMario *makeMarios();
void loadMarioTrickyOverhaul(TMario *player, JSUMemoryInputStream *stream);
void performMarioTrickyOverhaul(TMario *player, JSUMemoryInputStream *stream);
void setMarioOverhaul(TMarDirector *director);

} // namespace Multiplayer

namespace Music {

u32 setIsValid(MSStageInfo musicID);
void initMusic();
void stopMusicOnStop();
void stopMusicOnStageExit(TMarioGamePad *gamepad);

} // namespace Music

namespace Shine {

void manageShineVanish(JGeometry::TVec3<f32> *marioPos);
void isKillEnemiesShine(TConductor *gpConductor,
                        JGeometry::TVec3<f32> *playerCoordinates, f32 range);
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
void initFreeEggCard(MActorAnmBck *bckData);
u32 checkFreeEggCard(MActorAnmBck *bckData);
void saveNozzles(TYoshi *yoshi);
void restoreNozzles(TMario *player);

} // namespace Yoshi

} // namespace Patch

} // namespace SME