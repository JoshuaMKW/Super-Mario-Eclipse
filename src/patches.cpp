#include "GX.h"
#include "OS.h"

#include "sms/GC2D/ConsoleStr.hxx"
#include "sms/JSystem/J2D/J2DPrint.hxx"
#include "sms/JSystem/JKR/JKRFileLoader.hxx"
#include "sms/actor/Mario.hxx"
#include "sms/mapobj/MapObjTree.hxx"


#include "SME.hxx"
#include "defines.h"
#include "macros.h"


constexpr f32 DrawDistance = 300000.0f * 100.0f;

// extern -> SME.cpp
// 0x802320E0
SME_PURE_ASM void SME::Patch::Fixes::shadowCrashPatch() {
  asm volatile("cmpwi       4, 0            \n\t"
               "li          0, 0            \n\t"
               "beqlr-                      \n\t"
               "lhz         0, 0x18 (4)     \n\t"
               "blr                         \n\t");
}

// extern -> SME.cpp
// 0x802571F0
u32 SME::Patch::Fixes::patchYStorage() {
  TMario *player;
  SME_FROM_GPR(31, player);

  if (player->mState != static_cast<u32>(TMario::State::IDLE))
    player->mSpeed.y = 0.0f;

  return 0;
}

static bool canDiePlane(f32 floorY) {
  TMario *player;
  SME_FROM_GPR(31, player);

  Vec playerPos;
  player->JSGGetTranslation(&playerPos);

  return (floorY > playerPos.y) && !player->mAttributes.mIsGameOver;
}
SME_PATCH_BL(SME_PORT_REGION(0x8024FB54, 0, 0, 0), canDiePlane);
SME_WRITE_32(SME_PORT_REGION(0x8024FB58, 0, 0, 0), 0x2C030000);
SME_WRITE_32(SME_PORT_REGION(0x8024FB5C, 0, 0, 0), 0x41820084);

// make tree leaf count dynamic, based on number of leaf col files
static TMapObjTree *getLeafCount(TMapObjTree *tree) {
  char cacheBuffer[128];
  char buffer[128];

  snprintf(cacheBuffer, 96, "/scene/mapObj/%sLeaf", tree->mRegisterName);
  strcat(cacheBuffer, "%02d.col");

  s32 num = 0;
  while (true && num < 255) {
    snprintf(buffer, 100, cacheBuffer, num + 1);
    if (!JKRFileLoader::getGlbResource(buffer)) {
      tree->mLeafCount = num;
      return tree;
    }
    num += 1;
  }
  tree->mLeafCount = 0;
  return tree;
}
SME_PATCH_B(SME_PORT_REGION(0x801F6AE4, 0, 0, 0), getLeafCount);
SME_PATCH_B(SME_PORT_REGION(0x801F6B20, 0, 0, 0), getLeafCount);
SME_PATCH_B(SME_PORT_REGION(0x801F6B5C, 0, 0, 0), getLeafCount);
SME_PATCH_B(SME_PORT_REGION(0x801F6B98, 0, 0, 0), getLeafCount);
SME_PATCH_B(SME_PORT_REGION(0x801F6BD4, 0, 0, 0), getLeafCount);

static SME_PURE_ASM bool makeWaterHitCheckForDeath(TBGCheckData *col) {
  // clang-format off
  asm volatile (
    "lhz 0, 0 (3)             \n\t"
    "cmpwi 0, 2048            \n\t"
    "bne .makeWaterCheck_tmp0 \n\t"
    "li 0, 1025               \n\t"
    ".makeWaterCheck_tmp0:    \n\t"
    SME_PORT_REGION (
      "lis 12, 0x8018           \n\t"
      "ori 12, 12, 0xC36C       \n\t",

      "lis 12, 0           \n\t"
      "ori 12, 12, 0       \n\t",

      "lis 12, 0           \n\t"
      "ori 12, 12, 0       \n\t",

      "lis 12, 0           \n\t"
      "ori 12, 12, 0       \n\t"
    )
    "mtctr 12                 \n\t"
    "bctr                     \n\t"
  );
  // clang-format on
}
SME_PATCH_B(SME_PORT_REGION(0x8018C368, 0, 0, 0), makeWaterHitCheckForDeath);

// Make illegal data not downwarp anymore
SME_WRITE_32(SME_PORT_REGION(0x8018D08C, 0, 0, 0), 0x60000000);

static void patchRideMovementUpWarp(Mtx out, Vec *ride, Vec *pos) {
  TMario *player;
  SME_FROM_GPR(30, player);

  if (!(player->mState & static_cast<u32>(TMario::State::AIRBORN))) {
    PSMTXMultVec(out, ride, pos);
  }
}
// SME_PATCH_BL(SME_PORT_REGION(0x80250514, 0, 0, 0), patchRideMovementUpWarp);

static void initBinaryNullptrPatch(TSpcBinary *binary) {
  if (binary)
    binary->init();
}
SME_PATCH_BL(SME_PORT_REGION(0x80289098, 0, 0, 0), initBinaryNullptrPatch);

