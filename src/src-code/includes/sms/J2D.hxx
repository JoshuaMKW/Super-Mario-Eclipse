#ifndef J2D_H
#define J2D_H

#include "types.h"
#include "MTX.h"
#include "sms/JKR.hxx"
#include "sms/JSUStream.hxx"
#include "sms/JUT.hxx"

typedef u32 *ResFONT; // just to be more percise

class J2DGrafContext
{
public:
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

class J2DOrthoGraph : public J2DGrafContext
{
public:
    JUTRect _D8;
    u32 _DC;
    u32 _E0;
    u32 _E4;
    f32 _E8;
    f32 _EC;
};

class J2DPane
{
public:
    u32 *vTable; // _0
    u16 _4;
    u16 _6;       // padding?
    u32 id;       // _8
    u8 isVisible; // _C
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

enum J2DTextBoxHBinding
{
    Center,
    Right,
    Left
};

enum J2DTextBoxVBinding
{
    VBindingCenter,
    VBindingBottom,
    VBindingTop
};

enum J2DBinding
{
    BindingBottom = (1 << 0),
    BindingTop = (1 << 1),
    BindingRight = (1 << 2),
    BindingLeft = (1 << 3),
};

enum J2DWrapMode
{
    None,
    Clamp,
    Repeat,
    Mirror,
};

class J2DScreen : public J2DPane
{
public:
    u8 _EC;
    u8 _ED; // padding?
    u8 _EE; // ^^
    u8 _EF; // ^^
    u32 _D0;
    u8 _D4[0xF0 - 0xD4];
    u32 _F0;
};

class J2DSetScreen : public J2DScreen
{
};

class J2DTextBox : public J2DPane
{
public:
    JUTFont *font; // _EC
    u32 _F0;
    u32 _F4;
    J2DTextBoxHBinding hBinding; // _F8
    J2DTextBoxVBinding vBinding; // _FC
    u32 *strPtr;
    u32 _104;
    u32 _108;
    u32 _10C;
    u32 _110;
    u32 _114;
    u32 _118;
    u32 _11C;
    u32 _120;
};

class J2DPicture : public J2DPane
{
public:
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

class J2DPrint
{
public:
    u32 *vTable;        // _0
    JUTFont *font; // _4
    u32 _8;
    u32 _C;
    // there's more
};

#endif