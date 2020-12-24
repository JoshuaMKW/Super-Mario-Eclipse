#ifndef MARIOPARAMS_H
#define MARIOPARAMS_H

#include "types.h"
#include "sms/JGeometry.hxx"
#include "sms/JUT.hxx"
#include "sms/actor/Mario.hxx"

class MarioParamsFile
{

public:
    enum FluddCleanType
    {
        NONE,
        CLEAN,
        GOOP
    };

    struct
    {
        u8 mJumpCount;             //0x0000
        bool mCanRideYoshi;        //0x0001
        bool mCanUseFludd;         //0x0002
        bool mMarioHasHelmet;      //0x0003
        bool mMarioHasGlasses;     //0x0004
        bool mMarioHasShirt;       //0x0005
        u16 _01;                   //0x0006
        u16 mHealth;               //0x0008
        u16 mMaxHealth;            //0x000A
        u16 mOBStep;               //0x000C
        u16 mOBMax;                //0x000E
        f32 mSizeMultiplier;       //0x0010
        f32 _02[0x8 / 4];          //0x0014
        f32 mGravityMulti;         //0x001C
        f32 mBaseBounce1Multi;     //0x0020
        f32 mBaseBounce2Multi;     //0x0024
        f32 mBaseBounce3Multi;     //0x0028
        f32 mMaxFallNoDamageMulti; //0x002C
        f32 mBaseJumpHeightMulti;  //0x0030
        f32 mMultiJumpMultiplier;  //0x0034
        f32 mMultiJumpFSpeedMulti; //0x0038
        f32 mSpeedMultiplier;      //0x003C

        struct
        {
            bool mCanUseNozzle[8];        //0x0040
            JUtility::TColor mWaterColor; //0x0048
            u8 mCleaningType;             //0x004C
            u8 _00;                       //0x004D
            u16 _01;                      //0x004E
            bool mBindToJointID[8];       //0x0050
            bool mCanCleanSeals;          //0x0058
            u8 _02;                       //0x0059
            u16 _03;                      //0x005A
        } FluddAttrs;

        f32 mWaterHealthMultiplier;   //0x005C
        s32 mNameOffset;              //0x0060
        f32 mThrowPowerMultiplier;    //0x0064
        f32 mSlideStrengthMultiplier; //0x0068
        s16 mWallHangMax;             //0x006C
        bool mGoopAffected;           //0x006E
        bool mCanHoldNPCs;            //0x006F
        bool mCanClimbWalls;          //0x0070

    } Attributes;

    void initializeMario(TMario *player);
};

struct MarioParams
{
    MarioParamsFile *mParams; //0x0000
    f32 mTerminalVelocity;    //0x0004

    struct
    {
        bool mIsAirborn : 1;
        bool mIsFaceUsed : 1;
        bool mIsSpinBounce : 1;
        bool mIsDisableInput : 1;
        u16 _00 : 16;
        u8 _01 : 4;
    } CollisionFlags; //0x0008

    u16 mPrevCollision;  //0x000A
    s32 mCollisionTimer; //0x000C
    bool mCollisionUsed; //0x0010
    u8 mMaxJumps;        //0x0011
    u8 mCurJump;         //0x0012
    s8 mPlayerID;        //0x0013

    struct
    {
        u8 mMainNozzle;   //0x0014
        u8 mSecondNozzle; //0x0015
        s32 mWaterLevel;  //0x0016
        bool mHadFludd;   //0x001A
        u8 _00;           //0x001B
    } FluddHistory;       //0x0014

    f32 mSizeContext;              //0x001C
    MarioParamsFile *_mBaseParams; //0x0020

    struct
    {
        f32 mGravity;
        f32 mBaseBounce1;
        f32 mBaseBounce2;
        f32 mBaseBounce3;
        f32 mMaxFallNoDamage;
        f32 mThrowPower;

        f32 mOceanOfs;
        f32 mWaterJumpHeightDifMax;
        f32 mWaterHealthDrainSpd;
        f32 mWaterHealthScubaDrainSpd;
        f32 _00[5];
    } DefaultAttrs; //0x0024

    bool _mFirstParamsDone; //0x0060

    JGeometry::TVec3<f32> mYoshiWaterSpeed;

    bool isMario();
    bool canUseNozzle(u8 nozzle);
    char *getPlayerName();
    u16 getPlayerKey();
};

#endif