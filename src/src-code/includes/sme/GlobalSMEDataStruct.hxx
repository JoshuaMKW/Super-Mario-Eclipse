#pragma once

#include "types.h"
#include "sms/JGeometry.hxx"
#include "sms/JKR.hxx"
#include "sms/actor/Mario.hxx"

#include "collision/WarpCollision.hxx"
#include "params/MarioParams.hxx"
#include "params/StageParams.hxx"

enum class LightContext : u8
{
    DISABLED,
    STATIC,
    FOLLOWPLAYER
};

struct GlobalSMEDataStruct
{
    SMEFile *mCurrentStageConfig; //0x0000

    struct 
    {
        bool mIsCompletionRewards;
        bool mIsAudioStreaming;
        bool mIsEmulated;
        bool mIsAudioStreamAllowed;
        bool mIsFreePlay;
    } Context;
    
    struct
    {
        LightContext mLightType;
        s32 mPrevShineCount;
        bool mSizeMorphing;
        JGeometry::TVec3<f32> mShineShadowCoordinates;
        f32 mPrevSize;
        f32 mNextSize;
        f32 mShineShadowBase;
        f32 mStepContext;
    } Light;

    struct
    {
        TMario *mPlayerArray[4];
        u8 mCurPlayerCount;
        u8 mMaxPlayerCount;
        bool mIsActive[4];
    } PlayerData;

    u32 *mPRMFile;
    WarpCollisionList *mWarpColArray;
    WarpCollisionList *mWarpColPreserveArray;
    JKRHeap *mCharacterHeap;
    JKRHeap *mGame6Heap;
    JKRHeap *mGlobalsHeap;

    TMario *getPlayerByID(u8 id) const;
    constexpr bool isMusicBeingStreamed() const { return this->Context.mIsAudioStreaming; };
    constexpr bool isFreePlayMode() const { return this->Context.mIsFreePlay; };
};