#pragma once

#include "types.h"
#include "mem.h"
#include "string.h"

class String {
  char *mString;
  size_t mBufferSize;
  size_t mStringSize;

public:
  static const size_t npos = -1;
  static const char numbers[10];

  String(size_t bufferSize);
  String(const char *str, size_t bufferSize);
  String(const String &, size_t bufferSize);
  virtual ~String();

  String &operator=(const String &str);
  String &operator=(const char *str);
  String &operator=(char str);

  String &operator+=(const String &str);
  String &operator+=(const char *str);
  String &operator+=(char chr);

  String &operator+(const String &str);
  String &operator+(const char *str);
  String &operator+(char chr);

  String &operator*=(size_t mul);
  String &operator*(size_t mul);

  static s32 stringToInt(const char *str, size_t base = 10) {
    return strtol(str, nullptr, base);
  };

  constexpr size_t begin() const { return 0; };
  constexpr size_t capacity() const { return this->mBufferSize; };
  const char *data() const { return this->mString; };
  constexpr bool empty() const { return this->mString[0] == '\0'; };
  constexpr size_t end() { return this->mStringSize; };
  constexpr size_t length() const { return this->mStringSize; };
  constexpr size_t maxSize() const { return 1024; };
  constexpr size_t size() const { return this->mStringSize; };
  s32 toInt(size_t base = 10) { return strtol(this->data(), nullptr, base); };

  void append(String &str);
  void append(String &str, size_t subpos, size_t sublen);
  void append(const char *str);
  void append(size_t len, const char chr);
  void assign(const char *str);
  void assign(const char *str, size_t pos, size_t len);
  void assign(size_t len, const char chr);
  void clear() { memset(this->mString, 0, this->mBufferSize); };
  size_t compare(const char *str);
  size_t compare(size_t pos, size_t len, const char *str);
  size_t compare(size_t pos, size_t len, const char *str, size_t subpos,
                 size_t sublen);
  size_t erase(size_t pos, size_t len = String::npos);
  size_t find(const char *str, size_t startPos = 0);
  size_t find(const char chr, size_t startPos = 0);
  size_t findAny(const char list[], size_t listSize, size_t startPos = 0);
  void insert(size_t pos, const char *str);
  size_t replace(size_t pos, size_t len, const char *newstr);
  size_t replace(size_t pos, size_t len, const char newstr);
  size_t replace(const char *oldstr, const char *newstr,
                 size_t replaceIndex = 0);
  void replaceAll(const char *oldstr, const char *newstr);
  void replaceAll(const char *oldstr, const char newstr);
  void resize(size_t size, const char fill);
  String *substr(size_t pos, size_t len = String::npos);
  static char *intToString(s32 num, char *buffer, size_t base = 10);
};