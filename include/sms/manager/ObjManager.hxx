#pragma once

#include "JDrama/JDRGraphics.hxx"
#include "JDrama/JDRViewObj.hxx"
#include "JSU/JSUMemoryStream.hxx"
#include "sms/actor/HitActor.hxx"
#include "sms/object/ObjChara.hxx"
#include "types.h"


class TModelDataLoadEntry{
  const char *mModelPath;
  u32 mModelFlags;
  u32 _0C;
};

/* Size -- 0x34 */
class TObjManager : public JDrama::TViewObj {
public:
  TObjManager(char const *);
  virtual ~TObjManager();

  virtual void load(JSUMemoryInputStream &) override;
  virtual TObjManager *searchF(u16, const char *) override;
  virtual void perform(u32, JDrama::TGraphics *) override;
  virtual void createModelData();
  virtual void createAnmData();
  virtual void createModelDataArray(const TModelDataLoadEntry *);

  void createModelDataArrayBase(const TModelDataLoadEntry *, const char *);
  MActorAnmData *getMActorAnmData();
  void *getModelDataKeeper();
  void manageObj(THitActor *obj);

  s32 mMaxObjs; // _10
  u32 _14;      // I think this is obj number
  u32 *_18;
  TObjChara *mObjChara;
  u32 _20;
  u32 _24;
  u32 _28;
  u32 _2C;
  u32 _30;
};