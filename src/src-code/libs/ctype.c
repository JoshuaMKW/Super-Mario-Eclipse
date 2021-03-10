#include "types.h"


#ifdef __CWCC__

int isxdigit(int c)
{
    if (c >= '0' && c <= '9')
        return TRUE;
    else if (c >= 'a' && c <= 'f')
        return TRUE;
    else if (c >= 'A' && c <= 'F')
        return TRUE;

    return FALSE;
}

int isupper(int c)
{
    if (c >= 'A' && c <= 'Z')
        return TRUE;

    return FALSE;
}

int isspace(int c)
{
    if (c >= '\t' && c <= '\r')
        return TRUE;
    else if (c == ' ')
        return TRUE;
    
    return FALSE;
}

int ispunct(int c)
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

int isprint(int c)
{
    if (c >= ' ' && c <= '~')
        return TRUE;
    else if (c > 0x7F)
        return TRUE;
    
    return FALSE;
}

int islower(int c)
{
    if (c >= 'a' && c <= 'z')
        return TRUE;

    return FALSE;
}

int isgraph(int c)
{
    if (c >= '!' && c <= '~')
        return TRUE;
    else if (c > 0x7F)
        return TRUE;
    
    return FALSE;
}

int isdigit(int c)
{
    if (c >= '0' && c <= '9')
        return TRUE;

    return FALSE;
}

int iscntrl(int c)
{
    if (c >= 0 && c <= 0x1F)
        return TRUE;
    else if (c == 0x7F)
        return TRUE;
    
    return FALSE;
}

int isalpha(int c)
{
    if (c >= 'a' && c <= 'z')
        return TRUE;
    else if (c >= 'A' && c <= 'Z')
        return TRUE;

    return FALSE;
}

int isalnum(int c)
{
    if (c >= '0' && c <= '9')
        return TRUE;
    else if (c >= 'a' && c <= 'z')
        return TRUE;
    else if (c >= 'A' && c <= 'Z')
        return TRUE;

    return FALSE;
}

#endif