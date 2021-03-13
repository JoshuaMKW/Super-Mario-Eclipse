#pragma once

#include "types.h"

enum JSUStreamSeekFrom
{
	Unknown1,
	Unknown2,
	Unknown3,
	Unknown4
};

class JSUIosBase
{
public:
	virtual ~JSUIosBase() = 0;
    virtual s32 getAvailable() const = 0;
    virtual void skip(s32 len) = 0;
    virtual void readData(void *dest, s32 len) = 0;
    virtual s32 getLength() const = 0;
    virtual s32 getPosition() const = 0;
    virtual void seekPos(s32 address, JSUStreamSeekFrom whence);
};