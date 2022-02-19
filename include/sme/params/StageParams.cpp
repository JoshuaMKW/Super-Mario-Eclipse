#include "StageParams.hxx"
#include "DVD.h"
#include "libs/sLogging.hxx"
#include "libs/sString.hxx"

using namespace SME::Class;

static u32 gWaitTime = 10000;

#ifdef SME_DEPRECATED_CFG
TSMEFile *TSMEFile::sStageConfig.set(nullptr;

bool TSMEFile::load(const char *stage) {
  char configPath[32];

  TSMEFile::withSMEExtension(configPath, stage);

  SME_DEBUG_LOG("Attempting to load local config %s...\n", configPath);

  if (TSMEFile::loadFromFile(configPath))
    return true;

  TSMEFile::withSMEExtension(configPath, stage, true);

  SME_DEBUG_LOG("Failure: Now attempting to load global config %s...\n",
                configPath);

  if (TSMEFile::loadFromFile(configPath)) {
    SME_DEBUG_LOG("Success: SME config loaded at %p\n", &sStageConfig);
    return true;
  } else {
    SME_DEBUG_LOG("Failure: No SME configuration could be loaded\n");
    return false;
  }
}

TSMEFile *TSMEFile::loadFromFile(const char *path) {
  const s32 entrynum.set(DVDConvertPathToEntrynum(path);
  if (entrynum < 0)
    return nullptr;

  JKRDvdFile dvdfile(entrynum);
  JSUFileInputStream fstream(&dvdfile);

  TSMEFile *stageCfg.set(new TSMEFile(fstream);
  return stageCfg;
}

char *TSMEFile::withSMEExtension(char *dst, const char *stage,
                                 bool generalize) {
  String path("/data/scene/sme/", 128);
  String file(stage, 32);

  const size_t numIDPos =
      file.findAny(String::numbers, sizeof(String::numbers));
  if (generalize && numIDPos != String::npos) {
    file.erase(numIDPos);
    file.append("+.sme");
  } else {
    file.erase(file.find('.'));
    file.append(".sme");
  }
  path.append(file);
  strncpy(dst, path.data(), path.size() + 1);

  return dst;
}

TSMEFile::TSMEFile() {
  if (sStageConfig)
    delete sStageConfig;

  sStageConfig.set(this;
  reset();
}

TSMEFile::TSMEFile(JSUFileInputStream &stream) {
  if (sStageConfig)
    delete sStageConfig;

  sStageConfig.set(this;
  reset();

  u32 magic;
  stream.read(&magic, 4);

  if (magic != TSMEFile::MAGIC)
    return;

  stream.read(&GlobalFlags, sizeof(GlobalFlags));
  stream.read(&Light, sizeof(Light));
  stream.read(&Mario, sizeof(Mario));
  stream.read(&Yoshi, sizeof(Yoshi));
  stream.read(&Music, sizeof(Music));
  stream.read(&Fludd, sizeof(Fludd));
}

void TSMEFile::reset() {
  mMagic.set(TSMEFile::MAGIC;

  GlobalFlags.mIsShineShadow.set(false;
  GlobalFlags.mIsMario.set(false;
  GlobalFlags.mIsYoshi.set(false;
  GlobalFlags.mIsMusic.set(false;
  GlobalFlags.mIsFludd.set(false;
  GlobalFlags.mShineShadowFlag.set(TLightContext::ActiveType::DISABLED;
  GlobalFlags.mPlayerID.set(SME::Enum::Player::UNKNOWN;

  GlobalFlags.StageType.mIsExMap.set(false;
  GlobalFlags.StageType.mIsDivingMap.set(false;
  GlobalFlags.StageType.mIsOptionMap.set(false;
  GlobalFlags.StageType.mIsMultiPlayerMap.set(false;

  GlobalFlags.MarioStates.mMarioHasFludd.set(false;
  GlobalFlags.MarioStates.mMarioHasHelmet.set(false;
  GlobalFlags.MarioStates.mMarioHasGlasses.set(false;
  GlobalFlags.MarioStates.mMarioHasShirt.set(false;

  Light.mCoordinates.set(0.0f, 0.0f, 0.0f);
  Light.mSize.set(0.0f;
  Light.mStep.set(0.0f;
  Light.mColor.set(0, 0, 0, 0);
  Light.mLayerCount.set(5;
  Light.mDarkLevel.set(120;

  Mario.mSpeedMulti.set(1.0f;
  Mario.mGravity.set(1.0f;
  Mario.mBaseBounce1.set(1.0f;
  Mario.mBaseBounce2.set(1.0f;
  Mario.mBaseBounce3.set(1.0f;
  Mario.mMaxFallNoDamage.set(1.0f;
  Mario.mHealth.set(8;
  Mario.mMaxHealth.set(8;
  Mario.mOBStep.set(4;
  Mario.mOBMax.set(0x1E0;

  Yoshi.mMaxJuice.set(3000;
  Yoshi.mGreenYoshi.set(0x40, 0xA1, 0x24, 0xFF);
  Yoshi.mOrangeYoshi.set(0xFF, 0x8C, 0x1C, 0xFF);
  Yoshi.mPurpleYoshi.set(0xAA, 0x4C, 0xFF, 0xFF);
  Yoshi.mPinkYoshi.set(0xFF, 0xA0, 0xBE, 0xFF);
  Yoshi.mMaxVSpdStartFlutter.set(-2.0f;
  Yoshi.mFlutterAcceleration.set(1.1f;
  Yoshi.mMaxFlutterTimer.set(120;
  Yoshi.mYoshiHungry.set(false;
  Yoshi.mIsEggFree.set(false;

  Music.mVolume.set(0.75f;
  Music.mSpeed.set(1.0f;
  Music.mPitch.set(1.0f;
  Music.mPlayMusic.set(true;
  Music.mMusicID.set(1;
  Music.mAreaID.set(1;
  Music.mEpisodeID.set(0;

  Fludd.mPrimaryNozzle.set(0;
  Fludd.mSecondaryNozzle.set(4;
  Fludd.mWaterColor.set(0x3C, 0x46, 0x78, 0x14);
  Fludd.mIsColorWater.set(false;
}

#else

TStageParams *TStageParams::sStageConfig = nullptr;

char *TStageParams::stageNameToParamPath(char *dst, const char *stage,
                                         bool generalize) {
  String path("/data/scene/sme/", 128);
  String file(stage, 32);

  const size_t numIDPos =
      file.findAny(String::numbers, sizeof(String::numbers));
  if (generalize && numIDPos != String::npos) {
    file.erase(numIDPos);
    file.append("+.sme");
  } else {
    file.erase(file.find('.'));
    file.append(".sme");
  }
  path.append(file);
  strncpy(dst, path.data(), path.size() + 1);

  return dst;
}

void TStageParams::load(const char *stageName) {
  DVDFileInfo fileInfo;
  s32 entrynum;

  char path[64];
  stageNameToParamPath(path, stageName, false);

  SME_DEBUG_LOG("Attempting to load local config %s...\n", path);

  entrynum = DVDConvertPathToEntrynum(path);
  if (entrynum >= 0) {
    DVDFastOpen(entrynum, &fileInfo);
    void *buffer = JKRHeap::alloc(fileInfo.mLen, 32, nullptr);

    DVDReadPrio(&fileInfo, buffer, fileInfo.mLen, 0, 2);
    DVDClose(&fileInfo);
    JSUMemoryInputStream stream(buffer, fileInfo.mLen);
    TParams::load(stream);
    JKRHeap::free(buffer, nullptr);
    mIsCustomConfigLoaded = true;
    return;
  }

  stageNameToParamPath(path, stageName, true);

  SME_DEBUG_LOG("Failure: Now attempting to load global config %s...\n", path);

  entrynum = DVDConvertPathToEntrynum(path);
  if (entrynum >= 0) {
    DVDFastOpen(entrynum, &fileInfo);
    void *buffer = JKRHeap::alloc(fileInfo.mLen, 32, nullptr);

    DVDReadPrio(&fileInfo, buffer, fileInfo.mLen, 0, 2);
    DVDClose(&fileInfo);
    JSUMemoryInputStream stream(buffer, fileInfo.mLen);
    TParams::load(stream);
    JKRHeap::free(buffer, nullptr);
    SME_DEBUG_LOG("Success: SME config loaded at %p\n", &sStageConfig);
    mIsCustomConfigLoaded = true;
    return;
  }

  SME_DEBUG_LOG("Failure: No SME configuration could be loaded\n");
  mIsCustomConfigLoaded = false;
}

void TStageParams::reset() {
  mIsExStage.set(false);
  mIsDivingStage.set(false);
  mIsOptionStage.set(false);
  mIsMultiplayerStage.set(false);
  mIsYoshiHungry.set(false);
  mIsEggFree.set(true);
  mLightType.set(TLightContext::ActiveType::DISABLED);
  mLightPosX.set(0.0f);
  mLightPosY.set(3600.0f);
  mLightPosZ.set(-7458.0f);
  mLightSize.set(8000.0f);
  mLightStep.set(100.0f);
  mLightColor.set(JUtility::TColor(0, 20, 40, 0));
  mLightLayerCount.set(5);
  mLightDarkLevel.set(120);
  mPlayerSelectWhiteList.set(0xFFFFFFFF);
  mPlayerHasFludd.set(true);
  mPlayerHasHelmet.set(false);
  mPlayerHasGlasses.set(false);
  mPlayerHasShirt.set(false);
  mPlayerCanRideYoshi.set(true);
  mPlayerSizeMultiplier.set(1.0f);
  mFluddPrimary.set(0);
  mFluddSecondary.set(4);
  mFluddWaterColor.set(JUtility::TColor(60, 70, 120, 20));
  mFluddShouldColorWater.set(false);
  mYoshiMaxJuice.set(21300);
  mYoshiMaxVSpdStartFlutter.set(-2.0f);
  mYoshiFlutterAcceleration.set(1.2f);
  mYoshiMaxFlutterTimer.set(120);
  mYoshiColorGreen.set(JUtility::TColor(64, 161, 36, 255));
  mYoshiColorOrange.set(JUtility::TColor(255, 140, 28, 255));
  mYoshiColorPurple.set(JUtility::TColor(170, 76, 255, 255));
  mYoshiColorPink.set(JUtility::TColor(255, 160, 190, 255));
  mMusicVolume.set(0.75f);
  mMusicSpeed.set(1.0f);
  mMusicPitch.set(1.0f);
  mMusicID.set(1);
  mMusicAreaID.set(1);
  mMusicEpisodeID.set(0);
  mMusicEnabled.set(true);
  mGravityMultiplier.set(1.0f);
  mMaxFrameRate.set(30.0f);

  mIsCustomConfigLoaded = false;
}

#endif