#pragma once

#include "sms/manager/JointModelManager.hxx"
#include "types.h"


class TPollutionManager : public TJointModelManager {
public:
  struct TPollutionInfo {

  };
  TPollutionManager(const char *);
  virtual ~TPollutionManager();

  virtual void load(JSUMemoryInputStream &) override;
  virtual void perform(u32, JDrama::TGraphics *) override;
  virtual TJointModel *newJointModel() const override;

  void clean(f32 x, f32 y, f32 z, f32 r);
  bool cleanedAll() const;
  f32 getPollutionDegree() const;
  u32 getPollutionType(f32 x, f32 y, f32 z) const;
  bool isPolluted(f32 x, f32 y, f32 z) const;
  void setDataAddress(TPollutionInfo *info);
  void stamp(u16, f32 x, f32 y, f32 z, f32 r);
  void stampGround(u16, f32 x, f32 y, f32 z, f32 r);
  void stampModel(J3DModel *model);

  u32 _24[0x1EC / 4];

  static u8 mEdgeAlpha;
  static s32 mFlushTime;
};