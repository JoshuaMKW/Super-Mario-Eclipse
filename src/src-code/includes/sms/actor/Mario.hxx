#ifndef MARIO_H
#define MARIO_H

#include "types.h"
#include "sms/JGeometry.hxx"
#include "sms/J3D.hxx"

#include "sms/actor/BaseActor.hxx"
#include "sms/actor/Yoshi.hxx"
#include "sms/m3d/M3UModel.hxx"
#include "sms/collision/BGCheck.hxx"
#include "sms/nozzle/Watergun.hxx"

#include "params/MarioParams.hxx"

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

    u8 mAnimID;
    u8 mFluddEnabled;  //Note: ENUM? 68 = enabled, 200 = disabled
    u8 mAnmTexPattern; //Note: 23 is the max value allowed
    u8 mMarioHand;     //Note: 1, 2, and 3 seem to be valid values, this determines the hand model used(?)
    u8 unk_1;          //Note: If bit 30 is set, it seems to activate a bit flag to multiple pointers, likely mtx related
    u8 unk_2;          //Note: Value seems to be set but is never used?
};

class TMarioCap
{
};

class M3UModelMario : public M3UModel
{
};

class TMarioSoundValues
{
public:
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
    u8 _2D; // padding?
    u8 _2E; // ^^
    u8 _2F; // ^^
};

struct TMarioControllerWork
{
    u32 _0; // unk
    u32 _4;
    u32 _8;
    u8 _C[0x1C - 0xC]; // unk
    f32 _1C;
};

class TMarioGamePad : public JUTGamePad
{
public:
    f32 _A8;
    f32 _AC;
    f32 _B0;
    f32 _B4;
    f32 _B8;
    f32 _BC;
    f32 stickX; // _C0
    f32 stickY; // _C4
    f32 _C8;
    f32 _CC;
    u32 _D0;
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

    u32 getInput()
    {
        return this->mButtons.mInput;
    }

    u32 getFrameInput()
    {
        return this->mButtons.mFrameInput;
    }

    bool isPressed(BUTTONS input)
    {
        return this->mButtons.mInput & input;
    }

    bool isFramePressed(BUTTONS input)
    {
        return this->mButtons.mFrameInput & input;
    }

    bool isRecordingInput()
    {
        return this->State.mReadInput;
    }

    void setEnabled(bool state)
    {
        this->State.mReadInput = state;
    }
};

