#pragma once

#include "types.h"

class JSUPtrList; // forward declaration

class JSUPtrLink
{
public:
	void *_0;
	JSUPtrList *mPtrList; // _4
	u32 _8;
	u32 _C;
};

class JSUPtrList
{
public:
	u32 _0;
	u32 _4;
	s32 mNumPtrs; // _8
};

template <typename T>
class JSUList : public JSUPtrList
{
};

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

	u32 *vTable; // _0
};

/* INPUT */

class JSUInputStream : public JSUIosBase
{
public:
	u8 _4;
	u8 _5; // padding?
	u8 _6; // ^^
	u8 _7; // ^^
};

class JSURandomInputStream : public JSUInputStream
{
};

class JSUMemoryInputStream : public JSURandomInputStream
{
public:
	void *_8;
	u32 mLength;	  // _C
	u32 mPosition; // _10
};

/* OUTPUT */
class JSUOutputStream : public JSUIosBase
{
public:
	u8 _4;
	u8 _5; // padding?
	u8 _6; // ^^
	u8 _7; // ^^
};

class JSURandomOutputStream : public JSUOutputStream
{
};

class JSUMemoryOutputStream : public JSURandomOutputStream
{
public:
	u32 _8;
	u32 mLength;	  // _C
	u32 mPosition; // _10
};