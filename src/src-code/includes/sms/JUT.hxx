#pragma once

#include "types.h"
#include "OS.h"
#include "PAD.h"

#include "sms/JKR.hxx"

class ResFONT;

namespace JUtility
{
    class TColor
    {
    public:
        TColor();
		void set(u8 r, u8 g, u8 b, u8 a);

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
    JUTPoint();

    u32 X; // _0
    u32 Y; // _4
};

class JUTRect
{
public:
    JUTRect();
	JUTRect(s32 x, s32 y, s32 width, s32 height);
	
	void set(s32 x, s32 y, s32 width, s32 height);
	void copy(const JUTRect &);
	void add(s32, s32);
	u32 intersect(const JUTRect &);
	void move(s32 x, s32 y);
	void resize(s32 width, s32 height);
	void reform(s32, s32, s32, s32);
	void normalize();
	bool isEmpty() const;

    s32 X;       // _0
    s32 Y;       // _4
    s32 mHeight; // _8
    s32 mWidth;  // _C
};

/* Size -- 0x1C */
class JUTFont
{
public:
    JUTFont();
	~JUTFont();
	
	void initiate(); // nullsub
	void setCharColor(JUtility::TColor);
	void setGradColor(JUtility::TColor, JUtility::TColor);
	void drawString_size_scale(f32, f32, f32, f32, const char *, u32, bool);
	void setGX(JUtility::TColor, JUtility::TColor);

    u32 _0;
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
    JUTResFont(const u32 *, JKRArchive *); // ResFont
	~JUTResFont();
	
	void initiate(const u32 *, JKRArchive *); // ResFont
	void setGX();
	void setGX(JUtility::TColor, JUtility::TColor);

    u32 _1C;
    u32 _20;
    u8 _24[0x44 - 0x24]; // GXTexObj
    u32 _44;
    ResFONT *mFont;
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
    Port4,
    PortRecorder
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
    JUTGamePad(EPadPort port);
	virtual ~JUTGamePad();

	void assign();
	void update();

    enum Buttons
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
        CButton();

		void clear();
		void update(const PADStatus *, u32);
		void setRepeat(u32, u32, u32);

        Buttons mInput; // _0
        Buttons mFrameInput;
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
        CStick();

		void clear();
		u32 update(s8, s8, EStickMode, WhichStick);
		u32 getButton();

        f32 mStickX;
        f32 mStickY;
        f32 mLengthFromNeutral;
        u16 mAngle;
        u16 _E; // i assume unused
    };

    class CRumble
    {
    public:
        void clear(JUTGamePad *);
		static void stopMotor(s32);
		static void stopMotorHard(s32);
		void update(u16);
		void setEnable(u32);

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
    u8 mPortConnected;    // _79
    u8 _7A;
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

class ResNTab
{
};

class JUTNameTab
{
public:
    JUTNameTab(const ResNTab *); // ResNTab*

	s32 getIndex(const char *name) const;
	u16 calcKeyCode(const char *name) const;
	char* getName(const u16);

    ResNTab *mResTab; // ResNTab*
    u32 _4;
    u16 _8;
};

class JUTDirectPrint
{
public:
    JUTDirectPrint();

	void start();
	void erase(int, int, int, int);
	void drawChar(int, int, int);
	void changeFrameBuffer(void *buffer, u16, u16);
	void drawString(u16, u16, char *);
	void drawString_f(u16, u16, const char *, ...);

    void *mBuffer; // _0
    u16 _4;
    u16 _6;
    u16 _8;
    u16 _A; // padding?
    u32 _C;
    u32 _10;
    void *_14;
};

class JUTConsole : public JKRDisposer
{
public:
    enum EConsoleType
    {
    };

    JUTConsole(unsigned int, unsigned int, bool);
    ~JUTConsole();

    void clear();
    JUTConsole *create(unsigned int, void *, u32);
    void doDraw(JUTConsole::EConsoleType) const;
    u32 getLineFromObjectSize(u32, unsigned int);
    u32 getLineOffset() const;
    u32 getUsedLine() const;
    void print(const char *);
    void print_f(const char *, ...);
    void scroll(int);

    u8 _00[0x4E];
};

class JUTException : public JKRThread
{
public:
    JUTException(JUTDirectPrint *printer);
	virtual ~JUTException();

	virtual void run();

	static void appendMapFile(char *mapPath);
    void create(JUTDirectPrint *);
	void createFB();
    void errorHandler(u16, OSContext *, u32, u32);
	void printContext(u16, OSContext *, u32, u32);
	static bool queryMapAddress(char *, u32, s32, u32 *, u32 *, char *, u32, bool, bool);

    void showFloat(OSContext *);
    void showGPR(OSContext *);
    void showGPRMap(OSContext *);
    void showMapInfo_subroutine(u32, bool);
    void showStack(OSContext *);

    u32 _60;
    JUTDirectPrint *mPrinter; // _64
};

void JUTConsole_print_f_va_(void);