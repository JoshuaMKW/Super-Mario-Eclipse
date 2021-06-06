//===-- udivdi3.c - Implement __udivdi3 -----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements __udivdi3 for the compiler_rt library.
//
//===----------------------------------------------------------------------===//

#ifndef __MWERKS__

#include "types.h"

typedef u64 fixuint_t;
typedef s64 fixint_t;
#include "int_div_impl.inc"

// Returns: a / b

u64 __udivdi3(u64 a, u64 b) {
  return __udivXi3(a, b);
}

#endif
