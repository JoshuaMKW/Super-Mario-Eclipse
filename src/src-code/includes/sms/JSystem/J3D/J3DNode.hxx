#pragma once

#include "types.h"
#include "J3DModel.hxx"

class J3DNode
{
public:
	J3DNode();

	virtual void init(J3DModelData *);
	virtual void updateIn();
	virtual void updateOut();
	virtual void entryIn();
	virtual void calcIn();
	virtual void calcOut();
	virtual u32 getType() const;
	virtual ~J3DNode();

	u32 _4;
	int (*mCallback)(J3DNode *, int);
	u32 _C; // padding?
	u32 *_10;
	J3DNode *mChildNode; // _14
};