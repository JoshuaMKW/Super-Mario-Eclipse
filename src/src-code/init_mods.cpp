#include "printf.h"
#include "sms/JSystem/JDrama.hxx"
#include "sms/JSystem/JKR/JKRDecomp.hxx"
#include "sms/JSystem/JKR/JKRHeap.hxx"
#include "sms/JSystem/JKR/JKRMemArchive.hxx"
#include "stdlib.h"


#include "SME.hxx"

extern OSAlarm gctAlarm;
extern void setUserCodes(OSAlarm *alarm, OSContext *context);

#ifdef SME_DEBUG
extern OSStopwatch gctStopwatch;
#endif

SME::Util::Memory::Protection::MemoryMap gCodeProtector;

// 0x80005328
// extern -> SME.cpp
void SME::Patch::Init::initCodeProtection() {
  OSInit();
  gCodeProtector.setIndex(2);
  gCodeProtector.setStart(0x80003800);
  gCodeProtector.setEnd(0x80373000);
  gCodeProtector.setPermission(SME::Util::Memory::Protection::READ);
  gCodeProtector.activate();
}

// 0x802A750C
// extern -> SME.cpp
JKRExpHeap *SME::Patch::Init::createGlobalHeaps(void *newHeap, size_t size,
                                                JKRHeap *rootHeap, bool unk_1) {
  SME::TGlobals::sGlobals.mCharacterHeap =
      JKRExpHeap::create(0x200000, JKRHeap::sRootHeap, false);
  SME::TGlobals::sGlobals.mGlobalsHeap =
      JKRExpHeap::create(0x10000, JKRHeap::sRootHeap, false);
  auto *gameHeap =
      JKRExpHeap::create(newHeap, size - 0x200000, rootHeap, unk_1);
  return gameHeap;
}

// 0x802A7140
// extern -> SME.cpp
u32 SME::Patch::Init::setupMarioDatas(char *filepath) {
  TMarioGamePad *gpGamePad = gpApplication.mGamePad1;
  u32 id;

  switch (gpGamePad->mButtons.mInput) {
  case TMarioGamePad::Buttons::DPAD_UP:
    id = 1;
    break;
  case TMarioGamePad::Buttons::DPAD_DOWN:
    id = 2;
    break;
  case TMarioGamePad::Buttons::DPAD_LEFT:
    id = 3;
    break;
  case TMarioGamePad::Buttons::DPAD_RIGHT:
    id = 4;
    break;
  default:
    id = 0;
  }

  sprintf(filepath, "/data/chr%d.szs", id);
  return DVDConvertPathToEntrynum(filepath);
}

// 0x802A716C
// extern -> SME.cpp
u32 *SME::Patch::Init::initFirstModel(char *path, u32 unk_1, u32 unk_2,
                                      u32 unk_3, JKRHeap *heap, u32 unk_4,
                                      u32 unk_5, u32 unk_6) {
  if (SME::TGlobals::sGlobals.mCharacterHeap) {
    heap = SME::TGlobals::sGlobals.mCharacterHeap;
    SME::TGlobals::sGlobals.mCharacterHeap->freeAll();
  }
  u32 *file = static_cast<u32 *>(SME::Util::loadArchive(path, heap));

  if (file)
    SME::TGlobals::sGlobals.mCharacterHeap->alloc((0x1F0000 - file[1]) & -32,
                                                  32);
  else
    SME::TGlobals::sGlobals.mCharacterHeap->alloc(0x1F0000 & -32, 32);

  return file;
}

