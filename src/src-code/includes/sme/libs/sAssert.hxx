#pragma once

#include "OS.h"
#include "sLogging.hxx"

#define SME_ASSERT(expr, msg, ...) \
    if (!(expr)) OSPanic(__FILE__, __LINE__, msg, ...)

#define SME_DEBUG_ASSERT(expr, msg, ...)
#ifdef SME_DEBUG
SME_ASSERT(expr, msg, ...)
#endif