#pragma once

#include "types.h"

#include "JDrama/JDRActor.hxx"
#include "JDrama/JDRGraphics.hxx"
#include "JGeometry.hxx"

class THitActor : public JDrama::TActor {
public:
  THitActor(const char *name);
  virtual ~THitActor();

  virtual void perform(u32, JDrama::TGraphics *) override;
  virtual bool receiveMessage(THitActor *sender, u32 msg);

  void initHitActor(u32 actorID, u16 collisionArrayCap, int collisionFlags,
                    f32 attackRadius, f32 attackHeight, f32 receiveRadius,
                    f32 receiveHeight);
  void calcEntryRadius();

  THitActor **mCollidingObjs; // 0x0044
  s16 mNumObjs;               // 0x0048
  s16 mMaxObjs;               // 0x004A
  u32 mObjectID;              // 0x004C | & 0x80000000 == Ignore Water Spray
  f32 mAttackRadius;          // 0x0050
  f32 mAttackHeight;          // 0x0054
  f32 mReceiveRadius;         // 0x0058
  f32 mReceiveHeight;         // 0x005C
  f32 mEntryRadius;           // 0x0060
  u32 mObjectType;            // 0x0064 | & 0x4 == Ignore Water Spray
};