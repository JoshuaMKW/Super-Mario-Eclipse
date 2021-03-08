 #pragma once

#include "types.h"
#include "OS.h"

class FletcherEncryptor
{
    static u16 encode16(const u8 *data, size_t len)
    {
        u32 c0, c1;
        for (c0 = c1 = 0; len > 0; ) {
            size_t blocklen = len;
            len -= blocklen > 5002 ? 5002 : blocklen;
            do {
                c0 = c0 + *data++;
                c1 = c1 + c0;
            } while (--blocklen);
            c0 = c0 % 255;
            c1 = c1 % 255;
        }
        return (c1 << 8 | c0);
    }

    static u32 encode32(const u16 *data, size_t len)
    {
        u32 c0, c1;
        len = (len + 1) & ~1;

        for (c0 = c1 = 0; len > 0; ) {
            size_t blocklen = len;
            len -= blocklen > 720 ? 720 : blocklen;
            do {
                c0 = c0 + *data++;
                c1 = c1 + c0;
            } while ((blocklen -= 2));
            c0 = c0 % 65535;
            c1 = c1 % 65535;
        }
        return (c1 << 16 | c0);
    }
};