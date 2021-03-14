#pragma once

#include "types.h"
#include "Mtx.h"
#include "../JSU/JSUInputStream.hxx"
#include "../JSU/JSUList.hxx"
#include "../JUT/JUTColor.hxx"
#include "../JUT/JUTRect.hxx"

class J2DGrafContext
{
public:
    J2DGrafContext(int, int, int, int);
	J2DGrafContext(const JUTRect &);
	~J2DGrafContext();

    void place(const JUTRect &);
	void place(int, int, int, int);
	void setPort();
	void setup2D();
	void setScissor();
	void setLookat();

	void scissor(const JUTRect &);
	void fillBox(const JUTRect &);

    u32 *vTable; // _0
    u32 _4; // padding?
    JUTRect _8;
    JUTRect _18;
    JUtility::TColor _28;
    JUtility::TColor _2C;
    JUtility::TColor _30;
    JUtility::TColor _34;
    u8 mLineWidth; // _38
    u8 _39;        // padding?
    u8 _3A;        // ^^
    u8 _3B;        // ^^
    u32 _3C;
    u32 _40;
    u8 _44[0xB4 - 0x44]; // who knows what's here
    u32 _B4;
    u32 _B8;
    u32 _BC;
    u32 _C0;
    u32 _C4;
    u32 _C8;
    u32 _CC;
    u32 _D0;
    u32 _D4;
};

class J2DPane
{
public:
    J2DPane();
	J2DPane(J2DPane *, s16, bool, u32, const JUTRect &);
	J2DPane(s16, u32, const JUTRect &);
	J2DPane(J2DPane *, JSURandomInputStream *, bool);
	virtual ~J2DPane();
	
	virtual void drawSelf(int, int, Mtx *);
	virtual bool setConnectParent(bool);
	virtual void drawSelf(int, int);
	virtual void resize(int, int);
	virtual void move(int, int);
	virtual void add(int, int);
	virtual u32* search(u32);
	virtual void makeMatrix(int, int);

	void draw(int, int, const J2DGrafContext *, bool);
	void clip(const JUTRect &);
	void setCullBack(u32); // GXCullback
	void setBasePosition(u32); // J2DBasePosition

    u16 _4;
    u16 _6;       // padding?
    u32 id;       // _8
    bool isVisible; // _C
    u8 _D;        // padding?
    u8 _E;        // ^^
    u8 _F;        // ^^
    u32 _10;
    JUTRect rect;         // _14
    JUTRect cRect;        // _24
    JUTRect clipRect;     // _34
    JUTRect scissorBound; // _44
    Mtx _54;
    u8 _84[0xB4 - 0x84];
    u32 _B4;
    u32 _B8;
    u8 _BC;
    f32 rotation;     // _C0
    u32 basePosition; // _C4 (J2DBasePosition)
    u32 cullingMode;  // _C8
    u8 _CC;
    u8 _CD;
    u8 _CE;
    u8 _CF; // connectParent
    JSUPtrList _D0;
    JSUPtrLink _DC;
};