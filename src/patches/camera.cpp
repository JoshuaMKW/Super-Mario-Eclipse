#include "GX.h"
#include "OS.h"

#include "J2D/J2DPrint.hxx"
#include "JKR/JKRFileLoader.hxx"
#include "sms/GC2D/ConsoleStr.hxx"
#include "sms/actor/Mario.hxx"
#include "sms/mapobj/MapObjNormalLift.hxx"
#include "sms/mapobj/MapObjTree.hxx"

#include "SME.hxx"
#include "defines.h"
#include "macros.h"

#if SME_EXTENDED_RENDER_DISTANCE

constexpr f32 DrawDistanceMultiplier = 100.0f;
constexpr f32 DrawDistance = 300000.0f * DrawDistanceMultiplier;

static bool cameraScaleRenderDistance(CPolarSubCamera *cam) {
  JSGSetProjectionFar__Q26JDrama7TCameraFf(cam, DrawDistance); // Draw Distance
  return cam->isNormalDeadDemo();
}
SME_PATCH_BL(SME_PORT_REGION(0x80023828, 0, 0, 0), cameraScaleRenderDistance);

static void scaleDrawDistanceNPC(f32 x, f32 y, f32 near, f32 far) {
  SetViewFrustumClipCheckPerspective__Fffff(x, y, near, far * DrawDistanceMultiplier);
}
SME_PATCH_BL(SME_PORT_REGION(0x8020A2A4, 0, 0, 0), scaleDrawDistanceNPC);

#endif