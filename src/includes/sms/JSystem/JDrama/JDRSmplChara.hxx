#pragma once

#include "types.h"

#include "../JGeometry.hxx"
#include "../JKR/JKRArchivePri.hxx"
#include "JDRCharacter.hxx"
#include "JDRNameRef.hxx"

namespace JDrama {

class TSmplChara : public TCharacter {
public:
  virtual ~TSmplChara();

  virtual void load(JSUMemoryInputStream &stream);
  virtual u32 *getRes(const char *resName);

  void mountArc(const char *resName);

  JKRArchive *mArchive; // C
};

} // namespace JDrama