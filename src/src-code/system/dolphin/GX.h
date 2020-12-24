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
    } RGB;

    typedef struct RGBA
    {
        u8 R;
        u8 B;
        u8 G;
        u8 A;
    } RGBA;

    void GXBegin(u32 type, u32 idx, u32 count);
    void GXSetArray(u32 type, u32 idx, u32 count);
    void GXSetVtxDesc(u32 type, u32 desc);
    void GXSetVtxAttrFmt(u32 fmt, u32 type, u32, u32, u32);

    void GXDrawCube();
    void GXDrawCubeFace();
    void GXDrawSphere(u8 majorCnt, u8 minorCnt);
    void GXDrawDone();

#ifdef __cplusplus
}
#endif

#endif