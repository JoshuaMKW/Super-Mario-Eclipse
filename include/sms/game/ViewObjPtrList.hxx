#pragma once

#include "J3D/J3DAnimation.hxx"
#include "JDrama/JDRGraphics.hxx"

template <class _T, class _A> class TViewObjPtrListT : public JDrama::TViewObj {
public:
  TViewObjPtrListT(const char *name) : JDrama::TViewObj(name){};
  virtual ~TViewObjPtrListT();

  virtual void load(JSUMemoryInputStream &stream) override;
  virtual void save(JSUMemoryOutputStream &stream) override;
  virtual void loadAfter() override;
  virtual TNameRef *searchF(u16, const char *) override;
  virtual void perform(u32, JDrama::TGraphics *) override;
  virtual void loadSuper(JSUMemoryInputStream &stream);
  virtual void loadAfterSuper();
};