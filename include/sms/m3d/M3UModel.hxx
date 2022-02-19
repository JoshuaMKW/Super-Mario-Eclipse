#pragma once

#include "J3D/J3DModel.hxx"
#include "JDrama/JDRGraphics.hxx"
#include "types.h"


class M3UMtxCalcSetInfo {};

class M3UModel {
public:
  virtual void changeMtxCalcAnmTransform(int, u8);
  virtual void changeAnmTexPattern(int, u8);
  virtual void setMtxCalc(const M3UMtxCalcSetInfo &);
  virtual void updateIn();
  virtual void updateOut();
  virtual void entryIn();
  virtual void entryOut();

  void perform(u32, JDrama::TGraphics *);
  void updateInMotion();
  void updateInTexPatternAnm();

  void *_04;         // 0x0000
  J3DModel *mModel;  // 0x0008
  u32 _0C[0x30 / 4]; // 0x000C
  u32 mLightID;      // _3C
  u8 _40;
  u32 mLightType; // _44
};

class M3UModelCommon {
public:
  virtual M3UMtxCalcSetInfo *getMtxCalc(const M3UMtxCalcSetInfo &) const;
};