#pragma once

#include "JGeometry.hxx"
#include "sms/actor/Item.hxx"
#include "types.h"
#include "JPA/JPABaseEmitter.hxx"


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
  u32 _158[0x38 / 4];               // 0x0158
  bool mIsNormalCamera;            // 0x0190
  JPABaseEmitter *mPromiEffect; // 0x0194 | Light Flare
  JPABaseEmitter *mSenkoEffect; // 0x0198 | Light rays
  JPABaseEmitter *mKiraEffect; // 0x019C | Sparkles
  JPABaseEmitter *mBowEffect; // 0x01A0 | Light bow
  u32 _1A4;
  TVec3f mGlowSize; // 0x01A8
  bool mIsAlreadyObtained;           // 0x01B4
  u32 _1B8[0x40 / 4];               // 0x01B8

  static f32 mBowRate;
  static f32 mCircleRate;
  static f32 mKiraRate;
  static f32 mPromiLife;
  static f32 mSenkoRate;
  static f32 mSpeedDownRate;
  static f32 mUpSpeed;
};