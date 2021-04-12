#pragma once

#include "types.h"
#include "JKRAramHeap.hxx"
#include "../JSU/JSUList.hxx"

class JKRAramBlock
{
public:
	JKRAramBlock(u32, u32, u32, u8, bool);
	virtual ~JKRAramBlock();

	JKRAramBlock *allocHead(u32, u8, JKRAramHeap *);
	JKRAramBlock *allocTail(u32, u8, JKRAramHeap *);
	
	JSUPtrLink mBlockPtr; // _4
	u32 _14;
	u32 _18;
	u32 _1C;
	u8 _20;
	u8 _21;
	u8 _22; // padding?
	u8 _23; // ^^
};