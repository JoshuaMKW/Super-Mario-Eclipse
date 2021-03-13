#pragma once

#include "types.h"
#include "J3DModel.hxx"

enum J3DDeformAttachFlag
{
};

class J3DSkinDeform
{
public:
	J3DSkinDeform();

	virtual void deform(J3DModel *);
	virtual ~J3DSkinDeform();

	void calcNrmMtx(J3DModel *);
	void initMtxIndexArray(J3DModelData *);

	u32 _4;
	u32 _8;
	u32 _C;
	u32 _10;
};