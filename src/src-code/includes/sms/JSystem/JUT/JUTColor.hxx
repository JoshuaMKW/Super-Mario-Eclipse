#pragma once

#include "types.h"

namespace JUtility
{
    class TColor
    {
    public:
        TColor();
		void set(u8 r, u8 g, u8 b, u8 a);

        u8 R; // _0
        u8 G; // _1
        u8 B; // _2
        u8 A; // _3
    };
}; // namespace JUtility