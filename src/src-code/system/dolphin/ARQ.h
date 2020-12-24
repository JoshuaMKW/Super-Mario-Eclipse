#ifndef ARQ_H
#define ARQ_H

#include "types.h"

typedef void (*ARQCallback)(u32 reqPtr);

struct ARQRequest
{
    ARQRequest *nextReq;
    u32 owner;
    u32 direction;
    u32 priority;
    u32 sourceAddr;
    u32 destAddr;
    u32 len;
    ARQCallback cb;
};

#ifdef __cplusplus
extern "C"
{
#endif

    void ARQInit();
    void ARQPostRequest(ARQRequest *nextReq, u32 owner, u32 direction, u32 priority, u32 sourceAddr, u32 destAddr, u32 len, ARQCallback cb);

#ifdef __cplusplus
}
#endif

#endif