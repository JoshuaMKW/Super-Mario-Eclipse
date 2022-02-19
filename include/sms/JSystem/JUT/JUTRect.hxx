#pragma once

#include "types.h"

class JUTRect
{
public:
    JUTRect();
	JUTRect(int x1, int y1, int x2, int y2);
	
	void set(int x1, int y1, int x2, int y2);
	void copy(const JUTRect &);
	void add(int, int);
	u32 intersect(const JUTRect &);
	void move(int x, int y);
	void resize(int width, int height);
	void reform(int, int, int, int);
	void normalize();
	bool isEmpty() const;

    int mX1;
    int mY1;
    int mX2;
    int mY2;
};