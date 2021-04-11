#pragma once

#include "GX.h"
#include "J3D/J3DDrawBuffer.hxx"
#include "JGeometry.hxx"
#include "JKR/JKRArchivePri.hxx"
#include "JSU/JSUMemoryStream.hxx"
#include "JStage.hxx"
#include "types.h"

class TCamConnector;

namespace JDrama {

class TNameRef {
public:
  TNameRef(const char *);
  virtual ~TNameRef();

  virtual u32 getType() const;
  virtual void load(JSUMemoryInputStream &stream);
  virtual void save(JSUMemoryOutputStream &stream);
  virtual void loadAfter();
  virtual TNameRef *searchF(u16, const char *);

  void genObject(JSUMemoryInputStream &stream, JSUMemoryInputStream &);
  u32 getType(JSUMemoryInputStream &stream, JSUMemoryInputStream &);
  void search(const char *);

  static s16 calcKeyCode(const char *);

  char *mTypeName;
  u16 mKeyCode;
};

class TNameRefGen {
public:
  virtual void load(JSUMemoryInputStream &stream);
  virtual TNameRef *getNameRef(const char *) const;

  TNameRef *getRootNameRef();
  static TNameRefGen *getInstance();

private:
  TNameRef *mRootName;
};

template <typename T> class TFlagT {
public:
  TFlagT(T);
  TFlagT(const TFlagT &);
  void set(T);

private:
  T mFlag;
};

struct TRect {
  u32 x; // 00
  u32 y; // 04
  u32 h; // 08
  u32 w; // 0C
};

class TGraphics {
public:
  void setViewport(const TRect &, f32, f32);

  u8 _0[0x20];
  u8 _20;
  u8 _21[0x54 - 0x21];
  u32 _54;
  u32 _58;
  u32 _5C;
  u32 _60;
};

class TViewObj : public TNameRef {
public:
  TViewObj(const char *);
  virtual ~TViewObj();

  virtual void perform(u32, TGraphics *) = 0;

  void testPerform(u32, TGraphics *);

  TFlagT<u16> mFlags; // 0x000C
};

class TPlacement : public TViewObj {
public:
  virtual ~TPlacement();

  virtual void load(JSUMemoryInputStream &);

private:
  JGeometry::TVec3<f32> mPosition;

public:
  TFlagT<u16> mFlags;
};

class TLightMap : public TViewObj {
public:
  class TLightInfo {
  public:
    TLightInfo();

    u32 _0;
    u32 _4;
  };

  virtual ~TLightMap();

  virtual void load(JSUMemoryInputStream &stream);
  virtual void perform(u32, TGraphics *);

  s32 mLightArryCount;                   // 10
  TLightMap::TLightInfo *mLightInfoArry; // 14
};

class TActor : public TPlacement, public JStage::TActor {
public:
  TActor(const char *);
  virtual ~TActor();

  virtual u32 getType() const override;
  virtual void load(JSUMemoryInputStream &) override;
  virtual void perform(u32, TGraphics *) override;
  virtual void JSGGetTranslation(Vec *) const override;
  virtual void JSGSetTranslation(const Vec &) override;
  virtual void JSGGetScaling(Vec *) const override;
  virtual void JSGSetScaling(const Vec &) override;
  virtual void JSGGetRotation(Vec *) const override;
  virtual void JSGSetRotation(const Vec &) override;

private:
  JGeometry::TVec3<f32> mSize;     // 24
  JGeometry::TVec3<f32> mRotation; // 30
  u32 _02[0x8 / 4];                // 3C
};

class TDirector : public TNameRef, public JStage::TSystem {
public:
  virtual ~TDirector();

  virtual TDirector *searchF(u16, const char *);
  virtual JStage::TObject *
  JSGFindObject(const char *, JStage::TEObject) const; // JStage::TEObject
  virtual s32 direct();

  u32 *_10;
  u32 *_14; // padding?
  u32 _18;  // ^^
  u32 _1C;  // ^^
  u32 _20;
};

class TCharacter : public TNameRef {
public:
  virtual ~TCharacter();
};

template <typename T, typename U> class TNameRefPtrListT : public TNameRef {
public:
  virtual ~TNameRefPtrListT();

  virtual void load(JSUMemoryInputStream &stream);
  virtual void loadAfter();
  T *searchF(u16, const char *);
};

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

class TDisplay {
public:
  TDisplay(u16, void *, void *, const GXRenderModeObj &);

  void startRendering();
  void endRendering();
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