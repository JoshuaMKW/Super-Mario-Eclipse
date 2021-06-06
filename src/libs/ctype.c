#include "types.h"

int isxdigit(int c)
{
    if (c >= '0' && c <= '9')
        return true;
    else if (c >= 'a' && c <= 'f')
        return true;
    else if (c >= 'A' && c <= 'F')
        return true;

    return false;
}

int isupper(int c)
{
    if (c >= 'A' && c <= 'Z')
        return true;

    return false;
}

int isspace(int c)
{
    if (c >= '\t' && c <= '\r')
        return true;
    else if (c == ' ')
        return true;
    
    return false;
}

int ispunct(int c)
{
    if (c >= '!' && c <= '/')
        return true;
    else if (c >= ':' && c <= '@')
        return true;
    else if (c >= '[' && c <= '`')
        return true;
    else if (c >= '{' && c <= '~')
        return true;
    else if (c > 0x7F)
        return true;

    return false;
}

int isprint(int c)
{
    if (c >= ' ' && c <= '~')
        return true;
    else if (c > 0x7F)
        return true;
    
    return false;
}

int islower(int c)
{
    if (c >= 'a' && c <= 'z')
        return true;

    return false;
}

int isgraph(int c)
{
    if (c >= '!' && c <= '~')
        return true;
    else if (c > 0x7F)
        return true;
    
    return false;
}

int isdigit(int c)
{
    if (c >= '0' && c <= '9')
        return true;

    return false;
}

int iscntrl(int c)
{
    if (c >= 0 && c <= 0x1F)
        return true;
    else if (c == 0x7F)
        return true;
    
    return false;
}

int isalpha(int c)
{
    if (c >= 'a' && c <= 'z')
        return true;
    else if (c >= 'A' && c <= 'Z')
        return true;

    return false;
}

int isalnum(int c)
{
    if (c >= '0' && c <= '9')
        return true;
    else if (c >= 'a' && c <= 'z')
        return true;
    else if (c >= 'A' && c <= 'Z')
        return true;

    return false;
}