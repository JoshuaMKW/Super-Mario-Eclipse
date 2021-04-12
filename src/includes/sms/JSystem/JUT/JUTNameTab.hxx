#pragma once

#include "types.h"

class ResNTab
{
};

class JUTNameTab
{
public:
    JUTNameTab(const ResNTab *); // ResNTab*

	s32 getIndex(const char *name) const;
	u16 calcKeyCode(const char *name) const;
	char* getName(const u16);

    ResNTab *mResTab; // ResNTab*
    u32 _4;
    u16 _8;
};