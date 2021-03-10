#ifndef CTYPE_H
#define CTYPE_H

#include "types.h"

#ifdef __cplusplus
extern "C"
{
#endif

    int tolower(int c);
    int toupper(int c);
    int isxdigit(int c);
    int isupper(int c);
    int isspace(int c);
    int ispunct(int c);
    int isprint(int c);
    int islower(int c);
    int isgraph(int c);
    int isdigit(int c);
    int iscntrl(int c);
    int isalpha(int c);
    int isalnum(int c);

#ifdef __cplusplus
}
#endif

#endif