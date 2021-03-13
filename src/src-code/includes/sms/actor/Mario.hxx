#pragma once

#include "types.h"
#include "sms/JSystem/JGeometry.hxx"
#include "sms/JSystem/J3D/J3DDrawBuffer.hxx"

#include "sms/actor/TakeActor.hxx"
#include "sms/actor/Yoshi.hxx"
#include "sms/m3d/M3UModel.hxx"
#include "sms/collision/BGCheck.hxx"
#include "sms/nozzle/Watergun.hxx"

#include "params/MarioParams.hxx"

class TMario;

struct TMarioAnimeData
{
    enum FLUDD
    {
        FLUDD_ENABLED = 68,
        FLUDD_DISABLED = 200
    };

    enum HAND
    {
        HAND_A,
        HAND_B,
        HAND_C
    };

    bool isPumpOK() const;

    u16 mAnimID;
    u16 mFluddEnabled; //Note: ENUM? 68 = enabled, 200 = disabled
    u8 mAnmTexPattern; //Note: 23 is the max value allowed
    u8 mMarioHand;     //Note: 1, 2, and 3 seem to be valid values, this determines the hand model used(?)
    u8 unk_1;          //Note: If bit 30 is set, it seems to activate a bit flag to multiple pointers, likely mtx related
    u8 unk_2;          //Note: Value seems to be set but is never used?
};

class TMarioCap
{
public:
    TMarioCap(TMario *);

    virtual perform(u32, JDrama::TGraphics *);
    
    void createMirrorModel();
    void mtxEffectHide();
    void mtxEffectShow();
};

class M3UModelMario : public M3UModel
{
public:
    virtual void updateIn() override;
    virtual void updateOut() override;
    
    void changeMtxCalcSIAnmBQAnmTransform(int, int, u8);
    void updateInMotion() override;
};

class TMarioSoundValues
{
public:
    TMarioSoundValues();

    u32 _0;
    u32 _4;
    u32 _8;
    u32 _C;
    u32 _10;
    u32 _14;
    u8 _18;
    u32 _1C;
    u8 _20;
    u16 _22;
    u8 _24;
    u16 _26;
    u8 _2A;
    u8 _2B;
    u8 _2C;
};

struct TMarioControllerWork
{
    enum Buttons
    {
        R = 0x20,
        A = 0x100,
        B = 0x200
    };

    s16 mStickHS16;
    s16 mStickVS16;
    Buttons mInput;
    Buttons mFrameInput;
    u8 mAnalogRU8;
    u8 mAnalogLU8;
    u16 _0E;
    f32 mStickH;
    f32 mStickV;
    f32 mStickDist;
    f32 mAnalogL;
    f32 mAnalogR;
};

class TMarioGamePad : public JUTGamePad
{
public:
    virtual ~TMarioGamePad();

    void onNeutralMarioKey();
    void read();
    void reset();
    void updateMeaning();

    f32 _A8;
    f32 _AC;
    f32 _B0;
    f32 _B4;
    f32 _B8;
    f32 _BC;
    f32 mStickX; // _C0
    f32 mStickY; // _C4
    f32 _C8;
    f32 _CC;
    u32 mMeaning;
    u32 _D4;
    u32 _D8;
    u16 _DC;
    u16 _DE;
    u16 _E0;

    struct
    {
        u16 _00 : 8;
        bool mDisable : 1;
        u8 _01 : 5;
        bool mReadInput : 1;
        u8 _02 : 1;
    } State; //0x00E2

    u16 _E4;
    u16 _E6; // padding?
    u32 _E8;
    u32 _EC; // padding?
};

