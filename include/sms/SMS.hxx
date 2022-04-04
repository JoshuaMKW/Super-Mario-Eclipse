#pragma once

#include "types.h"

#include "JGeometry.hxx"
#include "JUT/JUTColor.hxx"
#include "JUT/JUTConsole.hxx"
#include "JUT/JUTResFont.hxx"


#include "sms/actor/Mario.hxx"
#include "sms/camera/CameraMarioData.hxx"
#include "sms/camera/CameraShake.hxx"
#include "sms/camera/PolarSubCamera.hxx"
#include "sms/collision/MapCollisionData.hxx"
#include "sms/game/Application.hxx"
#include "sms/game/GCConsole2.hxx"
#include "sms/game/MarDirector.hxx"
#include "sms/game/Strategy.hxx"
#include "sms/game/Conductor.hxx"
#include "sms/manager/EffectObjManager.hxx"
#include "sms/manager/FlagManager.hxx"
#include "sms/manager/PollutionManager.hxx"
#include "sms/manager/RumbleManager.hxx"
#include "sms/manager/MarioParticleManager.hxx"
#include "sms/manager/ModelWaterManager.hxx"
#include "sms/map/Map.hxx"
#include "sms/option/CardLoad.hxx"
#include "sms/sound/MSound.hxx"


extern JUtility::TColor gYoshiBodyColor[4];
extern JGeometry::TVec3<f32> gShineShadowPos;
extern TMarioAnimeData gMarioAnimeData[336];
extern JUtility::TColor gModelWaterManagerWaterColor;
extern JUtility::TColor gYoshiJuiceColor[3];
extern TApplication gpApplication;
extern f32 gAudioVolume;
extern CPolarSubCamera *gpCamera;
extern TCameraMarioData *gpCameraMario;
extern TCameraShake *gpCameraShake;
extern TCardLoad *gpCardLoad;
extern TMap *gpMap;
extern TMapCollisionData *gpMapCollisionData;
extern f32 gEffectCoronaScale;
extern TPollutionManager *gpPollution;
extern float *gpNPCPtrSaveNormal;
extern RumbleMgr *gpPad1RumbleMgr;
extern RumbleMgr *gpPad2RumbleMgr;
extern RumbleMgr *gpPad3RumbleMgr;
extern RumbleMgr *gpPad4RumbleMgr;
extern TConductor *gpConductor;
extern TEffectObjManager *gpEffectObjManager;
extern TStrategy *gpStrategy;
extern TMario *gpMarioOriginal;
extern TMario *gpMarioAddress;
extern JGeometry::TVec3<f32> *gpMarioPos;
extern TModelWaterManager *gpModelWaterManager;
extern TMarioParticleManager *gpMarioParticleManager;
extern TMarDirector *gpMarDirector;
extern MSound *gpMSound;
extern TCardManager *gpCardManager;
extern JUTResFont *gpSystemFont;
extern JUTResFont *gpRomFont;
extern void *gpArcBufferMario;
extern void *spGameHeapBlock;
extern void *gArbkConsole;
extern void *gArbkGuide;
extern u32 gDemoBGM;
extern u32 gStageBGM;
extern JUTConsole *sReportConsole;
extern JUTConsole *sWarningConsole;
extern f32 gAudioPitch;
extern f32 gAudioSpeed;