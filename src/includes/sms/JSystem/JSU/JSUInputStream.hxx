#pragma once

#include "types.h"
#include "JSUStream.hxx"

class JSUInputStream : public JSUIosBase
{
public:
	JSUInputStream();
	virtual ~JSUInputStream();

	virtual s32 getAvailable() const = 0;
	virtual void skip(s32);
	virtual void readData(void *, s32) = 0;

	void read(void *, s32);
	void readString(void *, u16);
	char *readString();
};

class JSURandomInputStream : public JSUInputStream
{
public:
	virtual ~JSURandomInputStream();

	virtual s32 getAvailable() const override;
	virtual void skip(s32) override;
	virtual s32 getLength() const = 0;
	virtual s32 getPosition() const = 0;
	virtual s32 seekPos(s32, JSUStreamSeekFrom) = 0;

	u32 align(s32);
	u32 peek(s32);
	void seek(s32, JSUStreamSeekFrom);
};