class TMario : public TTakeActor
{

public:
    enum State
    {
        NUMBER = 0x0000000F,
        DOJUMP = 0x00000080,
        AIRBORN = 0x00000800,
        CUTSCENE = 0x00001000,
        WATERBORN = 0x00002000,
        RUNNING = 0x04000440,
        IDLE = 0x0C400201,
        STOP = 0x0C00023D,
        SPIN = 0x00000441,
        JUMPSPIN = 0x00000890,
        JUMPSPIN1 = 0x00000895,
        JUMPSPIN2 = 0x00000896,
        JUMP = 0x02000880,
        D_JUMP = 0x02000881,
        TRIPLE_J = 0x00000882,
        JMP_LAND = 0x04000470,
        HVY_LAND = 0x04000473,
        D_LAND = 0x04000472,
        T_LAND = 0x0800023A,
        JUMPSIDE = 0x00000880,
        BOUNCE = 0x00000884,
        SIDESTEP = 0x0C008220,
        SIDE_FLIP = 0x00000887,
        FALL = 0x0000088C,
        SWIM = 0x000024D7,
        DIVE = 0x0080088A,
        DIVEJUMP = 0x02000889,
        DIVESLIDE = 0x00800456,
        GOOPSLIDE = 0x0084045D,
        CLIMB = 0x18100340,
        CLIMBUP = 0x10100343,
        WALLJUMP = 0x02000886,
        WALLSLIDE = 0x000008A7,
        ROOFHANG = 0x00200349,
        HANG = 0x3800034B,
        HANGCLIMB = 0x3000054F,
        SLAMSTART = 0x008008A9,
        SLAM = 0x0080023C,
        SPRAY = 0x0C008220,
        THROWN = 0x000208B8,
        HOVER = 0x0000088B,
        STUCK = 0x0002033C,
        TALKING = 0x10001308,
        TURNING = 0x00000444,
        YOSHI_ESC = 0x0000089C,
        SHINE_C = 0x00001302, // Collected Shine Sprite
        DEATH = 0x00020467,
        DOOR_F_O = 0x00001321, // Door open fail
        WALL_S_L = 0x04000471,
        F_KNCK_H = 0x000208B0, // hard knockback from front (bumping into a wall from dive)
        KNCK_LND = 0x00020462, // Landing from front knockback
        KNCK_GND = 0x00020466, // Front knockback while grounded
        FIRE_HIT = 0x000208B7,
        FIRE_RVR = 0x08000239, // Recover from fire on ground
        HOVER_F = 0x0000088D,  // Falling from hover
        SLIDE_R1 = 0x000008A6, // Recover from slide by flipping
        SLIDE_R2 = 0x00000386, // Recover from slide by getting up
        R_SPRAY = 0x0C008220,  // Recover from spray
        G_POUND = 0x008008A9,  // Ground pounding
        NPC_PUTDOWN = 0x80000387,
        NPC_THROW = 0x80000588,
        NPC_JUMPTHROW = 0x820008AB
    };

    enum Status
    {
        ALIVE = 0x1,
        SEWER_FLOOR = 0x2,
        INVISIBLE = 0x4,
        NPC_TALKING = 0x8,
        LEAVING_WATER = 0x10,
        SLIP = 0x80,
        GAMEOVER = 0x400,
        UP_GROUND_POUND = 0x800,
        HASHELMET_FOLLOWCAMERA = 0x1000,
        HASHELMET = 0x2000,
        HASFLUDD = 0x8000,
        SPLASH = 0x10000,
        PUDDLE = 0x20000,
        SHIRT = 0x100000,
        GONE = 0x200000
    };

    enum Voice
    {
        CANNON_WAIL = 30911,
        TRIPLE_JUMP = 30902,
        JUMP = 30891,
        DIVE_OUT = 30897,
        FALL_LEDGE_GRAB = 30944
    };

    enum Animation
    {
        WALLHANG = 0x33,
        FALL = 0x4C,
        BOUNCE = 0x50,
        IDLE = 0xC3,
        SHINEGET = 0xCD,
        SPINJUMP = 0xF4
    };

    enum Effect
    {
        SMOKE_CLOUD = 0x1,
        ROCKET_SPRAY_MIST = 0x2,
        ROCKET_SPRAY_DROPS = 0x3,
        BURNING_SMOKE = 0x6,
        GROUND_SHARP_SHOCK = 0x7,
        STARS = 0xC,
        SPARKLE = 0xE,
        WATER_RECHARGE = 0xF,
        GROUND_SHOCK = 0x10,
        GROUND_SMOKE_PLUME = 0x11,
        WARP_IN_BODY_PIECES = 0x24,
        WARP_IN_BELT_PIECES = 0x25,
        WARP_IN_HAT_PIECES = 0x26,
        WARP_IN_RED_PIECES = 0x27,
        WARP_IN_BLUE_PIECES = 0x29,
        WARP_IN_BROWN_PIECES = 0x2A,
        WARP_IN_FLUDD_PIECES = 0x2D,
        WATER_RIPPLE = 0x30
    };

