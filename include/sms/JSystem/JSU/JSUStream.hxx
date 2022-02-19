#pragma once

#include "types.h"

enum JSUStreamSeekFrom
{
	BEGIN,
	CURRENT,
	END
};

class JSUIosBase
{
public:
    JSUIosBase();
	virtual ~JSUIosBase();

    u8 _4; // maybe state?
};