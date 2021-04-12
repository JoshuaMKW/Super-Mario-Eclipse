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

	void drawFullSet(int, int, int, int, J2DBinding, u32, bool, J2DWrapMode, J2DWrapMode, Mtx *);
	void draw(int, int, int, int, bool, bool, bool);
	void drawTexCoord(int, int, int, int, f32, f32, f32, f32, f32, f32, f32, f32, Mtx *);
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
};