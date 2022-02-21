#include "GX.h"
#include "GX_types.h"
#include "funcs.hxx"
#include "sms/GC2D/SelectShine.hxx"
#include "sms/actor/Mario.hxx"
#include "sms/actor/Yoshi.hxx"

#include "SME.hxx"

// ------ MIRROR SUNSHINE ------ //
//    Created by JoshuaMK 2022   //
// ----------------------------- //

#if SME_MIRROR_SUNSHINE

#if 1
static float _xyz[3] = {-1.0, 1.0, 1.0};
#define INV_SCALE _xyz[0], _xyz[1], _xyz[2]
#else
#define INV_SCALE -1.0, 1.0, 1.0
#endif

static void mirrorMode(Mtx mtx, u32 perspective) {
  PSMTXScaleApply(mtx, mtx, INV_SCALE);
  GXSetProjection(mtx, perspective);
}
SME_PATCH_BL(SME_PORT_REGION(0x800233a4, 0, 0, 0), mirrorMode);

static u32 mirrorCulling(void *factory, u32 mode) {
  u32 cullMode = newCullMode__18J3DMaterialFactoryCFi(factory, mode);

  TMarDirector *director = gpMarDirector;
  if (!director)
    return cullMode;

  if (cullMode == GX_CULL_FRONT) {
    cullMode = GX_CULL_BACK;
  } else if (cullMode == GX_CULL_BACK) {
    cullMode = GX_CULL_FRONT;
  }
  return cullMode;
}
SME_PATCH_BL(SME_PORT_REGION(0x802e570c, 0, 0, 0), mirrorCulling);

static u32 mirrorCulling_v21(void *factory, u32 mode) {
  u32 cullMode = newCullMode__22J3DMaterialFactory_v21CFi(factory, mode);

  TMarDirector *director = gpMarDirector;
  if (!director)
    return cullMode;

  if (cullMode == GX_CULL_FRONT) {
    cullMode = GX_CULL_BACK;
  } else if (cullMode == GX_CULL_BACK) {
    cullMode = GX_CULL_FRONT;
  }
  return cullMode;
}
SME_PATCH_BL(SME_PORT_REGION(0x8031e7c8, 0, 0, 0), mirrorCulling_v21);

// static SME_PURE_ASM void mirrorCullingGX(u32 cullmode) {
//   asm volatile("cmpwi 3, 1                \n\t"
//                "beq __n1                  \n\t"
//                "cmpwi 3, 2                \n\t"
//                "bne __n3                  \n\t"
//                "li 3, 1                   \n\t"
//                "b __n3                    \n\t"
//                "__n1:                     \n\t"
//                "li r3, 2                  \n\t"
//                "__n3:                     \n\t"
//                "cmpwi 3, 2                \n\t"
//                "lis 12, 0x8035            \n\t"
//                "ori 12, 12, 0xE214        \n\t"
//                "mtctr 12                  \n\t"
//                "bctr                      \n\t");
// }
// SME_PATCH_B(SME_PORT_REGION(0x8035e210, 0, 0, 0), mirrorCullingGX);

static Mtx *getLightPerspectiveForEffectMtx(Mtx *dst, f32 x, f32 y, f32 n,
                                            f32 f) {
  C_MTXPerspective(*dst, x, y, n, f);
  PSMTXScaleApply(*dst, *dst, INV_SCALE);
  return dst;
}
SME_PATCH_BL(SME_PORT_REGION(0x8022ba9c, 0, 0, 0),
             getLightPerspectiveForEffectMtx);

static Mtx *invertReflections(Mtx srcA, Mtx srcB, Mtx *dst) {
  PSMTXScaleApply(srcB, srcB, INV_SCALE);
  PSMTXConcat(srcA, srcB, *dst);
  return dst;
}
SME_PATCH_BL(SME_PORT_REGION(0x80193470, 0, 0, 0), invertReflections);
SME_PATCH_BL(SME_PORT_REGION(0x80193f80, 0, 0, 0), invertReflections);
SME_PATCH_BL(SME_PORT_REGION(0x8032f608, 0, 0, 0), invertReflections);
SME_PATCH_BL(SME_PORT_REGION(0x8032f7b4, 0, 0, 0), invertReflections);

static Mtx *invertDropletReflections(Mtx *dst, f32 fovy, f32 aspect, f32 scaleS,
                                     f32 scaleT, f32 transS, f32 transT) {
  Mtx invMtx;
  C_MTXLightPerspective(*dst, fovy, aspect, scaleS, scaleT, transS, transT);

  PSMTXScale(invMtx, INV_SCALE);
  PSMTXConcat(*dst, invMtx, *dst);

  return dst;
}
SME_PATCH_BL(SME_PORT_REGION(0x8027c1f0, 0, 0, 0), invertDropletReflections);

