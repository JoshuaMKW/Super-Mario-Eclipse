#ifndef ECLIPSE_HEADER
#define ECLIPSE_HEADER

#include "k_stdlib/kamek.h"
#include "define.hxx"

#define __VERSION__ "ALPHA"

#define PI 3.14159265f

/*
void *operator new(size_t size)
{
    return __nw__FUli(size, 32);
}

void operator delete(void *obj) throw()
{
    __dl__FPv(obj);
}
*/

static inline void flushAddr(void *addr)
{
    dcbf(addr);
    icbi(addr);
}

template <typename T>
struct RGB
{
    T R, G, B;
};

template <typename T>
struct RGBA
{
    T R, G, B, A;

    RGBA() {}
    RGBA(T R, T G, T B, T A)
    {
        this->R = R;
        this->G = G;
        this->B = B;
        this->A = A;
    }
};

class MarioParamsFile;
class SMEFile;
class TMario;
class TYoshi;
class TWaterGun;
class Vector3D;

struct AreaEpisodeArray
{
    u32 _00[0x10 / 4]; //0x0000
    u32 *startArray;   //0x0010
    u32 *endArray;     //0x0014
};

namespace JGeometry
{

    template <typename T>
    struct TVec3
    {
        T x;
        T y;
        T z;

        TVec3() {}

        TVec3(T a, T b, T c)
        {
            x = a;
            y = b;
            z = c;
        }
    };

}; // namespace JGeometry

namespace JDrama
{

    class TDisplay
    {
    };

    class TGraphics
    {
    };

    template <typename T>
    class TFlagT
    {

    public:
        T mFlag; //0x0000
    };

    class TNameRef
    {

    public:
        void *mVTable;   //0x0000
        char *mTypeName; //0x0004
        u16 mKeyCode;    //0x0008
    };

    class TViewObj : public TNameRef
    {

    public:
        TFlagT<u16> mFlags; //0x000C
    };

    class TPlacement : public TViewObj
    {

    public:
        JGeometry::TVec3<float> mPosition; //0x0010
        TFlagT<u16> mFlags;                //0x001C
    };

    class TCamera
    {

    public:
        u32 _00[0x10 / 4];                    //0x0000
        JGeometry::TVec3<float> mCoordinates; //0x0010
        u32 _01[0x14 / 4];                    //0x001C
    };

    class TLookAtCamera : public TCamera
    {

    public:
        JGeometry::TVec3<float> mUpVector;       //0x0030
        JGeometry::TVec3<float> mTargetPosition; //0x003C
        float mProjectionFovy;                   //0x0048
        float mProjectionAspect;                 //0x004C
    };

    class TActor
    {

    public:
        u32 *vTable;                          //0x0000
        u32 _00[0xC / 4];                     //0x0004
        JGeometry::TVec3<float> mCoordinates; //0x0010
        u32 _01[0x8 / 4];                     //0x001C
        JGeometry::TVec3<float> mSize;        //0x0024
        JGeometry::TVec3<float> mRotation;    //0x0030
        u32 _02[0x8 / 4];                     //0x003C
    };

}; // namespace JDrama

namespace JUTGamePad
{

    class CButton
    {

    public:
        u32 _00 : 4; //0x0018
        u32 mMainStickUp : 1;
        u32 mMainStickDown : 1;
        u32 mMainStickRight : 1;
        u32 mMainStickLeft : 1;
        u32 _01 : 4;
        u32 mCStickUp : 1;
        u32 mCStickDown : 1;
        u32 mCStickRight : 1;
        u32 mCStickLeft : 1;
        u32 _02 : 3;
        u32 mSButton : 1;
        u32 mYButton : 1;
        u32 mXButton : 1;
        u32 mBButton : 1;
        u32 mAButton : 1;
        u32 _03 : 1;
        u32 mLButton : 1;
        u32 mRButton : 1;
        u32 mZButton : 1;
        u32 mDPadUp : 1;
        u32 mDPadDown : 1;
        u32 mDPadRight : 1;
        u32 mDPadLeft : 1;
        u32 _04[0x8 / 4];    //0x0004
        u16 _05;             //0x000C
        u8 mLAnalog;         //0x000E
        u8 mRAnalog;         //0x000F
        float mLFloatAnalog; //0x0010
        float mRFloatAnalog; //0x0014
    };

    class CStick
    {

    public:
        float mXAxis;               //0x0000
        float mYAxis;               //0x0004
        float mDistanceFromNeutral; //0x0008
        short mAngle;               //0x000C
    };

}; // namespace JUTGamePad

typedef u32 JSUStreamSeekFrom;

struct JSUIosBase
{
    virtual ~JSUIosBase() = 0;
    virtual s32 getAvailable() const = 0;
    virtual void skip(s32 len) = 0;
    virtual void readData(void *dest, s32 len) = 0;
    virtual s32 getLength() const = 0;
    virtual s32 getPosition() const = 0;
    virtual void seekPos(s32 address, JSUStreamSeekFrom whence);
};

struct JSUInputStream : public JSUIosBase
{
    void read(void *dest, s32 len);
};

struct JSUOutputStream : public JSUIosBase
{
    void write(const void *source, s32 len);
};

class TCardBookmarkInfo
{

public:
    struct
    {
        u32 _00[0x1C / 4]; //0x0000
        u16 mShineCount;   //0x001C
        u16 _01;           //0x001E
    } FileData[3];
};

class TCardManager
{

public:
    u32 _00[0x46C / 4];            //0x0000
    TCardBookmarkInfo *mBookMarks; //0x046C
};

class TOptionRumbleUnit
{

public:
    u32 _00[0x1C / 4];     //0x0000
    u32 mAnimState;        //0x001C
    float mAnimSpeed;      //0x0088
    float mAnimFrame;      //0x008C
    s32 mXCoordA;          //0x0090
    s32 mYCoordA;          //0x0094
    s32 mXCoordB;          //0x0098
    s32 mYCoordB;          //0x009C
    float mAnimSizeFactor; //0x00A0
};

class TOptionSoundUnit
{

public:
    u32 _00[0x18 / 4]; //0x0000
    u32 mAnimState;    //0x0018
};

class TOptionSubtitleUnit
{

public:
    u32 _00[0x18 / 4]; //0x0000
    u32 mAnimState;    //0x0018
};

class TOptionControl
{

public:
    u32 _00[0x8 / 4];                   //0x0000
    TOptionRumbleUnit *mRumbleUnit;     //0x0008
    TOptionSoundUnit *mSoundUnit;       //0x000C
    TOptionSubtitleUnit *mSubtitleUnit; //0x0010
    u32 mOptionIndex;                   //0x0014
    bool mIsOptionChanged;              //0x0018
    bool mIsOptionSelected;             //0x0019
};

class TCardLoad
{

public:
    u32 _00[0x14 / 4];              //0x0000
    u32 mState;                     //0x0014
    u32 _01[0x75C / 4];             //0x0018
    TOptionControl *mOptionControl; //0x0774
};

class Talk2D2
{

public:
    u32 _00[0x28 / 4];  //0x0000
    bool isBillBoard;   //0x0028
    u8 _01;             //0x0029
    u16 _02;            //0x002A
    u32 _03[0x248 / 4]; //0x002C
    u32 lastMsgIndex;   //0x0274
    u32 curMsgIndex;    //0x0278
};

class J3DDrawBuffer
{
};

class J3DModel
{

public:
    u32 _00[0x14 / 4];          //0x0000
    Quaternion mSizeMultiplier; //0x0014
    u32 _01[0x40 / 4];          //0x0018
    Mtx *mJointArray[];    //0x0058
};

