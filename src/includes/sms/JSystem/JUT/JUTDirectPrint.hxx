#pragma once

#include "types.h"

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