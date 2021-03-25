#include "OS.h"
#include "SME.hxx"

#include "kuribo_sdk.h"

// c_kit.cpp
#include "sms/talk/Talk2D2.hxx"
#include "sms/actor/Mario.hxx"
extern void onSetup();
extern s32 onUpdate();
extern void onDraw2D();
extern bool SMS_IsExMap();
extern bool SMS_IsMultiplayerMap();
extern bool SMS_IsDivingMap();
extern bool SMS_IsOptionMap();
extern bool manageLightSize();
extern void formatTalkMessage(Talk2D2 *, char *, u32 *);
extern void realTimeCustomAttrsHandler(TMario *);

// camera.cpp
extern void modifyCameraRangeToSize(f32 *params, f32 *saveParams);

// CheatCode.cpp
extern void *handleDebugCheat(void *GCLogoDir);

// collision.cpp
extern void checkIsGlideBounce(TMario *player);
extern u16 checkIsRestoreTypeNoFallDamage(TBGCheckData *floor);
extern u32 updateCollisionContext(TMario *player);
extern u16 masterGroundCollisionHandler(TBGCheckData *colTriangle);
extern u32 masterAllCollisionHandler(TMario *player);

// file_flags.cpp
extern s32 setFileCompleteBool(TCardManager *gpCardManager);

// flagmanager.cpp
extern void resetStage(TFlagManager *flagManager);

// fludd.cpp
extern bool isPumpOK(TMarioAnimeData *animeData);
extern bool hasWaterCardOpen();
extern bool canCollectFluddItem(TMario *player);
extern void sprayGoopMapWrapGlobalMar(TPollutionManager *gpPollutionManager,
                                      f32 x, f32 y, f32 z, f32 r);
extern void sprayGoopMapWrapMar30(TPollutionManager *gpPollutionManager,
                                  f32 x, f32 y, f32 z, f32 r);
extern bool canCleanSeals(TWaterManager *gpWaterManager);
extern TWaterGun *bindFluddtojoint();
extern void checkExecWaterGun(TWaterGun *fludd);
extern void killTriggerNozzle();
extern void spamHoverWrapper(TNozzleTrigger *nozzle, u32 r4, TWaterEmitInfo *emitInfo);
extern bool checkAirNozzle();

// init_mods.cpp
extern void initCodeProtection();
extern u32 *createGlobalHeaps(u32 *newHeap, u32 size, u32 *rootHeap, u32 unk_1);
extern u32 setupMarioDatas(char *filepath);
extern u32 *initFirstModel(char *path, u32 unk_1, u32 unk_2, u32 unk_3, JKRHeap *heap, u32 unk_4, u32 unk_5, u32 unk_6);
extern u32 *initFileMods();
extern void initShineShadow();
extern void initSoundBank(u8 areaID, u8 episodeID);
extern void initMusicTrack();
extern TMario *fromMarioInit(TMario *player);
extern bool fromShadowMarioInit();
extern void initYoshi(void *anmSound, void *r4, u32 r5, f32 f1);
extern void initCardColors();
extern u32 initCollisionWarpLinks(char *name, u32 *dest);
extern void createUIHeap(u32 size, s32 alignment);

// mario.cpp
#include "sms/npc/BaseNPC.hxx"
extern "C" s16 shadowCrashPatch();
extern u32 updateContexts(TMario *player);
extern u32 carryOrTalkNPC(TBaseNPC *npc);
extern bool canGrabAtNPC();
extern bool canCarryNPC();

