#pragma once

#include "GraphWeb.hxx"
#include "types.h"


class TGraphTracer {
public:
  TGraphTracer();

  f32 calcSplineSpeed(f32);
  int getCurGraphIndex() const;
  TGraphWeb *getGraph() const;
  void moveTo(int);
  void setTo(int);
  void traceSpline(f32);

  TGraphWeb *mGraph; // 0x0000
  s32 mCurrentNode;  // 0x0004
  s32 mPreviousNode; // 0x0008
  f32 _00;
  f32 _01;
  f32 _02;
};