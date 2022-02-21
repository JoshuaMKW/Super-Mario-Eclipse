#pragma once

#include "types.h"
#include "JSUStream.hxx"

class JSUOutputStream : public JSUIosBase
{
public:
	virtual ~JSUOutputStream();

	virtual void skip(s32, signed char);
	virtual void writeData(void *, s32) = 0;

	void write(const void *, s32);
};

class JSURandomOutputStream : public JSUOutputStream
{
public:
	virtual ~JSURandomOutputStream();

	virtual size_t getLength() const = 0;
	virtual s32 getPosition() const = 0;
	virtual s32 seek(s32, JSUStreamSeekFrom);
	virtual s32 getAvailable() const;
	virtual s32 seekPos(s32, JSUStreamSeekFrom) = 0;
};