#pragma once

#include "defines.h"
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
    TGlobals();

    SME::Class::TSMEFile *mStageConfig;
    SME::Class::TPlayerParams *mPlayerCfgArray[SME_MAX_PLAYERS];
    TMario *mPlayers[SME_MAX_PLAYERS];
    bool mIsCompletionRewards;
    bool mIsAudioStreaming;
    bool mIsAudioStreamAllowed;
    bool mIsFreePlay;
    u8 mActivePlayers;
    u8 mMaxPlayers;
    
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

    u8 getActivePlayers() const { return mActivePlayers; }
    u8 getMaxPlayers() const { return mMaxPlayers; }
    TMario *getPlayerByIndex(u8 index) const;
    SME::Class::TSMEFile *getStageConfig() const { return mStageConfig; }
    SME::Class::TPlayerParams *getPlayerParams(u8 id) const;
    SME::Class::TPlayerParams *getPlayerParams(TMario *player) const;

    void setPlayerByIndex(u8 index, TMario *player);

    void registerPlayerParams(SME::Class::TPlayerParams *params);
    void deregisterPlayerParams(SME::Class::TPlayerParams *params);
    void clearAllPlayerParams();

    constexpr bool isCompletionRewarded() const { return mIsCompletionRewards; }
    constexpr bool isFreePlayMode() const { return mIsFreePlay; }
    constexpr bool isMultiplayerActive() const { return mActivePlayers > 1; }
    constexpr bool isMusicBeingStreamed() const { return mIsAudioStreaming; }
    constexpr bool isMusicStreamingAllowed() const { return mIsAudioStreamAllowed; }

    static SME::TGlobals sGlobals;
};

}