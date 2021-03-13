#pragma once

#include "types.h"
#include "JSUInputStream.hxx"
#include "JSUOutputStream.hxx"

class JSUMemoryInputStream : public JSURandomInputStream
{
public:
	void *_8;
	u32 mLength;	  // _C
	u32 mPosition; // _10
};

class JSUMemoryOutputStream : public JSURandomOutputStream
{
public:
	u32 _8;
	u32 mLength;	  // _C
	u32 mPosition; // _10
};