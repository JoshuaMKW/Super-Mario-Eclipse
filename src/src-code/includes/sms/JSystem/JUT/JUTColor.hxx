#pragma once

#include "types.h"

namespace JUtility
{
    class TColor
    {
    public:
        TColor();
        TColor(u8 r, u8 g, u8 b, u8 a) : r(r), g(g), b(b), a(a) {}
        
		void set(u8 r, u8 g, u8 b, u8 a);

        u8 r; // _0
        u8 g; // _1
        u8 b; // _2
        u8 a; // _3
    };
}; // namespace JUtility