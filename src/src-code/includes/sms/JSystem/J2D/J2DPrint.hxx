#pragma once

#include "types.h"
#include "J2DTextBox.hxx"
#include "../JUT/JUTColor.hxx"
#include "../JUT/JUTFont.hxx"

class J2DPrint
{
public:
    J2DPrint(JUTFont *, int, int, JUtility::TColor, JUtility::TColor);
	J2DPrint(JUTFont *, int);
	virtual ~J2DPrint();

	void initiate();
	void private_initiate(JUTFont *, int, int, JUtility::TColor, JUtility::TColor);
	static void setBuffer(u32);
	void locate(int, int);
	void setFontSize();
	void print(int, int, const char *, ...);
	void print(int, int, u8, const char *, ...);
	void getWidth(const char *, ...);
	void printReturn(const char *, int, int, J2DTextBoxHBinding, J2DTextBoxVBinding, int, int, u8);
    
    JUTFont *mFont; // _4
    u32 _8;
    u32 _C;
    u8 _10[0x38];
};