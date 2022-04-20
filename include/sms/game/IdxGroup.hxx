#pragma once

#include "J3D/J3DAnimation.hxx"
#include "JDrama/JDRViewObj.hxx"
#include "JGadget/List.hxx"
#include "ViewObjPtrList.hxx"
#include "sms/actor/HitActor.hxx"


class IdxGroup : public TViewObjPtrListT<THitActor, JDrama::TViewObj> {
public:
  IdxGroup(const char *name)
      : TViewObjPtrListT<THitActor, JDrama::TViewObj>(name){};
  virtual ~IdxGroup();

  virtual void loadSuper(JSUMemoryInputStream &stream) override;

  JGadget::TList_pointer<THitActor *> mHitActorList;
};