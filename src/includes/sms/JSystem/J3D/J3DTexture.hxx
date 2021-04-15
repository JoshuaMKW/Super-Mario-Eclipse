#pragma once

#include "types.h"

class J3DTexCoord
{
public:
	u8 _0;
	u8 _1;
	u8 _2;
	u8 _3; // padding?
};

class J3DTexture
{
public:
	u16 _0;
	u16 _2;
	u32 _4;
	u32 *vTable; // _8
};