    u32 _00;                               //0x0070
    u32 mActionState;                      //0x0074
    u32 mJumpingState;                     //0x0078
    u32 mState;                            //0x007C
    u32 mPrevState;                        //0x0080
    u16 mSubState;                         //0x0084
    u16 mSubStateTimer;                    //0x0086
    u32 _01;                               //0x0088
    f32 mBaseAcceleration;                 //0x008C
    u16 mAccelerationDirection;            //0x0090
    u16 _02;                               //0x0092
    JGeometry::TVec3<u16> mAngle;          //0x0094
    u16 _03;                               //0x009A
    u32 _04[0x8 / 4];                      //0x009C
    JGeometry::TVec3<f32> mSpeed;          //0x00A4
    f32 mForwardSpeed;                     //0x00B0
    u32 _05[0x24 / 4];                     //0x00B4
    TBGCheckData *mWallTriangle;           //0x00D8
    TBGCheckData *mRoofTriangle;           //0x00DC
    TBGCheckData *mFloorTriangle;          //0x00E0
    TBGCheckData *mFloorTriangleCopy;      //0x00E4
    f32 mCeilingAbove;                     //0x00E8
    f32 mFloorBelow;                       //0x00EC
    f32 mWaterHeight;                      //0x00F0
    u32 _07[0x14 / 4];                     //0x00F4
    TMarioControllerWork *mControllerWork; //0x0108
    u32 _07a[0xC / 4];                     //0x010C

    struct
    {
        u32 _04 : 10;
        bool mIsGone : 1;
        bool mIsShineShirt : 1;
        u32 _03 : 2;
        bool mIsWater : 1;
        bool mIsShallowWater : 1;
        bool mHasFludd : 1;
        u32 _02 : 1;
        bool mGainHelmet : 1;
        bool mGainHelmetFlwCamera : 1;
        bool mIsGroundPoundSitUp : 1;
        bool mIsGameOver : 1;
        u32 _01 : 5;
        bool mLeftRecentWater : 1;
        bool mTalkingNPC : 1;
        bool mIsVisible : 1;
        bool mAboveSewerFloor : 1;
        u32 _00 : 1;
    } mAttributes; //0x0118

    struct
    {
        u32 _03 : 14;
        bool mIsWater : 1;
        bool mIsShallowWater : 1;
        bool mHasFludd : 1;
        u32 _02 : 2;
        bool mGainHelmet : 1;
        bool mIsGroundPoundSitUp : 1;
        bool mIsTooBad : 1;
        u32 _01 : 5;
        bool mLeftRecentWater : 1;
        bool mTalkingNPC : 1;
        bool mIsVisible : 1;
        bool mAboveSewerFloor : 1;
        u32 _00 : 1;
    } mPrevAttributes; //0x011C

