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
	u16 mImageNum;
	void *mTexHeader;
	
	J3DTexture(u16 imagenum, void *texheader);
	virtual ~J3DTexture();
};