class TMario : public TTakeActor
{

public:
    enum STATE
    {
        SA_NUMBER = 0x0000000F,
        SA_DOJUMP = 0x00000080,
        SA_AIRBORN = 0x00000800,
        SA_CUTSCENE = 0x00001000,
        SA_WATERBORN = 0x00002000,
        SA_RUNNING = 0x04000440,
        SA_IDLE = 0x0C400201,
        SA_STOP = 0x0C00023D,
        SA_SPIN = 0x00000441,
        SA_JUMPSPIN = 0x00000890,
        SA_JUMPSPIN1 = 0x00000895,
        SA_JUMPSPIN2 = 0x00000896,
        SA_JUMP = 0x02000880,
        SA_D_JUMP = 0x02000881,
        SA_TRIPLE_J = 0x00000882,
        SA_JMP_LAND = 0x04000470,
        SA_HVY_LAND = 0x04000473,
        SA_D_LAND = 0x04000472,
        SA_T_LAND = 0x0800023A,
        SA_JUMPSIDE = 0x00000880,
        SA_BOUNCE = 0x00000884,
        SA_SIDESTEP = 0x0C008220,
        SA_SIDE_FLIP = 0x00000887,
        SA_FALL = 0x0000088C,
        SA_SWIM = 0x000024D7,
        SA_DIVE = 0x0080088A,
        SA_DIVEJUMP = 0x02000889,
        SA_DIVESLIDE = 0x00800456,
        SA_CLIMB = 0x18100340,
        SA_CLIMBUP = 0x10100343,
        SA_WALLJUMP = 0x02000886,
        SA_WALLSLIDE = 0x000008A7,
        SA_HANG = 0x3800034B,
        SA_HANGCLIMB = 0x3000054F,
        SA_SLAMSTART = 0x008008A9,
        SA_SLAM = 0x0080023C,
        SA_SPRAY = 0x0C008220,
        SA_THROWN = 0x000208B8,
        SA_HOVER = 0x0000088B,
        SA_STUCK = 0x0002033C,
        SA_TALKING = 0x10001308,
        SA_TURNING = 0x00000444,
        SA_YOSHI_ESC = 0x0000089C,
        SA_SHINE_C = 0x00001302, // Collected Shine Sprite
        SA_DEATH = 0x00020467,
        SA_DOOR_F_O = 0x00001321, // Door open fail
        SA_WALL_S_L = 0x04000471,
        SA_F_KNCK_H = 0x000208B0, // hard knockback from front (bumping into a wall from dive)
        SA_KNCK_LND = 0x00020462, // Landing from front knockback
        SA_KNCK_GND = 0x00020466, // Front knockback while grounded
        SA_FIRE_HIT = 0x000208B7,
        SA_FIRE_RVR = 0x08000239, // Recover from fire on ground
        SA_HOVER_F = 0x0000088D,  // Falling from hover
        SA_SLIDE_R1 = 0x000008A6, // Recover from slide by flipping
        SA_SLIDE_R2 = 0x00000386, // Recover from slide by getting up
        SA_R_SPRAY = 0x0C008220,  // Recover from spray
        SA_G_POUND = 0x008008A9   // Ground pounding
    };

    enum STATUS
    {
        ST_ALIVE = 0x1,
        ST_SEWER_FLOOR = 0x2,
        ST_INVISIBLE = 0x4,
        ST_NPC_TALKING = 0x8,
        ST_LEAVING_WATER = 0x10,
        ST_SLIP = 0x80,
        ST_GAMEOVER = 0x400,
        ST_UP_GROUND_POUND = 0x800,
        ST_HASHELMET_FOLLOWCAMERA = 0x1000,
        ST_HASHELMET = 0x2000,
        ST_HASFLUDD = 0x8000,
        ST_SPLASH = 0x10000,
        ST_PUDDLE = 0x20000,
        ST_SHIRT = 0x100000,
        ST_GONE = 0x200000
    };

    enum VOICE
    {
        V_CANNON_WAIL = 30911,
        V_TRIPLE_JUMP = 30902,
        V_JUMP = 30891,
        V_DIVE_OUT = 30897,
    };

    enum ANIMATION
    {
        A_WALLHANG = 0x33,
        A_FALL = 0x4C,
        A_BOUNCE = 0x50,
        A_IDLE = 0xC3,
        A_SHINEGET = 0xCD,
        A_SPINJUMP = 0xF4
    };

    enum EFFECT
    {
        E_SMOKE_CLOUD = 0x1,
        E_ROCKET_SPRAY_MIST = 0x2,
        E_ROCKET_SPRAY_DROPS = 0x3,
        E_BURNING_SMOKE = 0x6,
        E_GROUND_SHARP_SHOCK = 0x7,
        E_STARS = 0xC,
        E_SPARKLE = 0xE,
        E_WATER_RECHARGE = 0xF,
        E_GROUND_SHOCK = 0x10,
        E_GROUND_SMOKE_PLUME = 0x11,
        E_WARP_IN_BODY_PIECES = 0x24,
        E_WARP_IN_BELT_PIECES = 0x25,
        E_WARP_IN_HAT_PIECES = 0x26,
        E_WARP_IN_RED_PIECES = 0x27,
        E_WARP_IN_BLUE_PIECES = 0x29,
        E_WARP_IN_BROWN_PIECES = 0x2A,
        E_WARP_IN_FLUDD_PIECES = 0x2D,
        E_WATER_RIPPLE = 0x30
    };

