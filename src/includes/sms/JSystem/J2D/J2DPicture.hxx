#pragma once

#include "types.h"
#include "MTX.h"
#include "J2DPane.hxx"
#include "../JSU/JSUInputStream.hxx"

enum J2DBinding
{
    BindingBottom = (1 << 0),
    BindingTop = (1 << 1),
    BindingRight = (1 << 2),
    BindingLeft = (1 << 3),
};

enum J2DWrapMode
{
    WrapNone,
    WrapClamp,
    WrapRepeat,
    WrapMirror,
};

class J2DPicture : public J2DPane
{
public:
    J2DPicture(J2DPane *, JSURandomInputStream *, bool);
	virtual ~J2DPicture();

	virtual void drawSelf(int, int);
	virtual void drawSelf(int, int, Mtx *);

	void drawFullSet(int x, int y, int w, int h, J2DBinding binding, u32, bool, J2DWrapMode wrapH, J2DWrapMode wrapV, Mtx *);
	void draw(int x, int y, int w, int h, bool invertX, bool invertY, bool rotate90);
	void drawTexCoord(int x, int y, int w, int h, f32, f32, f32, f32, f32, f32, f32, f32, Mtx *);
    void remove(u8);
	void setTevMode();
	void swap(f32 &, f32 &);
	void setBlendKonstColor();
	void setBlendKonstAlpha();

    u32 *_EC;
    u8 _F0[0xFC - 0xF0];
    u8 _FC; // padding?
    u8 _FD; // ^^
    u8 _FE; // ^^
    u8 _FF; // ^^
    u8 _100[0x128 - 0x100];
    u32 _128;
    u32 _12C;
    u8 _130;
    u8 _131; // padding?
    u8 _132; // ^^
    u8 _133; // ^^
    u32 _134;
    u32 _138;
    JUtility::TColor mFillColor;
    u32 _140;
    u32 _144;
    u32 _148;
    u32 _14C;
    u32 _150;
    u32 _154;
    u32 _158;
};