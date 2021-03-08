#ifndef CTYPE_H
#define CTYPE_H

#include "types.h"

#ifdef __cplusplus
extern "C"
{
#endif

    u32 tolower(u32 c);
    u32 toupper(u32 c);
    u32 isxdigit(u32 c);
    u32 isupper(u32 c);
    u32 isspace(u32 c);
    u32 ispunct(u32 c);
    u32 isprint(u32 c);
    u32 islower(u32 c);
    u32 isgraph(u32 c);
    u32 isdigit(u32 c);
    u32 iscntrl(u32 c);
    u32 isalpha(u32 c);
    u32 isalnum(u32 c);

#ifdef __cplusplus
}
#endif

#endif