static void resetGlobalValues() {
  gModelWaterManagerWaterColor.set(0x3C, 0x46, 0x78, 0x14); // Water rgba
  gYoshiJuiceColor[0].set(0xFE, 0xA8, 0x02, 0x6E);          // Yoshi Juice rgba
  gYoshiJuiceColor[1].set(0x9B, 0x01, 0xFD, 0x6E);
  gYoshiJuiceColor[2].set(0xFD, 0x62, 0xA7, 0x6E);
  gYoshiBodyColor[0].set(0x40, 0xA1, 0x24, 0xFF); // Yoshi rgba
  gYoshiBodyColor[1].set(0xFF, 0x8C, 0x1C, 0xFF);
  gYoshiBodyColor[2].set(0xAA, 0x4C, 0xFF, 0xFF);
  gYoshiBodyColor[3].set(0xFF, 0xA0, 0xBE, 0xFF);
  *(f32 *)0x80415CA8 = 0.25f; // Mario overall speed
  gAudioVolume = 0.75f;
  gAudioPitch = 1.0f;
  gAudioSpeed = 1.0f;
}

// 0x802998B4
u32 *SME::Patch::Init::initFileMods() {
  u32 *objList;
  SME_FROM_GPR(r26, objList);

  TMarioGamePad *gpGamePad = gpApplication.mGamePad1;

#ifdef SME_DEBUG
  s8 characterID;

  switch (gpGamePad->mButtons.mInput) {
  case TMarioGamePad::Buttons::Z:
    characterID = 0;
    break;
  case TMarioGamePad::Buttons::DPAD_UP:
    characterID = 1;
    break;
  case TMarioGamePad::Buttons::DPAD_DOWN:
    characterID = 2;
    break;
  case TMarioGamePad::Buttons::DPAD_LEFT:
    characterID = 3;
    break;
  case TMarioGamePad::Buttons::DPAD_RIGHT:
    characterID = 4;
    break;
  default:
    characterID = -1;
  }
#else
  s8 characterID = -1;
#endif

  JKRMemArchive *marioVolumeData =
      static_cast<JKRMemArchive *>(JKRFileLoader::getVolume("mario"));
  u8 *params = marioVolumeData->getResource("/params.szs");

  char buffer[32];

  resetGlobalValues();
  SME::Class::TSMEFile::mStageConfig.reset();

  if (SME::Class::TSMEFile::mStageConfig.load(
          SME::Util::getStageName(&gpApplication))) {
    SME_DEBUG_LOG("Success: SME config loaded at %X\n",
                  SME::Class::TSMEFile::mStageConfig);
    if (characterID < 0)
      characterID = SME::Class::TSMEFile::mStageConfig.GlobalFlags.mPlayerID;
  }

  if (characterID >= 0) {
    // Attempt to swap character data
    sprintf(buffer, "/data/chr%d.szs", characterID);

    SME::TGlobals::sGlobals.mCharacterHeap->freeAll();

    u32 *marioData = static_cast<u32 *>(
        SME::Util::loadArchive(buffer, SME::TGlobals::sGlobals.mCharacterHeap));
    *(u32 *)gpArcBufferMario = (u32)marioData;

    SME::TGlobals::sGlobals.mCharacterHeap->alloc(
        (0x1F0000 - marioData[1]) & -32, 32);

    marioVolumeData->unmountFixed();
    marioVolumeData->mountFixed((void *)*(u32 *)gpArcBufferMario,
                                JKRMemBreakFlag::UNK_0);
    //__dt__13JKRMemArchiveFv(marioVolumeData);
    //__ct__13JKRMemArchiveFPvUl15JKRMemBreakFlag(marioVolumeData, *(u32
    //*)gpArcBufferMario, 0, 0);
  }

  void *allocation;
  if (params) {
    u32 filesize = marioVolumeData->getResSize(params);
    CompressionType compressionState = JKRDecomp::checkCompressed(params);

    switch (compressionState) {
    case CompressionType::SZS: {
      *(u32 *)0x8040E260 = params[1];
      allocation =
          JKRHeap::sCurrentHeap->alloc(params[1], 32, JKRHeap::sCurrentHeap);
      decompSZS_subroutine(params, static_cast<u8 *>(allocation));
      SME::TGlobals::sGlobals.mPRMFile = allocation;
    }
    case CompressionType::SZP:
      SME::TGlobals::sGlobals.mPRMFile = nullptr;
    case CompressionType::NONE: {
      allocation = JKRHeap::sCurrentHeap->alloc(filesize, 32);
      memcpy(allocation, params, filesize);
      SME::TGlobals::sGlobals.mPRMFile = allocation;
    }
    }

    JKRMemArchive *oldParams =
        static_cast<JKRMemArchive *>(JKRFileLoader::getVolume("params"));

    oldParams->unmountFixed();
    oldParams->mountFixed(SME::TGlobals::sGlobals.mPRMFile,
                          JKRMemBreakFlag::UNK_0);
  }

  return objList;
}

