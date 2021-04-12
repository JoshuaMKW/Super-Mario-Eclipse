#pragma once

#include "types.h"

class J3DDisplayListObj
{
public:
	u32 _0;
	u32 _4;
	u32 _8;
	u32 _C;
};

class J3DPacket
{
public:
	u32 *vTable;
	J3DPacket *mParentPacket; // _4
	J3DPacket *mChildPacket;  // _8
};

class J3DDrawPacket : public J3DPacket
{
public:
	u8 _C[0x30 - 0xC];
	u32 *_30;
};

class J3DCallbackPacket : public J3DPacket
{
};

class J3DShapePacket : public J3DCallbackPacket
{
};

class J3DMatPacket : public J3DDrawPacket
{
public:
	J3DShapePacket *mShapePacket; // _34
	u32 _38;
	u32 _3C;
	u32 _40;
	u32 _44;
};