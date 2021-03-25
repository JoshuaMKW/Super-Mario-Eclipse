#pragma once

#include "types.h"
#include "sms/JSystem/JGeometry.hxx"
#include "sms/JSystem/JKR/JKRHeap.hxx"
#include "sms/actor/Mario.hxx"

#include "collision/WarpCollision.hxx"
#include "params/MarioParams.hxx"
#include "params/StageParams.hxx"

#include "SME.hxx"

namespace SME
{

class TGlobals
{
    SME::Class::TSMEFile *mStageConfig;
    SME::Class::TPlayerParams *mPlayerCfgArray[4];
    bool mIsCompletionRewards;
    bool mIsAudioStreaming;
    bool mIsAudioStreamAllowed;
    bool mIsFreePlay;
    
    struct
    {
        SME::Enum::LightContext mLightType;
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

    void *mPRMFile;
    SME::Class::TWarpCollisionList *mWarpColArray;
    SME::Class::TWarpCollisionList *mWarpColPreserveArray;
    JKRExpHeap *mCharacterHeap;
    JKRExpHeap *mGame6Heap;
    JKRExpHeap *mGlobalsHeap;

    TMario *getPlayerByID(u8 id) const;

    SME::Class::TSMEFile *getStageConfig() const { return mStageConfig; };
    SME::Class::TPlayerParams *getPlayerParams(u8 id) const;
    SME::Class::TPlayerParams *getPlayerParams(TMario *player) const;

    constexpr bool isCompletionRewarded() const { return mIsCompletionRewards; }
    constexpr bool isFreePlayMode() const { return mIsFreePlay; }
    constexpr bool isMusicBeingStreamed() const { return mIsAudioStreaming; }
    constexpr bool isMusicStreamingAllowed() const { return mIsAudioStreamAllowed; }
};

}