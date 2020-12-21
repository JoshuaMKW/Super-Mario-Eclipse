#ifndef WCHAR_H
#define WCHAR_H

#include "stdarg.h"

#ifdef __cplusplus
extern "C"
{
#endif

    int vswprintf(wchar_t *, size_t, const wchar_t *, va_list);
    wchar_t *wcschr(const wchar_t *, wchar_t);
    int wcscmp(const wchar_t *, const wchar_t *);
    wchar_t *wcscpy(wchar_t *, const wchar_t *);
    wchar_t *wcsncpy(wchar_t *, const wchar_t *, size_t);
    size_t wcslen(const wchar_t *);
    wchar_t *wmemchr(const wchar_t, wchar_t, size_t);
    wchar_t *wmemcpy(wchar_t *, const wchar_t *, size_t);

#ifdef __cplusplus
}
#endif

#endif