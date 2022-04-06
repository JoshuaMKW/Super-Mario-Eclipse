#pragma once

#include "JGeometry.hxx"

class JPABaseEmitter;

class JPABaseParticle {
public:
  virtual void executeParticleCallBack(JPABaseEmitter *);
  virtual void drawParticleCallBack();

  void calcGlobalPosition();
  void getCurrentPosition(TVec3f &);
  void incTimer();
  void initGlobalPosition();
};