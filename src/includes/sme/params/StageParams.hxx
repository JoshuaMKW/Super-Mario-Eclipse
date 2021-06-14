#pragma once

#include "DVD.h"
#include "ctype.h"
#include "sms/JSystem/JGeometry.hxx"
#include "sms/JSystem/JSU/JSUFileStream.hxx"
#include "sms/JSystem/JUT/JUTColor.hxx"
#include "sms/sound/MSound.hxx"
#include "string.h"
#include "types.h"

#include "LightContext.hxx"
#include "Player.hxx"
#include "libs/sMemory.hxx"
#include "libs/sString.hxx"

namespace SME::Class {

#ifdef SME_DEPRECATED_CFG
class TSMEFile {
public:
  static constexpr u32 MAGIC = 'CODE';
  static TSMEFile *sStageConfig;

  static bool load(const char *stageName);
  static char *withSMEExtension(char *dst, const char *stage,
                                bool generalize = false);

  TSMEFile();
  TSMEFile(JSUFileInputStream &stream);

  bool isMagicValid() const { return mMagic == TSMEFile::MAGIC; }
  void reset();

  u32 mMagic;

  struct {
    bool mIsShineShadow;                        // 0x0010
    bool mIsMario;                              // 0x0011
    bool mIsYoshi;                              // 0x0012
    bool mIsMusic;                              // 0x0013
    bool mIsFludd;                              // 0x0014
    TLightContext::ActiveType mShineShadowFlag; // 0x0015
    SME::Enum::Player mPlayerID;                // 0x0016

    struct {
      u8 _00 : 4;
      bool mIsExMap : 1;
      bool mIsDivingMap : 1;
      bool mIsOptionMap : 1;
      bool mIsMultiPlayerMap : 1;
    } StageType; // 0x0017

    struct {
      u8 _00 : 4;
      bool mMarioHasFludd : 1;
      bool mMarioHasHelmet : 1;
      bool mMarioHasGlasses : 1;
      bool mMarioHasShirt : 1;
    } MarioStates; // 0x0018
  } GlobalFlags;

  struct {
    JGeometry::TVec3<f32> mCoordinates; // 0x0020
    f32 mSize;                          // 0x002C
    f32 mStep;                          // 0x0030
    JUtility::TColor mColor;            // 0x0034
    u8 mLayerCount;                     // 0x0038
    u8 mDarkLevel;                      // 0x0039
  } Light;

  struct {
    f32 mSpeedMulti;      // 0x003C
    f32 mGravity;         // 0x0040
    f32 mBaseBounce1;     // 0x0044
    f32 mBaseBounce2;     // 0x0048
    f32 mBaseBounce3;     // 0x004C
    f32 mMaxFallNoDamage; // 0x0050
    u16 mHealth;          // 0x0054
    u16 mMaxHealth;       // 0x0056
    u16 mOBStep;          // 0x0058
    u16 mOBMax;           // 0x005A
  } Mario;

  struct {
    s32 mMaxJuice;                 // 0x005C
    JUtility::TColor mGreenYoshi;  // 0x0060
    JUtility::TColor mOrangeYoshi; // 0x0064
    JUtility::TColor mPurpleYoshi; // 0x0068
    JUtility::TColor mPinkYoshi;   // 0x006C
    f32 mMaxVSpdStartFlutter;      // 0x0070
    f32 mFlutterAcceleration;      // 0x0074
    u16 mMaxFlutterTimer;          // 0x0078
    bool mYoshiHungry;             // 0x007A
    bool mIsEggFree;               // 0x007B
  } Yoshi;

  struct {
    f32 mVolume;     // 0x007C
    f32 mSpeed;      // 0x0080
    f32 mPitch;      // 0x0084
    bool mPlayMusic; // 0x0088
    u16 mMusicID;    // 0x0089
    u8 mAreaID;      // 0x008B
    u8 mEpisodeID;   // 0x008C
  } Music;

  struct {
    u8 mPrimaryNozzle;            // 0x0090
    u8 mSecondaryNozzle;          // 0x0091
    JUtility::TColor mWaterColor; // 0x0092
    bool mIsColorWater;           // 0x0096
  } Fludd;

private:
  static TSMEFile *loadFromFile(const char *path);
};
#else

struct TStageParams : public TParams {
#define CONSTRUCT_PARAM(name, val)                                             \
  name(this, val, JDrama::TNameRef::calcKeyCode(SME_STRINGIZE(name)),          \
       SME_STRINGIZE(name))

