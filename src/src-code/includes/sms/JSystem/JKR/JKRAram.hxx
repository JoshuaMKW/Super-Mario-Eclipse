#pragma once

#include "types.h"
#include "JKRDvdRipper.hxx"
#include "JKRThread.hxx"

class JKRAram : public JKRThread
{
public:
	JKRAram(u32, u32, s32);
	virtual ~JKRAram();

	virtual void run();

	static u32* create(u32, u32, s32, s32, s32);
	static s32 mainRamToAram(u8 *, u32, u32, JKRExpandSwitch, u32, JKRHeap *, int);
	static s32 aramToMainRam(u32, u8 *, u32, JKRExpandSwitch, u32, JKRHeap *, int, u32 *);

	u32 _60;
	u32 _64;
	u32 _68;
	u32 _70;
	u32 _74;
	u32 _78;
	u32 _7C;
	u32 _80;
	u32 _84;
};

void JKRDecompressFromAramToMainRam(u32, void *, u32, u32, u32);