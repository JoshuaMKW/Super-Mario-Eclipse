#ifndef STDIO_H
#define STDIO_H

#include "types.h"
#include "stdarg.h"

#ifdef __cplusplus
extern "C"
{
#endif

    int snprintf(char *, size_t, const char *, ...);
    int sprintf(char *, const char *, ...);
    int vsnprintf(char *, size_t, const char *, va_list);
    int vsprintf(char *, const char *, va_list);

#ifdef __cplusplus
}
#endif

#endif