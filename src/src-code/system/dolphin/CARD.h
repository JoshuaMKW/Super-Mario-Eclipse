#ifndef CARD_H
#define CARD_H

#include "types.h"

// this seems to be what is referenced in a lot of functions
typedef void (*CARDCallback)(u32 chan, u32 res);

typedef struct CARDInfo
{
    u32 channel;
    u32 fileNo;

    u32 offset;
    u32 length;
    u16 block;
} CARDInfo;

#ifdef __cplusplus
extern "C"
{
#endif

    void CARDInit();
    void CARDCheck(u32 chan);
    u32 CARDCreate(u32 chan, u8 *fileName, u32 size, CARDInfo *cardInfo);
    u32 CARDCreateAsync(u32 chan, u8 *fileName, u32 size, CARDInfo *cardInfo, CARDCallback cardCB);
    u32 CARDFormat(u32 chan);
    u32 CARDFormatAsync(u32 chan, CARDCallback cardCB);

#ifdef __cplusplus
}
#endif

#endif