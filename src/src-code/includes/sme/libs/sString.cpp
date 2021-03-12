#include "types.h"
#include "printf.h"
#include "stdlib.h"
#include "string.h"

#include "libs/sMath.hxx"
#include "libs/sMemory.hxx"
#include "libs/sString.hxx"

constexpr const char String::numbers[10] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };

String::String(size_t bufferSize) : mBufferSize(Min(bufferSize, 1024)), mStringSize(0)
{
    this->mString = (char *)Memory::calloc(this->mBufferSize, 4);
}

String::String(const char *str, size_t bufferSize) : mBufferSize(Min(bufferSize, 1024)), mStringSize(0)
{
    this->mString = (char *)Memory::calloc(this->mBufferSize, 4);
    this->assign(str);
}

String::~String()
{
    Memory::free(this->mString);
}

/*
String &String::operator =(const String &str)
{
    strncpy(this->mString, str.mString, this->mBufferSize);
    return *this;
}

String &String::operator =(const char *str)
{
    strncpy(this->mString, str, this->mBufferSize);
    return *this;
}

String &String::operator =(char chr)
{
    this->mString[0] = chr;
    this->mString[1] = '\0';
    return *this;
}

String &String::operator +=(const String &str)
{
    strcat(this->mString, str.mString);
    return *this;
}

String &String::operator +=(const char *str)
{
    strcat(this->mString, str);
    return *this;
}

String &String::operator +=(char chr)
{
    size_t strend = this->end();

    this->mString[strend] = chr;
    this->mString[strend+1] = '\0';
    return *this;
}

String &String::operator +(const String &str)
{
    String *newStr = (String *)Memory::malloc(sizeof(String), 4);
    newStr->mString = (char *)Memory::malloc(this->mBufferSize, 4);

    strncpy(newStr->mString, this->mString, this->mBufferSize);
    strcat(newStr->mString, str.mString);

    return *newStr;
}

String &String::operator +(const char *str)
{
    String *newStr = (String *)Memory::malloc(sizeof(String), 4);
    newStr->mString = (char *)Memory::malloc(this->mBufferSize, 4);

    strncpy(newStr->mString, this->mString, this->mBufferSize);
    strcat(newStr->mString, str);

    return *newStr;
}

String &String::operator +(char chr)
{
    String *newStr = (String *)Memory::malloc(sizeof(String), 4);
    newStr->mString = (char *)Memory::malloc(this->mBufferSize, 4);

    strncpy(newStr->mString, this->mString, this->mBufferSize);
    
    size_t strend = newStr->end();

    newStr->mString[strend] = chr;
    newStr->mString[strend+1] = '\0';

    return *newStr;
}

String &String::operator *=(size_t mul)
{
    char oldStr[this->mBufferSize];
    strncpy(oldStr, this->mString, this->mBufferSize);

    for (u32 i = 1; i < mul; i++)
    {
        strcat(this->mString, oldStr);
    }
    return *this;
}

String &String::operator *(size_t mul)
{
    String *newStr = (String *)Memory::malloc(sizeof(String), 4);
    newStr->mString = (char *)Memory::malloc(this->mBufferSize, 4);

    for (u32 i = 0; i < mul; i++)
    {
        strcat(newStr->mString, this->mString);
    }
    return *newStr;
}
*/

void String::append(String &str)
{
    strcat(this->mString, str.mString);
    this->mStringSize += str.mStringSize;
}

void String::append(String &str, size_t subpos, size_t sublen)
{
    strncpy(this->mString + this->mStringSize, str.mString + subpos, sublen + 1);
    this->mStringSize += (str.mStringSize - subpos) - (str.mStringSize - sublen);
}

void String::append(const char *str)
{
    strcat(this->mString, str);
    this->mStringSize += strlen(str);
}

void String::append(size_t len, const char chr)
{
    len = Min(len, this->mBufferSize);
    size_t pos = this->end();

    for (; pos < len; pos++)
    {
        this->mString[pos] = chr;
    }
    this->mString[pos] = '\0';
    this->mStringSize += len;
}

void String::assign(const char *str)
{
    size_t stringLen = strlen(str);

    strncpy(this->mString, str, Min(stringLen, this->mBufferSize));
    this->mStringSize = stringLen;
}