// shine.cpp
#include "sms/game/Conductor.hxx"
extern void manageShineVanish(JGeometry::TVec3<f32> *marioPos);
extern void isKillEnemiesShine(TConductor *gpConductor, JGeometry::TVec3<f32> *playerCoordinates, f32 range);
extern void restoreMario(TMarDirector *gpMarDirector, u32 curState);
extern void checkBootOut(TMarDirector *gpMarDirector, u32 curState);
extern u32 extendShineIDLogic(TFlagManager *flagManager, u32 flagID);
extern void shineObjectStringMod(JSUInputStream *stream, u8 *dst, u32 size);
extern void shineFlagSetter(TFlagManager *flagManager, u32 flag, s32 val);
extern u32 shineFlagGetter(TFlagManager *flagManager, u32 flag);
extern u32 shineSetClamper(TFlagManager *flagManager, u32 flag);
extern u32 shineGetClamper(TFlagManager *flagManager, u32 flag);
extern void extendFlagManagerLoad(JSUInputStream &stream);
extern void extendFlagManagerSave(JSUOutputStream &stream);
extern void thinkSetBootFlag(void *shineFader, u32 unk_1, u32 unk_2);
extern u32 loadAfterMaskState();
extern void setKillState();
extern "C" void thinkCloseCamera();
extern "C" void animationFreezeCheck();

// stage.cpp
extern void setStageOnExit(TGameSequence *gpSequence, s8 stage, s8 episode);
extern void startEpisodeSelect(void *selectMenu);

// yoshi.cpp
extern bool isYoshiDie(TMario *player);
extern bool isYoshiEggNeedFruit(THitActor *gpFruit);
extern u8 isYoshiEggFree(TEggYoshi *gpEgg, THitActor *gpFruit);
extern bool isYoshiMaintainFluddModel();
extern void maybeYoshiDrown(TYoshi *yoshi);
extern bool canMountYoshi(u32 state);
extern f32 getYoshiYPos_(TYoshi *yoshi);
extern void fixYoshiJuiceDecrement();
extern void canYoshiSpray(TWaterGun *gpWaterGun);
extern u32 calcYoshiSwimVelocity(TMario *player, u32 arg1);
extern u32 isYoshiWaterFlutter();
extern bool isYoshiValidDrip(TYoshi *yoshi);
extern void initFreeEggCard(MActorAnmBck *bckData);
extern u32 checkFreeEggCard(MActorAnmBck *bckData);
extern void saveNozzles(TYoshi *yoshi);
extern void restoreNozzles(TMario *player);


// utilities/card.cpp
extern s32 mountCard(TCardManager *cardManager, bool r4);
extern s32 probeCard(TCardManager *cardManager);
extern char *formatCardMessage(char *dst, const char *src, s32 len);

extern OSAlarm gctAlarm;
extern void setUserCodes(OSAlarm *alarm, OSContext *context);

#ifdef SME_DEBUG
    extern OSStopwatch gctStopwatch;
#endif

