#pragma once

#include "types.h"
#include "OS.h"
#include "JKRThread.hxx"

enum CompressionType
{
	NONE,
	SZP,
	SZS
};

class JKRDecompCommand
{
public:
	JKRDecompCommand();
	~JKRDecompCommand();

	u32 _0;
	u8 *_4;
	u8 *_8;
	u32 _C;
	u32 _10;
	u32 _14;
	JKRDecompCommand *mSelf; // _18
	u32 _1C;
	u32 _20;
	OSMessageQueue mMessageQueue; // _28
	OSMessage mMessage;			  // _48
};

class JKRDecomp : public JKRThread
{
public:
	JKRDecomp(s32);
	virtual ~JKRDecomp();

	virtual void run();

	static void create(s32);
	static void sendCommand(JKRDecompCommand *);
	void orderSync(u8 *, u8 *, u32, u32);
	void decode(u8 *, u8 *, u32, u32);
	static void decodeSZP(u8 *src, u8 *dest, u32, u32);
	static void decodeSZS(u8 *src, u8 *dest, u32, u32);
	static CompressionType checkCompressed(u8 *);
};

s32 decompSZS_subroutine(u8 *src, u8 *dest);
u32* firstSrcData();
u32* nextSrcData(u8 *);