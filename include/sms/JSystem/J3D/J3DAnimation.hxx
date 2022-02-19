#pragma once

#include "JUT/JUTNameTab.hxx"
#include "J3DTransform.hxx"
#include "types.h"


class J3DTexNoAnm {
public:
  u32 *vTable; // _0
  u16 _4;
  u16 _6; // padding?
};

class J3DAnmColor {};

class J3DAnmTevRegKey {};

class J3DAnmTextureSRTKey {};

class J3DMatColorAnm {};

class J3DTevKColorAnm {};

class J3DTexNtxAnm {};

class J3DAnmKeyTableBase {};

class J3DAnmClusterKey {
public:
  virtual ~J3DAnmClusterKey();
  virtual f32 getWeight(u16) const;

  f32 _04;
  u32 _08;
  void *_0C;
  f32 *_10;
  J3DAnmKeyTableBase *mKeyTableBase;
};

class J3DAnmCluster : public J3DAnmClusterKey {
public:
  virtual ~J3DAnmCluster();
  virtual f32 getWeight(u16) const;
};

class J3DAnmClusterFull : J3DAnmCluster {
public:
  virtual ~J3DAnmClusterFull();
  virtual f32 getWeight(u16) const;
};

class J3DAnmTransformKey {
public:
  virtual ~J3DAnmTransformKey();
  virtual void getTransform(u16, J3DTransformInfo *) const;
  virtual void calcTransform(f32, u16, J3DTransformInfo *) const;
};

class J3DAnmTransform : J3DAnmTransformKey {
public:
  virtual ~J3DAnmTransform();
  virtual void getTransform(u16, J3DTransformInfo *) const override;
};

class J3DAnmTransformFull : J3DAnmTransform {
public:
  virtual ~J3DAnmTransformFull();
  virtual void getTransform(u16, J3DTransformInfo *) const override;
};

class J3DFrameCtrl {
public:
  enum Control { OSCILLATION = 4 };

  J3DFrameCtrl();
  virtual ~J3DFrameCtrl();

  bool checkPass(f32);
  void init(s16);
  void update();

  s8 _04; // anim state
  struct {
    bool _00 : 6;
    bool mIsAnmReversed : 1;
    bool mIsAnmDead : 1;
  } mAnmFlags;
  s16 _06;
  s16 mNumFrames;
  s16 _0A;
  f32 mFrameRate;
  f32 mCurFrame;
};

class J3DAnmBase {
public:
  u16 _0;
  u16 _2;
  f32 _4;
  u32 _8;      // guessed
  u32 *vTable; // _C
};

class J3DAnmTexPattern : public J3DAnmBase {
public:
  u32 _10;
  u32 _14;
  u16 _18;
  u16 _1A;
  u32 _1C;
  JUTNameTab *mNameTab; // _20
};