void String::assign(const char *str, size_t pos, size_t len)
{
    len = Min(len, strlen(str) - pos);
    strncpy(this->mString, str + pos, Min(len, this->mBufferSize - pos));
    this->mStringSize = len - pos;
}

void String::assign(size_t len, const char chr)
{
    len = Min(len, this->mBufferSize);
    memset(this->mString, chr, len);
    this->mStringSize = len;
}

size_t String::erase(size_t pos, size_t len)
{
    if (len == String::npos)
    {
        this->mString[pos] = '\0';
        this->mStringSize = pos;
    }
    else
    {
        strncpy(this->mString + pos, this->mString + (pos + len), (this->mStringSize - (pos + len)) + 1);
        this->mStringSize -= len;
    }
    return pos;
}

size_t String::find(const char *str, size_t startPos)
{
    if (startPos >= this->length())
        return String::npos;

    const char *addr = strstr(this->mString + startPos, str);
    if (!addr)
        return String::npos;
        
    return addr - this->mString;
}

size_t String::find(const char chr, size_t startPos)
{
    if (startPos >= this->length())
        return String::npos;

    const char *addr = strchr(this->mString + startPos, chr);
    if (!addr)
        return String::npos;
        
    return addr - this->mString;
}

size_t String::findAny(const char list[], size_t startPos)
{
    if (startPos >= this->length())
        return String::npos;

    for (u32 i = 0; list[i] != '\0'; i++)
    {
        char *addr = strchr(this->mString + startPos, list[i]);
        if (addr)
            return addr - this->mString;
    }
        
    return String::npos;
}

void String::insert(size_t pos, const char *str)
{
    size_t insertlen = strlen(str);

    memcpy(this->mString + (pos + insertlen), this->mString + pos, (this->mStringSize - pos) + 1);
    strncpy(this->mString + pos, str, insertlen);

    this->mStringSize += strlen(str);
}

size_t String::replace(size_t pos, size_t len, const char *str)
{
    if (pos >= this->length())
        return String::npos;

    this->erase(pos, len);
    this->insert(pos, str);
    return pos;
}

size_t String::replace(size_t pos, size_t len, const char str)
{
    if (pos >= this->length())
        return String::npos;

    this->erase(pos, len);
    this->insert(pos, &str);
    return pos;
}

size_t String::replace(const char *oldstr, const char *newstr, size_t replaceIndex)
{
    size_t oldStrLen = strlen(oldstr);

    size_t pos = this->find(oldstr);
    for (u32 i = 0; i < replaceIndex; ++i)
    {
        pos = this->find(oldstr, pos + oldStrLen);
        if (pos == String::npos)
        {
            return String::npos;
        }
    }
    this->replace(pos, oldStrLen, newstr);
    return pos;
}

void String::replaceAll(const char *oldstr, const char *newstr)
{
    size_t pos = 0;
    while (pos != String::npos)
    {
        pos = this->replace(oldstr, newstr);
    }
}

void String::replaceAll(const char *oldstr, const char newstr)
{
    size_t pos = 0;
    while (pos != String::npos)
    {
        pos = this->replace(oldstr, &newstr);
    }
}

void String::resize(size_t size, const char fill)
{
    if (size < this->size())
    {
        this->mString[size] = '\0';
        return;
    }
    else
    {
        this->append(size - this->size(), fill);
    }
}

String *String::substr(size_t pos, size_t len)
{
    String *substr = new String(Max(len, this->mBufferSize));

    len = Min(this->size() - pos, len);
    if (pos < this->size())
    {
        strncpy(substr->mString, this->mString + pos, len);
    }
    return substr;
}

char * String::intToString(s32 num, char *buffer, size_t base)
{
    switch(base)
    {
    case 2:
        snprintf(buffer, 35, "0b%b", num);
        break;
    case 7:
        snprintf(buffer, 14, "0o%o", num);
        break;
    case 16:
        snprintf(buffer, 11, "0x%X", num);
        break;
    case 10:
    default:
        snprintf(buffer, 11, "%d", num);
    }
    return buffer;
}