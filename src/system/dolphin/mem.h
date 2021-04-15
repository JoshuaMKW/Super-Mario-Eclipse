#ifndef MEM_H
#define MEM_H

#include "types.h"

#ifdef __cplusplus
extern "C"
{
#endif

    void *memchr(const void *ptr, int chr, size_t num);
    int memcmp(const void *ptr1, const void *ptr2, size_t num);
    void *memset(void *ptr, u32 fill, size_t num);
    void *memcpy(void *dst, const void *src, size_t num);
    void *memmove(void *dst, const void *src, size_t num);

#ifdef __cplusplus
}
#endif

#endif