class M3UModelMario
{

public:
    void *_00[2];     //0x0000
    J3DModel *mModel; //0x0008
};

class TRailNode
{

public:
    JGeometry::TVec3<s16> mPosition; //0x0000
    s16 mNeighborCount;              //0x0006
    u32 _00[0xC / 4];                //0x0008
    s16 mNeighborIDs[8];             //0x0014
    float mNeightborDistances[8];    //0x0024
};

class TGraphWeb
{

public:
    u32 *mNodes;          //0x0000
    TRailNode *mRailNode; //0x0004
    s32 mNodeCount;       //0x0008
    char *mGraphName;     //0x000C
    u32 _00[0x8 / 4];     //0x0010
    void *mVTable;        //0x0018
};

class TGraphTracer
{

public:
    TGraphWeb *mGraph; //0x0000
    s32 mCurrentNode;  //0x0004
    s32 mPreviousNode; //0x0008
    float _00;
    float _01;
    float _02;
};

template <typename T>
class TSpineBase
{

public:
    T *mTarget;        //0x0000
    u32 _00;           //0x0004
    u32 mVTableIndex;  //0x0008
    u32 _01[0x8 / 4];  //0x000C
    void *mCurVTable;  //0x0014
    void *mVTableCopy; //0x0018
    void *mPrevVTable; //0x001C
    u32 mStateTimer;   //0x0020
    void *mVTable;     //0x0024
};

class TBGCheckData
{

public:
    u16 mCollisionType;               //0x0000
    s16 mValue4;                      //0x0002
    u16 _00;                          //0x0004
    u16 _01;                          //0x0006
    float mMinHeight;                 //0x0008
    float mMaxHeight;                 //0x000C
    JGeometry::TVec3<float> mVertexA; //0x0010
    JGeometry::TVec3<float> mVertexB; //0x001C
    JGeometry::TVec3<float> mVertexC; //0x0028
    float _02[0x10 / 4];
    u32 *callbackVTable; //0x0044
};

class THitActor : public JDrama::TActor
{

public:
    THitActor **mCollidingObjs; //0x0044
    s16 mNumObjs;               //0x0048
    s16 mMaxObjs;               //0x004A
    u32 mObjectID;              //0x004C
    float mAttackRadius;        //0x0050
    float mAttackHeight;        //0x0054
    float mReceiveRadius;       //0x0058
    float mReceiveHeight;       //0x005C
    float _01;                  //0x0060
    u32 mObjectType;            //0x0064
};

class TTakeActor : public THitActor
{

public:
    TTakeActor *mHolder;     //0x0068
    TTakeActor *mHeldObject; //0x006C
};

class TLiveActor : public TTakeActor
{

public:
    u32 _00[0x1C / 4];                           //0x0070
    TSpineBase<TLiveActor> *mSpineBase;          //0x008C
    u32 _01;                                     //0x0090
    JGeometry::TVec3<float> mPositionalVelocity; //0x0094
    JGeometry::TVec3<float> mRotationalVelocity; //0x00A0
    JGeometry::TVec3<float> mSpeed;              //0x00AC
    float mShadowRadius;                         //0x00B8
    float mMaxSpeed;                             //0x00BC
    float _02;                                   //0x00C0
    TBGCheckData *mFloorBelow;                   //0x00C4
    u32 _03;                                     //0x00C8
    float mGravity;                              //0x00CC
    u32 _04[0x20 / 4];                           //0x00D0
};

class TMapObjBase : public TLiveActor
{

public:
    struct
    {
        u32 _00 : 7;
        bool mIsSunken : 1;
        u32 _01 : 1;
        bool mIsRide : 1;
        u32 _02 : 1;
        bool mIsCanTaken : 1;
        u32 _03 : 16;
        bool mIsShadowDead : 1;
        u32 _04 : 2;
        bool mIsObjDead : 1;
    } DrawInfo; //0x00F0

    u32 _00[0x8 / 4];  //0x00F4
    u16 mState;        //0x00FC
    u16 _01;           //0x00FE
    u32 _02[0x30 / 4]; //0x0100
};

class TSpineEnemy : public TLiveActor
{

public:
    u32 _00[0x34 / 4];          //0x00F0
    TGraphTracer *mGraphTracer; //0x0124
    u32 _01[0x28 / 4];          //0x0128
};

class TCameraInbetween
{

public:
    u32 _00[0x30 / 4];                   //0x0000
    JGeometry::TVec3<float> mSubjectPos; //0x0030
};

class CPolarSubCamera : public JDrama::TLookAtCamera
{

public:
    u32 _00[0x1C / 4];            //0x0050
    TCameraInbetween *mInbetween; //0x006C
    u32 _01[0x34 / 4];            //0x0070
    u16 _02;                      //0x00A4
    u16 mHorizontalAngle;         //0x00A6
    float mInterpolateDistance;   //0x00A8
};

class TJointModel
{

public:
    u32 _00[0x30 / 4]; //0x0000
};

class TPollutionPos
{
};

class TPollutionObj
{

public:
    u32 _00[0x30 / 4];       //0x0000
    u32 mCleanedDegree;      //0x0030
    TPollutionPos mPosition; //0x005C
};

class TPollutionLayer : public TJointModel
{

public:
    u16 mPollutionEffect; //0x0030
};

class TPollutionManager
{
};

class TBossManta : TSpineEnemy
{

public:
    u32 _00[0x3C / 4];       //0x0150
    u32 mGeneration;         //0x018C
    float mSpeed;            //0x0190
    float mAngularVelocity;  //0x0194
    u32 _01;                 //0x0198
    u32 mDamageCounter;      //0x019C
    u32 mInvincibilityTimer; //0x01A0
};

class TBGPolDrop
{

public:
    enum DropStatus
    {
        DEAD,
        ALIVE,
        HIT
    };

    JDrama::TNameRef mNameRef;            //0x0000
    JDrama::TFlagT<u16> mFlags;           //0x000C
    JGeometry::TVec3<float> mCoordinates; //0x0010
    u16 _00;                              //0x001C
    u16 _01;                              //0x001E
    void *mVTable;                        //0x0020
    JGeometry::TVec3<float> _02;          //0x0024
    JGeometry::TVec3<float> _03;          //0x0030
    u32 _04;                              //0x003C
    u32 _05;                              //0x0040
    JGeometry::TVec3<float> mVelocity;    //0x0044
    u32 _06[0x8 / 4];                     //0x0050
    u32 mStatus;                          //0x0058
};

class TBossGesso : public TSpineEnemy
{

public:
    enum Attacks
    {
        SINGLE,
        DOUBLE,
        SKIPROPE,
        UNISON,
        SHOOT
    };

    u32 _00[0x18 / 4];          //0x0150
    u32 mAttackState;           //0x0168
    u32 mAttackTimer;           //0x016C
    u32 _01[0x10 / 4];          //0x0170
    TBGPolDrop *mPollutionDrop; //0x0180
    u32 _02[0xC / 4];           //0x0184
    u16 _03;                    //0x0190
    u8 _04;                     //0x0192
    u8 mGoopLevel;              //0x0193
    u16 mAttackCount;           //0x0194
    u16 _05;                    //0x0196
    u32 _06[0x18 / 4];          //0x0198
};

class TMapCollisionGroundEntry
{

public:
    u32 *mVTable;                            //0x0000
    TMapCollisionGroundEntry *mNextTriangle; //0x0004
    TBGCheckData *mColTriangle;              //0x0008
};

class TBGArray
{

public:
    TBGCheckData mFloorTriangles[]; //0x0000
};

