#pragma once

#include "types.h"
#include "GX.h"

#include "J3D/J3DDrawBuffer.hxx"
#include "JGeometry.hxx"
#include "JKR/JKRArchivePri.hxx"
#include "JSU/JSUMemoryStream.hxx"
#include "JUT/JUTGamePad.hxx"
#include "sms/game/PerformList.hxx"
#include "JStage.hxx"

class TCamConnector;

namespace JDrama {



class TAmbColor : public TViewObj, public JStage::TAmbientLight {
public:
  TAmbColor();
  virtual ~TAmbColor();

  virtual void load(JSUMemoryInputStream &stream);
  virtual void perform(u32, TGraphics *);

  u8 r; // 14
  u8 g; // 15
  u8 b; // 16
  u8 a; // 17
};

class TDrawBufObj : public TViewObj {
public:
  TDrawBufObj();
  TDrawBufObj(u32, u32, const char *);
  virtual ~TDrawBufObj();

  virtual void load(JSUMemoryInputStream &stream);
  virtual void perform(u32, TGraphics *);

  J3DDrawBuffer *mDrawBuffer; // 10
  u32 mBufferSize;            // 14
  u32 _18;                    // init'd to 7
};

class TCamera : public TPlacement, public JStage::TCamera {
public:
  virtual ~TCamera();

  virtual u32 getType() const;
  virtual u32 JSGGetFlag() const;
  virtual void JSGSetFlag(u32 flag);
  virtual f32 JSGGetProjectionNear() const;
  virtual void JSGSetProjectionNear(f32 projectionNear);
  virtual f32 JSGGetProjectionFar() const;
  virtual void JSGSetProjectionFar(f32 projectionFar);

private:
  u16 mFlag;           // 24
  f32 mProjectionNear; // 28
  f32 mProjectionFar;  // 2C
};

class TLookAtCamera : public TCamera {
public:
  virtual ~TLookAtCamera();

  virtual void perform(u32, TGraphics *);

  virtual s32 JSGGetProjectionType() const;
  virtual void JSGSetProjectionType(JStage::TECameraProjection);
  virtual f32 JSGGetProjectionFovy() const;
  virtual void JSGSetProjectionFovy(const f32 projectionFovy);
  virtual f32 JSGGetProjectionAspect() const;
  virtual void JSGSetProjectionAspect(const f32 projectionAspect);
  virtual void JSGGetViewPosition(Vec *) const;
  virtual void JSGSetViewPosition(const Vec &viewPos);
  virtual void JSGGetViewUpVector(Vec *) const;
  virtual void JSGSetViewUpVector(const Vec &upVector);
  virtual void JSGGetViewTargetPosition(Vec *) const;
  virtual void JSGSetViewTargetPosition(const Vec &targetPos);

private:
  JGeometry::TVec3<f32> mUpVector;  // 30
  JGeometry::TVec3<f32> mTargetPos; // 3C
  f32 mProjectionFovy;              // 48
  f32 mProjectionAspect;            // 4C
};

class TPolarCamera : public TCamera {
public:
  virtual ~TPolarCamera();

  virtual void perform(u32, TGraphics *);
  virtual void load(JSUMemoryInputStream &stream);

  virtual s32 JSGGetProjectionType() const;
  virtual void JSGSetProjectionType(JStage::TECameraProjection);
  virtual f32 JSGGetProjectionFovy() const;
  virtual void JSGSetProjectionFovy(f32 projectionFovy);
  virtual f32 JSGGetProjectionAspect() const;
  virtual void JSGSetProjectionAspect(f32 projectionAspect);

private:
  f32 mProjectionFovy;   // 30
  f32 mProjectionAspect; // 34
};

class TOrthoProj : public TCamera {
public:
  virtual ~TOrthoProj();

  virtual void load(JSUMemoryInputStream &stream);
  virtual void perform(u32, TGraphics *);

  virtual s32 JSGGetProjectionType() const;
  virtual void JSGSetProjectionType(JStage::TECameraProjection);
  virtual f32 *JSGGetProjectionField() const;
  virtual void JSGSetProjectionField(const f32 *projectionField);

private:
  JGeometry::TVec4<f32> mProjectionField; // 30
};

class TVideo {
public:
  TVideo();

  void setNextXFB(const void *);
  void waitForRetrace(u16);

  GXRenderModeObj _0;
  GXRenderModeObj _3C;
  u32 _78;
  void *mNextFB;     // 7C
  s32 mTicks;        // 80
  s32 mRetraceCount; // 84
};

class TDisplay {
public:
  TDisplay(u16 retraceCount, void *, void *, const GXRenderModeObj &);

  void startRendering();
  void endRendering();

  u32 _00[4];
  GXRenderModeObj mRenderObj;
  u16 mRetraceCount;
  u16 _4E;
  u32 _50[0x10 / 4];
  JDrama::TVideo *mVideo; // 0x0060
  u16 _64;
};

class TViewConnector : public TViewObj {
public:
  TViewConnector(TViewObj *viewObj1, TViewObj *viewObj2, TFlagT<u16> flags,
                 const char *objName);
  virtual ~TViewConnector();

  virtual void perform(u32, TGraphics *);

  TViewObj *mViewObj1; // 10
  TViewObj *mViewObj2; // 14
  u16 mFlag;           // 18
};

class TViewport : public TViewObj {
public:
  TViewport(const TRect &, const char *);
  virtual ~TViewport();

  virtual void load(JSUMemoryInputStream &stream);
  virtual void perform(u32, TGraphics *);

  TRect mViewportRect; // 10
  u16 _20;
};

class TScreen : public TViewConnector {
public:
  TScreen(const TRect &, const char *);
  virtual ~TScreen();

  void assignCamera(TViewObj *);
  void assignViewObj(TViewObj *);

  TViewport *mViewport;         // 10
  TCamConnector *mCamConnecter; // 14
};

class TSmplChara : public TCharacter {
public:
  virtual ~TSmplChara();

  virtual void load(JSUMemoryInputStream &stream);
  virtual u32 *getRes(const char *resName);

  void mountArc(const char *resName);

  JKRArchive *mArchive; // C
};

bool IsEqualRenderModeVIParams(const GXRenderModeObj &,
                               const GXRenderModeObj &);
bool IsRenderModeHalfAspectRatio(const GXRenderModeObj &);
f32 GetRenderModeYScale(const GXRenderModeObj &);
void CalcRenderModeXFBHeight(GXRenderModeObj *, u16);
void CalcRenderModeVIXOrigin(GXRenderModeObj *);
void CalcRenderModeVIYOrigin(GXRenderModeObj *);
void CopyRenderModeSamplePattern(GXRenderModeObj *, const u8 *[2]);
void CopyRenderModeVFilter(GXRenderModeObj *, const u8 *);

s32 GetVIWidthMax();
s32 GetVIHeightMax();

} // namespace JDrama