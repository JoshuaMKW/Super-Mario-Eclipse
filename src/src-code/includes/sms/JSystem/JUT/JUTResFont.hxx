#pragma once

#include "types.h"
#include "JUTColor.hxx"
#include "JUTFont.hxx"
#include "../JKR/JKRArchivePri.hxx"

class ResFONT;

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