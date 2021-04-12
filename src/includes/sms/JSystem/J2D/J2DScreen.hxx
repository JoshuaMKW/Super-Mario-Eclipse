#pragma once

#include "types.h"
#include "J2DPane.hxx"
#include "../JKR/JKRArchivePri.hxx"

class J2DScreen : public J2DPane
{
public:
    virtual ~J2DScreen();

	virtual void drawSelf(int, int, Mtx *);
	virtual u32* search(u32);

	void makeHiearachyPanes(J2DPane *, JSURandomInputStream *, bool, bool, bool, u32 *);
	u32 makeUserPane(s16, J2DPane *, JSURandomInputStream *);
	u32 makeUserPane(u32, J2DPane *, JSURandomInputStream *);

	void stop();
	void draw(u32, u32, const J2DGrafContext *);

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
public:
    J2DSetScreen(const char *, JKRArchive *);

	void makeHiearachyPanes(J2DPane *, JSURandomInputStream *, bool, bool, bool, u32 *);
};