// 0x80280180
void SME::Patch::Init::initShineShadow() {
  if (SME::Class::TSMEFile::mStageConfig.FileHeader.mMAGIC !=
          SME::Class::TSMEFile::MAGIC ||
      !SME::Class::TSMEFile::mStageConfig.GlobalFlags.mIsShineShadow)
    return;

  if (TFlagManager::smInstance->Type4Flag.mShineCount < SME_MAX_SHINES) {
    SME::TGlobals::sGlobals.mLightData.mLightType =
        SME::Class::TSMEFile::mStageConfig.GlobalFlags.mShineShadowFlag;
    SME::TGlobals::sGlobals.mLightData.mShineShadowBase =
        SME::Class::TSMEFile::mStageConfig.Light.mSize;
    SME::TGlobals::sGlobals.mLightData.mPrevShineCount =
        TFlagManager::smInstance->Type4Flag.mShineCount;

    gpModelWaterManager->mColor =
        SME::Class::TSMEFile::mStageConfig.Light.mColor;
    gpModelWaterManager->mDarkLevel =
        SME::Class::TSMEFile::mStageConfig.Light.mDarkLevel;
    gpModelWaterManager->mLayerCount =
        SME::Class::TSMEFile::mStageConfig.Light.mLayerCount;
    gpModelWaterManager->mSphereStep =
        SME::Class::TSMEFile::mStageConfig.Light.mStep;
    gpModelWaterManager->mSize = SME::Class::TSMEFile::mStageConfig.Light.mSize;
    gpModelWaterManager->LightType.mMaskObjects = true;
    gpModelWaterManager->LightType.mShowShadow = true;

    if (SME::Class::TSMEFile::mStageConfig.GlobalFlags.mShineShadowFlag ==
        SME::Enum::LightContext::FOLLOWPLAYER) {
      SME::TGlobals::sGlobals.mLightData.mNextSize =
          SME::Class::TSMEFile::mStageConfig.Light.mSize;
      for (u32 i = 0; i < TFlagManager::smInstance->Type4Flag.mShineCount;
           ++i) {
        SME::TGlobals::sGlobals.mLightData.mNextSize +=
            (10000 / SME_MAX_SHINES) + i * 2.0f;
      }
      gpModelWaterManager->mSize = SME::TGlobals::sGlobals.mLightData.mNextSize;
      gpModelWaterManager->mSphereStep = gpModelWaterManager->mSize / 2.0f;
    }
    SME::TGlobals::sGlobals.mLightData.mShineShadowCoordinates =
        SME::Class::TSMEFile::mStageConfig.Light.mCoordinates;
  } else {
    SME::TGlobals::sGlobals.mLightData.mLightType =
        SME::Enum::LightContext::DISABLED;
  }
}

// 0x802B7A4C
void SME::Patch::Init::initSoundBank(u8 areaID, u8 episodeID) {
  if (SME::Class::TSMEFile::mStageConfig.FileHeader.mMAGIC !=
          SME::Class::TSMEFile::MAGIC ||
      !SME::Class::TSMEFile::mStageConfig.GlobalFlags.mIsMusic)
    setMSoundEnterStage__10MSMainProcFUcUc(areaID, episodeID);
  else
    setMSoundEnterStage__10MSMainProcFUcUc(
        SME::Class::TSMEFile::mStageConfig.Music.mAreaID,
        SME::Class::TSMEFile::mStageConfig.Music.mEpisodeID);
}

