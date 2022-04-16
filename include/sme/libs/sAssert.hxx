#pragma once

#include "OS.h"
#include "PPCArch.h"
#include "printf.h"
#include "equivtype.hxx"

#include "Globals.hxx"

#if defined(__GNUC__) || (defined(__MWERKS__) && (__MWERKS__ >= 0x3000)) ||    \
    (defined(__ICC) && (__ICC >= 600))
#define _SmeFunc __PRETTY_FUNCTION__
#elif defined(__FUNCSIG__)
#define _SmeFunc __FUNCSIG__
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901)
#define _SmeFunc __func__
#else
#define _SmeFunc "(unknown)"
#endif

#define SME_ERROR(msg, ...)                                                    \
  char errmsg[256];                                                            \
  snprintf(errmsg, 256, "[SME] %s: %s", (_SmeFunc), (msg));                    \
  OSPanic(__FILE__, __LINE__, errmsg, ##__VA_ARGS__);                          \
  __OSUnhandledException(6, OSGetCurrentContext(), 0);

#define SME_ASSERT(expr, msg, ...)                                             \
  if (!(expr)) {                                                               \
    SME_ERROR(msg, ##__VA_ARGS__);                                             \
  }

#if defined(SME_DEBUG) && !defined(SME_RELEASE)
#define SME_DEBUG_ASSERT(expr, msg, ...) SME_ASSERT(expr, msg, ##__VA_ARGS__)
#else
#define SME_DEBUG_ASSERT(expr, msg, ...)                                       \
  if (SME::TGlobals::isDebugMode()) {                                          \
    SME_ASSERT(expr, msg, ##__VA_ARGS__);                                      \
  }
#endif

#define SME_ASSERT_SAME_TYPE(T1, T2, emsg) static_assert(is_equal_type<T1, T2>(), emsg)