class TMapCollisionData
{

public:
    float _00;                                   //0x0000
    float _01;                                   //0x0004
    u32 _02[0x8 / 4];                            //0x0008
    u32 mUnkSize;                                //0x0010
    u32 _03[0x8 / 4];                            //0x0014
    u32 mFloorArraySize;                         //0x001C
    u32 _04[0x8 / 4];                            //0x0020
    TBGArray *mFloorData;                        //0x0028
    TMapCollisionGroundEntry **mConnectedFloors; //0x002C ?
    u32 _05[0x8 / 4];                            //0x0030
    u32 mCheckDataLength;                        //0x0038
    u32 _06;                                     //0x003C
    u16 _07;                                     //0x0040
};

class TMap
{

public:
    u32 _00[0x10 / 4];                 //0x000C
    TMapCollisionData *mCollisionData; //0x0010
};

class TSMSFader
{

public:
    u32 _00[0x18 / 4]; //0x0000
    RGB<u8> mColor;    //0x0018
    s8 mState;         //0x001B
    bool _01;          //0x001C
    u32 mFadeStatus;   //0x0020
    u32 mStatusID;     //0x0024
    float mFadeTime;   //0x0028
    float _02;         //0x002C
};

class TGCConsole2
{

public:
    u32 _00[0x24 / 4];         //0x0000
    s32 mShineCount;           //0x0024
    s32 mCurWater;             //0x0028
    u32 _01[0x18 / 4];         //0x002C
    bool mIsDEBSAlert;         //0x0044
    bool mWaterCardRising;     //0x0045
    bool mIsWaterCard;         //0x0046
    u8 _02;                    //0x0047
    u16 _03;                   //0x0048
    u8 _04;                    //0x004A
    bool mWaterCardFalling;    //0x004B
    u32 _05[0x4C / 4];         //0x004C
    u16 _06;                   //0x0098
    RGBA<u8> mWaterLeftPanel;  //0x009A
    RGBA<u8> mWaterRightPanel; //0x009E
    RGBA<u8> mJuiceCardOrange; //0x00A2
    RGBA<u8> mJuiceCardPurple; //0x00A6
    RGBA<u8> mJuiceCardPink;   //0x00AA
    u16 _07;                   //0x00AE
    u32 _08[0x1F0 / 4];        //0x00B0
    u32 *mWaterCardInfo;       //0x02A0
    u32 _09[0x14 / 4];         //0x02A4
    u32 mWaterCardTopHeight;   //0x02B8
    u32 _10[0x34 / 4];         //0x02BC
};

//This is not officially named, simply fits a thing I needed lul
class TBckData
{

public:
    u32 mAnimationID; //0x0000
    u32 _00[0xC / 4]; //0x0004
    float mFrameRate; //0x0010
    float mCurFrame;  //0x0014
};

//This is not officially named, simply fits a thing I needed lul
class MActor
{

public:
    u32 _00[0xC / 4];   //0x0000
    TBckData *mBckInfo; //0x000C
};

class TFlagManager
{

public:
    struct
    {
        u8 m1Type[0x76]; //0x0000
        u16 _00;         //0x0076
    } Type1Flag;

    struct
    {
        s32 mSaveCount;         //0x0078
        s32 mLifeCount;         //0x007C
        s32 mGelato5Record;     //0x0080
        s32 mPianta2Record;     //0x0084
        s32 _01[0x8 / 4];       //0x0088
        s32 mDelfinoCoinRecord; //0x0090
        s32 mBiancoCoinRecord;  //0x0094
        s32 mRiccoCoinRecord;   //0x0098
        s32 mGelatoCoinRecord;  //0x009C
        s32 mPinnaCoinRecord;   //0x00A0
        s32 mSirenaCoinRecord;  //0x00A4
        s32 mPiantaCoinRecord;  //0x00A8
        s32 mNokiCoinRecord;    //0x00AC
        s32 _02[0x18 / 4];      //0x00B0
        s32 mNoki5Record;       //0x00C8
    } Type2Flag;

    struct
    {
        u8 _00 : 2; //0x00CC
        bool mLostLifePrev : 1;
        bool mPlazaDemoWatched : 1;
        bool mWatchPeachKidnappedPrev : 1;
        bool mPpdBJRBalloonsPrev : 1;
        bool mShineGetPrev : 1;
        u8 _01 : 2;
        bool mPlaneCrashWatched : 1; //0x00CD
        u8 _02 : 1;
        bool mCourtWatched : 1;
        bool mPeachKNAPFMVWatched : 1;
        bool mFluddTheftViewed : 1;
        u8 _03 : 1;
        u8 _04[2]; //0x00CE
    } Type3Flag;

    struct
    {
        s32 mShineCount;    //0x00D0
        s32 mBlueCoinCount; //0x00D4
        s32 mGoldCoinCount; //0x00D8
        s32 mEpisodeNumber; //0x00DC
        s32 mSecondNozzle;  //0x00E0
    } Type4Flag;

    struct
    {
        bool mShineSpawned : 1; //0x00E4
        bool mRiccoUnlockable : 1;
        bool mGelatoUnlockable : 1;
        bool mSunflowersRescue : 1;
        bool mNokiAvailable : 1;
        bool mPiantismoRaceComplete : 1;
        bool _00 : 1;
        bool mMantaSpawned : 1;
        bool mHotelRising : 1; //0x00E5
        bool mRedCoinSwitchPressed : 1;
        bool mMechaBowserDefeated : 1;
        bool mWigglerFalling : 1;
        bool mMoleDefeated : 1;
        u8 _01 : 3;
        u8 _02[0xE]; //0x00E6
    } Type5Flag;

    struct
    {
        s32 mRedCoinCount;        //0x00F4
        s32 mBJRBalloonCount;     //0x00F8
        s32 _00;                  //0x00FC
        s32 mShadowMarioEvent;    //0x0100
        u32 _01[0xF0 / 4];        //0x0104
        u8 mCustomShineIDs[0x80]; //0x01F4
        struct
        {
            bool mIsGamePlus : 1; //0x0274
            u8 mHUDElement : 4;
            u32 _00 : 27;
            u32 _01 : 32;
            u32 _02 : 32;
        } CustomFlags;
    } Type6Flag;

    struct
    {
        bool mRumbleDisabled : 1; //0x0284
        bool mSurroundSound : 1;
        bool mSubtitlesDisabled : 1;
        u8 _00 : 5;
    } Type7Flag;

    struct
    {
        s32 mLanguage; //0x0288
    } Type8Flag;

    struct
    {
        bool mRumbleEnabled : 1; //0x028C
        bool mSubtitlesEnabled : 1;
    } Type9Flag;

    struct
    {
        s32 mSoundMode; //0x0290
        s32 mLanguage;  //0x0294
    } TypeAFlag;

    long long mTimeOnLastSave;      //0x0298
    long long mTimeOnLastSaveCopy;  //0x02A0
    u8 mType1FlagSave[120];         //0x02A8
    u8 mType2FlagSave[84];          //0x0320
    long long mSavedTimeOnLastSave; //0x0378
};

class TMarioGamePad
{
public:
    enum BUTTONS
    {
        MAINSTICK_UP = 0x8000000,
        MAINSTICK_DOWN = 0x4000000,
        MAINSTICK_RIGHT = 0x2000000,
        MAINSTICK_LEFT = 0x1000000,
        CSTICK_UP = 0x80000,
        CSTICK_DOWN = 0x40000,
        CSTICK_RIGHT = 0x20000,
        CSTICK_LEFT = 0x10000,
        START = 0x1000,
        Y = 0x800,
        X = 0x400,
        B = 0x200,
        A = 0x100,
        L = 0x40,
        R = 0x20,
        Z = 0x10,
        DPAD_UP = 0x8,
        DPAD_DOWN = 0x4,
        DPAD_RIGHT = 0x2,
        DPAD_LEFT = 0x1
    };

private:
    virtual ~TMarioGamePad() = 0;

