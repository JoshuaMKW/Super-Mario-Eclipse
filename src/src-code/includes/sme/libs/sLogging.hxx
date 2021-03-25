#pragma once

#include "OS.h"

#define SME_STRINGIZE_(x) #x
#define SME_STRINGIZE(x) SME_STRINGIZE_(x)

#define SME_WIDE_(s) L##s
#define SME_WIDE(s) SME_WIDE_(s)

#define SME_CONCATENATE_(a, b) a##b
#define SME_CONCATENATE(a, b) SME_CONCATENATE_(a, b)

#define SME_DEBUG_LOG(msg, ...) 
#ifdef SME_DEBUG
OSReport("[SME] %s", msg, ##__VA_ARGS__)
#endif