  TStageParams(const char *prm)
      : TParams(),
        CONSTRUCT_PARAM(mIsExStage, false),
        CONSTRUCT_PARAM(mIsDivingStage, false),
        CONSTRUCT_PARAM(mIsOptionStage, false),
        CONSTRUCT_PARAM(mIsMultiplayerStage, false),
        CONSTRUCT_PARAM(mIsYoshiHungry, false),
        CONSTRUCT_PARAM(mIsEggFree, false),
        CONSTRUCT_PARAM(mLightType, TLightContext::ActiveType::DISABLED),
        CONSTRUCT_PARAM(mLightPosX, 0.0f),
        CONSTRUCT_PARAM(mLightPosY, 3600.0f),
        CONSTRUCT_PARAM(mLightPosZ, -7458.0f),
        CONSTRUCT_PARAM(mLightSize, 8000.0f),
        CONSTRUCT_PARAM(mLightStep, 100.0f),
        CONSTRUCT_PARAM(mLightColor, JUtility::TColor(0, 20, 40, 0)),
        CONSTRUCT_PARAM(mLightLayerCount, 5),
        CONSTRUCT_PARAM(mLightDarkLevel, 120),
        CONSTRUCT_PARAM(mPlayerSelectWhiteList, 0xFFFFFFFF),
        CONSTRUCT_PARAM(mPlayerHealth, 8),
        CONSTRUCT_PARAM(mPlayerMaxHealth, 8),
        CONSTRUCT_PARAM(mPlayerHasFludd, true),
        CONSTRUCT_PARAM(mPlayerHasHelmet, false),
        CONSTRUCT_PARAM(mPlayerHasGlasses, false),
        CONSTRUCT_PARAM(mPlayerHasShirt, false),
        CONSTRUCT_PARAM(mPlayerCanRideYoshi, true),
        CONSTRUCT_PARAM(mPlayerCanHoldNPCs, false),
        CONSTRUCT_PARAM(mPlayerSizeMultiplier, 1.0f),
        CONSTRUCT_PARAM(mFluddPrimary, 0),
        CONSTRUCT_PARAM(mFluddSecondary, 4),
        CONSTRUCT_PARAM(mFluddWaterColor,JUtility::TColor(60, 70, 120, 20)),
        CONSTRUCT_PARAM(mYoshiMaxJuice, 21300),
        CONSTRUCT_PARAM(mYoshiMaxVSpdStartFlutter, -2.0f),
        CONSTRUCT_PARAM(mYoshiFlutterAcceleration, 1.2f),
        CONSTRUCT_PARAM(mYoshiMaxFlutterTimer, 120), 
        CONSTRUCT_PARAM(mYoshiColorGreen, JUtility::TColor(64, 161, 36, 255)),
        CONSTRUCT_PARAM(mYoshiColorOrange, JUtility::TColor(255, 140, 28, 255)),
        CONSTRUCT_PARAM(mYoshiColorPurple, JUtility::TColor(170, 76, 255, 255)),
        CONSTRUCT_PARAM(mYoshiColorPink, JUtility::TColor(255, 160, 190, 255)),
        CONSTRUCT_PARAM(mMusicVolume, 0.75f),
        CONSTRUCT_PARAM(mMusicSpeed, 1.0f),
        CONSTRUCT_PARAM(mMusicPitch, 1.0f),
        CONSTRUCT_PARAM(mMusicID, 1),
        CONSTRUCT_PARAM(mMusicAreaID, 1),
        CONSTRUCT_PARAM(mMusicEpisodeID, 0),
        CONSTRUCT_PARAM(mMusicEnabled, true),
        CONSTRUCT_PARAM(mGravityMultiplier, 1.0f),
        CONSTRUCT_PARAM(mMaxFrameRate, 30.0f) {
    delete sStageConfig;
    sStageConfig = this;

    load(prm);
  }

#undef CONSTRUCT_PARAM

  static TStageParams *sStageConfig;

  static char *stageNameToParamPath(char *dst, const char *stage,
                                    bool generalize = false);

  bool isCustomConfig() const { return mIsCustomConfigLoaded; }
  void load(const char *stageName);
  void reset();

  // Stage Info
  TParamRT<bool> mIsExStage;
  TParamRT<bool> mIsDivingStage;
  TParamRT<bool> mIsOptionStage;
  TParamRT<bool> mIsMultiplayerStage;
  TParamRT<bool> mIsYoshiHungry;
  TParamRT<bool> mIsEggFree;

  // Light Info
  TParamRT<TLightContext::ActiveType> mLightType;
  TParamRT<f32> mLightPosX;
  TParamRT<f32> mLightPosY;
  TParamRT<f32> mLightPosZ;
  TParamRT<f32> mLightSize;
  TParamRT<f32> mLightStep;
  TParamRT<JUtility::TColor> mLightColor;
  TParamRT<u8> mLightLayerCount;
  TParamRT<u8> mLightDarkLevel;

  // Player Info
  TParamRT<u32> mPlayerSelectWhiteList;
  TParamRT<u16> mPlayerHealth;
  TParamRT<u16> mPlayerMaxHealth;
  TParamRT<bool> mPlayerHasFludd;
  TParamRT<bool> mPlayerHasHelmet;
  TParamRT<bool> mPlayerHasGlasses;
  TParamRT<bool> mPlayerHasShirt;
  TParamRT<bool> mPlayerCanRideYoshi;
  TParamRT<bool> mPlayerCanHoldNPCs;
  TParamRT<f32> mPlayerSizeMultiplier;

  // Fludd Info
  TParamRT<u8> mFluddPrimary;
  TParamRT<u8> mFluddSecondary;
  TParamRT<JUtility::TColor> mFluddWaterColor;

  // Yoshi Info
  TParamRT<s32> mYoshiMaxJuice;
  TParamRT<f32> mYoshiMaxVSpdStartFlutter;
  TParamRT<f32> mYoshiFlutterAcceleration;
  TParamRT<u16> mYoshiMaxFlutterTimer;
  TParamRT<JUtility::TColor> mYoshiColorGreen;
  TParamRT<JUtility::TColor> mYoshiColorOrange;
  TParamRT<JUtility::TColor> mYoshiColorPurple;
  TParamRT<JUtility::TColor> mYoshiColorPink;

  // Music Info
  TParamRT<f32> mMusicVolume;
  TParamRT<f32> mMusicSpeed;
  TParamRT<f32> mMusicPitch;
  TParamRT<u16> mMusicID;
  TParamRT<u8> mMusicAreaID;
  TParamRT<u8> mMusicEpisodeID;
  TParamRT<bool> mMusicEnabled;

  // Global Info
  TParamRT<f32> mGravityMultiplier;
  TParamRT<f32> mMaxFrameRate;

  private:
  bool mIsCustomConfigLoaded;
};

#endif

} // namespace SME::Class