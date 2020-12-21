#ifndef INFOMANAGER_H
#define INFOMANAGER_H

#include "types.h"
#include "sms/JGeometry.hxx"
#include "sms/JKR.hxx"
#include "sms/actor/Mario.hxx"

#include "collision/WarpCollision.hxx"
#include "params/MarioParams.hxx"
#include "params/StageParams.hxx"

class SMEInfoManager
{
public:
    SMEFile *mFile; //0x0000

    struct
    {
        JGeometry::TVec3<f32> mShineShadowCoordinates; //0x0000
        u16 mPrevShineCount;                           //0x0004
        bool mSizeMorphing;                            //0x0006
        u8 mLightType;                                 //0x0007
        f32 mPrevSize;                                 //0x0008
        f32 mNextSize;                                 //0x000C
        f32 mShineShadowBase;                          //0x001C
        f32 mStepContext;                              //0x0020
    } Light;

    struct
    {
        TMario *mMario[4];     //0x003C
        u8 mCurPlayerCount;    //0x004C
        u8 mMaxPlayerCount;    //0x004D
        bool mIsActive[4];     //0x004E
        s8 mCurPlayerID[4];    //0x0052
        u8 mCurPlayerTimer[4]; //0x0056
        u8 mMaxPlayerTimer;    //0x005A
        bool mIsFreePlay;      //0x005B
    } PlayerData;

    JKRHeap *mCharacterHeap;                  //0x005C
    bool mIsCompletionRewards;                //0x0060
    bool mIsAudioStreaming;                   //0x0061
    bool mIsEmulated;                         //0x0062
    bool mIsAudioStreamAllowed;               //0x0063
    u32 *mPRMFile;                            //0x0064
    WarpCollisionList *mWarpColArray;         //0x0068
    WarpCollisionList *mWarpColPreserveArray; //0x0070
    JKRHeap *mGame6Heap;                      //0x0074
};

#endif