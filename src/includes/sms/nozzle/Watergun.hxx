#pragma once

#include "JGeometry.hxx"
#include "sms/nozzle/NozzleBase.hxx"
#include "sms/nozzle/NozzleTrigger.hxx"
#include "sms/params/Params.hxx"
#include "types.h"


class TMario;

struct TWaterEmitInfo : public TParams {
  TWaterEmitInfo(const char *prm);

  TParamT<s32> mNum;
  TParamT<s16> mAlive;
  TParamT<s16> mAttack;
  TParamT<JGeometry::TVec3<f32>> mDir;
  TParamT<JGeometry::TVec3<f32>> mPos;
  TParamT<JGeometry::TVec3<f32>> mV;
  TParamT<f32> mDirTremble;
  TParamT<f32> mPow;
  TParamT<f32> mPowTremble;
  TParamT<f32> mSize;
  TParamT<f32> mSizeTremble;
  TParamT<f32> mHitRadius;
  TParamT<f32> mHitHeight;
  TParamT<s32> mFlag;
  TParamT<s16> mType;

private:
  TParamT<s16> __padding;
};

class TWaterGun {

public:
  enum NozzleType { Spray, Rocket, Underwater, Yoshi, Hover, Turbo, Sniper };

  u32 _00[0x8 / 4];                      // 0x0000
  TMario *mMario;                        // 0x0008
  TNozzleBase mNozzleDeform;             // 0x000C
  TNozzleTrigger mNozzleDeformBomb;      // 0x0390
  TNozzleTrigger mNozzleRocket;          // 0x0720
  TNozzleBase mNozzleUnderWater;         // 0x0AB0
  TNozzleBase mNozzleYoshiDeform;        // 0x0E34
  TNozzleTrigger mNozzleYoshiDeformBomb; // 0x11B8
  TNozzleTrigger mNozzleHover;           // 0x1548
  TNozzleTrigger mNozzleTurbo;           // 0x18D8
  TNozzleBase *mNozzleList[6];           // 0x1C68
  s32 mCurrentWater;                     // 0x1C80
  u8 mCurrentNozzle;                     // 0x1C84
  u8 mSecondNozzle;                      // 0x1C85
  bool mIsEmitWater;                     // 0x1C86
  u8 _13;                                // 0x1C87
  u32 _14[0x8 / 4];                      // 0x1C88
  JGeometry::TVec3<f32> mGeometry[10];   // 0x1C90
  u32 _15;                               // 0x1D08
  TWaterEmitInfo *mEmitInfo;             // 0x1D0C
  u32 _16;                               // 0x1D10
  char *mPrmPath;                        // 0x1D14
  char *mRocketPath;                     // 0x1D18
  u32 _17[0x24 / 4];                     // 0x1D1C
  float mHoverMaxHeight;                 // 0x1D40
  u32 _18[0x78 / 4];                     // 0x1D44
  // 1DBC
};