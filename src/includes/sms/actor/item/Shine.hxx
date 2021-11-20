#pragma once

#include "JGeometry.hxx"
#include "sms/actor/Item.hxx"
#include "types.h"


class TShine : public TItem {
public:
  TShine(const char *);
  virtual ~TShine();

  virtual void loadAfter() override;
  virtual void perform(u32, JDrama::TGraphics *) override;
  virtual bool receiveMessage(THitActor *, u32) override;
  virtual void control() override;
  virtual void kill() override;
  virtual void initMapObj() override;
  virtual void loadBeforeInit(JSUMemoryInputStream &) override;
  virtual void makeMActors() override;
  virtual void calc() override;
  virtual void touchPlayer(THitActor *) override;

  void appearSimple(int);
  void appearWithDemo(const char *);
  void appearWithTime(int, int, int, int);
  void appearWithTimeCallback(u32, u32);
  void movingCircle();

  u32 mType;                       // 0x0154
  u32 _02[0x38 / 4];               // 0x0158
  bool mIsNormalCamera;            // 0x0190
  u8 _03;                          // 0x0191
  u16 _04;                         // 0x0192
  u32 _05[0x14 / 4];               // 0x0194
  JGeometry::TVec3<f32> mGlowSize; // 0x01A8
  u8 mIsAlreadyObtained;           // 0x01B4
  u8 _06;                          // 0x01B5
  u16 _07;                         // 0x01B6
  u32 _08[0x40 / 4];               // 0x01B8

  static f32 mBowRate;
  static f32 mCircleRate;
  static f32 mKiraRate;
  static f32 mPromiLife;
  static f32 mSenkoRate;
  static f32 mSpeedDownRate;
  static f32 mUpSpeed;
};