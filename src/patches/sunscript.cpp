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

#if defined(SME_BUGFIXES) || defined(SME_CRASHFIXES)

static void initBinaryNullptrPatch(TSpcBinary *binary) {
  if (binary)
    binary->init();
}
SME_PATCH_BL(SME_PORT_REGION(0x80289098, 0x80280E24, 0, 0), initBinaryNullptrPatch);

#endif