static void invertMarioControl(JUTGamePad *controller) {
  controller->update();

  TMarDirector *director = gpMarDirector;
  if (!director || director->mCurState == 0xA)
    return;

  JUTGamePad::CStick &ctrlStick = controller->mControlStick;
  JUTGamePad::CStick &cStick = controller->mCStick;
  ctrlStick.update(-SME_PORT_REGION(*(s8 *)0x80404456, *(s8 *)0x80404456,
                                    *(s8 *)0x80404456, *(s8 *)0x80404456),
                   SME_PORT_REGION(*(s8 *)0x80404457, *(s8 *)0x80404457,
                                   *(s8 *)0x80404457, *(s8 *)0x80404457),
                   JUTGamePad::Clamped, JUTGamePad::WhichStick_ControlStick);
  cStick.update(-SME_PORT_REGION(*(s8 *)0x80404458, *(s8 *)0x80404458,
                                 *(s8 *)0x80404458, *(s8 *)0x80404458),
                SME_PORT_REGION(*(s8 *)0x80404459, *(s8 *)0x80404459,
                                *(s8 *)0x80404459, *(s8 *)0x80404459),
                JUTGamePad::Clamped, JUTGamePad::WhichStick_CStick);
}
SME_PATCH_BL(SME_PORT_REGION(0x802c8e54, 0, 0, 0), invertMarioControl);


SME_WRITE_32(SME_PORT_REGION(0x800DF008, 0, 0, 0), 0x38600001);
SME_WRITE_32(SME_PORT_REGION(0x800DF04C, 0, 0, 0), 0x38600002);
SME_WRITE_32(SME_PORT_REGION(0x80199324, 0, 0, 0), 0x38600001);
SME_WRITE_32(SME_PORT_REGION(0x801A01F8, 0, 0, 0), 0x38600002);
SME_WRITE_32(SME_PORT_REGION(0x801AC178, 0, 0, 0), 0x38600001);
SME_WRITE_32(SME_PORT_REGION(0x801AD534, 0, 0, 0), 0x38600001);
SME_WRITE_32(SME_PORT_REGION(0x801AE6B0, 0, 0, 0), 0x38600001);
SME_WRITE_32(SME_PORT_REGION(0x801D9340, 0, 0, 0), 0x38600001);
SME_WRITE_32(SME_PORT_REGION(0x801DA778, 0, 0, 0), 0x38600001);
SME_WRITE_32(SME_PORT_REGION(0x801AE6B0, 0, 0, 0), 0x38600001);
SME_WRITE_32(SME_PORT_REGION(0x801F3814, 0, 0, 0), 0x38600001);
SME_WRITE_32(SME_PORT_REGION(0x801F4334, 0, 0, 0), 0x38600001);
SME_WRITE_32(SME_PORT_REGION(0x8022D294, 0, 0, 0), 0x38600001);
SME_WRITE_32(SME_PORT_REGION(0x8022D700, 0, 0, 0), 0x38600001);
SME_WRITE_32(SME_PORT_REGION(0x8022F294, 0, 0, 0), 0x38600001);
SME_WRITE_32(SME_PORT_REGION(0x8022F314, 0, 0, 0), 0x38600002);
SME_WRITE_32(SME_PORT_REGION(0x8022F368, 0, 0, 0), 0x38600001);
SME_WRITE_32(SME_PORT_REGION(0x8022F710, 0, 0, 0), 0x38600001);
SME_WRITE_32(SME_PORT_REGION(0x8022F740, 0, 0, 0), 0x38600002);
SME_WRITE_32(SME_PORT_REGION(0x8022F7C0, 0, 0, 0), 0x38600002);
SME_WRITE_32(SME_PORT_REGION(0x8022F8F0, 0, 0, 0), 0x38600002);
SME_WRITE_32(SME_PORT_REGION(0x80244288, 0, 0, 0), 0x38600001);
SME_WRITE_32(SME_PORT_REGION(0x8027CA80, 0, 0, 0), 0x38600002);
SME_WRITE_32(SME_PORT_REGION(0x8027CAA8, 0, 0, 0), 0x38600001);
SME_WRITE_32(SME_PORT_REGION(0x8027D5A4, 0, 0, 0), 0x38600001);
SME_WRITE_32(SME_PORT_REGION(0x8027D5CC, 0, 0, 0), 0x38600002);
SME_WRITE_32(SME_PORT_REGION(0x8027D660, 0, 0, 0), 0x38600001);
SME_WRITE_32(SME_PORT_REGION(0x8027D688, 0, 0, 0), 0x38600002);
SME_WRITE_32(SME_PORT_REGION(0x8027D72C, 0, 0, 0), 0x38600002);
SME_WRITE_32(SME_PORT_REGION(0x8027D824, 0, 0, 0), 0x38600002);
SME_WRITE_32(SME_PORT_REGION(0x8027D8F8, 0, 0, 0), 0x38600001);
SME_WRITE_32(SME_PORT_REGION(0x8027DC08, 0, 0, 0), 0x38600001);
SME_WRITE_32(SME_PORT_REGION(0x8027DE80, 0, 0, 0), 0x38600001);
SME_WRITE_32(SME_PORT_REGION(0x8027DFBC, 0, 0, 0), 0x38600002);

#endif