#include "types.h"

#include "sms/mapobj/MapObjInit.hxx"
#include "obj/WaterBalloon.hxx"

#include "SME.hxx"

constexpr u16 cexp_calcKeyCode(const char *str) {
  u32 i = 0;
  for (; *str != '\0'; i = (*str++) + i * 3) {
  }
  return i;
}

static constexpr size_t OBJNewCount = 2;

static constexpr size_t LoadAddrTableSize = 2;
static u16 *sObjLoadAddrTable[LoadAddrTableSize][2]{
    {(u16 *)SME_PORT_REGION(0x801B1772, 0, 0, 0),
     (u16 *)SME_PORT_REGION(0x801B178A, 0, 0, 0)},
    {(u16 *)SME_PORT_REGION(0x801B1AF2, 0, 0, 0),
     (u16 *)SME_PORT_REGION(0x801B1AFA, 0, 0, 0)}};

static char sLiveManagerName[]{0x96, 0xD8, 0x83, 0x7D, 0x83, 0x6C, 0x81, 0x5B,
                               0x83, 0x57, 0x83, 0x83, 0x81, 0x5B, 0x00, 0x00};

// clang-format off
hit_data waterBalloon_hit_data{
    ._00 = 150.0f,
    ._04 = 60.0f,
    ._08 = 60.0f,
    ._0C = 90.0f
};

obj_hit_info waterBalloon_collision_data{
    ._00 = 1,
    .mType = 0xDC000000,
    ._08 = 0,
    .mHitData = &waterBalloon_hit_data
};

sound_data waterBalloon_sound_data{
    ._00 = 0xFFFFFFFF,
    ._04 = 0x3801,
    ._08 = 0xFFFFFFFF,
    .mUnkSoundID = 0x1807,
    .mReboundSoundID = 0xFFFFFFFF,
    ._14 = 0xFFFFFFFF,
    ._18 = 0xFFFFFFFF,
    ._1C = 0xFFFFFFFF,
    ._20 = 0xFFFFFFFF,
    ._24 = 0xFFFFFFFF,
};

sound_info waterBalloon_sound_info{
    .mLength = 10,
    .mSoundData = &waterBalloon_sound_data
};

ObjPhysicalData waterBalloon_physical_data{
    .mGravity = 0.3f,
    .mFloorBounceSpeed = 0.3f,
    .mWallBounceSpeed = 0.1f,
    ._0C = 0.1f,
    .mFloorBrakeFactor = 0.9f,
    ._14 = 0.97f,
    .mAirBrakeFactor = 1.0f,
    ._1C = {5.0f, 0.5f, 0.7f, 0.0f},
    .mThrowDistance = 2.0f,
    .mThrowHeight = 15.0f
};

ObjPhysicalInfo waterBalloon_physical_info{
    ._00 = 13,
    .mPhysicalData = &waterBalloon_physical_data,
    ._0C = 2
};

ObjData waterBalloonData{
    .mMdlName = const_cast<char *>("waterballoon"),
    .mObjectID = 0x40000420,
    .mLiveManagerName =
        sLiveManagerName, // const_cast<char *>("木マネージャー")
    .mObjKey = nullptr, // const_cast<char *>("waterballoon"),
    ._10 = 0,
    .mObjCollisionData = &waterBalloon_collision_data,
    .mMapCollisionInfo = nullptr,
    .mSoundInfo = &waterBalloon_sound_info,
    .mPhysicalInfo = &waterBalloon_physical_info,
    .mSinkData = nullptr,
    ._28 = nullptr,
    .mBckMoveData = nullptr,
    ._30 = 50.0f,
    .mUnkFlags = 0x02130100,
    .mKeyCode = cexp_calcKeyCode("waterballoon")
};

ObjData blowWindData{
    .mMdlName = const_cast<char *>("blowwind"),
    .mObjectID = 0x40000421,
    .mLiveManagerName =
        sLiveManagerName, // const_cast<char *>("木マネージャー")
    .mObjKey = nullptr, // const_cast<char *>("blowwind"),
    ._10 = 0,
    .mObjCollisionData = nullptr,
    .mMapCollisionInfo = nullptr,
    .mSoundInfo = nullptr,
    .mPhysicalInfo = nullptr,
    .mSinkData = nullptr,
    ._28 = nullptr,
    .mBckMoveData = nullptr,
    ._30 = 50.0f,
    .mUnkFlags = 0,
    .mKeyCode = cexp_calcKeyCode("blowwind")
};
// clang-format on

static ObjData *sObjDataTableNew[OBJDataTableSize + OBJNewCount];

extern TWaterBalloon *sWaterBalloon;

// SME.cpp
extern void makeExtendedObjDataTable() {
  memcpy(sObjDataTableNew, sObjDataTable,
         sizeof(u32) * OBJDataTableSize - 1); // last entry is default null
  sObjDataTableNew[OBJDataTableSize - 1] = &waterBalloonData;
  sObjDataTableNew[OBJDataTableSize] = &blowWindData;
  sObjDataTableNew[OBJDataTableSize + 1] = sObjDataTable[OBJDataTableSize - 1];
  {
    u32 addr = reinterpret_cast<u32>(&sObjDataTableNew);
    u16 lo = addr;
    u16 hi = (addr >> 16) + (lo >> 15);
    for (u32 i = 0; i < LoadAddrTableSize; ++i) {
      Memory::PPC::write<u16>(sObjLoadAddrTable[i][0], hi);
      Memory::PPC::write<u16>(sObjLoadAddrTable[i][1], lo);
    }
  }
}

void objects_staticResetter() {
    sWaterBalloon = nullptr;
}