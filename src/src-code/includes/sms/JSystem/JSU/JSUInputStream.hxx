#pragma once

#include "types.h"
#include "JSUStream.hxx"

class JSUInputStream : public JSUIosBase
{
public:
	u8 _4;
	u8 _5; // padding?
	u8 _6; // ^^
	u8 _7; // ^^
};

class JSURandomInputStream : public JSUInputStream
{
};