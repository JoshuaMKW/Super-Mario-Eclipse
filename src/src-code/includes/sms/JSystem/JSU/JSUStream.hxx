#pragma once

#include "types.h"

enum JSUStreamSeekFrom
{
	Unknown1,
	Unknown2,
	Unknown3,
	Unknown4
};

class JSUIosBase
{
public:
    JSUIosBase();
	virtual ~JSUIosBase();

    u8 _4; // maybe state?
};