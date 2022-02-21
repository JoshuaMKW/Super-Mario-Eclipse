#pragma once

#include "types.h"
#include "MTX.h"

class J3DShape
{
public:
	u32 countBumpMtxNum() const;
	void draw() const;
	void initialize();
	void loadVtxArray() const;
	void makeVcdVatCmd();
	void makeVtxArrayCmd();

	u32 _0;
	u16 _4;
	u16 _6;
	u32 _8;
	f32 _C;
	f32 _10;
	f32 _14;
	f32 _18;
	f32 _1C;
	f32 _20;
	f32 _24;
	u32 _2C;
	bool _30;
	u8 _31; // padding?
	u8 _32; // ^^
	u8 _33; // ^^
	u32 _34;
	u32 _38; // might be a class instance
	u32 _3C; // padding?
	u32 _40; // padding?
	u32 *_44;
	u32 _48;
	u32 _4C;
	u32 _50;
	u32 _54;
	u32 _58;
};

class J3DShapeMtx
{
public:
	virtual ~J3DShapeMtx();

	virtual u32 getType() const;
	virtual u16 getUseMtxNum() const;
	virtual s16 getUseMtxIndex(u16) const;
	virtual void load() const;
	virtual void calcNBTScale(const Vec &, f32[3][3][3], f32[3][3][3]);

	void loadMtxIndx_NCPU(int, u16) const;
	void loadMtxIndx_PCPU(int, u16) const;
	void loadMtxIndx_PNCPU(int, u16) const;
	void loadMtxIndx_PNGP(int, u16) const;

	s16 mMtxIndex;
};