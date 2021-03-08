#ifndef STDLIB_H
#define STDLIB_H

#include "types.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define offsetof(st, m) ((size_t)&(((st *)0)->m))

    int abs(int);
    long labs(long);
    int atoi(const char *str);

#ifdef __cplusplus
}
#endif

#endif