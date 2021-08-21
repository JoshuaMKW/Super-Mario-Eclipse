#pragma once

#include "types.h"

class JUTRect
{
public:
    JUTRect();
	JUTRect(s32 x1, s32 y1, s32 x2, s32 y2);
	
	void set(s32 x1, s32 y1, s32 x2, s32 y2);
	void copy(const JUTRect &);
	void add(s32, s32);
	u32 intersect(const JUTRect &);
	void move(s32 x, s32 y);
	void resize(s32 width, s32 height);
	void reform(s32, s32, s32, s32);
	void normalize();
	bool isEmpty() const;

    s32 mX1;
    s32 mY1;
    s32 mX2;
    s32 mY2;
};