    u32 _00[0x14 / 4];         //0x0004
    BUTTONS mButtons;          //0x0018
    BUTTONS mFrameButtons;     //0x001C
    u32 _01;                   //0x0020
    u16 _02;                   //0x0024
    u8 mLButtonAnalogU8;       //0x0026
    u8 mRButtonAnalogU8;       //0x0027
    float mLButtonAnalogFloat; //0x0028
    float mRButtonAnalogFloat; //0x002C
    u32 _03[0x18 / 4];         //0x0030
    float mMainStickLeftRight; //0x0048
    float mMainStickUpDown;    //0x004C
    float mMainStickOffset;    //0x0050
    u32 _04;                   //0x0054
    float mCStickLeftRight;    //0x0058
    float mCStickUpDown;       //0x005C
    float mCStickOffset;       //0x0060
    u32 _05;                   //0x0064
    u32 _06[0x78 / 4];         //0x0058
    u16 _07;                   //0x00E0
    u8 _08;                    //0x00E2

    struct
    {
        u8 _00 : 6;
        bool mReadInput : 1;
        u8 _01 : 1;
    } State; //0x00E3

public:
    bool isPressed(BUTTONS input)
    {
        return this->mButtons & input;
    }

    bool isFramePressed(BUTTONS input)
    {
        return this->mFrameButtons & input;
    }

    bool isRecordingInput()
    {
        return this->State.mReadInput;
    }

    BUTTONS getInput() { return this->mButtons; }
    BUTTONS getFrameInput() { return this->mFrameButtons; }
    float getLAnalogf() { return this->mLButtonAnalogFloat; }
    float getRAnalogf() { return this->mRButtonAnalogFloat; }
    u8 getLAnalog() { return this->mLButtonAnalogU8; }
    u8 getRAnalog() { return this->mRButtonAnalogU8; }
    float getMainStickAnalogX() { return this->mMainStickLeftRight; }
    float getMainStickAnalogY() { return this->mMainStickUpDown; }
    float getMainStickDist() { return this->mMainStickOffset; }
    float getCStickAnalogX() { return this->mCStickLeftRight; }
    float getCStickAnalogY() { return this->mCStickUpDown; }
    float getCStickDist() { return this->mCStickOffset; }
    void setEnabled(bool enabled) { this->State.mReadInput = enabled; }
};

class TConductor
{
};

class TDemoCannon : public TMapObjBase
{
};

class TMarioCap
{
};

class TNozzleBase
{

public:
    u32 _00[0x2C / 4];         //0x0000
    float mEmitSize;           //0x002C
    u32 _01[0x24 / 4];         //0x0030
    float mEmitRandom;         //0x0054
    u32 _02[0x10 / 4];         //0x0058
    float mDistance;           //0x0068
    u32 _03[0x38 / 4];         //0x006C
    float mMasterScale;        //0x00A4
    u32 _04[0x10 / 4];         //0x00A8
    float mDistortionStrength; //0x00B8
    u32 _05[0x10 / 4];         //0x00BC
    s32 mMaxWater;             //0x00CC
    u32 _06[0x38 / 4];         //0x00D0
    s16 mWaterStep;            //0x0108
    u32 _10[0x278 / 4];        //0x010C
};

class TNozzleTrigger : public TNozzleBase
{

public:
    u8 _00;    //0x0384
    u8 _01;    //0x0385
    u16 _02;   //0x0386
    float _03; //0x0388
    u32 _04;   //0x038C
};

class TWaterGun
{

public:
    enum NOZZLETYPE 
    {
        Spray,
        Rocket,
        Underwater,
        Yoshi,
        Hover,
        Turbo,
        Sniper
    };

    u32 _00[0x8 / 4];                      //0x0000
    TMario *mMario;                        //0x0008
    TNozzleBase mNozzleDeform;             //0x000C
    TNozzleTrigger mNozzleDeformBomb;      //0x0390
    TNozzleTrigger mNozzleRocket;          //0x0720
    TNozzleBase mNozzleUnderWater;         //0x0AB0
    TNozzleBase mNozzleYoshiDeform;        //0x0E34
    TNozzleTrigger mNozzleYoshiDeformBomb; //0x11B8
    TNozzleTrigger mNozzleHover;           //0x1548
    TNozzleTrigger mNozzleTurbo;           //0x18D8
    TNozzleBase *mNozzleList[6];           //0x1C68
    s32 mCurrentWater;                     //0x1C80
    NOZZLETYPE mCurrentNozzle;             //0x1C84
    NOZZLETYPE mSecondNozzle;              //0x1C85
    bool mIsEmitWater;                     //0x1C86
    u8 _13;                                //0x1C87
    u32 _14[0x8 / 4];                      //0x1C88
    JGeometry::TVec3<float> mGeometry[10]; //0x1C90
    u32 _15;                               //0x1D08
    u32 *mEmitInfo;                        //0x1D0C
    char *mPrmPath;                        //0x1D14
    char *mRocketPath;                     //0x1D18
    u32 _16[0xA0 / 4];                     //0x1D1C
};

class TNozzleBox : public TMapObjBase
{

public:
    u32 _00[0x18 / 4];        //0x0130
    u32 mNozzleToRegen;       //0x0148
    TMapObjBase *mNozzleItem; //0x014C
    float mThrowLength;       //0x0150
    float mThrowHeight;       //0x0154
    char *mStringID;          //0x0158
    bool mIsValid;            //0x015C
    u8 _01;                   //0x015D
    u16 _02;                  //0x015E
    u16 _03;                  //0x0160
    u16 _04;                  //0x0162
};

class TEggYoshi : public TMapObjBase
{

public:
    u32 _00[0x18 / 4]; //0x0130
    MActor *mActor;    //0x0148
    s32 mWantedFruit;  //0x014C
    TMario *mMario;    //0x0150
};

class TWaterManager
{

public:
    u32 _00[0xC / 4];    //0x0000
    u8 mSprayQuality;    //0x000C
    u8 _01;              //0x000D
    u16 _02;             //0x000E
    s16 mMaxEmit;        //0x0010
    s16 mEmitCount;      //0x0012
    u32 _03[0x5D48 / 4]; //0x0014
    u16 _04;             //0x5D5C
    u8 _05;              //0x5D5E
    s8 mWaterCardType;   //0x5D5F

    struct
    {
        u8 _00 : 6;
        bool mMaskObjects : 1;
        bool mShowShadow : 1;
        u8 _01 : 8;
    } LightType; //0x5D60

    u16 _06;             //0x5D62
    u32 _07[0xA8 / 4];   //0x5D64
    float mSize;         //0x5E0C
    float _08[0x30 / 4]; //0x5E10
    float mSphereStep;   //0x5E40
    u8 mLayerCount;      //0x5E44
    u8 mDarkLevel;       //0x5E45
    u8 _09;              //0x5E46
    RGBA<u8> mColor;     //0x5E47
};

class TYoshi
{

public:
    enum TYPE 
    {
        GREEN,
        ORANGE,
        PURPLE,
        PINK
    };

    enum STATE 
    {
        EGG = 0,
        DROWNING = 3,
        DYING = 4,
        UNMOUNTED = 6,
        MOUNTED = 8
    };

