#ifndef SMS_H
#define SMS_H

#include "types.h"

#include "sms/JUT.hxx"
#include "sms/JGeometry.hxx"

#include "sms/actor/Mario.hxx"
#include "sms/camera/PolarSubCamera.hxx"
#include "sms/camera/CameraMarioData.hxx"
#include "sms/camera/CameraShake.hxx"
#include "sms/collision/MapCollision.hxx"
#include "sms/game/Application.hxx"
#include "sms/game/MarDirector.hxx"
#include "sms/game/GCConsole2.hxx"
#include "sms/manager/FlagManager.hxx"
#include "sms/manager/PollutionManager.hxx"
#include "sms/manager/RumbleManager.hxx"
#include "sms/manager/WaterManager.hxx"
#include "sms/map/Map.hxx"
#include "sms/option/CardLoad.hxx"
#include "sms/sound/MSound.hxx"

JUtility::TColor gYoshiBodyColor[4];
JGeometry::TVec3<f32> gShineShadowPos;
JUtility::TColor gFluddWaterColor;
JUtility::TColor gYoshiJuiceColor[3];
TApplication gpApplication;
f32 gAudioVolume;
CPolarSubCamera *gpCamera;
TCameraMarioData *gpCameraMario;
TCameraShake *gpCameraShake;
TCardLoad *gpCardLoad;
TMap *gpMap;
TMapCollisionData *gpMapCollisionData;
f32 gEffectCoronaScale;
TPollutionManager *gpPollution;
RumbleMgr *gpPad1RumbleMgr;
RumbleMgr *gpPad2RumbleMgr;
RumbleMgr *gpPad3RumbleMgr;
RumbleMgr *gpPad4RumbleMgr;
TMario *gpMarioAddress;
JGeometry::TVec3<f32> *gpMarioPos;
TWaterManager *gpModelWaterManager;
TFlagManager *gpFlagManager;
TMarDirector *gpMarDirector;
MSound *gpMSound;
TCardManager *gpCardManager;
void *gpArcBufferMario;
void *spGameHeapBlock;
void *gpArBkConsole;
void *gpArBkGuide;
u32 gDemoBGM;
u32 gStageBGM;
JKRHeap *sSystemHeap;
JKRHeap *sCurrentHeap;
JKRHeap *sRootHeap;
f32 gAudioPitch;
f32 gAudioSpeed;

/*
//define global addresses
#define WaterAddr 0x803DD898
#define CustomInfoInstance 0x80003BD0
#define YoshiColor 0x8039F934
#define ShineShadowCoordinates 0x803A1C94
#define YoshiJuiceColor 0x803DD89C
#define TApplicationInstance 0x803E9700
#define SVolumeList 0x804042B4
#define StreamVolume 0x8040C1C0
#define CPolarSubCameraInstance 0x8040D0A8
#define TCameraShakeInstance 0x8040D0B8
#define TCardLoadInstance 0x8040DDE0
#define TMapInstance 0x8040DE98
#define TMapCollisionDataInstance 0x8040DEA0
#define TPollutionManagerInstance 0x8040DED0
#define GamePad1Instance 0x8040E0D0
#define GamePad2Instance 0x8040E0D4
#define GamePad3Instance 0x8040E0D8
#define GamePad4Instance 0x8040E0DC
#define TMarioInstance 0x8040E108
#define TMarioCoordinates 0x8040E10C
#define TWaterManagerInstance 0x8040E138
#define TFlagManagerInstance 0x8040E160
#define TMarDirectorInstance 0x8040E178
#define MSoundInstance 0x8040E17C
#define TCardManagerInstance 0x8040E184
#define ARCBufferMario 0x8040E198
#define GameHeapBlock 0x8040E1A4
#define ArBkConsole 0x8040E1A8
#define ArBkGuide 0x8040E1B0
#define DemoBGM 0x8040E1EC
#define StageBGM 0x8040E1F0
#define JKRSystemHeap 0x8040E290
#define JKRCurrentHeap 0x8040E294
#define JKRRootHeap 0x8040E298
#define StreamPitch 0x80417248
#define StreamSpeed 0x8041731C
*/

#endif