#pragma once

#include "types.h"
#include "MTX.h"
#include "J3DPacket.hxx"
#include "J3DShape.hxx"
#include "J3DTexture.hxx"

class J3DColorBlock
{
public:
	virtual void reset(J3DColorBlock *);

	virtual ~J3DColorBlock();
};

class J3DPEBlock
{
public:
	virtual void reset(J3DPEBlock *);
	//virtual void setFog(J3DFog *);

	virtual ~J3DPEBlock();
};

class J3DTevBlock
{
public:
	virtual void reset(J3DTevBlock *);

	virtual ~J3DTevBlock();
};

class J3DIndBlock
{
public:
	virtual void reset(J3DIndBlock *);

	virtual ~J3DIndBlock();
};

class J3DTexBlock
{
public:
	virtual void reset(J3DTexBlock *);

	virtual ~J3DTexBlock();
};

class J3DTexGenBlock
{
public:
	virtual void reset(J3DTexGenBlock *);

	virtual ~J3DTexGenBlock();
};

class J3DMaterial
{
public:
	J3DMaterial();
	~J3DMaterial();

	void addShape(J3DShape *);
	void calc(Mtx44 *);
	void change();
	void copy(J3DMaterial *);
	size_t countDLSize();
	J3DColorBlock *createColorBlock(int);
	J3DIndBlock *createIndBlock(int);
	J3DPEBlock *createPEBlock(int, u32);
	J3DTevBlock *createTevBlock(int);
	J3DTexGenBlock *createTexGenBlock(int);
	void initialize();
	void load();
	void makeDisplayList();
	J3DDisplayListObj *newSharedDisplayList(u32);
	void safeMakeDisplayList();
	void setCurrentMtx();

	u32 *_0;
	J3DShape *shape; // _4
	u32 _8;
	u16 _C;
	u16 _E; //padding?
	u32 _10;
	u32 _14; // padding?
	u32 *_18;
	u32 _1C;
	u32 *_20; // *TItemManager
	u32 *_24; // *TItemManager
	u32 *_28;
	u32 _2C; // *TItemManager
	u32 _30;
	u32 *_34;
	u32 _38;
	J3DDisplayListObj dispListObj; // _3C
};

class J3DMaterialBlock
{
public:
	u32 _00;
	u32 _04;
	u16 _08;
	u32 _0C;
	u32 _10;
	u32 mMatOffset;
};

class J3DMaterialBlock_v21 : public J3DMaterialBlock
{
};

class J3DMaterialTable
{
public:
	J3DMaterialTable();
	virtual ~J3DMaterialTable();

	void clear();

	s16 _04;
	u32 _08;
	J3DTexture *mTexture;
	u32 _10;
	u32 _14;
};

class J3DColorChan
{
public:
	u16 colorChanID; // _0
};

class J3DGXColor
{
};