    s8 mState;                            //0x0000
    u8 _00;                               //0x0001
    u16 mSubState;                        //0x0002 ??
    u32 _01;                              //0x0004
    s32 mMaxJuice;                        //0x0008
    s32 mCurJuice;                        //0x000C
    TMario *mMario;                       //0x0010
    u32 _02[0xC / 4];                     //0x0014
    JGeometry::TVec3<float> mCoordinates; //0x0020
    u32 _03[0x8 / 4];                     //0x002C
    MActor *mAnimationTable;              //0x0034
    u32 _04[0x4C / 4];                    //0x0038
    float mRedComponent;                  //0x0084
    float mGreenComponent;                //0x0088
    float mBlueComponent;                 //0x008C
    u32 _05[0x28 / 4];                    //0x0090
    u8 mFlutterState;                     //0x00B8
    u8 _06;                               //0x00B9
    u16 mFlutterTimer;                    //0x00BA
    u16 mMaxFlutterTimer;                 //0x00BC
    u16 _07;                              //0x00BE
    float mMaxVSpdStartFlutter;           //0x00C0
    float mFlutterAcceleration;           //0x00C4
    u32 _08[0x8 / 4];                     //0x00C8
    s8 mType;                             //0x00D0
    u8 _09;                               //0x00D1
    u16 _10;                              //0x00D2
    u32 _11[0x1C / 4];                    //0x00D4
    TEggYoshi *mEgg;                      //0x00F0

    bool isGreenYoshi();
    bool isGreenYoshiMounted();
    static bool isGreenYoshiAscendingWater(TMario *gpMario);
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
        ST_HASHELMET_FOLLOWCAMERA = 0x00001000,
        ST_HASHELMET = 0x00002000,
        ST_HASFLUDD = 0x00008000,
        ST_SPLASH = 0x00010000,
        ST_PUDDLE = 0x00020000,
        ST_SHIRT = 0x00100000,
        ST_GONE = 0x00200000
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

    struct CustomInfo
    {
        MarioParamsFile *mParams; //0x0000
        float mTerminalVelocity;  //0x0004

        struct
        {
            bool mIsAirborn : 1;
            bool mIsFaceUsed : 1;
            bool mIsSpinBounce : 1;
            bool mIsDisableInput : 1;
            u16 _00 : 12;
        } /*__attribute__((packed))*/ CollisionFlags; //0x0008

        u16 mPrevCollision;  //0x000A
        s32 mCollisionTimer; //0x000C
        bool mCollisionUsed; //0x0010
        u8 mMaxJumps;        //0x0011
        u8 mCurJump;         //0x0012
        u8 mPlayerID;        //0x0013

        struct
        {
            TWaterGun::NOZZLETYPE mMainNozzle;   //0x0014
            TWaterGun::NOZZLETYPE mSecondNozzle; //0x0015
            s32 mWaterLevel;                     //0x0016
            bool mHadFludd;                      //0x001A
            u8 _00;                              //0x001B
        } /*__attribute__((packed))*/ FluddHistory;                          //0x0014

        float mSizeContext;            //0x001C
        MarioParamsFile *_mBaseParams; //0x0020

        struct
        {
            float mGravity;
            float mBaseBounce1;
            float mBaseBounce2;
            float mBaseBounce3;
            float mMaxFallNoDamage;
            float mThrowPower;

            float mOceanOfs;
            float mWaterJumpHeightDifMax;
            float mWaterHealthDrainSpd;
            float mWaterHealthScubaDrainSpd;
            float _00[5];
        } DefaultAttrs; //0x0024

        bool _mFirstParamsDone; //0x0060
        u8 _00;             //0x0061
        u16 _01;            //0x0062
    };

    /*
    virtual ~TMario() = 0;
    virtual u32 getType() = 0;
    virtual void load(JSUInputStream &stream) = 0;
    virtual void save(JSUOutputStream &stream) = 0;
    virtual void loadAfter() = 0;
    virtual void searchF(u32 unk1, u32 unk2) = 0;
    virtual void perform(JDrama::TGraphics *graphics) = 0;
    */

    u32 _00[0xC / 4];                 //0x0070
    u32 mState;                       //0x007C
    u32 mPrevState;                   //0x0080
    u16 mSubState;                    //0x0084
    u16 mSubStateTimer;               //0x0086
    u32 _01;                          //0x0088
    float mBaseAcceleration;          //0x008C
    u16 mAccelerationDirection;       //0x0090
    u16 _02;                          //0x0092
    JGeometry::TVec3<u16> mAngle;     //0x0094
    u16 _03;                          //0x009A
    u32 _04[0x8 / 4];                 //0x009C
    JGeometry::TVec3<float> mSpeed;   //0x00A4
    float mForwardSpeed;              //0x00B0
    u32 _05[0x24 / 4];                //0x00B4
    TBGCheckData *mWallTriangle;      //0x00D8
    TBGCheckData *mRoofTriangle;      //0x00DC
    TBGCheckData *mFloorTriangle;     //0x00E0
    TBGCheckData *mFloorTriangleCopy; //0x00E4
    float mCeilingAbove;              //0x00E8
    float mFloorBelow;                //0x00EC
    float mWaterHeight;               //0x00F0
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

    u16 mHealth;                     //0x0120
    u16 _08;                         //0x0122
    u32 _09[0x8 / 4];                //0x0124
    float mWaterHealth;              //0x012C
    float mMaxWaterHealth;           //0x0130
    u32 _10[0x28 / 4];               //0x0134
    float mCollisionXZSize;          //0x015C
    u32 _11[0x138 / 4];              //0x0160
    u32 mInitialWater;               //0x0298
    u32 _12[0x10 / 4];               //0x029C
    float mLastGroundedHeight;       //0x02AC
    u32 _13[0x8 / 4];                //0x02B0
    u16 _14;                         //0x02B8
    u16 mOBTimer;                    //0x02BA
    u32 _15[0xC8 / 4];               //0x02BC
    TTakeActor *mGrabTarget;         //0x0384
    u32 _16[0xC / 4];                //0x0388
    J3DDrawBuffer *mDrawBufferA;     //0x0394
    J3DDrawBuffer *mDrawBufferB;     //0x0398
    u32 _17[0xC / 4];                //0x039C
    M3UModelMario *mModelData;       //0x03A8
    u32 _18[0x18 / 4];               //0x03AC
    u8 mBindBoneIDArray[12];         //0x03C4
    u32 _19[0x10 / 4];               //0x03D0
    TMarioCap *mCap;                 //0x03E0
    TWaterGun *mFludd;               //0x03E4
    u32 _20[0x8 / 4];                //0x03E8
    TYoshi *mYoshi;                  //0x03F0
    CustomInfo *mCustomInfo;         //0x03F4
    u32 _21[0x104 / 4];              //0x03F8
    TMarioGamePad *mController;      //0x04FC
    u32 _22[0x8C / 4];               //0x0500
    u16 mMaxHealth;                  //0x058C
    u16 _23;                         //0x058E
    u32 _24[0x10 / 4];               //0x0590
    float mMaxGroundSpeed;           //0x05A0
    u32 _25[0x22C / 4];              //0x05A4
    float mBaseBounceSpeed1;         //0x07D0
    u32 _26[0x10 / 4];               //0x07D4
    float mBaseBounceSpeed2;         //0x07E4
    u32 _27[0x10 / 4];               //0x07E8
    float mBaseBounceSpeed3;         //0x07F8
    u32 _28[0x24 / 4];               //0x07FC
    float mThrowPower;               //0x0820
    u32 _29[0x9C / 4];               //0x0824
    float mMaxFallNoDamage;          //0x08C0
    u32 _30[0xC4 / 4];               //0x08C4
    u16 mOBStep;                     //0x0988
    u16 _31;                         //0x098A
    u32 _32[0x10 / 4];               //0x098C
    u16 mOBMax;                      //0x099C
    u16 _33;                         //0x099E
    u32 _34[0x178 / 4];              //0x09A0
    float mGravity;                  //0x0B18
    u32 _35[0x38 / 4];               //0x0B1C
    float mAirborneHSpeedMul;        //0x0B54
    u32 _36[0x10 / 4];               //0x0B58
    float mDefaultAccelerationMul;   //0x0B68
    u32 _37[0x24 / 4];               //0x0B6C
    float mSideFlipStrength;         //0x0B90
    u32 _38[0xEC / 4];               //0x0B94
    float mHipDropSpeedSlow;         //0x0C80
    u32 _39[0x10 / 4];               //0x0C84
    float mHipDropSpeedFast;         //0x0C94
    u32 _40[0x4A8 / 4];              //0x0C98
    float mWaterJumpHeightDifMax;    //0x1140
    u32 _41[0x24 / 4];               //0x1144
    float mOceanOfs;                 //0x1168
    u32 _42[0x100 / 4];              //0x116C
    float mWaterHealthDrainSpd;      //0x126C
    u32 _43[0x10 / 4];               //0x1270
    float mWaterHealthScubaDrainSpd; //0x1280
    u32 _44[0x10 / 4];               //0x1284
    float mWaterHealthIncreaseSpd;   //0x1294
    u32 _45[0x40 / 4];               //0x1298
    s16 mWallAnimTimer;              //0x12D8
    u16 _46;                         //0x12DA
    u32 _47[0x10 / 4];               //0x12DC
    s16 mWallHangTimer;              //0x12EC
    u32 _48[0x124 / 4];              //0x12F0
    float mTRopeAirborneAccelMul;    //0x1414
    u32 _49[0xE04 / 4];              //0x1418
    float mVSpeedYoshiMul;           //0x221C
    u32 _50[0x4C / 4];               //0x2220
    float mFSpeedFlutterMul;         //0x226C
    u32 _51[0x10 / 4];               //0x2270
    float mBSpeedFlutterMul;         //0x2280
    u32 _52[0x200C / 4];             //0x2284
    float mAllSpeedMultiplier;       //0x4290