// 0x802983F0
// 0x80298420
void SME::Patch::Init::initMusicTrack() {
  if (SME::Class::TSMEFile::mStageConfig.Music.mPlayMusic) {
    gStageBGM = 0x80010000 | SME::Class::TSMEFile::mStageConfig.Music.mMusicID;
    gAudioSpeed = SME::Class::TSMEFile::mStageConfig.Music.mSpeed;
    gAudioPitch = SME::Class::TSMEFile::mStageConfig.Music.mPitch;
    gAudioVolume =
        Max(Min(SME::Class::TSMEFile::mStageConfig.Music.mVolume, 1), 0);
  }
  startStageBGM__10MSMainProcFUcUc();
}

static void initFludd(TMario *player, SME::Class::TPlayerParams *params) {
  SME_ASSERT(params != nullptr, "Can't init fludd with non existant params!");
  if (params) {
    gModelWaterManagerWaterColor = params->getParams()->mWaterColor.get();

    if (!params->canUseNozzle(static_cast<TWaterGun::NozzleType>(
            player->mFludd->mCurrentNozzle))) {
      for (u8 i = 0; i < 7; ++i) {
        if (params->canUseNozzle(static_cast<TWaterGun::NozzleType>(i))) {
          player->mFludd->mCurrentNozzle = i;
          player->mAttributes.mHasFludd = params->getCanUseFludd();
          player->mFludd->mCurrentWater =
              player->mFludd->mNozzleList[(u8)player->mFludd->mCurrentNozzle]
                  ->mMaxWater;
          break;
        } else if (i == 6) {
          player->mAttributes.mHasFludd = false;
          params->setCanUseFludd(false);
        }
      }
    }

    if (!params->canUseNozzle(static_cast<TWaterGun::NozzleType>(
            player->mFludd->mSecondNozzle))) {
      for (u8 i = 0; i < 7; ++i) {
        if (params->canUseNozzle(static_cast<TWaterGun::NozzleType>(i))) {
          player->mFludd->mSecondNozzle = i;
          player->mAttributes.mHasFludd = params->getCanUseFludd();
          break;
        }
        player->mFludd->mSecondNozzle = player->mFludd->mCurrentNozzle;
      }
    }
  }

  if (SME::Class::TSMEFile::mStageConfig.FileHeader.mMAGIC ==
          SME::Class::TSMEFile::MAGIC &&
      SME::Class::TSMEFile::mStageConfig.GlobalFlags.mIsFludd) {
    if (SME::Class::TSMEFile::mStageConfig.Fludd.mIsColorWater) {
      gModelWaterManagerWaterColor =
          SME::Class::TSMEFile::mStageConfig.Fludd.mWaterColor;
    }
    player->mFludd->mCurrentNozzle =
        SME::Class::TSMEFile::mStageConfig.Fludd.mPrimaryNozzle;
    player->mFludd->mSecondNozzle =
        SME::Class::TSMEFile::mStageConfig.Fludd.mSecondaryNozzle;

    player->mFludd->mCurrentWater =
        player->mFludd->mNozzleList[(u8)player->mFludd->mCurrentNozzle]
            ->mMaxWater;
  }
}

