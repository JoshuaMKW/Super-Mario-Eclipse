#pragma once

#include "types.h"
#include "J3DPacket.hxx"

class J3DDrawBuffer
{
public:
	J3DDrawBuffer(size_t);

	void draw() const;
	void drawHead() const;
	void drawTail() const;
	bool entryImm(J3DPacket *, u16);
	bool entryInvalidSort(J3DMatPacket *);
	bool entryMatAnmSort(J3DMatPacket *);
	bool entryMatSort(J3DMatPacket *);
	bool entryModelSort(J3DMatPacket *);
	bool entryNonSort(J3DMatPacket *);
	bool entryZSort(J3DMatPacket *);
	void frameInit();

	u32 *mFrames; //?
	size_t mSize; // _4
	u32 _8;
	u32 _C;
	f32 _10;
	f32 _14;
	f32 _18;
	u32 _1C;
	J3DPacket *mPacket;
};