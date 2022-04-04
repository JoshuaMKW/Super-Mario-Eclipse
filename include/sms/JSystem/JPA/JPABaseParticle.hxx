#pragma once

#include "JGeometry.hxx"

class JPABaseEmitter;

class JPABaseParticle {
public:
  virtual void executeParticleCallBack(JPABaseEmitter *);
  virtual void drawParticleCallBack();

  void calcGlobalPosition();
  void getCurrentPosition(JGeometry::TVec3<f32> &);
  void incTimer();
  void initGlobalPosition();
};