static void scaleDrawDistanceNPC(f32 x, f32 y, f32 near, f32 far) {
  SetViewFrustumClipCheckPerspective__Fffff(x, y, near, far * 2);
}
SME_PATCH_BL(SME_PORT_REGION(0x8020A2A4, 0, 0, 0), scaleDrawDistanceNPC);

static f32 sLastFactor = 1.0f;
static bool cameraQOLFixes(CPolarSubCamera *cam) {
  JSGSetProjectionFar__Q26JDrama7TCameraFf(cam, DrawDistance); // Draw Distance

  const f32 factor = Math::scaleLinearAtAnchor(gpMarioAddress->mForwardSpeed / 100.0f, 0.5f, 1.0f);
  sLastFactor = Max(sLastFactor - 0.01f, 1.0f);

  if (factor > 1.0f && gpMarioAddress->mState == static_cast<u32>(TMario::State::DIVESLIDE)) {
    sLastFactor = factor;
    reinterpret_cast<f32 *>(cam)[0x48 / 4] *= factor;
  } else {
    reinterpret_cast<f32 *>(cam)[0x48 / 4] *= sLastFactor;
  }
  return cam->isNormalDeadDemo();
}
SME_PATCH_BL(SME_PORT_REGION(0x80023828, 0, 0, 0), cameraQOLFixes);

// READY GO TEXT PATCH FOR THIS BULLSHIT THING DADDY NINTENDO DID
SME_WRITE_32(SME_PORT_REGION(0x80171C30, 0, 0, 0), 0x2C000005);
SME_WRITE_32(SME_PORT_REGION(0x80171C38, 0, 0, 0), 0x38000005);

static void normalizeHoverSlopeSpeed(f32 floorPos) {
  TMario *player;
  SME_FROM_GPR(22, player);

  player->mPosition.y = floorPos;

  if (!(player->mState == static_cast<u32>(TMario::State::HOVER)))
    return;

  const f32 playerRotY = f32(player->mAngle.y) / 182.0f;
  const Vec playerForward = {sinf(Math::angleToRadians(-playerRotY)), 0.0f,
                             cosf(Math::angleToRadians(playerRotY))};
  const Vec up = {0.0f, 1.0f, 0.0f};

  Vec floorNormal;
  PSVECNormalize(reinterpret_cast<Vec *>(player->mFloorTriangle->getNormal()),
                 &floorNormal);

  const f32 slopeStrength = PSVECDotProduct(&up, &floorNormal);
  if (slopeStrength < 0.0f)
    return;

  const f32 lookAtRatio =
      Math::Vector3::lookAtRatio(playerForward, floorNormal);

  player->mForwardSpeed =
      Min(player->mForwardSpeed,
          10.0f * Math::clamp(Math::scaleLinearAtAnchor(slopeStrength,
                                                        lookAtRatio, 1.0f),
                              0.0f, 1.0f));
}
SME_PATCH_BL(SME_PORT_REGION(0x802568F0, 0, 0, 0), normalizeHoverSlopeSpeed);

static void touchEnemy__item(THitActor *touched) {
  //...
}

// Make enemies collect coins
// SME_WRITE_32(SME_PORT_REGION(0x803CA494, 0, 0, 0), 0x801bf2c0);

#if 0
static u8 sLineBuffer[sizeof(String) * 64];
static JKRSolidHeap sLineHeap(&sLineBuffer, 64, JKRHeap::sRootHeap, false);

static SME_NO_INLINE size_t getSplitLines(const char *str, String **out, size_t maxLines = __UINT32_MAX__) {
  String string(str, 1024);

  size_t nlinePos = string.find('\n', 0);
  size_t nlineLast = String::npos;
  size_t numLines = 0;
  do {
    out[numLines] = new (&sLineHeap, 4) String(nlineLast+1 + nlinePos);
    string.substr(out[numLines], nlineLast+1, nlinePos == String::npos ? String::npos : nlinePos + 1);
    numLines += 1;
  } while (nlinePos != String::npos && numLines < maxLines-1);

  return numLines;
}

static int sPrintX = 0, sPrintY = 0;

static void _capturePrintPos(J2DPane *pane, int x, int y) {
  sPrintX = x;
  sPrintY = y;
  pane->makeMatrix(x, y);
}
SME_PATCH_BL(SME_PORT_REGION(0x802d0bec, 0, 0, 0), _capturePrintPos);

static void cullJ2DPrint(J2DPrint *printer, int unk_0, int unk_1, u8 unk_2, const char *formatter, ...) {
  constexpr int fontWidth = 20;

  va_list vargs;
  va_start(vargs, formatter);

  char *msg = va_arg(vargs, char *);

  va_end(vargs);

  if (sPrintX > 0 && (sPrintX + strlen(msg)) < SME::TGlobals::getScreenWidth())
    printer->print(0, 0, unk_2, formatter, msg);

  String finalStr(1024);

  String *lines[64];
  size_t numLines = getSplitLines(msg, lines, 64);

  for (u32 i = 0; i < numLines; ++i) {
    String *line = lines[i];
    size_t cullL = Max((-sPrintX / fontWidth) - 2, 0);
    size_t cullR = ((-sPrintX + (line->size() * fontWidth)) / SME::TGlobals::getScreenWidth()) + 2;

    line->substr(const_cast<char *>(finalStr.data()) + finalStr.end(), cullL, cullR);
  }

  printer->print(0, 0, unk_2, formatter, finalStr.data());
}
SME_PATCH_BL(SME_PORT_REGION(0x802d0c20, 0, 0, 0), cullJ2DPrint);

