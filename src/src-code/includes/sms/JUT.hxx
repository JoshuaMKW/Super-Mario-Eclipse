#ifndef JUT_H
#define JUT_H

#include "types.h"
#include "OS.h"
#include "PAD.h"

#include "sms/JKR.hxx"

namespace JUtility
{
    class TColor
    {
    public:
        TColor();
        TColor(const u8 R, const u8 G, const u8 B, const u8 A)
        {
            this->R = R;
            this->G = G;
            this->B = B;
            this->A = A;
        }

        TColor &operator =(const TColor &other)
        {
            this->R = other.R;
            this->G = other.G;
            this->B = other.B;
            this->A = other.A;
            return *this;
        }

        u8 R; // _0
        u8 G; // _1
        u8 B; // _2
        u8 A; // _3
    };
}; // namespace JUtility

class JUTPoint
{
public:
    u32 X; // _0
    u32 Y; // _4
};

class JUTRect
{
public:
    s32 X;       // _0
    s32 Y;       // _4
    s32 mHeight; // _8
    s32 mWidth;  // _C
};

/* Size -- 0x1C */
class JUTFont
{
public:
    u32 *vTable; // _0
    u8 _4;
    u8 _5; // padding?
    u8 _6; // ^^
    u8 _7; // ^^
    u32 _8;
    JUtility::TColor mColor1; // _C
    JUtility::TColor mColor2; // _10
    JUtility::TColor mColor3; // _14
    JUtility::TColor mColor4; // _18
};

/* Size -- 0x6C */
class JUTResFont : public JUTFont
{
public:
    u32 _1C;
    u32 _20;
    u8 _24[0x44 - 0x24]; // GXTexObj
    u32 _44;
    u32 _48;
    u32 _4C; // padding?
    u32 _50;
    u32 _54;
    u32 _58;
    u16 _5C;
    u16 _5E;
    u16 _60;
    u16 _62;
    u16 _64;
    u16 _66;
    u32 _68;
};

enum EPadPort
{
    Port1,
    Port2,
    Port3,
    Port4
};

enum EStickMode
{
    NonClamped,
    Clamped
};

enum WhichStick
{
    ControlStick,
    CStick
};

class JUTGamePad : public JKRDisposer
{
public:
	virtual ~JUTGamePad();

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

    class CButton
    {
    public:
        u32 mInput; // _0
        u32 mFrameInput;
        u32 _8;
        u8 mAnalogA;  // _C
        u8 mAnalogB;  // _D
        u8 mTriggerL; // _E
        u8 mTriggerR; // _F
        f32 mAnalogL; // _10
        f32 mAnalogR; // _14
        u32 _18;
        u32 _1C;
        u32 _20;
        u32 _24;
        u32 _28;
        u32 _2C;
    };

    class CStick
    {
    public:
        f32 mStickX;
        f32 mStickY;
        f32 mLengthFromNeutral;
        u16 mAngle;
        u16 _E; // i assume unused
    };

    class CRumble
    {
    public:
        u32 _0;
        u32 _4;
        u32 _8;
        u32 _C;
    };

    class C3ButtonReset
    {
    public:
        static s32 sResetPattern;
    };

    CButton mButtons;     // _18
    CStick mControlStick; // _48
    CStick mCStick;       // _58
    CRumble mCumble;      // _68
    u16 mPort;            // _78
    u16 _7A;              // padding?
    JSUPtrLink mPtrLink;  // _7C
    u32 _8C;
    u32 _90;
    u32 _94;
    u32 _98;
    u8 mResetFlag;     // _9C
    u8 _9D;            // padding?
    u8 _9E;            // ^^
    u8 _9F;            // ^^
    OSTick  mResetTime; // _A0
};

class JUTNameTab
{
public:
    u32 *mResTab; // ResNTab*
    u32 _4;
    u16 _8;
};

class JUTDirectPrint
{
public:
    void *mBuffer; // _0
    u16 _4;
    u16 _6;
    u16 _8;
    u16 _A; // padding?
    u32 _C;
    u32 _10;
    void *_14;
};

class JUTException : public JKRThread
{
public:
    virtual ~JUTException();

	virtual void run();

    u32 _60;
    JUTDirectPrint *mPrinter; // _64
};

#endif