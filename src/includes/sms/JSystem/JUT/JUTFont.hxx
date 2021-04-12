#pragma once

#include "types.h"
#include "JUTColor.hxx"

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
    u32 _8;
    JUtility::TColor mColor1; // _C
    JUtility::TColor mColor2; // _10
    JUtility::TColor mColor3; // _14
    JUtility::TColor mColor4; // _18
};