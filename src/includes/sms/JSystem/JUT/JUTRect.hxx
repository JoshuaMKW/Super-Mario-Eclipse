#pragma once

#include "types.h"

class JUTRect
{
public:
    JUTRect();
	JUTRect(s32 x, s32 y, s32 width, s32 height);
	
	void set(s32 x, s32 y, s32 width, s32 height);
	void copy(const JUTRect &);
	void add(s32, s32);
	u32 intersect(const JUTRect &);
	void move(s32 x, s32 y);
	void resize(s32 width, s32 height);
	void reform(s32, s32, s32, s32);
	void normalize();
	bool isEmpty() const;

    s32 mX;
    s32 mY;
    s32 mHeight;
    s32 mWidth;
};