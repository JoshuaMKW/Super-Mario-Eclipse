#ifndef THP_H
#define THP_H

#include "types.h"

#ifdef __cplusplus
extern "C"
{
#endif

    struct VideoInfo
    {
        u32 h;
        u32 w;
    };

    struct GXRenderModeObj
    {
        u32 _0; // some VI shit
        u16 _4;
        u16 _6;
        u16 _8;
        u16 _A;
        u16 _C;
        u16 _E;
        u16 _10;
        u32 _12;
        u8 _16;
        u8 _17;
        u8 _18[12][2];
        u8 _19[7];
    };

    bool THPPlayerInit(u32);
    bool THPPlayerOpen(u8 *, u32);
    bool THPPlayerClose();
    u32 THPPlayerCalcNeedMemory();
    bool THPPlayerSetBuffer(u8 *);
    bool THPPlayerPlay();
    bool THPPlayerPrepare(u32, u32, u32);
    void THPPlayerStop();
    bool THPPlayerPause();
    void THPPlayerDrawDone();
    u32 THPPlayerDrawCurrentFrame(GXRenderModeObj *, u32, u32, u32, u32);
    bool THPPlayerPrepare(u32, u32, u32);
    void THPPlayerQuit();
    bool THPPlayerGetVideoInfo(VideoInfo *);

#ifdef __cplusplus
}
#endif

#endif