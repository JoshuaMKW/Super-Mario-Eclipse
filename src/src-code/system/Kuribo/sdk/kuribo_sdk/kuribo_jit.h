#pragma once

#include "kuribo_types.h"

#define KX_API extern "C"

//! @brief A compiled function handle.
//!
typedef void (*kxCompiledFunction)();

//! @brief Compile a GCT to JIT instructions via KuriboJIT.
//!
//! The Kuribo Gecko JIT. The fastest codehandler, by some 200x. It's important
//! to allocate enough memory to hold a code. Generally, expect to use
//!   1.5x the size of the original GCT.
//!
//! @return A function to call to execute the code list. (Many codes rely on
//! this being once a frame.)
//!
//! NOTE: Will return NULL if operations failed.
//!
KX_API
kxCompiledFunction kxGeckoJitCompileCodes(
    //! Where KuriboJIT will put JIT instructions to execute.
    void* free_space_begin,
    //! The size (in bytes) of the memory region.
    u32 free_space_size,
    //! The start of your GCT. Header/footer are not needed.
    const void* gct_begin,
    //! The size (in bytes) of your GCT.
    u32 gct_size);

//! @brief Run the compiled code.
//!
//! @return Return if operation succeeded.
//!
inline bool kxGeckoJitRunCompiledFunction(kxCompiledFunction func) {
  if (!func)
    return false;

  func();
  return true;
}