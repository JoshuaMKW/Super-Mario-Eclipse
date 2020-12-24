#include "types.h"

BOOL isxdigit(u32 c)
{
    if (c >= '0' && c <= '9')
        return TRUE;
    else if (c >= 'a' && c <= 'f')
        return TRUE;
    else if (c >= 'A' && c <= 'F')
        return TRUE;

    return FALSE;
}

BOOL isupper(u32 c)
{
    if (c >= 'A' && c <= 'Z')
        return TRUE;

    return FALSE;
}

BOOL isspace(u32 c)
{
    if (c >= '\t' && c <= '\r')
        return TRUE;
    else if (c == ' ')
        return TRUE;
    
    return FALSE;
}

BOOL ispunct(u32 c)
{
    if (c >= '!' && c <= '/')
        return TRUE;
    else if (c >= ':' && c <= '@')
        return TRUE;
    else if (c >= '[' && c <= '`')
        return TRUE;
    else if (c >= '{' && c <= '~')
        return TRUE;
    else if (c > 0x7F)
        return TRUE;

    return FALSE;
}

BOOL isprint(u32 c)
{
    if (c >= ' ' && c <= '~')
        return TRUE;
    else if (c > 0x7F)
        return TRUE;
    
    return FALSE;
}

BOOL islower(u32 c)
{
    if (c >= 'a' && c <= 'z')
        return TRUE;

    return FALSE;
}

BOOL isgraph(u32 c)
{
    if (c >= '!' && c <= '~')
        return TRUE;
    else if (c > 0x7F)
        return TRUE;
    
    return FALSE;
}

BOOL isdigit(u32 c)
{
    if (c >= '0' && c <= '9')
        return TRUE;

    return FALSE;
}

BOOL iscntrl(u32 c)
{
    if (c >= 0 && c <= 0x1F)
        return TRUE;
    else if (c == 0x7F)
        return TRUE;
    
    return FALSE;
}

BOOL isalpha(u32 c)
{
    if (c >= 'a' && c <= 'z')
        return TRUE;
    else if (c >= 'A' && c <= 'Z')
        return TRUE;

    return FALSE;
}

BOOL isalnum(u32 c)
{
    if (c >= '0' && c <= '9')
        return TRUE;
    else if (c >= 'a' && c <= 'z')
        return TRUE;
    else if (c >= 'A' && c <= 'Z')
        return TRUE;

    return FALSE;
}