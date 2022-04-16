#pragma once

#include "JDrama/JDRGraphics.hxx"
#include "JDrama/JDRViewObj.hxx"
#include "JSU/JSUMemoryStream.hxx"
#include "sms/actor/HitActor.hxx"
#include "sms/object/ObjChara.hxx"
#include "sms/object/EffectObjBase.hxx"
#include "JGadget/List.hxx"
#include "JGadget/Allocator.hxx"
#include "types.h"


/* Size -- 0x34 */
class TEffectObjManager : public JDrama::TViewObj {
public:
  TEffectObjManager(const char *);
  virtual ~TEffectObjManager();

  virtual void load(JSUMemoryInputStream &) override;
  virtual void perform(u32, JDrama::TGraphics *) override;

  char *mName;
  u16 mKeyCode;
  u32 _0C;
  u32 _10;
  JGadget::TList<TEffectObjBase *, JGadget::TAllocator> mEffectObjList;
  TEffectObjBase **mEffectObjs;

  static constexpr size_t DefaultEffectNum = 5;
};