KURIBO_MODULE_BEGIN("Eclipse", "JoshuaMK", __VERSION__ "[" SME_STRINGIZE(SME_MAX_SHINES) " Shines]")
{
    KURIBO_EXECUTE_ON_LOAD {
        SME_DEBUG_LOG("Codeblocker - Creating OSAlarm at %p; Calls %p every %0.4f seconds\n", &gctAlarm, &setUserCodes, 0.001f);
        #ifdef SME_DEBUG
        OSInitStopwatch(&gctStopwatch, "Codeblocker");
        #endif
        OSCreateAlarm(&gctAlarm);
        OSSetPeriodicAlarm(&gctAlarm, OSGetTime(), OSMillisecondsToTicks(1), &setUserCodes);
        SME_DEBUG_LOG("Actor flags offset = %X\n", offsetof(JDrama::TNameRef, mKeyCode));
        SME_DEBUG_LOG("Actor position offset = %X\n", offsetof(JDrama::TPlacement, mPosition));
        SME_DEBUG_LOG("Actor size offset = %X\n", offsetof(JDrama::TActor, mSize));
    }
    KURIBO_EXECUTE_ON_UNLOAD {
        SME_DEBUG_LOG("-- Destroying Module --\n");
        #ifdef SME_DEBUG
        OSStopStopwatch(&gctStopwatch);
        #endif
        OSCancelAlarm(&gctAlarm);
    }

    /* -- HOOKS -- */

    // c_kit.cpp
    KURIBO_PATCH_BL(0x802998B8, onSetup);
    KURIBO_PATCH_BL(0x802A616C, onUpdate);
    KURIBO_PATCH_BL(0x80143F14, onDraw2D);
    KURIBO_PATCH_B(0x802A8B58, SMS_IsExMap);
    KURIBO_PATCH_B(0x802A8B30, SMS_IsMultiplayerMap);
    KURIBO_PATCH_B(0x802A8AFC, SMS_IsDivingMap);
    KURIBO_PATCH_B(0x802A8AE0, SMS_IsOptionMap);
    KURIBO_PATCH_BL(0x8027C6A4, manageLightSize);
    kWrite32(0x8027C6A8, 0x28030001);
    KURIBO_PATCH_BL(0x80153DE8, formatTalkMessage);
    KURIBO_PATCH_BL(0x80153E1C, formatTalkMessage);
    KURIBO_PATCH_BL(0x8024D3A8, realTimeCustomAttrsHandler);

    // camera.cpp
    KURIBO_PATCH_B(0x80027548, modifyCameraRangeToSize);

    // CheatCode.cpp
    KURIBO_PATCH_B(0x80295B6C, handleDebugCheat);

    // collision.cpp
    KURIBO_PATCH_BL(0x80258334, checkIsGlideBounce);
    KURIBO_PATCH_BL(0x80264CFC, checkIsGlideBounce);
    KURIBO_PATCH_BL(0x8024C558, checkIsRestoreTypeNoFallDamage);
    KURIBO_PATCH_BL(0x802500B8, updateCollisionContext);
    KURIBO_PATCH_BL(0x80250CA0, masterGroundCollisionHandler);
    KURIBO_PATCH_BL(0x8025059C, masterAllCollisionHandler);

    // file_flags.cpp
    KURIBO_PATCH_BL(0x802B1794, setFileCompleteBool);

    // flagmanager.cpp
    KURIBO_PATCH_B(0x80294EF4, resetStage);

    // fludd.cpp
    KURIBO_PATCH_B(0x80248F14, isPumpOK);
    kWrite32(0x803DCA02, 0x00300000 | TMarioAnimeData::FLUDD::FLUDD_ENABLED);
    KURIBO_PATCH_BL(0x8014206C, hasWaterCardOpen);
    kWrite32(0x80142070, 0x28030000);
    KURIBO_PATCH_BL(0x80283058, canCollectFluddItem);
    KURIBO_PATCH_BL(0x800678C4, sprayGoopMapWrapGlobalMar);
    KURIBO_PATCH_BL(0x801A3ED0, sprayGoopMapWrapGlobalMar);
    KURIBO_PATCH_BL(0x801B42D8, sprayGoopMapWrapGlobalMar);
    KURIBO_PATCH_BL(0x8024E710, sprayGoopMapWrapMar30);
    KURIBO_PATCH_BL(0x8027F7DC, sprayGoopMapWrapGlobalMar);
    KURIBO_PATCH_BL(0x8027F94C, sprayGoopMapWrapGlobalMar);
    KURIBO_PATCH_BL(0x800FED3C, canCleanSeals);
    kWrite32(0x800FED40, 0x2C030000);
    KURIBO_PATCH_BL(0x8024D53C, bindFluddtojoint);
    KURIBO_PATCH_BL(0x8024E548, checkExecWaterGun);
    KURIBO_PATCH_BL(0x8026C370, killTriggerNozzle);
    KURIBO_PATCH_BL(0x8026C018, spamHoverWrapper);
    KURIBO_PATCH_BL(0x80262580, checkAirNozzle);
    kWrite32(0x80262584, 0x2C030000);

    // init_mods.cpp
    KURIBO_PATCH_BL(0x80005328, &initCodeProtection);
    KURIBO_PATCH_BL(0x802A750C, &createGlobalHeaps);
    KURIBO_PATCH_BL(0x802A7140, &setupMarioDatas);
    KURIBO_PATCH_BL(0x802A716C, &initFirstModel);
    KURIBO_PATCH_BL(0x802998B4, &initFileMods);
    KURIBO_PATCH_BL(0x80280180, &initShineShadow);
    KURIBO_PATCH_BL(0x802B7A4C, &initSoundBank);
    KURIBO_PATCH_BL(0x802983F0, &initMusicTrack);
    KURIBO_PATCH_BL(0x80298420, &initMusicTrack);
    kWrite32(0x80276C90, 0x60000000);
    KURIBO_PATCH_BL(0x80276C94, &fromMarioInit);
    KURIBO_PATCH_BL(0x800397DC, &fromShadowMarioInit);
    KURIBO_PATCH_BL(0x80271580, &initYoshi);
    KURIBO_PATCH_BL(0x8029CCB0, &initCardColors);
    KURIBO_PATCH_BL(0x802B8B20, &initCollisionWarpLinks);
    KURIBO_PATCH_BL(0x802B57E4, &createUIHeap);

    // mario.cpp
    KURIBO_PATCH_BL(0x802320E0, shadowCrashPatch);
    KURIBO_PATCH_BL(0x802500B8, updateContexts);
    KURIBO_PATCH_BL(0x8029A87C, carryOrTalkNPC);
    KURIBO_PATCH_BL(0x802815F0, canGrabAtNPC);
    kWrite32(0x802815F4, 0x2C030000);
    KURIBO_PATCH_BL(0x80207430, canCarryNPC);
    kWrite32(0x80207434, 0x2C030000);

    // shine.cpp
    KURIBO_PATCH_BL(0x801BD664, manageShineVanish);
    kWrite32(0x801BD668, 0x48000568);
    KURIBO_PATCH_BL(0x802413E0, isKillEnemiesShine);
    KURIBO_PATCH_BL(0x802995BC, checkBootOut);
    kWrite32(0x80297BE8, 0x60848200);
    KURIBO_PATCH_BL(0x80293B10, extendShineIDLogic);
    KURIBO_PATCH_BL(0x801BCC98, shineObjectStringMod);
    kWrite32(0x803DEE50, shineFlagSetter);
    kWrite32(0x803DEE7C, shineFlagGetter);
    KURIBO_PATCH_BL(0x802946D4, shineSetClamper);
    kWrite32(0x80294744, 0x60000000);
    KURIBO_PATCH_BL(0x8029474C, shineGetClamper);
    /*Shine casts, fix light*/
    kWrite32(0x80412548, (f32)SME_MAX_SHINES);
    kWrite32(0x80293AF8, 0x3BFF03E7);
    kWrite32(0x802946B8, 0x280003E7);
    kWrite32(0x8017BE78, 0x5464037E);
    kWrite32(0x8017BEF4, 0x5464037E);
    kWrite32(0x8017BF34, 0x5464037E);
    kWrite32(0x801BCE30, 0x5464037E);
    kWrite32(0x801FF850, 0x5404037E);
    kWrite32(0x802946B4, 0x5480043E);
    kWrite32(0x80294730, 0x5480043E);
    kWrite32(0x80294734, 0x280003E7);
    kWrite32(0x80297BA0, 0x5404037E);
    KURIBO_PATCH_BL(0x80294334, extendFlagManagerLoad);
    kWrite32(0x80294338, 0x48000010);
    KURIBO_PATCH_BL(0x802939B8, extendFlagManagerSave);
    kWrite32(0x802939BC, 0x48000014);
    KURIBO_PATCH_BL(0x80297BD8, thinkSetBootFlag);
    KURIBO_PATCH_BL(0x801BCD20, loadAfterMaskState);
    kWrite32(0x801BCD24, 0x28030002);
    KURIBO_PATCH_BL(0x801BCEEC, setKillState);
    KURIBO_PATCH_BL(0x8029A590, thinkCloseCamera);
    kWrite32(0x8029A594, 0x28000000);
    KURIBO_PATCH_BL(0x802999D8, animationFreezeCheck);
    kWrite32(0x802999DC, 0x48000034);
    //Make Shines glow more
    kWrite32(0x803C9190, 0x3F19999A);

    // stage.cpp
    KURIBO_PATCH_BL(0x80299230, setStageOnExit);
    KURIBO_PATCH_BL(0x80175F58, startEpisodeSelect);

    // yoshi.cpp
    kWrite32(0x8026E068, 0x2C000001); //Turn green when out of juice
    kWrite32(0x8026E0A0, 0x60000000); //No flickering
    kWrite32(0x8026EE14, 0x4800020C); //Yoshi can't starve
    KURIBO_PATCH_BL(0x8026EB00, isYoshiDie);
    kWrite32(0x8026EB04, 0x2C030000);
    kWrite32(0x8026EB08, 0x41820518);

    KURIBO_PATCH_BL(0x8026EBFC, isYoshiDie);
    kWrite32(0x8026EC00, 0x2C030000);
    kWrite32(0x8026EC04, 0x4182041C);

    KURIBO_PATCH_BL(0x8026F218, isYoshiDie);
    kWrite32(0x8026F21C, 0x2C030000);
    kWrite32(0x8026F220, 0x41820164);
    KURIBO_PATCH_BL(0x801BC868, isYoshiEggNeedFruit);
    KURIBO_PATCH_BL(0x801BC8B4, isYoshiEggFree);
    kWrite32(0x801BC8B8, 0xA01E00FC);
    kWrite32(0x801BC8BC, 0x2C00000B);
    kWrite32(0x801BC8C0, 0x418200E4);
    kWrite32(0x801BC8C4, 0x2C030001);
    kWrite32(0x801BC8C8, 0x4182003C);
    kWrite32(0x801BC8CC, 0x418100D8);
    kWrite32(0x801BC8D0, 0x48000134);
    KURIBO_PATCH_BL(0x8024D68C, isYoshiMaintainFluddModel);
    kWrite32(0x8024D690, 0x2C030000);
    KURIBO_PATCH_BL(0x8024F240, maybeYoshiDrown);
    KURIBO_PATCH_BL(0x802810F8, canMountYoshi);
    KURIBO_PATCH_BL(0x80281148, getYoshiYPos_);
    kWrite32(0x802810FC, 0x2C030000);
    KURIBO_PATCH_BL(0x8026E810, fixYoshiJuiceDecrement);
    KURIBO_PATCH_BL(0x8024E58C, canYoshiSpray);
    KURIBO_PATCH_BL(0x80273198, calcYoshiSwimVelocity);
    KURIBO_PATCH_BL(0x80270078, isYoshiWaterFlutter);
    kWrite32(0x8027007C, 0x7C7E1B78);
    kWrite32(0x80270080, 0x60000000);
    kWrite32(0x80270084, 0x60000000);
    KURIBO_PATCH_BL(0x8024E788, isYoshiValidDrip);
    KURIBO_PATCH_B(0x801BC128, initFreeEggCard);
    KURIBO_PATCH_BL(0x801BC380, checkFreeEggCard);
    KURIBO_PATCH_BL(0x8028121C, saveNozzles);
    KURIBO_PATCH_BL(0x8024EC18, restoreNozzles);
    kWrite32(0x8024EC2C, 0x60000000);

    // utilities/card.cpp
    KURIBO_PATCH_BL(0x802B20F8, mountCard);
    KURIBO_PATCH_BL(0x80163C40, probeCard);
    KURIBO_PATCH_BL(0x8015B2C0, formatCardMessage);
    KURIBO_PATCH_BL(0x8015B2F8, formatCardMessage);
    KURIBO_PATCH_BL(0x8015B5BC, formatCardMessage);
    KURIBO_PATCH_BL(0x8015B5E4, formatCardMessage);
    KURIBO_PATCH_BL(0x8015B638, formatCardMessage);
    KURIBO_PATCH_BL(0x8015B660, formatCardMessage);
    KURIBO_PATCH_BL(0x8015BCB8, formatCardMessage);
    KURIBO_PATCH_BL(0x8015BCE0, formatCardMessage);
    KURIBO_PATCH_BL(0x8015BE24, formatCardMessage);
    KURIBO_PATCH_BL(0x8015BE4C, formatCardMessage);
    KURIBO_PATCH_BL(0x8015BE9C, formatCardMessage);
    KURIBO_PATCH_BL(0x8015BEC4, formatCardMessage);
    KURIBO_PATCH_BL(0x8015C508, formatCardMessage);
    KURIBO_PATCH_BL(0x8015D194, formatCardMessage);
    KURIBO_PATCH_BL(0x8015D1CC, formatCardMessage);
    KURIBO_PATCH_BL(0x8015DEFC, formatCardMessage);
    KURIBO_PATCH_BL(0x8015DF34, formatCardMessage);
    KURIBO_PATCH_BL(0x8015E34C, formatCardMessage);
    KURIBO_PATCH_BL(0x8015E374, formatCardMessage);
    KURIBO_PATCH_BL(0x8015E3C8, formatCardMessage);
    KURIBO_PATCH_BL(0x8015E3F0, formatCardMessage);
    KURIBO_PATCH_BL(0x8015F970, formatCardMessage);
    KURIBO_PATCH_BL(0x8015F9A8, formatCardMessage);
    KURIBO_PATCH_BL(0x80161320, formatCardMessage);
    KURIBO_PATCH_BL(0x80161358, formatCardMessage);
    KURIBO_PATCH_BL(0x8016889C, formatCardMessage);
    KURIBO_PATCH_BL(0x801688D4, formatCardMessage);
    KURIBO_PATCH_BL(0x80169224, formatCardMessage);
    KURIBO_PATCH_BL(0x8016925C, formatCardMessage);
    KURIBO_PATCH_BL(0x8016950C, formatCardMessage);
    KURIBO_PATCH_BL(0x80169534, formatCardMessage);
    KURIBO_PATCH_BL(0x8016958C, formatCardMessage);
    KURIBO_PATCH_BL(0x801695B4, formatCardMessage);
    KURIBO_PATCH_BL(0x80169ACC, formatCardMessage);
    KURIBO_PATCH_BL(0x80169B04, formatCardMessage);
    KURIBO_PATCH_BL(0x8016A02C, formatCardMessage);
    KURIBO_PATCH_BL(0x8016A064, formatCardMessage);
    KURIBO_PATCH_BL(0x8016A3D0, formatCardMessage);
    KURIBO_PATCH_BL(0x8016A3F8, formatCardMessage);
    KURIBO_PATCH_BL(0x8016A44C, formatCardMessage);
    KURIBO_PATCH_BL(0x8016A474, formatCardMessage);
    KURIBO_PATCH_BL(0x8016B19C, formatCardMessage);
    KURIBO_PATCH_BL(0x8016B1D4, formatCardMessage);

    /* -- PATCHES -- */

    //Restore Chao Seed
    kWrite32(0x802FD1A0, 0x808D8C70);

    //Show Exception Handler
    kWrite32(0x8029D0BC, 0x60000000);

    #ifdef SME_DEBUG
        //Skip FMVs
        kWrite32(0x802B5E8C, 0x38600001);
        kWrite32(0x802B5EF4, 0x38600001);
    #endif

    #ifdef SME_DEBUG
        //Level Select
        kWrite32(0x802A6788, 0x3BC00009);
    #endif

    //Fix Infinte Flutter
    kWrite32(0x8028113C, 0xC002F824);

    //Remove Dive While Wall Sliding
    kWrite32(0x8024BC10, 0x48000068);

    //Flood Till Corona Beat
    kWrite32(0x8029961C, 0x38840077);

    //Map on D Pad down
    kWrite32(0x8017A830, 0x5400077B);
    kWrite32(0x80297A60, 0x5400077B);

    //Hover move on roofs
    kWrite32(0x802569BC, 0xC02300B0);

    //Global surfing bloopies
    kWrite32(0x801B74F8, 0x60000000);
    kWrite32(0x801B74FC, 0x60000000);
    kWrite32(0x801B7500, 0x60000000);
    kWrite32(0x801B7504, 0x60000000);
    kWrite32(0x801B7508, 0x60000000);
    kWrite32(0x801B750C, 0x60000000);
    kWrite32(0x801B7510, 0x387E0780);
    kWrite32(0x801B7514, 0x4810BA9D);
    kWrite32(0x801B7518, 0x28030000);
    kWrite32(0x801B751C, 0x418200A4);
}
KURIBO_MODULE_END();