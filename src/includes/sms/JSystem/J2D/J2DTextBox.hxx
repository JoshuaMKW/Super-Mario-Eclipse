#pragma once

#include "types.h"
#include "J2DPane.hxx"
#include "../JSU/JSUInputStream.hxx"
#include "../JUT/JUTColor.hxx"
#include "../JUT/JUTFont.hxx"
#include "../JUT/JUTResFont.hxx"

enum class J2DTextBoxHBinding
{
    Center,
    Right,
    Left
};

enum class J2DTextBoxVBinding
{
    Center,
    Bottom,
    Top
};

class J2DTextBox : public J2DPane
{
public:
    J2DTextBox() {}
    J2DTextBox(const ResFONT *, const char *);
	J2DTextBox(J2DPane *, JSURandomInputStream *, bool);
	J2DTextBox(u32, const JUTRect &, const ResFONT *, const char *, J2DTextBoxHBinding, J2DTextBoxVBinding);
	virtual ~J2DTextBox();
	
	virtual void resize(int, int);
	virtual bool setConnectParent(bool);
	virtual void drawSelf(int, int);
	virtual void drawSelf(int, int, Mtx *);

	void initiate(const ResFONT *, const char *, J2DTextBoxHBinding, J2DTextBoxVBinding);
	
	char *getStringPtr() const;
	size_t setString(const char *, ...);
	void setFont(JUTFont *);
	void draw(int, int);

    JUTFont *mFont; // _EC
    u32 _F0;
    u32 _F4;
    J2DTextBoxHBinding mHBinding; // _F8
    J2DTextBoxVBinding mVBinding; // _FC
    u32 *mStrPtr;
    u32 _104;
    u32 _108;
    JUtility::TColor mColor;
    JUtility::TColor mGradient;
    u32 _114;
    u32 _118;
    u32 _11C;
    u32 _120;
    bool mInitialized;
};