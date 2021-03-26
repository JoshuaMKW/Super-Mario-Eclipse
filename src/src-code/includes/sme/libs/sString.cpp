#include "printf.h"
#include "stdlib.h"
#include "string.h"
#include "types.h"

#include "libs/sMath.hxx"
#include "libs/sMemory.hxx"
#include "libs/sString.hxx"

constexpr const char String::numbers[10] = {'0', '1', '2', '3', '4',
                                            '5', '6', '7', '8', '9'};

String::String(size_t bufferSize)
    : mBufferSize(Min(bufferSize, 1024)), mStringSize(0) {
  mString = static_cast<char *>(SME::Util::Memory::calloc(mBufferSize, 4));
}

String::String(const char *str, size_t bufferSize)
    : mBufferSize(Min(bufferSize, 1024)), mStringSize(0) {
  mString = static_cast<char *>(SME::Util::Memory::calloc(mBufferSize, 4));
  assign(str);
}

String::String(const String &str, size_t bufferSize)
    : mBufferSize(Min(bufferSize, 1024)), mStringSize(0) {
  mString = static_cast<char *>(SME::Util::Memory::calloc(mBufferSize, 4));
  assign(str.data());
}

String::~String() { SME::Util::Memory::free(mString); }

String &String::operator=(const String &str) {
  mStringSize = str.size();
  strncpy(mString, str.data(), mStringSize + 1);
  return *this;
}

String &String::operator=(const char *str) {
  mStringSize = strlen(str);
  strncpy(mString, str, mStringSize + 1);
  return *this;
}

String &String::operator=(char chr) {
  mString[0] = chr;
  mString[1] = '\0';
  mStringSize = 2;
  return *this;
}

String &String::operator+=(const String &str) {
  strcat(mString, str.data());
  mStringSize += str.size();
  return *this;
}

String &String::operator+=(const char *str) {
  strcat(mString, str);
  mStringSize += strlen(str);
  return *this;
}

String &String::operator+=(char chr) {
  size_t strend = end();

  mString[strend] = chr;
  mString[strend + 1] = '\0';
  mStringSize += 1;
  return *this;
}

String &String::operator+(const String &str) {
  String *newStr = new String(mString, 256);
  newStr->mString = static_cast<char *>(SME::Util::Memory::malloc(256, 4));

  strcat(newStr->mString, str.data());
  newStr->mStringSize += str.size();

  return *newStr;
}

String &String::operator+(const char *str) {
  String *newStr = new String(mString, 256);
  newStr->mString = static_cast<char *>(SME::Util::Memory::malloc(256, 4));

  strcat(newStr->mString, str);
  newStr->mStringSize += strlen(str);

  return *newStr;
}

String &String::operator+(char chr) {
  String *newStr = new String(mString, 256);
  newStr->mString = static_cast<char *>(SME::Util::Memory::malloc(256, 4));

  strncpy(newStr->mString, mString, mStringSize);

  size_t strend = newStr->end();

  newStr->mString[strend] = chr;
  newStr->mString[strend + 1] = '\0';
  newStr->mStringSize += 1;

  return *newStr;
}

String &String::operator*=(size_t mul) {
  size_t length = end();

  for (u32 i = 1; i < mul; i++)
    strncpy(&mString[length * i], mString, length);

  return *this;
}

String &String::operator*(size_t mul) {
  String *newStr = new String(mString, 256);
  newStr->mString = static_cast<char *>(SME::Util::Memory::malloc(256, 4));

  for (u32 i = 1; i < mul; i++)
    strcat(newStr->mString, mString);

  return *newStr;
}

void String::append(String &str) {
  strcat(mString, str.mString);
  mStringSize += str.mStringSize;
}

void String::append(String &str, size_t subpos, size_t sublen) {
  strncpy(mString + mStringSize, str.mString + subpos, sublen + 1);
  mStringSize += (str.mStringSize - subpos) - (str.mStringSize - sublen);
}

void String::append(const char *str) {
  strcat(mString, str);
  mStringSize += strlen(str);
}

void String::append(size_t len, const char chr) {
  size_t pos = end();

  for (; pos < len; pos++) {
    mString[pos] = chr;
  }
  mString[pos] = '\0';
  mStringSize += len;
}

void String::assign(const char *str) {
  size_t stringLen = strlen(str);

  strncpy(mString, str, stringLen);
  mStringSize = stringLen;
}

void String::assign(const char *str, size_t pos, size_t len) {
  strncpy(mString, str + pos, len);
  mStringSize = len - pos;
}

void String::assign(size_t len, const char chr) {
  memset(mString, chr, len);
  mStringSize = len;
}

size_t String::erase(size_t pos, size_t len) {
  if (len == String::npos) {
    mString[pos] = '\0';
    mStringSize = pos;
  } else {
    strncpy(mString + pos, mString + (pos + len),
            (mStringSize - (pos + len)) + 1);
    mStringSize -= len;
  }
  return pos;
}

size_t String::find(const char *str, size_t startPos) {
  if (startPos >= length())
    return String::npos;

  const char *addr = strstr(mString + startPos, str);
  if (!addr)
    return String::npos;

  return addr - mString;
}

size_t String::find(const char chr, size_t startPos) {
  if (startPos >= length())
    return String::npos;

  const char *addr = strchr(mString + startPos, chr);
  if (!addr)
    return String::npos;

  return addr - mString;
}

size_t String::findAny(const char list[], size_t startPos) {
  if (startPos >= length())
    return String::npos;

  for (u32 i = 0; list[i] != '\0'; i++) {
    char *addr = strchr(mString + startPos, list[i]);
    if (addr)
      return addr - mString;
  }

  return String::npos;
}

void String::insert(size_t pos, const char *str) {
  size_t insertlen = strlen(str);

  memcpy(mString + (pos + insertlen), mString + pos, (mStringSize - pos) + 1);
  strncpy(mString + pos, str, insertlen);

  mStringSize += strlen(str);
}

size_t String::replace(size_t pos, size_t len, const char *str) {
  if (pos >= length())
    return String::npos;

  erase(pos, len);
  insert(pos, str);
  return pos;
}

size_t String::replace(size_t pos, size_t len, const char str) {
  if (pos >= length())
    return String::npos;

  erase(pos, len);
  insert(pos, &str);
  return pos;
}

size_t String::replace(const char *oldstr, const char *newstr,
                       size_t replaceIndex) {
  size_t oldStrLen = strlen(oldstr);

  size_t pos = find(oldstr);
  for (u32 i = 0; i < replaceIndex; ++i) {
    pos = find(oldstr, pos + oldStrLen);
    if (pos == String::npos) {
      return String::npos;
    }
  }
  replace(pos, oldStrLen, newstr);
  return pos;
}

void String::replaceAll(const char *oldstr, const char *newstr) {
  size_t pos = 0;
  while (pos != String::npos) {
    pos = replace(oldstr, newstr);
  }
}

void String::replaceAll(const char *oldstr, const char newstr) {
  size_t pos = 0;
  while (pos != String::npos) {
    pos = replace(oldstr, &newstr);
  }
}

void String::resize(size_t size, const char fill) {
  if (size < this->size()) {
    mString[size] = '\0';
    return;
  } else {
    append(size - this->size(), fill);
  }
}

String *String::substr(size_t pos, size_t len) {
  String *substr = new String(Max(len, mBufferSize));

  len = Min(size() - pos, len);
  if (pos < size()) {
    strncpy(substr->mString, mString + pos, len);
  }
  return substr;
}

char *String::intToString(s32 num, char *buffer, size_t base) {
  switch (base) {
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