static void initMario(TMario *player, bool isMario) {
  SME::Class::TPlayerParams *params =
      new SME::Class::TPlayerParams(player, isMario);
  SME::TGlobals::sGlobals.registerPlayerParams(params);

  if (SME::Class::TSMEFile::mStageConfig.FileHeader.mMAGIC ==
          SME::Class::TSMEFile::MAGIC &&
      SME::Class::TSMEFile::mStageConfig.GlobalFlags.mIsMario) {
    params->setPlayerID(
        SME::Class::TSMEFile::mStageConfig.GlobalFlags.mPlayerID);
    player->mHealth = SME::Class::TSMEFile::mStageConfig.Mario.mHealth;
    player->mDeParams.mTramplePowStep1.set(
        SME::Class::TSMEFile::mStageConfig.Mario.mBaseBounce1);
    player->mDeParams.mTramplePowStep2.set(
        SME::Class::TSMEFile::mStageConfig.Mario.mBaseBounce2);
    player->mDeParams.mTramplePowStep3.set(
        SME::Class::TSMEFile::mStageConfig.Mario.mBaseBounce3);
    player->mDeParams.mDamageFallHeight.set(
        SME::Class::TSMEFile::mStageConfig.Mario.mMaxFallNoDamage);
    player->mDeParams.mHPMax.set(
        SME::Class::TSMEFile::mStageConfig.Mario.mMaxHealth);
    player->mDeParams.mIllegalPlaneCtInc.set(
        SME::Class::TSMEFile::mStageConfig.Mario.mOBStep);
    player->mDeParams.mIllegalPlaneTime.set(
        SME::Class::TSMEFile::mStageConfig.Mario.mOBMax);
    player->mJumpParams.mGravity.set(
        SME::Class::TSMEFile::mStageConfig.Mario.mGravity);
    player->mAttributes.mGainHelmet =
        SME::Class::TSMEFile::mStageConfig.GlobalFlags.MarioStates
            .mMarioHasHelmet;
    player->mAttributes.mHasFludd = SME::Class::TSMEFile::mStageConfig
                                        .GlobalFlags.MarioStates.mMarioHasFludd;
    player->mAttributes.mIsShineShirt =
        SME::Class::TSMEFile::mStageConfig.GlobalFlags.MarioStates
            .mMarioHasShirt;

    if (SME::Class::TSMEFile::mStageConfig.GlobalFlags.MarioStates
            .mMarioHasGlasses)
      wearGlass__6TMarioFv(player);
  }

  if (isMario)
    initFludd(player, params);
}

// 0x80276C94
TMario *SME::Patch::Init::fromMarioInit(TMario *player) {
  TMario *(*virtualFunc)(TMario *);
  SME_FROM_GPR(r12, virtualFunc);

  virtualFunc(player); // call vtable func
  initMario(player, true);
  return player;
}

// 0x800397DC
bool SME::Patch::Init::fromShadowMarioInit() {
  TMario *player;
  asm volatile("lwz %0, 0x150 (r31)" : "=r"(player));
  initMario(player, false);
  return SMS_isMultiPlayerMap__Fv();
}

// 0x80271580
void SME::Patch::Init::initYoshi(MAnmSound *anmSound, void *r4, u32 r5,
                                 f32 f1) {
  initAnmSound__9MAnmSoundFPvUlf(anmSound, r4, r5, f1);

  TYoshi *yoshi;
  SME_FROM_GPR(r31, yoshi);

  if (SME::Class::TSMEFile::mStageConfig.FileHeader.mMAGIC !=
          SME::Class::TSMEFile::MAGIC ||
      !SME::Class::TSMEFile::mStageConfig.GlobalFlags.mIsYoshi)
    return;

  gYoshiJuiceColor[0] = SME::Class::TSMEFile::mStageConfig.Yoshi.mOrangeYoshi;
  gYoshiJuiceColor[1] = SME::Class::TSMEFile::mStageConfig.Yoshi.mPurpleYoshi;
  gYoshiJuiceColor[2] = SME::Class::TSMEFile::mStageConfig.Yoshi.mPinkYoshi;

  gYoshiBodyColor[0] = SME::Class::TSMEFile::mStageConfig.Yoshi.mGreenYoshi;
  gYoshiBodyColor[1] = SME::Class::TSMEFile::mStageConfig.Yoshi.mOrangeYoshi;
  gYoshiBodyColor[2] = SME::Class::TSMEFile::mStageConfig.Yoshi.mPurpleYoshi;
  gYoshiBodyColor[3] = SME::Class::TSMEFile::mStageConfig.Yoshi.mPinkYoshi;

  yoshi->mMaxJuice = SME::Class::TSMEFile::mStageConfig.Yoshi.mMaxJuice;
  yoshi->mMaxVSpdStartFlutter =
      SME::Class::TSMEFile::mStageConfig.Yoshi.mMaxVSpdStartFlutter;
  yoshi->mFlutterAcceleration =
      SME::Class::TSMEFile::mStageConfig.Yoshi.mFlutterAcceleration;
  yoshi->mMaxFlutterTimer =
      SME::Class::TSMEFile::mStageConfig.Yoshi.mMaxFlutterTimer;
}

