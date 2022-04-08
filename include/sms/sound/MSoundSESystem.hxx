#pragma once

#include "JGeometry.hxx"
#include "sms/sound/JAISound.hxx"
#include "types.h"


namespace MSoundSESystem {
class MSoundSE {
public:
  static void startSoundNpcActor(u32, Vec const *, u32,
                                 JAISound **, u32, u8);
  static bool checkSoundArea(u32, Vec const &);
  static void startSoundActorWithInfo(u32, Vec const *,
                                      Vec *, f32, u32, u32,
                                      JAISound **, u32, u8);
  static void startSoundSystemSE(u32 sfx, u32, JAISound **, u32);
  static void startSoundActor(u32, Vec const *, u32,
                              JAISound **, u32, u8);
  static u32 getRandomID(u32);
  static void construct();
};
}; // namespace MSoundSESystem

using namespace MSoundSESystem;