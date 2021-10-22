#pragma once

#include "types.h"

#include "../JGadget/List.hxx"
#include "../JGeometry.hxx"
#include "JDRNameRef.hxx"

namespace JDrama {

template <typename T, typename U>
class TNameRefPtrListT : public TNameRef, public TList_pointer_void {
public:
  virtual ~TNameRefPtrListT();

  virtual void load(JSUMemoryInputStream &stream);
  virtual void loadAfter();
  T *searchF(u16, const char *);
};

} // namespace JDrama