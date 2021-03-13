#pragma once

#include "types.h"
#include "OS.h"
#include "JKRDisposer.hxx"

class JKRAramHeap : public JKRDisposer
{
public:
	enum EAllocMode
	{
		HEAD = 0,
		TAIL = 1
	};

	JKRAramHeap(u32, u32);
	virtual ~JKRAramHeap();

	void* alloc(u32 size, EAllocMode allocMode);
	void* allocFromHead(u32 size);
	void* allocFromTail(u32 size);

	OSMutex mMutex; // _18
	s32 mSize;		// _30
	u32 _34;
	u32 _38;
	u32 _3C;
	u8 _40;
	u8 _41;
	u8 _42;
	u8 _43;
	u8 _44;
};