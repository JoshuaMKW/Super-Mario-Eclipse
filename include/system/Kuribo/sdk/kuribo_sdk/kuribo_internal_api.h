//! @file
//! @brief Core structures for interacting with Kuribo modules
//!

#pragma once

#include "kuribo_types.h"

/* Compatible kernel-api version */
#define KURIBO_CORE_VERSION 0

/* Return status from a kernel call. */
enum __KExit {
  KURIBO_EXIT_SUCCESS = 1,
  KURIBO_EXIT_FAILURE = 0,
};

/* Kernel calls */
enum __KReason {
  KURIBO_REASON_LOAD,              /* Load this module. */
  KURIBO_REASON_UNLOAD,            /* Reverse modifications from loading. */
  KURIBO_REASON_INQUIRE_META_DESC, /* Fill in __kuribo_simple_meta_v0. */
};

/* Data to be filled in by simple metadata inquiry. */
typedef struct __kuribo_simple_meta_v0 {
  const char* compiler_name;
  const char* build_date;

  /* user-reported data */
  const char* module_name;
  const char* module_author;
  const char* module_version;
} __kuribo_simple_meta_v0;

typedef struct {
  u32 core_version; /* must match KURIBO_CORE_VERSION */

  /* kxRegisterProcedure, if available */
  void (*register_procedure)(const char* symbol, u32 value);
  /* kxGetProcedure, if available */
  u32 (*get_procedure)(const char* symbol);

  /* Valid only for ATTACH events */
  char* start_address;

  /* Communication channel */
  union {
    /* for KURIBO_REASON_INQUIRE_META_DESC */
    __kuribo_simple_meta_v0* fillin_meta;
  } udata;
} __kuribo_module_ctx_t;

typedef int (*kuribo_module_prologue)(int k_reason,
                                      __kuribo_module_ctx_t* k_ctx);