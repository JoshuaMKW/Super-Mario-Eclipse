#pragma once

#include "types.h"

#include "JDrama/JDRCharacter.hxx"
#include "JKR/JKRArchivePri.hxx"
#include "JSU/JSUMemoryStream.hxx"

class TObjChara : public JDrama::TCharacter {
public:
  virtual ~TObjChara();

  virtual void load(JSUMemoryInputStream &);
  virtual JKRArchive *getGlbResource(const char *) const;
};