    void setCustomAttributes();
};

class TShine : public TMapObjBase
{

public:
    u32 mObjectID;                     //0x0134
    u32 _09a[0x1C / 4];                //0x0138
    u32 mType;                         //0x0154
    u32 _09b[0x50 / 4];                //0x0158
    JGeometry::TVec3<float> mGlowSize; //0x01A8
    u8 isAlreadyObtained;              //0x01B4
    u8 _10;                            //0x01B5
    u16 _11;                           //0x01B6
    u32 _12[0x40 / 4];                 //0x01B8
};

class TMarDirector
{

public:
    enum STATUS 
    {
        INTRO_INIT = 0,
        INTRO_PLAYING = 1,
        NORMAL = 4,
        PAUSE_MENU = 5,
        SAVE_CARD = 11
    };

    enum STATE 
    {
        WARP_OUT = 2
    };

    u32 _00[0x4C / 4];       //0x0000
    u16 mGameState;          //0x004C
    u16 _02;                 //0x004E
    u32 _03[0x14 / 4];       //0x0050
    u8 mLastState;           //0x0064
    u8 _04;                  //0x0065
    u16 _05;                 //0x0066
    u32 _06[0xC / 4];        //0x0068
    TGCConsole2 *mGCConsole; //0x0074
    u32 _07;                 //0x0078
    u8 mAreaID;              //0x007C
    u8 mEpisodeID;           //0x007D
    u16 _09;                 //0x007E
    u32 _10[0x2C / 4];       //0x0080
    u32 *sNextState;         //0x00AC
    u32 _11;                 //0x00B0
    u8 mNextState;           //0x00B4
    u32 _12[0x1C / 4];       //0x00B8
    u32 *mGame6Data;         //0x00D4
    u32 *mAramArchive;       //0x00D8
    u32 _13[0x4C / 4];       //0x00DC
    u8 mNextStateA;          //0x0124 ?
    u32 _14[0x12C / 4];      //0x0128
    TDemoCannon *mCannonObj; //0x0254
    u32 _15;                 //0x0258
    TShine *mCollectedShine; //0x025C
};

class TGameSequence
{

public:
    enum AREA 
    {
        AIRPORT = 0x0,
        DOLPIC = 0x1,
        BIANCO = 0x2,
        RICCO = 0x3,
        MAMMA = 0x4,
        PINNABEACH = 0x5,
        SIRENA = 0x6,
        DELFINO = 0x7,
        MONTE = 0x8,
        MARE = 0x9,
        NONE = 0xA,
        SCALE = 0xB,
        TEST10 = 0xC,
        PINNAPARCO = 0xD,
        CASINO = 0xE,
        OPTION = 0xF,
        MAREUNDERSEA = 0x10,
        DOLPICEX0 = 0x14,
        DOLPICEX1 = 0x15,
        DOLPICEX2 = 0x16,
        DOLPICEX3 = 0x17,
        DOLPICEX4 = 0x18,
        BIANCOEX1 = 0x1D,
        RICOEX0 = 0x1E,
        RICOEX1 = 0x1F,
        MAMMAEX0 = 0x20,
        MAMMAEX1 = 0x21,
        SIRENAEX0 = 0x28,
        SIRENAEX1 = 0x29,
        MONTEEX0 = 0x2A,
        MAREEX0 = 0x2C,
        COROEX0 = 0x2E,
        COROEX1 = 0x2F,
        COROEX2 = 0x30,
        COROEX4 = 0x32,
        COROEX5 = 0x33,
        COROEX6 = 0x34,
        BIANCOBOSS = 0x37,
        DELFINOBOSS = 0x38,
        MAREBOSS = 0x39,
        PINNABOSS = 0x3A,
        RICCOBOSS = 0x3B,
        CORONABOSS = 0x3C
    };

    u8 mAreaID;
    u8 mEpisodeID;
    u16 _00;
};

class TApplication
{

public:
    u32 _00;                        //0x0000
    TMarDirector *mMarDirector;     //0x0004
    u16 _01;                        //0x0008
    TGameSequence mPrevScene;       //0x000A
    TGameSequence mCurrentScene;    //0x000E
    TGameSequence mNextScene;       //0x0012
    u16 _02;                        //0x0016
    u32 _03;                        //0x0018
    JDrama::TDisplay *mDisplay;     //0x001C
    TMarioGamePad *mGamePad1;       //0x0020
    TMarioGamePad *mGamePad2;       //0x0024
    TMarioGamePad *mGamePad3;       //0x0028
    TMarioGamePad *mGamePad4;       //0x002C
    AreaEpisodeArray *mStringPaths; //0x0030
    TSMSFader *mFader;              //0x0034
    u32 _04[0x8 / 4];               //0x0038
    u32 *mJKRExpHeapHi;             //0x0040
};

class SMEFile
{

public:
    struct
    {
        char mMAGIC[4];         //0x0000 ("CODE")
        u32 *mLoadAddress;      //0x0004
        u32 mFileSize;          //0x0008
        u8 mTotalSections;      //0x000C
        bool mIsExecutable;     //0x000D
        u16 mNextSectionOffset; //0x000E
        bool mIsShineShadow;    //0x0010
        bool mIsMario;          //0x0011
        bool mIsYoshi;          //0x0012
        bool mIsMusic;          //0x0013
        bool mIsFludd;          //0x0014
        u8 mShineShadowFlag;    //0x0015