    u16 mHealth;                   //0x0120
    u16 _08;                       //0x0122
    u32 _09[0x8 / 4];              //0x0124
    f32 mWaterHealth;              //0x012C
    f32 mMaxWaterHealth;           //0x0130
    u32 _10[0x28 / 4];             //0x0134
    f32 mCollisionXZSize;          //0x015C
    u32 _11[0x138 / 4];            //0x0160
    u32 mInitialWater;             //0x0298
    u32 _12[0x10 / 4];             //0x029C
    f32 mLastGroundedHeight;       //0x02AC
    u32 _13[0x8 / 4];              //0x02B0
    u16 _14;                       //0x02B8
    u16 mOBTimer;                  //0x02BA
    u32 _15[0xC8 / 4];             //0x02BC
    TTakeActor *mGrabTarget;       //0x0384
    u8 _16;                        //0x0388
    u8 mSurfGessoID;               //0x0389
    u16 _16a;                      //0x038A
    f32 mHolderHeightDiff;         //0x038C
    u32 _16b;                      //0x0390
    J3DDrawBuffer *mDrawBufferA;   //0x0394
    J3DDrawBuffer *mDrawBufferB;   //0x0398
    u32 _17[0xC / 4];              //0x039C
    M3UModelMario *mModelData;     //0x03A8
    u32 _18[0x18 / 4];             //0x03AC
    u8 mBindBoneIDArray[12];       //0x03C4
    u32 _19[0x10 / 4];             //0x03D0
    TMarioCap *mCap;               //0x03E0
    TWaterGun *mFludd;             //0x03E4
    u32 _20[0x8 / 4];              //0x03E8
    TYoshi *mYoshi;                //0x03F0
    void *mSurfGesso;              //0x03F4
    u32 _20a[0x8 / 4];             //0x03F8
    MarioParams *mCustomInfo;      //0x0400
    u32 _21[0xF8 / 4];             //0x0404
    TMarioGamePad *mController;    //0x04FC
    u32 _22[0x8C / 4];             //0x0500
    u16 mMaxHealth;                //0x058C
    u16 _23;                       //0x058E
    u32 _24[0x10 / 4];             //0x0590
    f32 mMaxGroundSpeed;           //0x05A0
    u32 _25[0x10 / 4];             //0x05A4
    f32 mTurboNozzleSpeed;         //0x05B4
    u32 _25a[0x218 / 4];           //0x05B8
    f32 mBaseBounceSpeed1;         //0x07D0
    u32 _26[0x10 / 4];             //0x07D4
    f32 mBaseBounceSpeed2;         //0x07E4
    u32 _27[0x10 / 4];             //0x07E8
    f32 mBaseBounceSpeed3;         //0x07F8
    u32 _28[0x24 / 4];             //0x07FC
    f32 mThrowPower;               //0x0820
    u32 _29[0x9C / 4];             //0x0824
    f32 mMaxFallNoDamage;          //0x08C0
    u32 _30[0x24 / 4];             //0x08C4
    f32 mWallBonkThreshold;        //0x08E8
    u32 _30a[0x9C / 4];            //0x08EC
    u16 mOBStep;                   //0x0988
    u16 _31;                       //0x098A
    u32 _32[0x10 / 4];             //0x098C
    u16 mOBMax;                    //0x099C
    u16 _33;                       //0x099E
    u32 _34[0x178 / 4];            //0x09A0
    f32 mGravity;                  //0x0B18
    u32 _35[0x10 / 4];             //0x0B1C
    f32 mTwirlGravity;             //0x0B2C
    u32 _35a[0x24 / 4];            //0x0B30
    f32 mAirborneHSpeedMul;        //0x0B54
    u32 _36[0x10 / 4];             //0x0B58
    f32 mJumpAccelMul;             //0x0B68
    u32 _37[0x24 / 4];             //0x0B6C
    f32 mSideFlipStrength;         //0x0B90
    u32 _38[0x60 / 4];             //0x0B94
    f32 mTurboJumpFSpeed;          //0x0BF4
    u32 _38a[0x10 / 4];            //0x0BF8
    f32 mTurboJumpYSpeed;          //0x0C08
    u32 _38b[0x74 / 4];            //0x0C0C
    f32 mHipDropSpeedSlow;         //0x0C80
    u32 _39[0x10 / 4];             //0x0C84
    f32 mHipDropSpeedFast;         //0x0C94
    u32 _40[0x1DC / 4];            //0x0C98
    f32 mDiveStartSpeed;           //0x0E74
    u32 _40a[0x68 / 4];            //0x0E78
    f32 mRunSlowdownFactor;        //0x0EE0
    u32 _40aa[0x10 / 4];           //0x0EE4
    f32 mRunSlowdownFactor2;       //0x0EF4
    u32 _40b[0xC4 / 4];            //0x0EF8
    f32 mWaterMaxDiffToInteract;   //0x0FBC
    u32 _40c[0x144 / 4];           //0x0FC0
    f32 mWaterHighBuoyant;         //0x1104
    u32 _40d[0x10 / 4];            //0x1108
    f32 mWaterLowBuoyant;          //0x1118
    u32 _40e[0x10 / 4];            //0x111C
    f32 mWaterYSpdMultiplier;      //0x112C
    u32 _40f[0x10 / 4];            //0x1130
    f32 mWaterJumpHeightDiffMax;   //0x1140
    u32 _41[0x10 / 4];             //0x1144
    f32 mWaterEntryMaxHeight;      //0x1154
    u32 _41a[0x10 / 4];            //0x1158
    f32 mOceanOfs;                 //0x1168
    u32 _42[0x100 / 4];            //0x116C
    f32 mWaterHealthDrainSpd;      //0x126C
    u32 _43[0x10 / 4];             //0x1270
    f32 mWaterHealthScubaDrainSpd; //0x1280
    u32 _44[0x10 / 4];             //0x1284
    f32 mWaterHealthIncreaseSpd;   //0x1294
    u32 _45[0x40 / 4];             //0x1298
    s16 mWallAnimTimer;            //0x12D8
    u16 _46;                       //0x12DA
    u32 _47[0x10 / 4];             //0x12DC
    s16 mWallHangTimer;            //0x12EC
    u32 _48[0x124 / 4];            //0x12F0
    f32 mTRopeJumpAccelMul;        //0x1414
    u32 _49[0x1DC / 4];            //0x1418
    f32 mPoleClimbSpeedMul;        //0x15F4
    u32 _49a[0x38 / 4];            //0x15F8
    f32 mPoleGrabDistPadding;      //0x1630
    u32 _49b[0xB38 / 4];           //0x1634
    f32 mHoverYSpdMultiplier;      //0x216C
    u32 _49c[0x98 / 4];            //0x2170
    f32 mFSpeedYoshiMul;           //0x2208
    u32 _50[0x10 / 4];             //0x220C
    f32 mVSpeedYoshiMul;           //0x221C
    u32 _51[0x4C / 4];             //0x2220
    f32 mFSpeedFlutterMul;         //0x226C
    u32 _52[0x10 / 4];             //0x2270
    f32 mBSpeedFlutterMul;         //0x2280
    u32 _53[0x200C / 4];           //0x2284
};