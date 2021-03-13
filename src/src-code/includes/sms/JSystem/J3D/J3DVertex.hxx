#pragma once

#include "types.h"

class J3DVertexData
{
public:
	u32 _0;
	u32 _4;
	u32 _8;
	u32 _C;
	u32 _10;
	u32 _14;
	u32 _18;
	u32 _1C;
	u32 _20;
	u32 _24;
	u32 _28;
	u32 _2C;
	u32 _30;
	u32 _34;
	u32 _38;
	u32 _3C;
	u32 _40;
};

class J3DVertexBuffer
{
public:
    J3DVertexBuffer(J3DVertexData *);

    void copyTransformedVtxArray();

	J3DVertexData *mVtxData; // _0
	u32 _4;
	u32 _8;
	u32 _C;
	u32 _10;
	u32 _14;
	u32 _18;
	u32 _1C;
	u32 _20;
	u32 _24;
	u32 _28;
	u32 _2C;
	u32 _30;
	u32 _34;

    virtual ~J3DVertexBuffer(); // _38
};

class J3DDrawMtxData
{
public:
    J3DDrawMtxData();
    ~J3DDrawMtxData();
    
	u16 _0;
	u32 _4;
	u32 _8;
};