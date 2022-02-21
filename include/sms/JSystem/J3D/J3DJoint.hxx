#pragma once

#include "types.h"
#include "J3DMaterial.hxx"
#include "J3DNode.hxx"

class J3DJoint : public J3DNode
{
public:

	virtual void updateIn() override;
	virtual void updateOut() override;
	virtual void entryIn() override;
	virtual void calcIn() override;
	virtual void calcOut() override;
	virtual u32 getType() const override;
	virtual ~J3DJoint();

	void addMesh(J3DMaterial *);
	void initialize();


	u16 _18;
	u8 _1A;
	u8 _1B;
	f32 _1C;
	f32 _20;
	f32 _24;
	u16 _28;
	u16 _2A;
	u16 _2C;
	u16 _2E; // padding?
	f32 _30;
	f32 _34;
	f32 _38;
	f32 _3C;
	u32 _40;
	u32 _44;
	u32 _48;
	u32 _50;
	u32 _54;
	u32 _58;
	u32 _5C;
	J3DMaterial *mMaterial; // _60
};