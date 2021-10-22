#pragma once

#include "../JSU/JSUMemoryStream.hxx"
#include "types.h"

namespace JDrama {

class TNameRef {
public:
  TNameRef(const char *);
  virtual ~TNameRef();

  virtual u32 getType() const;
  virtual void load(JSUMemoryInputStream &stream);
  virtual void save(JSUMemoryOutputStream &stream);
  virtual void loadAfter();
  virtual TNameRef *searchF(u16, const char *);

  void genObject(JSUMemoryInputStream &stream, JSUMemoryInputStream &);
  u32 getType(JSUMemoryInputStream &stream, JSUMemoryInputStream &);
  void search(const char *);

  static s16 calcKeyCode(const char *);

  char *mTypeName;
  u16 mKeyCode;
  u16 _0A;
};

} // namespace JDrama