// 0x8029CCB0
void SME::Patch::Init::initCardColors() {
  if (SME::Class::TSMEFile::mStageConfig.FileHeader.mMAGIC ==
          SME::Class::TSMEFile::MAGIC &&
      SME::Class::TSMEFile::mStageConfig.GlobalFlags.mIsYoshi) {
    gpMarDirector->mGCConsole->mJuiceCardOrange =
        SME::Class::TSMEFile::mStageConfig.Yoshi.mOrangeYoshi;
    gpMarDirector->mGCConsole->mJuiceCardPurple =
        SME::Class::TSMEFile::mStageConfig.Yoshi.mPurpleYoshi;
    gpMarDirector->mGCConsole->mJuiceCardPink =
        SME::Class::TSMEFile::mStageConfig.Yoshi.mPinkYoshi;
  }

  if (SME::Class::TSMEFile::mStageConfig.FileHeader.mMAGIC ==
          SME::Class::TSMEFile::MAGIC &&
      SME::Class::TSMEFile::mStageConfig.GlobalFlags.mIsFludd &&
      SME::Class::TSMEFile::mStageConfig.Fludd.mIsColorWater) {
    gpMarDirector->mGCConsole->mWaterLeftPanel =
        SME::Class::TSMEFile::mStageConfig.Fludd.mWaterColor;
    gpMarDirector->mGCConsole->mWaterRightPanel.r = SME::Util::Math::lerp<u8>(
        0, SME::Class::TSMEFile::mStageConfig.Fludd.mWaterColor.r, 0.8125);
    gpMarDirector->mGCConsole->mWaterRightPanel.g = SME::Util::Math::lerp<u8>(
        0, SME::Class::TSMEFile::mStageConfig.Fludd.mWaterColor.g, 0.8125);
    gpMarDirector->mGCConsole->mWaterRightPanel.b = SME::Util::Math::lerp<u8>(
        0, SME::Class::TSMEFile::mStageConfig.Fludd.mWaterColor.b, 0.8125);
    gpMarDirector->mGCConsole->mWaterRightPanel.a = SME::Util::Math::lerp<u8>(
        0, SME::Class::TSMEFile::mStageConfig.Fludd.mWaterColor.a, 0.8125);
  }
}

/*This works by taking the target id and matching it to the
/ ID of the first entry to have the same home ID as the target.
/
/ This makes a half decent linking system for the collision
/ triangles for functionalities like linked warping!
*/

static void parseWarpLinks(TMapCollisionData *col,
                           SME::Class::TWarpCollisionList *links, u32 validID,
                           u32 idGroupSize = 0) {
  u32 curDataIndex = 0;

  for (u32 i = 0; i < col->mFloorArraySize; ++i) {
    if (((col->mColTable[i].mCollisionType & 0x7FFF) - validID) <=
            idGroupSize ||
        ((col->mColTable[i].mCollisionType & 0x7FFF) - validID) <=
            idGroupSize + 1000) {

      links->mColList[curDataIndex] = SME::Class::TCollisionLink(
          &col->mColTable[i], (u8)(col->mColTable[i].mValue4 >> 8),
          (u8)col->mColTable[i].mValue4);
      if (curDataIndex >= 0xFF)
        break;
      ++curDataIndex;
    }
  }
  links->mArrayLength = curDataIndex;
}

