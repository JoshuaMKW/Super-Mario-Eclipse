#pragma once

#include "DVD.h"
#include "ctype.h"
#include "sms/JSystem/JGeometry.hxx"
#include "sms/JSystem/JUT/JUTColor.hxx"
#include "sms/sound/MSound.hxx"
#include "string.h"
#include "types.h"

#include "libs/sMemory.hxx"
#include "libs/sString.hxx"

namespace SME::Enum {
enum class LightContext : u8 { DISABLED, STATIC, FOLLOWPLAYER };
}

namespace SME::Class {

class TSMEFile {
  static bool openFile(const char *path);

public:
  struct {
    u32 mMAGIC;         // 0x0000 ("CODE")
    void *mLoadAddress; // 0x0004
    u32 mFileSize;      // 0x0008
    void *mCallBack;    // 0x000C
  } FileHeader;

  struct {
    bool mIsShineShadow;                      // 0x0010
    bool mIsMario;                            // 0x0011
    bool mIsYoshi;                            // 0x0012
    bool mIsMusic;                            // 0x0013
    bool mIsFludd;                            // 0x0014
    SME::Enum::LightContext mShineShadowFlag; // 0x0015
    s8 mPlayerID;                             // 0x0016

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

    u8 _19;
    u16 _1A;
    u32 _1C;
  } GlobalFlags;

  struct {
    JGeometry::TVec3<f32> mCoordinates; // 0x0020
    f32 mSize;                          // 0x002C
    f32 mStep;                          // 0x0030
    JUtility::TColor mColor;            // 0x0034
    u8 mLayerCount;                     // 0x0038
    u8 mDarkLevel;                      // 0x0039
    u16 _00;                            // 0x003A
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
    u8 _8D;
    u16 _8E;
  } Music;

  struct {
    u8 mPrimaryNozzle;            // 0x0090
    u8 mSecondaryNozzle;          // 0x0091
    JUtility::TColor mWaterColor; // 0x0092
    bool mIsColorWater;           // 0x0096
  } Fludd;

  static constexpr u32 MAGIC = 0x434F4445;
  static TSMEFile mStageConfig;

  static bool load(const char *stageName);
  static void reset() { memset(&TSMEFile::mStageConfig, 0, sizeof(TSMEFile)); };
  static char *withSMEExtension(char *dst, const char *stage,
                                bool generalize = false);
};

} // namespace SME::Class