#else

static int sPrintX = 0, sPrintY = 0;

static void _capturePrintPos(J2DPane *pane, int x, int y) {
  sPrintX = x;
  sPrintY = y;
  pane->makeMatrix(x, y);
}
// SME_PATCH_BL(SME_PORT_REGION(0x802d0bec, 0, 0, 0), _capturePrintPos);

static void cullJ2DPrint(J2DPrint *printer, int unk_0, int unk_1, u8 unk_2,
                         const char *formatter, ...) {
  constexpr f32 fontWidth = 16;

  va_list vargs;
  va_start(vargs, formatter);

  char *msg = va_arg(vargs, char *);

  va_end(vargs);

  if ((sPrintX > 0 &&
       (sPrintX + strlen(msg)) < SME::TGlobals::getScreenWidth()) ||
      strchr(msg, '\n') != nullptr)
    printer->print(0, 0, unk_2, formatter, msg);

  String culledMsg(msg, 1024);

  f32 cullL = Max((f32(-sPrintX) / fontWidth) - 2.0f, 0);
  f32 cullR = Max(((f32(-sPrintX) + (f32(culledMsg.size()) * fontWidth)) /
                   SME::TGlobals::getScreenWidth()) +
                      2.0f,
                  cullL);

  culledMsg.substr(&culledMsg, size_t(cullL), size_t(cullR));
  culledMsg.append('\0');

  printer->print(int(cullL * fontWidth), 0, unk_2, formatter, culledMsg.data());
}
// SME_PATCH_BL(SME_PORT_REGION(0x802d0c20, 0, 0, 0), cullJ2DPrint);

static Mtx44 sDrawMtx;

static void captureTextboxDrawMtx(Mtx44 mtx, u8 index) {
  PSMTXCopy(mtx, sDrawMtx);
  GXLoadPosMtxImm(mtx, index);
}
SME_PATCH_BL(SME_PORT_REGION(0x802d0bf8, 0, 0, 0), captureTextboxDrawMtx);

static void maybePrintChar(JUTFont *font, f32 x, f32 y, f32 w, f32 h, int ascii,
                           bool unk_1) {
  const int offset = static_cast<int>(
      (SME::TGlobals::getScreenToFullScreenRatio() - 1.0f) * 600.0f);
  const int fontWidth = font->getWidth();

  int absX = static_cast<int>(sDrawMtx[0][3] + x);
  int absY = static_cast<int>(sDrawMtx[1][3] + y);

  if (absX + fontWidth > -offset && absX < SME::TGlobals::getScreenWidth())
    font->drawChar_scale(x, y, w, h, ascii, unk_1);
}
SME_PATCH_BL(SME_PORT_REGION(0x802cec2c, 0, 0, 0), maybePrintChar);

static OSStopwatch stopwatch;
static bool sInitialized = false;
static bool sIsWaiting = false;
static OSTick sLastStart = 0;
static void J2D_BenchMarkPrint(J2DTextBox *printer, int x, int y) {
  if (!sInitialized) {
    OSInitStopwatch(&stopwatch, "J2DPrintTest");
    sInitialized = true;
  }

  if (!sIsWaiting) {
    sLastStart = OSGetTick();
    sIsWaiting = true;
  }

  OSStartStopwatch(&stopwatch);
  printer->draw(x, y);
  OSStopStopwatch(&stopwatch);

  if (sIsWaiting && OSTicksToSeconds(OSGetTick() - sLastStart) > 5.0f) {
    OSDumpStopwatch(&stopwatch);
  }
}
// SME_PATCH_BL(SME_PORT_REGION(0x80144010, 0, 0, 0), J2D_BenchMarkPrint);

#endif

// Title Screen Never Fades to THP
SME_WRITE_32(SME_PORT_REGION(0x8016D53C, 0, 0, 0), 0x48000344);

// Load msound.aaf from AudioRes folder (NTSC-U) [Xayrga/JoshuaMK]
SME_WRITE_32(SME_PORT_REGION(0x80014F9C, 0, 0, 0), 0x60000000);

static bool sIs100ShineSpawned = false;
static bool is100CoinShine(TFlagManager *manager, u32 id) {
  if (!sIs100ShineSpawned && manager->getFlag(id) > 100) {
    sIs100ShineSpawned = true;
    return true;
  }
  return false;
}
SME_PATCH_BL(SME_PORT_REGION(0x801BED3C, 0, 0, 0), is100CoinShine);
SME_WRITE_32(SME_PORT_REGION(0x801BED40, 0, 0, 0), 0x2C030001);

void patches_staticResetter() {
  sIs100ShineSpawned = false;
}