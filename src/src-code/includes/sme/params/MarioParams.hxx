#pragma once

#include "types.h"
#include "mem.h"

#include "sms/JSystem/JGeometry.hxx"
#include "sms/JSystem/JUT/JUTColor.hxx"
#include "sms/actor/Mario.hxx"
#include "funcs.hxx"

class MarioParamsFile
{

public:
    enum FluddCleanType
    {
        NONE,
        CLEAN,
        GOOP
    };

    struct Attributes
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

        struct FluddAttrs
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
        s16 mMaxClimbQuarterFrames;   //0x0072
    } Attributes;
};

class MarioParams
{
    TMario *mPlayer; //0x0000

public:
    MarioParams() {}
    MarioParams(TMario *player, bool isMario)
    {
        this->init(player, isMario);
    }

    MarioParamsFile *mParams;      //0x0004
    MarioParamsFile *_mBaseParams; //0x0008
    f32 mTerminalVelocity;         //0x000C

    struct
    {
        bool mIsAirborn : 1;
        bool mIsFaceUsed : 1;
        bool mIsSpinBounce : 1;
        bool mIsDisableInput : 1;
        u16 _00 : 16;
        u8 _01 : 4;
    } CollisionFlags; //0x0010

    u16 mPrevCollision;          //0x0014
    s32 mCollisionTimer;         //0x0016
    bool mCollisionUsed;         //0x001A
    u8 mMaxJumps;                //0x001B
    u8 mCurJump;                 //0x001C
    s8 mPlayerID;                //0x001D
    s16 mClimbQuarterFramesLeft; //0x001E
    bool mIsClimbTired;          //0x0020

    struct
    {
        u8 mMainNozzle;   //0x0024
        u8 mSecondNozzle; //0x0025
        s32 mWaterLevel;  //0x0026
        bool mHadFludd;   //0x002A
    } FluddHistory;

    f32 mSizeContext; //0x002C

    struct
    {
        f32 mGravity;
        f32 mTwirlGravity;
        f32 mDiveStartSpeed;
        f32 mBaseBounce1;
        f32 mBaseBounce2;
        f32 mBaseBounce3;
        f32 mMaxFallNoDamage;
        f32 mThrowPower;
        f32 mAttackHeight;
        f32 mAttackRadius;
        f32 mReceiveHeight;
        f32 mReceiveRadius;
        f32 mOceanOfs;
        f32 mWaterJumpHeightDiffMax;
        f32 mWaterMaxDiffToInteract;
        f32 mWaterEntryMaxHeight;
        f32 mWaterHealthDrainSpd;
        f32 mWaterHealthScubaDrainSpd;
        f32 mWaterYSpdMultiplier;
        f32 mWaterHighBuoyant;
        f32 mWaterLowBuoyant;
        f32 mCollisionHeight;
        f32 mCollisionWidth;
        f32 mRunSpeedMulti;
        f32 mMaxGroundSpeed;
        f32 mHipDropSpeedSlow;
        f32 mHipDropSpeedFast;
        f32 mPoleGrabDistPadding;
        f32 mPoleClimbSpeedMul;
        f32 mHoverYSpdMultiplier;
        f32 mHoverMaxHeight;
        f32 mTurboNozzleSpeed;
        f32 mTurboJumpFSpeed;
        f32 mTurboJumpYSpeed;
    } DefaultAttrs; //0x002C

    JGeometry::TVec3<f32> mYoshiWaterSpeed;

private:
    bool mIsEMario;
    bool mInitialized;

    //FUNCS

    void _setDefaults();

public:
    const MarioParamsFile *getFile() const { return this->_mBaseParams; };
    const TMario *getPlayer() const { return this->mPlayer; };
    const u16 getPlayerKey() { return calcKeyCode__Q26JDrama8TNameRefFPCc(this->getPlayerName()); };
    bool isMario() const { return !this->mIsEMario; };
    bool isInitialized() const { return this->mInitialized; };
    void reset() { memcpy(this->mParams, this, (sizeof(MarioParamsFile))); };

    bool canUseNozzle(TWaterGun::NozzleType nozzle) const;
    const char *getPlayerName() const;
    void init(TMario *player, bool isMario);
    void update();
    bool setFile(MarioParamsFile *data);
    void setPlayer(TMario *player);
};