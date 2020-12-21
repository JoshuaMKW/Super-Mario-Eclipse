#ifndef GX_H
#define GX_H

#include "types.h"

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct RGB
    {
        u8 R;
        u8 B;
        u8 G;
    };

    typedef struct RGBA
    {
        u8 R;
        u8 B;
        u8 G;
        u8 A;
    };

#ifdef __cplusplus
}
#endif

#endif