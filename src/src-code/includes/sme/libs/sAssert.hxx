#pragma once

#include "OS.h"
#include "sLogging.hxx"

#define SME_ASSERT(expr, ...) \
    if (!(expr)) OSPanic(__FILE__, __LINE__, __VA_ARGS__)

#define SME_DEBUG_ASSERT(expr, ...)
#ifdef SME_DEBUG
SME_ASSERT(expr, __VA_ARGS__)
#endif