        struct
        {
            bool mWalking : 1; //0x0016
            bool mJumping : 1;
            bool mHovering : 1;
            bool mSliding : 1;
            bool mSwimming : 1;
            bool mMarioOilRunning : 1;
            bool mMarioOilSlip : 1;
            bool mMarioHasFludd : 1;
            bool mMarioHasHelmet : 1;
            bool mMarioHasGlasses : 1;
            bool mMarioHasShirt : 1;
            u8 _00 : 4;
        } MarioStates;

        struct
        {
            u8 _00 : 4;
            bool mIsExMap : 1;
            bool mIsDivingMap : 1;
            bool mIsOptionMap : 1;
            bool mIsMultiPlayerMap : 1;
        } StageType; //0x0018

        u8 mPlayerID; //0x0019
        u16 _01;      //0x001A
        u32 _02;      //0x001C
    } FileHeader;

    struct
    {
        JGeometry::TVec3<float> mCoordinates; //0x0020
        float mSize;                          //0x002C
        float mStep;                          //0x0030
        RGBA<u8> mColor;                      //0x0034
        u8 mLayerCount;                       //0x0038
        u8 mDarkLevel;                        //0x0039
        u16 _00;                              //0x003A
    } Light;

    struct
    {
        float mSpeedMulti;      //0x003C
        float mGravity;         //0x0040
        float mBaseBounce1;     //0x0044
        float mBaseBounce2;     //0x0048
        float mBaseBounce3;     //0x004C
        float mMaxFallNoDamage; //0x0050
        u16 mHealth;            //0x0054
        u16 mMaxHealth;         //0x0056
        u16 mOBStep;            //0x0058
        u16 mOBMax;             //0x005A
        u32 _00;                //0x005C
    } Mario;

    struct
    {
        s32 mMaxJuice;              //0x0060
        RGBA<u8> mGreenYoshi;       //0x0064
        RGBA<u8> mOrangeYoshi;      //0x0068
        RGBA<u8> mPurpleYoshi;      //0x006C
        RGBA<u8> mPinkYoshi;        //0x0070
        float mMaxVSpdStartFlutter; //0x0074
        float mFlutterAcceleration; //0x0078
        u16 mMaxFlutterTimer;       //0x007C
        bool mYoshiHungry;          //0x007E
        bool mIsEggFree;            //0x007F
    } Yoshi;

    struct
    {
        float mVolume;   //0x0080
        float mSpeed;    //0x0084
        float mPitch;    //0x0088
        bool mPlayMusic; //0x008C
        u16 mMusicID;    //0x008D
        u8 mAreaID;      //0x008F
        u8 mEpisodeID;   //0x0090
    } Music;

    struct
    {
        TWaterGun::NOZZLETYPE mPrimaryNozzle;   //0x0091
        TWaterGun::NOZZLETYPE mSecondaryNozzle; //0x0092
        RGBA<u8> mWaterColor;                   //0x0093
        bool mIsColorWater;                     //0x0097
    } Fludd;

    static SMEFile *loadFile(char *stringPath)
    {
        u32 handle[0x3C / 4];
        char buffer[(sizeof(SMEFile) + 63) & -32];
        SMEFile *tmp = (SMEFile *)(((u32)buffer + 31) & -32);

        for (u32 i = 0; DVDGetDriveStatus() != DVD_STATE_END; ++i)
        {
            if (i > 100000)
            {
                return NULL;
            }
        }

        if (!DVDOpen(stringPath, handle))
            return NULL;

        if (DVDReadPrio(handle, tmp, 32, 0, 2) < DVD_ERROR_OK)
        {
            DVDClose(handle);
            return NULL;
        }

        u32 *loadAddress;
        if (tmp->FileHeader.mLoadAddress == NULL)
        {
            loadAddress = (u32 *)malloc(sizeof(SMEFile) + 31, 32); //Create an allocation
        }
        else
        {
            loadAddress = tmp->FileHeader.mLoadAddress;
        }

        if (DVDReadPrio(handle, loadAddress, tmp->FileHeader.mFileSize, 0, 2) < DVD_ERROR_OK)
        {
            DVDClose(handle);
            return NULL;
        }
        DVDClose(handle);
        return (SMEFile *)loadAddress;
    }

    static char *parseExtension(char *filepath, const char *stage, bool generalize = false)
    {
        u32 len = strlen(filepath);

        for (u32 i = 0; stage[i] != '\0' && stage[i] != '.'; ++i)
        {
            if (generalize == true && (stage[i] >= '0' && stage[i] <= '9'))
            {
                filepath[len] = '+';
                filepath[len + 1] = '.';
                filepath[len + 2] = 's';
                filepath[len + 3] = 'm';
                filepath[len + 4] = 'e';
                filepath[len + 5] = '\0';
                return filepath;
            }
            filepath[len] = stage[i];
            ++len;
        }

        filepath[len] = '.';
        filepath[len + 1] = 's';
        filepath[len + 2] = 'm';
        filepath[len + 3] = 'e';
        filepath[len + 4] = '\0';
        return filepath;
    }
};

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
        u8 mJumpCount;                           //0x0000
        bool mCanRideYoshi;                      //0x0001
        bool mCanUseFludd;                       //0x0002
        bool mMarioHasHelmet;                    //0x0003
        bool mMarioHasGlasses;                   //0x0004
        bool mMarioHasShirt;                     //0x0005
        u16 _01;                                 //0x0006
        u16 mHealth;                             //0x0008
        u16 mMaxHealth;                          //0x000A
        u16 mOBStep;                             //0x000C
        u16 mOBMax;                              //0x000E
        float mSizeMultiplier;                   //0x0010
        float _02[0x8 / 4];                      //0x0014
        float mGravityMulti;                     //0x001C
        float mBaseBounce1Multi;                 //0x0020
        float mBaseBounce2Multi;                 //0x0024
        float mBaseBounce3Multi;                 //0x0028
        float mMaxFallNoDamageMulti;             //0x002C
        float mBaseJumpHeightMulti;              //0x0030
        float mMultiJumpMultiplier;              //0x0034
        float mMultiJumpFSpeedMulti;             //0x0038
        float mSpeedMultiplier;                  //0x003C

        struct
        {
            bool mCanUseNozzle[8];        //0x0040
            RGBA<u8> mWaterColor;         //0x0048
            FluddCleanType mCleaningType; //0x004C
            u8 _00;                       //0x004D
            u16 _01;                      //0x004E
            bool mBindToJointID[8];       //0x0050
            bool mCanCleanSeals;          //0x0058
            u8 _02;                       //0x0059
            u16 _03;                      //0x005A
        } FluddAttrs;

        float mWaterHealthMultiplier;   //0x005C
        s32 mNameOffset;                //0x0060
        float mThrowPowerMultiplier;    //0x0064
        float mSlideStrengthMultiplier; //0x0068
        s16 mWallHangMax;               //0x006C
        bool mGoopAffected;             //0x006E
        bool mCanHoldNPCs;              //0x006F
        bool mCanClimbWalls;            //0x0070

    } Attributes;
};

class Vector3D
{

public:
    template <typename T>
    static T getNormalAngle(JGeometry::TVec3<float> vec)
    {
        return (T)radiansToAngle(atan2(vec.x, vec.z));
    }

    static float getResultant(JGeometry::TVec3<float> vec)
    {
        return fabsf__Ff(vec.x) + fabsf__Ff(vec.y) + fabsf__Ff(vec.z);
    }

