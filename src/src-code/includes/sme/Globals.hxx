#pragma once

#include "types.h"
#include "sms/JSystem/JGeometry.hxx"
#include "sms/JSystem/JKR/JKRHeap.hxx"
#include "sms/actor/Mario.hxx"

#include "collision/WarpCollision.hxx"
#include "params/MarioParams.hxx"
#include "params/StageParams.hxx"

namespace SME
{

struct TGlobals
{
    SME::Class::TSMEFile *mStageConfig;
    SME::Class::TPlayerParams *mPlayerCfgArray[4];
    bool mIsCompletionRewards;
    bool mIsAudioStreaming;
    bool mIsAudioStreamAllowed;
    bool mIsFreePlay;
    
    struct
    {
        JGeometry::TVec3<f32> mShineShadowCoordinates;
        s32 mPrevShineCount;
        f32 mPrevSize;
        f32 mNextSize;
        f32 mShineShadowBase;
        f32 mStepContext;
        SME::Enum::LightContext mLightType;
        bool mSizeMorphing;
    } mLightData;

    void *mPRMFile;
    SME::Class::TWarpCollisionList *mWarpColArray;
    SME::Class::TWarpCollisionList *mWarpColPreserveArray;
    JKRExpHeap *mCharacterHeap;
    JKRExpHeap *mGame6Heap;
    JKRExpHeap *mGlobalsHeap;
    bool mPlayerHasGeckoCodes;

    TMario *getPlayerByID(u8 id) const;

    SME::Class::TSMEFile *getStageConfig() const { return mStageConfig; };
    SME::Class::TPlayerParams *getPlayerParams(u8 id) const;
    SME::Class::TPlayerParams *getPlayerParams(TMario *player) const;
    void registerPlayerParams(SME::Class::TPlayerParams *params);
    void deregisterPlayerParams(SME::Class::TPlayerParams *params);

    constexpr bool isCompletionRewarded() const { return mIsCompletionRewards; }
    constexpr bool isFreePlayMode() const { return mIsFreePlay; }
    constexpr bool isMusicBeingStreamed() const { return mIsAudioStreaming; }
    constexpr bool isMusicStreamingAllowed() const { return mIsAudioStreamAllowed; }

    static SME::TGlobals sGlobals;
};

}