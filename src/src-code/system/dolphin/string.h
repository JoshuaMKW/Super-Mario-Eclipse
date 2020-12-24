#ifndef STRING_H
#define STRING_H

#include "types.h"

#ifdef __cplusplus
extern "C"
{
#endif

    char *strcpy(char *dst, const char *src);
    char *strncpy(char *dst, const char *src, size_t num);
    char *strcat(char *dst, const char *src);
    char *strncat(char *dst, const char *src, size_t num);
    size_t strlen(const char *src);
    int strcmp(const char *ptr1, const char *ptr2);
    int strncmp(const char *ptr1, const char *ptr2, size_t num);
    int stricmp(const char *ptr1, const char *ptr2);
    char *strchr(const char *ptr, int chr);
    char *strrchr(const char *ptr, int chr);
    char *strstr(const char *src, const char *str);

#ifdef __cplusplus
}
#endif

#endif