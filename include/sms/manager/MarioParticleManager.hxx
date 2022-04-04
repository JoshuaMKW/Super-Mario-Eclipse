#pragma once

#include "JDrama/JDRGraphics.hxx"
#include "JDrama/JDRViewObj.hxx"
#include "JGadget/Allocator.hxx"
#include "JGadget/List.hxx"
#include "JSU/JSUMemoryStream.hxx"
#include "sms/actor/HitActor.hxx"
#include "sms/object/EffectObjBase.hxx"
#include "sms/object/ObjChara.hxx"
#include "types.h"

/* Size -- 0x34 */
class TMarioParticleManager : public JDrama::TViewObj {
public:
  struct TInfo {};

  TMarioParticleManager(char const *);
  virtual ~TMarioParticleManager();

  virtual void perform(u32, JDrama::TGraphics *) override;

  TInfo *createEffectInfoAry(int);
  JPABaseEmitter *emit(s32 effect, const JGeometry::TVec3<f32> *, u8, const void *);
  JPABaseEmitter *emitAndBindToMtx(s32 effect, Mtx matrix, u8, const void *);
  JPABaseEmitter *emitAndBindToMtxPtr(s32 effect, Mtx matrix, u8, const void *);
  JPABaseEmitter *emitAndBindToPosPtr(s32 effect, const JGeometry::TVec3<f32> *, u8,
                            const void *);
  JPABaseEmitter *emitAndBindToSRTMtxPtr(s32 effect, Mtx matrix, u8, const void *);
  JPABaseEmitter *emitParticleCallback(
      s32 effect, const JGeometry::TVec3<f32> *, u8,
      void * /*JPACallBackBase2<JPABaseEmitter *, JPABaseParticle *> **/,
      const void *);
  void emitTry(s32 effect, TInfo *info, u8);
  JPABaseEmitter *emitWithRotate(s32 effect, const JGeometry::TVec3<f32> *, s16 x, s16 y,
                       s16 z, u8, const void *);
  s32 getAvailableIdx(s32 effect, u8, const void *);
};