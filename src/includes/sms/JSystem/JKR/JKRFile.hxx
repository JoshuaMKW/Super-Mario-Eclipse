#pragma once

#include "types.h"
#include "JKRDisposer.hxx"

class JKRFile : JKRDisposer
{
public:
    virtual ~JKRFile();

    virtual bool open(const char *fileName) = 0;
	virtual void close() = 0;
	virtual s32 writeData(void const *addr, s32 length, s32 offset) = 0;
	virtual s32 readData(void *addr, s32 length, s32 offset) = 0;
	virtual s32 getFileSize() const = 0;
};