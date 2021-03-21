#pragma once

#include "types.h"
#include "JSUInputStream.hxx"
#include "JSUOutputStream.hxx"

class JSUMemoryInputStream : public JSURandomInputStream
{
public:
	virtual ~JSUMemoryInputStream();

	virtual void readData(void *, s32) override;
	virtual s32 getLength() const override;
	virtual s32 getPosition() const override;
	virtual s32 seekPos(s32, JSUStreamSeekFrom) override;

	void setBuffer(const void *, s32);

	void *mBuffer;
	u32 mLength;	  // _C
	u32 mPosition; // _10
};

class JSUMemoryOutputStream : public JSURandomOutputStream
{
public:
	virtual ~JSUMemoryOutputStream();

	virtual void writeData(void *, s32) override;
	virtual s32 getLength() const override;
	virtual s32 getPosition() const override;
	virtual s32 seek(s32, JSUStreamSeekFrom) override;
	virtual s32 getAvailable() const override;
	virtual s32 seekPos(s32, JSUStreamSeekFrom) override;

	void *mBuffer;
	u32 mLength;	  // _C
	u32 mPosition; // _10
};