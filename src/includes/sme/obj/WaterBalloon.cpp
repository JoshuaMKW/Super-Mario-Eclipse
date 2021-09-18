#include "WaterBalloon.hxx"
#include "sms/mapobj/MapObjGeneral.hxx"
#include "GX.h"
#include "types.h"

#include "SME.hxx"

TWaterBalloon::TWaterBalloon(const char *name) : TMapObjGeneral(name) {}

TWaterBalloon::~TWaterBalloon() {}

void TWaterBalloon::perform(u32 flags, JDrama::TGraphics *graphics) {
  Mtx matrix;
  PSMTXScale(matrix, mSize.x, mSize.y, mSize.z);
  PSMTXTrans(matrix, mPosition.x, mPosition.y, mPosition.z);

  GXClearVtxDesc();

  GXSetVtxDesc(GX_VA_POS, GX_INDEX8);
  GXSetVtxDesc(GX_VA_CLR0, GX_INDEX8);

  GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
  GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);

  GXSetCullMode(GX_CULL_BACK);
  GXSetNumChans(1);

  GXSetChanCtrl(4, 0, 0, 0, 1, 2, 2);
  GXSetChanCtrl(5, 0, 0, 0, 0, 0, 2);

  GXSetChanMatColor(4, GXColor{.r = 127, .g = 20, .b = 127});

  GXSetNumTexGens(0);
  GXSetCurrentMtx(0);

  GXLoadPosMtxImm(matrix, 0);
  GXLoadNrmMtxImm(matrix, 0);

  GXSetNumTevStages(1);
  GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORDNULL, GX_TEXMAP_NULL, GX_COLOR0);

  GXSetTevOp(GX_TEVSTAGE0, 4);

  GXSetZCompLoc(GX_TRUE);
  GXSetZMode(GX_TRUE, GX_ZC_LINEAR, GX_TRUE);

  GXSetAlphaCompare(GX_ALPHA_BUMP, 0, 1, GX_ALPHA_BUMP, 0);
  GXSetBlendMode(GX_BM_BLEND, 1, 0, 5);

  GXDrawSphere(8, 8);
}


bool TWaterBalloon::receiveMessage(THitActor *actor, u32 message) {
  return TMapObjGeneral::receiveMessage(actor, message);
}

void TWaterBalloon::control() { TMapObjGeneral::control(); }

void TWaterBalloon::kill() { TMapObjGeneral::kill(); }

void TWaterBalloon::appear() { TMapObjGeneral::appear(); }

void TWaterBalloon::touchActor(THitActor *actor) { blast(); }

s32 TWaterBalloon::getLivingTime() const { return -1; }
s32 TWaterBalloon::getFlushTime() const { return -1; }

void TWaterBalloon::hold(TTakeActor *actor) { TMapObjGeneral::hold(actor); }
void TWaterBalloon::put() { TMapObjGeneral::put(); }

void TWaterBalloon::thrown() {
  constexpr f32 YSpeed = 30.0f;
  constexpr f32 FSpeed = 50.0f;
  SME_DEBUG_LOG("Thrown, holder = %X\n", mHolder);
  mSpeed.y = YSpeed;
  mSpeed.x = -sinf(mRotation.y) * FSpeed;
  mSpeed.z = cosf(mRotation.y) * FSpeed;

  mHolder = nullptr;
}

void TWaterBalloon::touchGround(JGeometry::TVec3<f32> *pos) { blast(); }

void TWaterBalloon::touchWall(JGeometry::TVec3<f32> *pos, TBGWallCheckRecord *record) { blast(); }

void TWaterBalloon::touchRoof(JGeometry::TVec3<f32> *pos) { blast(); }


void TWaterBalloon::blast() {
  SME_DEBUG_LOG("Blasting!\n");
  kill();
}