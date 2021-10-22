#pragma once

#include "JSU/JSUMemoryStream.hxx"
#include "JDRNameRef.hxx"
#include "types.h"

namespace JDrama {

class TNameRefGen {
public:
  virtual void load(JSUMemoryInputStream &stream);
  virtual TNameRef *getNameRef(const char *) const;

  TNameRef *getRootNameRef();
  static TNameRefGen *getInstance();

private:
  TNameRef *mRootName;
};

} // namespace JDrama