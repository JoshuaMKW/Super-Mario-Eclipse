#pragma once

#include "OS.h"
#include "macros.h"
#include "printf.h"

#define SME_MAX_LOG_BUFFER 1024
static char sLogBuffer[SME_MAX_LOG_BUFFER]

#define SME_LOG(msg, ...) {                                          \
    snprintf(sLogBuffer, SME_MAX_LOG_BUFFER, msg, ##__VA_ARGS__);    \
    OSReport("[SME] %s", msg);                                       \
}

#ifdef SME_DEBUG
#define SME_DEBUG_LOG(msg, ...) SME_LOG(msg, ##__VA_ARGS__)
#else
#define SME_DEBUG_LOG(msg, ...)
#endif
