#pragma once

#include "types.h"
#include "../JSU/JSUList.hxx"

class JKRDisposer
{
public:
	JKRDisposer();
	virtual ~JKRDisposer();
	
	u32 _4;
	JSUPtrLink mPtrLink; // _8
};