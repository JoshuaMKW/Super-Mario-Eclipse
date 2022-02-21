#pragma once

#include "types.h"

class TSpcBinary {
  void *mHeader; // _0

public:
  TSpcBinary(void *binary);
  virtual ~TSpcBinary();

  virtual void initUserBuiltin();

  void *getHeader() const;
  void bindSystemDataToSymbol(const char *symbol, u32 address);
  u32 searchSymbol(const char *symbol);
  void init();
};
