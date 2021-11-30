#pragma once

#include "types.h"

namespace SME::Util {

constexpr u16 cexp_calcKeyCode(const char *str) {
  u32 i = 0;
  for (; *str != '\0'; i = (*str++) + i * 3);
  return i;
}

} // namespace SME::Util