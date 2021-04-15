#ifndef STDLIB_H
#define STDLIB_H

#include "types.h"

#ifdef __cplusplus
extern "C"
{
#endif

    int abs(int);
    long labs(long);
    int atoi(const char *str);

#ifdef __cplusplus
}
#endif

#endif