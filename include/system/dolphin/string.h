#ifndef STRING_H
#define STRING_H

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

char *strcpy(char *dst, const char *src);
char *strncpy(char *dst, const char *src, size_t num);
char *strcat(char *dst, const char *src);
size_t strlen(const char *src);
int strcmp(const char *ptr1, const char *ptr2);
char *strchr(const char *ptr, int chr);
char *strrchr(const char *ptr, int chr);
char *strstr(const char *src, const char *str);
int strtol(const char *str, char **endptr, int base);
unsigned int strtoul(const char *str, char **endptr, int base);

#ifdef __cplusplus
}
#endif

#endif