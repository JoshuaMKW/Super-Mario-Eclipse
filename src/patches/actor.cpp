#include "SME.hxx"
#include "defines.h"
#include "macros.h"

#if defined(SME_BUGFIXES) || defined(SME_CRASHFIXES)

static SME_PURE_ASM void shadowCrashPatch() {
  asm volatile("cmpwi       4, 0            \n\t"
               "li          0, 0            \n\t"
               "beqlr-                      \n\t"
               "lhz         0, 0x18 (4)     \n\t"
               "blr                         \n\t");
}
SME_PATCH_BL(SME_PORT_REGION(0x802320E0, 0, 0, 0), shadowCrashPatch);

static u32 clampRotation(TLiveActor *actor) {
  JGeometry::TVec3<f32> &rot = actor->mRotation;

  auto clampPreserve = [](f32 rotation) {
    if (rotation > 360.0f)
      rotation -= 360.0f;
    else if (rotation < -360.0f)
      rotation += 360.0f;
    return rotation;
  };

  rot.x = clampPreserve(rot.x);
  rot.y = clampPreserve(rot.y);
  rot.z = clampPreserve(rot.z);

  return actor->mStateFlags.asU32;
}
SME_PATCH_BL(SME_PORT_REGION(0x80217EDC, 0, 0, 0), clampRotation);
SME_WRITE_32(SME_PORT_REGION(0x80217EE0, 0, 0, 0), 0x70600201);

static void touchEnemy__item(THitActor *touched) {
  //...
}

// Make enemies collect coins
// SME_WRITE_32(SME_PORT_REGION(0x803CA494, 0, 0, 0), 0x801bf2c0);

#endif