    u32 _00[0xC / 4];                 //0x0070
    u32 mState;                       //0x007C
    u32 mPrevState;                   //0x0080
    u16 mSubState;                    //0x0084
    u16 mSubStateTimer;               //0x0086
    u32 _01;                          //0x0088
    f32 mBaseAcceleration;            //0x008C
    u16 mAccelerationDirection;       //0x0090
    u16 _02;                          //0x0092
    JGeometry::TVec3<u16> mAngle;     //0x0094
    u16 _03;                          //0x009A
    u32 _04[0x8 / 4];                 //0x009C
    JGeometry::TVec3<f32> mSpeed;     //0x00A4
    f32 mForwardSpeed;                //0x00B0
    u32 _05[0x24 / 4];                //0x00B4
    TBGCheckData *mWallTriangle;      //0x00D8
    TBGCheckData *mRoofTriangle;      //0x00DC
    TBGCheckData *mFloorTriangle;     //0x00E0
    TBGCheckData *mFloorTriangleCopy; //0x00E4
    f32 mCeilingAbove;                //0x00E8
    f32 mFloorBelow;                  //0x00EC
    f32 mWaterHeight;                 //0x00F0
    u32 _07[0x24 / 4];                //0x00F4

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
    u32 _16[0xC / 4];              //0x0388
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
    MarioParams *mCustomInfo;      //0x03F4
    u32 _21[0x104 / 4];            //0x03F8
    TMarioGamePad *mController;    //0x04FC
    u32 _22[0x8C / 4];             //0x0500
    u16 mMaxHealth;                //0x058C
    u16 _23;                       //0x058E
    u32 _24[0x10 / 4];             //0x0590
    f32 mMaxGroundSpeed;           //0x05A0
    u32 _25[0x22C / 4];            //0x05A4
    f32 mBaseBounceSpeed1;         //0x07D0
    u32 _26[0x10 / 4];             //0x07D4
    f32 mBaseBounceSpeed2;         //0x07E4
    u32 _27[0x10 / 4];             //0x07E8
    f32 mBaseBounceSpeed3;         //0x07F8
    u32 _28[0x24 / 4];             //0x07FC
    f32 mThrowPower;               //0x0820
    u32 _29[0x9C / 4];             //0x0824
    f32 mMaxFallNoDamage;          //0x08C0
    u32 _30[0xC4 / 4];             //0x08C4
    u16 mOBStep;                   //0x0988
    u16 _31;                       //0x098A
    u32 _32[0x10 / 4];             //0x098C
    u16 mOBMax;                    //0x099C
    u16 _33;                       //0x099E
    u32 _34[0x178 / 4];            //0x09A0
    f32 mGravity;                  //0x0B18
    u32 _35[0x38 / 4];             //0x0B1C
    f32 mAirborneHSpeedMul;        //0x0B54
    u32 _36[0x10 / 4];             //0x0B58
    f32 mDefaultAccelerationMul;   //0x0B68
    u32 _37[0x24 / 4];             //0x0B6C
    f32 mSideFlipStrength;         //0x0B90
    u32 _38[0xEC / 4];             //0x0B94
    f32 mHipDropSpeedSlow;         //0x0C80
    u32 _39[0x10 / 4];             //0x0C84
    f32 mHipDropSpeedFast;         //0x0C94
    u32 _40[0x4A8 / 4];            //0x0C98
    f32 mWaterJumpHeightDifMax;    //0x1140
    u32 _41[0x24 / 4];             //0x1144
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
    f32 mTRopeAirborneAccelMul;    //0x1414
    u32 _49[0xE04 / 4];            //0x1418
    f32 mVSpeedYoshiMul;           //0x221C
    u32 _50[0x4C / 4];             //0x2220
    f32 mFSpeedFlutterMul;         //0x226C
    u32 _51[0x10 / 4];             //0x2270
    f32 mBSpeedFlutterMul;         //0x2280
    u32 _52[0x200C / 4];           //0x2284
};

#endif