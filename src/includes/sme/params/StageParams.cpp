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

bool TStageParams::load(const char *stageName) {
  char buffer[64];

  stageNameToParamPath(buffer, stageName, false);
  if (DVDConvertPathToEntrynum(buffer) >= 0) {
    TParams::load(buffer);
    return true;
  }

  stageNameToParamPath(buffer, stageName, true);
  if (DVDConvertPathToEntrynum(buffer) >= 0) {
    TParams::load(buffer);
    return true;
  }

  return false;
}

void TStageParams::reset() {
  static_cast<TParamT<bool>>(mIsExStage).set(false);
  static_cast<TParamT<bool>>(mIsDivingStage).set(false);
  static_cast<TParamT<bool>>(mIsOptionStage).set(false);
  static_cast<TParamT<bool>>(mIsMultiplayerStage).set(false);
  static_cast<TParamT<bool>>(mIsYoshiHungry).set(false);
  static_cast<TParamT<bool>>(mIsEggFree).set(false);
  static_cast<TParamT<TLightContext::ActiveType>>(mLightType)
      .set(TLightContext::ActiveType::DISABLED);
  static_cast<TParamT<f32>>(mLightPosX).set(0.0f);
  static_cast<TParamT<f32>>(mLightPosY).set(3600.0f);
  static_cast<TParamT<f32>>(mLightPosZ).set(-7458.0f);
  static_cast<TParamT<f32>>(mLightSize).set(8000.0f);
  static_cast<TParamT<f32>>(mLightStep).set(100.0f);
  static_cast<TParamT<JUtility::TColor>>(mLightColor)
      .set(JUtility::TColor(0, 20, 40, 0));
  static_cast<TParamT<u8>>(mLightLayerCount).set(5);
  static_cast<TParamT<u8>>(mLightDarkLevel).set(120);
  static_cast<TParamT<u32>>(mPlayerSelectWhiteList).set(0xFFFFFFFF);
  static_cast<TParamT<bool>>(mPlayerCanRideYoshi).set(true);
  static_cast<TParamT<bool>>(mPlayerHasHelmet).set(false);
  static_cast<TParamT<bool>>(mPlayerHasGlasses).set(false);
  static_cast<TParamT<bool>>(mPlayerHasShirt).set(false);
  static_cast<TParamT<bool>>(mPlayerCanHoldNPCs).set(false);
  static_cast<TParamT<f32>>(mPlayerSizeMultiplier).set(1.0f);
  static_cast<TParamT<u8>>(mFluddPrimary).set(0);
  static_cast<TParamT<u8>>(mFluddSecondary).set(4);
  static_cast<TParamT<JUtility::TColor>>(mFluddWaterColor)
      .set(JUtility::TColor(60, 70, 120, 20));
  static_cast<TParamT<s32>>(mYoshiMaxJuice).set(21300);
  static_cast<TParamT<f32>>(mYoshiMaxVSpdStartFlutter).set(-2.0f);
  static_cast<TParamT<f32>>(mYoshiFlutterAcceleration).set(1.2f);
  static_cast<TParamT<u16>>(mYoshiMaxFlutterTimer).set(120);
  static_cast<TParamT<JUtility::TColor>>(mYoshiColorGreen)
      .set(JUtility::TColor(64, 161, 36, 255));
  static_cast<TParamT<JUtility::TColor>>(mYoshiColorOrange)
      .set(JUtility::TColor(255, 140, 28, 255));
  static_cast<TParamT<JUtility::TColor>>(mYoshiColorPurple)
      .set(JUtility::TColor(170, 76, 255, 255));
  static_cast<TParamT<JUtility::TColor>>(mYoshiColorPink)
      .set(JUtility::TColor(255, 160, 190, 255));
  static_cast<TParamT<f32>>(mMusicVolume).set(0.75f);
  static_cast<TParamT<f32>>(mMusicSpeed).set(1.0f);
  static_cast<TParamT<f32>>(mMusicPitch).set(1.0f);
  static_cast<TParamT<u16>>(mMusicID).set(1);
  static_cast<TParamT<u8>>(mMusicAreaID).set(1);
  static_cast<TParamT<u8>>(mMusicEpisodeID).set(0);
  static_cast<TParamT<bool>>(mMusicEnabled).set(true);
  static_cast<TParamT<f32>>(mGravityMultiplier).set(1.0f);
  static_cast<TParamT<f32>>(mMaxFrameRate).set(30.0f);
}

#endif