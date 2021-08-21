#ifndef GX_H
#define GX_H

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _GXRenderModeObj {
  u32 mTVMode;
  u16 mFBWidth;
  u16 mEFBHeight;
  u16 mExternFBHeight;
  u16 mVIOrigX;
  u16 mVIOrigY;
  u16 mVIWidth;
  u16 mVIHeight;
  u32 mExternalFrameBufferMode;
  u8 mIsFieldRendering;
  u8 mIsAntiAliasing;
  u8 mSamplePattern[12][2];
  u8 mVFilter[7];
} GXRenderModeObj;

typedef struct _GXColor {
  u8 r;
  u8 g;
  u8 b;
  u8 a;
} GXColor;

typedef struct _GXColorS10 {
  s16 r;
  s16 g;
  s16 b;
  s16 a;
} GXColorS10;

typedef enum _GXTevRegID {
  GX_TEV_PREV,
  GX_TEV_REG0,
  GX_TEV_REG1,
  GX_TEV_REG2,
  GX_MAX_TEV_REG
} GXTevRegID;

typedef enum _GXTevKColorID {
  GX_KCOLOR0,
  GX_KCOLOR1,
  GX_KCOLOR2,
  GX_KCOLOR3,
  GX_MAX_KCOLOR
} GXTevKColorID;

typedef enum _GXChannelID {
  GX_COLOR0,
  GX_COLOR1,
  GX_ALPHA0,
  GX_ALPHA1,
  GX_COLOR0A0,
  GX_COLOR1A1,
  GX_COLOR_ZERO,
  GX_ALPHA_BUMP,
  GX_ALPHA_BUMPN,
  GX_COLOR_NULL = 0xFF
} GXChannelID;

typedef enum _GXCullback {
  GX_CULL_NONE,
  GX_CULL_FRONT,
  GX_CULL_BACK,
  GX_CULL_ALL
} GXCullback;

void GXSetArray(u32 type, u32 idx, u32 count);
void GXDrawCube();
void GXDrawCubeFace();
void GXDrawSphere(u8 majorCnt, u8 minorCnt);
void GXDrawDone();
void GXSetNumChans(u32);
void GXSetNumTevStages(u32);
void GXSetNumTexGens(u32);
void GXSetTevOrder(u32, u32, u32, u32);
void GXSetTevOp(u32, u32);
void GXSetBlendMode(u32, u32, u32, u32);
void GXSetVtxAttrFmt(u32 fmt, u32 type, u32, u32, u32);
void GXClearVtxDesc();
void GXSetVtxDesc(u32 type, u32 desc);
void GXSetTevColorIn(u32, u32, u32, u32, u32);
void GXSetTevAlphaIn(u32, u32, u32, u32, u32);
void GXSetTevColorOp(u32, u32, u32, u32, u32, u32);
void GXSetTevAlphaOp(u32, u32, u32, u32, u32, u32);
void GXSetTevColor(u32, u32);
void GXSetChanMatColor(s32, u32);
void GXBegin(u32 type, u32 idx, u32 count);
void GXInitTexObjLOD(u32, u32, u32, u32, u32, u32, f32, f32, f32);
void GXCallDisplayList(u32, u32);
void GXSetNumIndStages(u32);
void GXSetTevDirect(u32);
void GXSetTexCoordGen2(u32, u32, u32, u32, u32, u32);
void GXSetViewport(f32 xPos, f32 yPos, f32 width, f32 height, f32 nearZ,
                   f32 farZ);
void GXSetViewportJitter(f32 xPos, f32 yPos, f32 width, f32 height, f32 nearZ,
                         f32 farZ, u32 field);
void GXSetScissor(s32 x1, s32 y1, s32 width, s32 height);

#ifdef __cplusplus
}
#endif

#endif