    static float magnitude(JGeometry::TVec3<float> vec)
    {
        return sqrtf(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z);
    }

    static JGeometry::TVec3<float> getUnitVector(JGeometry::TVec3<float> vec)
    {
        float len = getResultant(vec);
        JGeometry::TVec3<float> unit(vec.x / len, vec.y / len, vec.z / len);
        return unit;
    }

    static JGeometry::TVec3<float> cross(JGeometry::TVec3<float> a, JGeometry::TVec3<float> b)
    {
        JGeometry::TVec3<float> cross((a.y * b.z) - (a.z * b.y),
                                      (a.z * b.x) - (a.x * b.z),
                                      (a.x * b.y) - (a.y * b.x));
        return cross;
    }

    static float dot(JGeometry::TVec3<float> a, JGeometry::TVec3<float> b)
    {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }
    
    static JGeometry::TVec3<float> normalize(JGeometry::TVec3<float> vec)
    {
        float m = magnitude(vec);
        JGeometry::TVec3<float> n(vec.x / m, vec.y / m, vec.z / m);
        return n;
    }

    static JGeometry::TVec3<float> getNormal(JGeometry::TVec3<float> a, JGeometry::TVec3<float> b, JGeometry::TVec3<float> c)
    {
        JGeometry::TVec3<float> vectorA(b.x - a.x, b.y - a.y, b.z - a.z);
        JGeometry::TVec3<float> vectorB(c.x - a.x, c.y - a.y, c.z - a.z);

        JGeometry::TVec3<float> normal = normalize(cross(vectorA, vectorB));
        return normal;
    }
};

template <typename T>
inline T lerp(T a, T b, float factor)
{
    return a + factor * (b - a);
}

template <typename T>
inline T getYAngleBetween(JGeometry::TVec3<float> a, JGeometry::TVec3<float> b)
{
    return (T)radiansToAngle(atan2f(b.x - a.x, b.z - a.z));
}

inline JGeometry::TVec3<float> getTriCenter(JGeometry::TVec3<float> a, JGeometry::TVec3<float> b, JGeometry::TVec3<float> c)
{
    JGeometry::TVec3<float> center((a.x + b.x + c.x) / 3.0f, (a.y + b.y + c.y) / 3.0f, (a.z + b.z + c.z) / 3.0f);
    return center;
}

inline float angleToRad(float angle)
{
    return (PI / 180.0f) * angle;
}

inline float radToAngle(float rad)
{
    return (180.0f / PI) * rad;
}

inline u32 branchToAddr(u32 *bAddr)
{
    s32 offset;
    if (*bAddr & 0x2000000)
        offset = (*bAddr & 0x3FFFFFD) - 0x4000000;
    else
        offset = *bAddr & 0x3FFFFFD;
    return (u32)bAddr + offset;
}

/*
/ r = roof (max value)
/ b = steepness
/ c = x offset
/ f = floor (min value)
/
/ Graphing Calculator: https://www.desmos.com/calculator/kn9tpwdan5
*/
static float sigmoidCurve(float x, float f, float r, float c, float b)
{
    float roof = fabsf__Ff(r - f);
    return f + (roof / (1.0f + expf((b * -1.0f) * (x - c))));
}

class CollisionLink
{

public:
    TBGCheckData *mColTriangle; //0x0000
    u8 mTargetID;               //0x0004
    u8 mThisID;                 //0x0005
    u16 mPadding;               //0x0006

    CollisionLink() {}
    CollisionLink(TBGCheckData *tri, u8 targetID, u8 selfID)
    {
        this->mColTriangle = tri;
        this->mTargetID = targetID;
        this->mThisID = selfID;
    }
};

class WarpCollisionList
{

public:
    u32 arrayLength;              //0x0000
    CollisionLink mColList[0xFF]; //0x0004

    TBGCheckData *resolveCollisionWarp(TBGCheckData *colTriangle)
    {
        if ((u8)(colTriangle->mValue4 >> 8) == 0xFF)
            return NULL;

        return this->getNearestTarget(colTriangle);
    }

    TBGCheckData *getNearestTarget(TBGCheckData *colTriangle)
    {
        CollisionLink members[0xFF];
        float nearestDist = 1000000.0f;
        s32 index = 0;

        for (u32 i = 0; i < this->arrayLength; ++i)
        {
            if (this->mColList[i].mThisID == 0xFF)
                continue;
            if (this->mColList[i].mThisID == (u8)(colTriangle->mValue4 >> 8) && this->mColList[i].mColTriangle != colTriangle)
            {
                members[index] = this->mColList[i];
                index++;
            }
        }

        JGeometry::TVec3<float> a;
        JGeometry::TVec3<float> b;
        JGeometry::TVec3<float> _set;
        index = -1;

        for (u32 i = 0; i <= index; ++i)
        {
            a = getTriCenter(colTriangle->mVertexA, colTriangle->mVertexB, colTriangle->mVertexC);
            b = getTriCenter(members[i].mColTriangle->mVertexA, members[i].mColTriangle->mVertexB, members[i].mColTriangle->mVertexC);
            _set = JGeometry::TVec3<float>(b.x - a.x, b.y - a.y, b.z - a.z);

            if (Vector3D::magnitude(_set) < nearestDist)
            {
                nearestDist = Vector3D::magnitude(_set);
                index = i;
            }
        }
        if (index == -1)
            return NULL;

        return members[index].mColTriangle;
    }
};

struct CustomInfo
{
    SMEFile *mFile; //0x0000

    struct
    {
        u16 mPrevShineCount;                             //0x0004
        bool mSizeMorphing;                              //0x0006
        u8 mLightType;                                   //0x0007
        float mPrevSize;                                 //0x0008
        float mNextSize;                                 //0x000C
        JGeometry::TVec3<float> mShineShadowCoordinates; //0x0010
        float mShineShadowBase;                          //0x001C
        float mStepContext;                              //0x0020
    } Light;

    struct
    {
        JGeometry::TVec3<float> yoshiWaterSpeed; //0x0024
    } Mario;

    struct
    {
        TWaterGun::NOZZLETYPE mCurrentNozzle; //0x0030
        TWaterGun::NOZZLETYPE mSecondNozzle;  //0x0031
        u16 _00;                              //0x0032
        s32 mCurrentWater;                    //0x0034
        bool mHadFludd;                       //0x0038
        u8 _01;                               //0x0039
    } Fludd;

    struct
    {
        TMario *mMario[4];     //0x003C
        u8 mCurPlayerCount;    //0x004C
        u8 mMaxPlayerCount;    //0x004D
        bool mIsActive[4];     //0x004E
        u8 mCurPlayerID[4];    //0x0052
        u8 mCurPlayerTimer[4]; //0x0056
        u8 mMaxPlayerTimer;    //0x005A
        bool mIsFreePlay;      //0x005B
    } PlayerData;

    u32 *mCharacterHeap;                      //0x005C
    bool mIsCompletionRewards;                //0x0060
    bool mIsAudioStreaming;                   //0x0061
    bool mIsEmulated;                         //0x0062
    bool mIsAudioStreamAllowed;               //0x0063
    u32 *mPRMFile;                            //0x0064
    WarpCollisionList *mWarpColArray;         //0x0068
    MarioParamsFile *_mCharacterFile;         //0x006C
    WarpCollisionList *mWarpColPreserveArray; //0x0070
    u32 *mGame6Heap;                          //0x0074
};

static CustomInfo gInfo;
static RGBA<u8> waterColor;
static RGBA<u8> juiceColor;
static RGBA<u8> yoshiColor;

#endif