// 0x802B8B20
u32 SME::Patch::Init::initCollisionWarpLinks(const char *name) {
  SME::Class::TWarpCollisionList *warpDataArray =
      new (32) SME::Class::TWarpCollisionList();
  SME::Class::TWarpCollisionList *warpDataPreserveArray =
      new (32) SME::Class::TWarpCollisionList();
  SME::TGlobals::sGlobals.mWarpColArray = warpDataArray;
  SME::TGlobals::sGlobals.mWarpColPreserveArray = warpDataPreserveArray;

  if (warpDataArray) {
    parseWarpLinks(gpMapCollisionData, warpDataArray, 16040);
    parseWarpLinks(gpMapCollisionData, warpDataPreserveArray, 16041, 1);
  }

  return JDrama::TNameRef::calcKeyCode(name);
}

// 0x802B57E4
void SME::Patch::Init::createUIHeap(u32 size, s32 alignment) {
  SME::TGlobals::sGlobals.mGame6Heap =
      JKRExpHeap::create(size, JKRHeap::sSystemHeap, false);
  gpMarDirector->mGame6Data = (u32 *)SME::Util::Memory::malloc(size, alignment);
}

// 0x802A72A4
u32 SME::Patch::Init::initHUDElements(char *filepath) {
  char buffer[32];
  s32 entrynum;

  sprintf(buffer, filepath,
          TFlagManager::smInstance->Type6Flag.CustomFlags.mHUDElement);
  entrynum = DVDConvertPathToEntrynum(buffer);

  if (entrynum < 0)
    sprintf(filepath, filepath, 0);
  else
    strcpy(filepath, buffer);

  return DVDConvertPathToEntrynum(filepath);
}
// kmCall(0x802A72A4, &initHUDElements);

static JKRMemArchive *switchArchive(char *curArchive, void *newArchive) {
  auto *volume =
      reinterpret_cast<JKRMemArchive *>(JKRFileLoader::getVolume(curArchive));
  volume->unmountFixed();
  volume->mountFixed(newArchive, JKRMemBreakFlag::UNK_0);
  return volume;
}

// 0x80172440
JKRMemArchive *SME::Patch::Init::switchHUDOnStageLoad(char *curArchive,
                                                      u32 *gameUI) {
  char buffer[32];

  if (gpApplication.mGamePad1->mButtons.mInput &
      TMarioGamePad::Buttons::DPAD_UP)
    TFlagManager::smInstance->Type6Flag.CustomFlags.mHUDElement = 1;
  else if (gpApplication.mGamePad1->mButtons.mInput &
           TMarioGamePad::Buttons::DPAD_DOWN)
    TFlagManager::smInstance->Type6Flag.CustomFlags.mHUDElement = 0;

  sprintf(buffer, "/data/game_%d.arc",
          TFlagManager::smInstance->Type6Flag.CustomFlags
              .mHUDElement);              //"/data/game_%d.arc"
  strcpy(strstr(buffer, ".arc"), ".szs"); //".arc", ".szs"

  if (DVDConvertPathToEntrynum(buffer) >= 0) {
    SME::Util::Memory::free(gpMarDirector->mGame6Data);
    *gameUI =
        loadToMainRAM__12JKRDvdRipperFPCcPUc15JKRExpandSwitchUlP7JKRHeapQ212JKRDvdRipper15EAllocDirectionUlPi(
            buffer, 0, 1, 0, gpMarDirector->mGame6Data, 1, 0, 0);
  }

  return switchArchive(curArchive, gameUI);
}
// kmCall(0x80172440, &switchHUDOnStageLoad);

// 0x802B57E4
JKRHeap *SME::Patch::Init::useCustomHUDHeap(u32 size, s32 alignment) {
  return SME::TGlobals::sGlobals.mGame6Heap;
}
// kmCall(0x802B57E4, &useCustomHUDHeap);