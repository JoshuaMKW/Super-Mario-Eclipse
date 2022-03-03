#pragma once

#include "types.h"

constexpr size_t OBJDataTableSize = 360;
const char gLiveManagerName[]{0x96, 0xD8, 0x83, 0x7D, 0x83, 0x6C, 0x81, 0x5B,
                              0x83, 0x57, 0x83, 0x83, 0x81, 0x5B, 0x00, 0x00};
const char gUnkManagerName[]{0x93, 0x47, 0x83, 0x4F, 0x83, 0x8B,
                             0x81, 0x5B, 0x83, 0x76, 0x00, 0x00};

enum MapColType { MAP_STATIC, MAP_MOVE, MAP_WARP };

struct obj_info {
  u16 _00;
  u16 _02;
  void *_04;
};

struct sink_data {
  f32 _00;
  f32 _04;
};

struct sound_data {
  u32 _00;
  u32 _04;
  u32 _08;
  u32 mUnkSoundID;
  u32 mReboundSoundID;
  u32 _14;
  u32 _18;
  u32 _1C;
  u32 _20;
  u32 _24;
};

struct sound_info {
  size_t mLength;
  sound_data *mSoundData;
};

struct anim_data {
  char *mMdlName;
  char *mName;
  u32 _08;
  u32 _0C;
  u32 _10;
};

struct hit_data {
  f32 mAttackRadius;
  f32 mAttackHeight;
  f32 mReceiveRadius;
  f32 mReceiveHeight;
};

struct obj_hit_info {
  u32 _00;
  u32 mType;
  u32 _08;
  hit_data *mHitData;
};

struct map_col_data {
  const char *mColName;
  MapColType mColType;
};

struct map_col_info {
  size_t mColNum;
  map_col_data *mColData;
};

struct ObjPhysicalData {
  f32 mGravity;
  f32 mFloorBounceSpeed;
  f32 mWallBounceSpeed;
  f32 mRotationStopFactor; //?
  f32 mFloorBrakeFactor;
  f32 mRollBrakeFactor; //?
  f32 mAirBrakeFactor;
  f32 mRollBrakeFactor2;
  f32 _20[0xC / 4];
  f32 mThrowDistance;
  f32 mThrowHeight;
};

struct ObjPhysicalInfo {
  u32 _00; // Always 0xD?
  ObjPhysicalData *mPhysicalData;
  u32 _0C;
};

struct ObjData {
  const char *mMdlName;
  u32 mObjectID;
  const char *mLiveManagerName;
  const char *mObjKey;
  void *mAnimInfo;
  obj_hit_info *mObjCollisionData;
  obj_info *mMapCollisionInfo;
  sound_info *mSoundInfo;
  ObjPhysicalInfo *mPhysicalInfo;
  sink_data *mSinkData;
  void *_28;
  void *mBckMoveData;
  f32 _30;
  u32 mUnkFlags;
  u32 mKeyCode; //?
};

extern ObjData *sObjDataTable[OBJDataTableSize];