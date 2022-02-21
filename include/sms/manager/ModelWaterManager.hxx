#pragma once

#include "types.h"

#include "JDrama/JDRGraphics.hxx"
#include "JDrama/JDRViewObj.hxx"
#include "JGeometry.hxx"
#include "JSU/JSUMemoryStream.hxx"
#include "JUT/JUTColor.hxx"
#include "sms/nozzle/Watergun.hxx"

class TModelWaterManager : public JDrama::TViewObj {
public:
  virtual ~TModelWaterManager();

  virtual void perform(u32, JDrama::TGraphics *) override;
  virtual void load(JSUMemoryInputStream &) override;
  virtual void loadAfter() override;

  void askDoWaterHitCheck();
  void askHitWaterParticleOnGround(const JGeometry::TVec3<f32> &);
  void calcDrawVtx(Mtx44);
  void calcVMAll(Mtx44);
  void calcVMMtxGround(Mtx44, f32, const JGeometry::TVec3<f32> &,
                       const JGeometry::TVec3<f32> &, Mtx44);
  void calcVMMtxWall(Mtx44, f32, const JGeometry::TVec3<f32> &,
                     const JGeometry::TVec3<f32> &, Mtx44);
  void calcWorldMinMax();
  void drawMirror(Mtx44);
  void drawRefracAndSpec() const;
  void drawShineShadowVolume(Mtx44);
  void drawSilhouette(Mtx44);
  void drawTouching();
  void drawWaterVolume(Mtx44);
  u8 emitRequest(const TWaterEmitInfo &info);
  void garbageCollect();
  void makeEmit(const TWaterEmitInfo &info);
  void move();
  void wind(const JGeometry::TVec3<f32> &);

  s16 mMaxEmit;        // 0x0010
  s16 mEmitCount;      // 0x0012
  u32 _03[0x5D48 / 4]; // 0x0014
  u16 _04;             // 0x5D5C
  u8 _05;              // 0x5D5E
  s8 mWaterCardType;   // 0x5D5F

  struct {
    u8 _00 : 6;
    bool mMaskObjects : 1;
    bool mShowShadow : 1;
    u8 _01 : 8;
  } LightType; // 0x5D60

  u16 _06;                 // 0x5D62
  u32 _07[0xA8 / 4];       // 0x5D64
  f32 mSize;               // 0x5E0C
  f32 _08[0x30 / 4];       // 0x5E10
  f32 mSphereStep;         // 0x5E40
  u8 mLayerCount;          // 0x5E44
  u8 mDarkLevel;           // 0x5E45
  u8 _09;                  // 0x5E46
  JUtility::TColor mColor; // 0x5E47

  static THitActor mStaticHitActor;
};