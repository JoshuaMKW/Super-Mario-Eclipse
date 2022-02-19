#pragma once

#include "OS.h"
#include "macros.h"
#include "printf.h"

#include "Globals.hxx"

#define SME_LOG(msg, ...)                                                      \
  {                                                                            \
    char errmsg[256];                                                          \
    sprintf(errmsg, "[SME] %s", (msg));                                        \
    OSReport(errmsg, ##__VA_ARGS__);                                           \
  }
  
#define SME_FUNCTION_LOG(msg, ...)                                             \
  {                                                                            \
    char errmsg[256];                                                          \
    sprintf(errmsg, "[SME] %s %s", SME_FUNC_SIG, (msg));                       \
    OSReport(errmsg, ##__VA_ARGS__);                                           \
  }

#if defined(SME_DEBUG) && !defined(SME_RELEASE)
#define SME_DEBUG_LOG(msg, ...) SME_LOG((msg), ##__VA_ARGS__)
#else
#define SME_DEBUG_LOG(msg, ...)                                                \
  if (SME::TGlobals::isDebugMode()) {                                          \
    SME_LOG((msg), ##__VA_ARGS__)                                              \
  }
#endif

#define SME_CONSOLE_LOG(msg, ...)                                              \
  if (!SME::isGameEmulated()) {                                                \
    SME_LOG((msg), ##__VA_ARGS__);                                             \
  }

#define SME_EVAL_LOG(eval)                                                     \
 if (SME::TGlobals::isDebugMode()) {                                           \
  SME_LOG(SME_STRINGIZE(eval) " = %d\n", eval);                                \
 }                                                                             \
