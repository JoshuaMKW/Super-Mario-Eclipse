#ifndef GD_H
#define GD_H

#include "types.h"

typedef struct _GDLObj
{
    u8 *start; // _0
    u32 len;   // _4
    u8 *ptr;   // _8
    u8 *top;   // _C
} GDLObj;

extern GDLObj *__GDCurrentDL;
typedef void (*GDOverflowCallback)(void);

#ifdef __cplusplus
extern "C"
{
    void GDInitGDLObj(GDLObj *displayList, void *start, u32 len);
    void GDFlushCurrToMem();
    void GDPadCurr32();
    void GDOverflowed();
    void GDSetOverflowCallback(GDOverflowCallback cb);
}
#endif

#endif