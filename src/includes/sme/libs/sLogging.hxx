#pragma once

#include "OS.h"
#include "printf.h"
#include "macros.h"

#define SME_LOG(msg, ...) {                                                    \
    char errmsg[256];                                                          \
    sprintf(errmsg, "[SME] %s", (msg));                                        \
    OSReport(errmsg, ##__VA_ARGS__);                                           \
}

#if defined(SME_DEBUG) && !defined(SME_RELEASE)
#define SME_DEBUG_LOG(msg, ...) SME_LOG(msg, ##__VA_ARGS__)
#else
#define SME_DEBUG_LOG(msg, ...)
#endif
