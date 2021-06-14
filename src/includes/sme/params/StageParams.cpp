#include "StageParams.hxx"
#include "DVD.h"
#include "libs/sLogging.hxx"
#include "libs/sString.hxx"

using namespace SME::Class;

static u32 gWaitTime = 10000;

#ifdef SME_DEPRECATED_CFG
TSMEFile *TSMEFile::sStageConfig = nullptr;

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
  const s32 entrynum = DVDConvertPathToEntrynum(path);
  if (entrynum < 0)
    return nullptr;

  JKRDvdFile dvdfile(entrynum);
  JSUFileInputStream fstream(&dvdfile);

  TSMEFile *stageCfg = new TSMEFile(fstream);
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

  sStageConfig = this;
  reset();
}

TSMEFile::TSMEFile(JSUFileInputStream &stream) {
  if (sStageConfig)
    delete sStageConfig;

  sStageConfig = this;
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
  mMagic = TSMEFile::MAGIC;

  GlobalFlags.mIsShineShadow = false;
  GlobalFlags.mIsMario = false;
  GlobalFlags.mIsYoshi = false;
  GlobalFlags.mIsMusic = false;
  GlobalFlags.mIsFludd = false;
  GlobalFlags.mShineShadowFlag = TLightContext::ActiveType::DISABLED;
  GlobalFlags.mPlayerID = SME::Enum::Player::UNKNOWN;

  GlobalFlags.StageType.mIsExMap = false;
  GlobalFlags.StageType.mIsDivingMap = false;
  GlobalFlags.StageType.mIsOptionMap = false;
  GlobalFlags.StageType.mIsMultiPlayerMap = false;

  GlobalFlags.MarioStates.mMarioHasFludd = false;
  GlobalFlags.MarioStates.mMarioHasHelmet = false;
  GlobalFlags.MarioStates.mMarioHasGlasses = false;
  GlobalFlags.MarioStates.mMarioHasShirt = false;

  Light.mCoordinates.set(0.0f, 0.0f, 0.0f);
  Light.mSize = 0.0f;
  Light.mStep = 0.0f;
  Light.mColor.set(0, 0, 0, 0);
  Light.mLayerCount = 5;
  Light.mDarkLevel = 120;

  Mario.mSpeedMulti = 1.0f;
  Mario.mGravity = 1.0f;
  Mario.mBaseBounce1 = 1.0f;
  Mario.mBaseBounce2 = 1.0f;
  Mario.mBaseBounce3 = 1.0f;
  Mario.mMaxFallNoDamage = 1.0f;
  Mario.mHealth = 8;
  Mario.mMaxHealth = 8;
  Mario.mOBStep = 4;
  Mario.mOBMax = 0x1E0;

  Yoshi.mMaxJuice = 3000;
  Yoshi.mGreenYoshi.set(0x40, 0xA1, 0x24, 0xFF);
  Yoshi.mOrangeYoshi.set(0xFF, 0x8C, 0x1C, 0xFF);
  Yoshi.mPurpleYoshi.set(0xAA, 0x4C, 0xFF, 0xFF);
  Yoshi.mPinkYoshi.set(0xFF, 0xA0, 0xBE, 0xFF);
  Yoshi.mMaxVSpdStartFlutter = -2.0f;
  Yoshi.mFlutterAcceleration = 1.1f;
  Yoshi.mMaxFlutterTimer = 120;
  Yoshi.mYoshiHungry = false;
  Yoshi.mIsEggFree = false;

  Music.mVolume = 0.75f;
  Music.mSpeed = 1.0f;
  Music.mPitch = 1.0f;
  Music.mPlayMusic = true;
  Music.mMusicID = 1;
  Music.mAreaID = 1;
  Music.mEpisodeID = 0;

  Fludd.mPrimaryNozzle = 0;
  Fludd.mSecondaryNozzle = 4;
  Fludd.mWaterColor.set(0x3C, 0x46, 0x78, 0x14);
  Fludd.mIsColorWater = false;
}

#else



#endif