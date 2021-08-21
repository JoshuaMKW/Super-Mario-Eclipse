#include "GX.h"

#include "sms/GC2D/ConsoleStr.hxx"
#include "sms/JSystem/JKR/JKRFileLoader.hxx"
#include "sms/actor/Mario.hxx"
#include "sms/mapobj/MapObjTree.hxx"


#include "SME.hxx"
#include "macros.h"

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

static u32 fixShootingStarsNoDelete() {
  u32 *emitterManager4D2 = *(u32 **)SME_PORT_REGION(0x8040E1E4, 0, 0, 0);
  TConsoleStr *consoleStr;
  SME_FROM_GPR(31, consoleStr);

  for (s32 i = 2; i >= 0; --i) {
    deleteEmitter__17JPAEmitterManagerFP14JPABaseEmitter(
        emitterManager4D2, consoleStr->mJPABaseEmitters[i]);
    consoleStr->mJPABaseEmitters[i] = nullptr;
  }

  return 0;
}
SME_PATCH_BL(SME_PORT_REGION(0x80171314, 0, 0, 0), fixShootingStarsNoDelete);
SME_WRITE_32(SME_PORT_REGION(0x80171318, 0, 0, 0), 0x809F0028);

static void fixShootingStarsWideScreen(TBoundPane *pane, u32 size,
                                       JUTPoint &begin,
                                       JUTPoint &mid,
                                       JUTPoint &end) {
  const f32 ratio = SME::TGlobals::getScreenToFullScreenRatio() * 1.2f;

  mid.x *= ratio;
  end.x *= ratio;

  pane->setPanePosition(size, begin, mid, end);
}
SME_PATCH_BL(SME_PORT_REGION(0x80170EFC, 0, 0, 0), fixShootingStarsWideScreen);
SME_PATCH_BL(SME_PORT_REGION(0x80170F34, 0, 0, 0), fixShootingStarsWideScreen);
SME_PATCH_BL(SME_PORT_REGION(0x80170F6C, 0, 0, 0), fixShootingStarsWideScreen);

static void fixDemoMasksWideScreen(TConsoleStr *consoleStr) {
  loadAfter__Q26JDrama8TNameRefFv(consoleStr);

  const f32 ratio = SME::TGlobals::getScreenToFullScreenRatio();

  for (u32 i = 0; i < 2; ++i) {
    JUTRect &rect = consoleStr->mDemoWipeExPanes[i]->mPane->mRect;
    rect.mX1 = static_cast<s32>(-((ratio-1) * 600.0f));
    rect.mX2 = static_cast<s32>(SME::TGlobals::getScreenWidth());

    consoleStr->mDemoWipeExPanes[i]->mRect.copy(rect);
  }

  JUTRect *rect = &consoleStr->mDemoMaskExPanes[0]->mPane->mRect;
  rect->mX1 = static_cast<s32>(-((ratio-1) * 600.0f));

  consoleStr->mDemoMaskExPanes[0]->mRect.copy(*rect);

  rect = &consoleStr->mDemoMaskExPanes[1]->mPane->mRect;
  rect->mX2 = static_cast<s32>(SME::TGlobals::getScreenWidth());

  consoleStr->mDemoMaskExPanes[1]->mRect.copy(*rect);
}
SME_PATCH_BL(SME_PORT_REGION(0x801723F0, 0, 0, 0), fixDemoMasksWideScreen);

static JUTRect sGuideBorderRects[2];
static J2DPane sGuideBorderPanes[2];

static void fixGuideWideScreenOpen(TSMSFader *fader, u32 type, f32 time, f32 delay) {
  fader->startWipe(type, time, delay);

  TConsoleStr *consoleStr = gpMarDirector->mGCConsole->mConsoleStr;
  const f32 ratio = SME::TGlobals::getScreenToFullScreenRatio();

  JUTRect *rect = &consoleStr->mDemoMaskExPanes[0]->mPane->mRect;
  J2DPane *pane = consoleStr->mDemoMaskExPanes[0]->mPane;

  sGuideBorderRects[0] = *rect;
  sGuideBorderPanes[0] = *pane;
  
  rect->mX1 = static_cast<s32>(-((ratio-1) * 600.0f));
  rect->mX2 = 0;
  pane->mAlpha = 0xFF;
  pane->mIsVisible = true;

  consoleStr->mDemoMaskExPanes[0]->mRect.copy(*rect);

  rect = &consoleStr->mDemoMaskExPanes[1]->mPane->mRect;
  pane = consoleStr->mDemoMaskExPanes[1]->mPane;

  sGuideBorderRects[1] = *rect;
  sGuideBorderPanes[1] = *pane;

  rect->mX1 = 600;
  rect->mX2 = static_cast<s32>(SME::TGlobals::getScreenWidth());
  pane->mAlpha = 0xFF;
  pane->mIsVisible = true;

  consoleStr->mDemoMaskExPanes[1]->mRect.copy(*rect);
}
SME_PATCH_BL(SME_PORT_REGION(0x8017940C, 0, 0, 0), fixGuideWideScreenOpen);


static void fixGuideWideScreenClose(TSMSFader *fader, u32 type, f32 time, f32 delay) {
  fader->startWipe(type, time, delay);

  TConsoleStr *consoleStr = gpMarDirector->mGCConsole->mConsoleStr;

  JUTRect *rect = &consoleStr->mDemoMaskExPanes[0]->mRect;
  J2DPane *pane = consoleStr->mDemoMaskExPanes[0]->mPane;
  
  *rect = sGuideBorderRects[0];
  *pane = sGuideBorderPanes[0];

  rect = &consoleStr->mDemoMaskExPanes[1]->mRect;
  pane = consoleStr->mDemoMaskExPanes[1]->mPane;
  
  *rect = sGuideBorderRects[1];
  *pane = sGuideBorderPanes[1];
}
SME_PATCH_BL(SME_PORT_REGION(0x80179880, 0, 0, 0), fixGuideWideScreenClose);

static void renderGuideWideScreenFix(J2DScreen *screen, int x, int y, J2DGrafContext *context) {
  screen->draw(x, y, context);
  
  if (!SME::TGlobals::isWideScreen())
    return;

  TConsoleStr *consoleStr = gpMarDirector->mGCConsole->mConsoleStr;
  consoleStr->mOpeningDemoScreen->draw(x, y, context);
}
SME_PATCH_BL(SME_PORT_REGION(0x80179390, 0, 0, 0), renderGuideWideScreenFix);

static void fixDEBSWideScreenText(s32 x1, s32 y1, s32 width, s32 height) {
  const f32 ratio = SME::TGlobals::getScreenToFullScreenRatio();

  const s32 offset = static_cast<s32>(((ratio-1) * 45.0f));
  x1 -= offset;

  GXSetScissor(x1, y1, width, height);
}
SME_PATCH_BL(SME_PORT_REGION(0x80143FDC, 0, 0, 0), fixDEBSWideScreenText);

static void fixDEBSWideScreenPanel(TGCConsole2 *console) {
  loadAfter__Q26JDrama8TNameRefFv(console);

  const f32 ratio = SME::TGlobals::getScreenToFullScreenRatio();
  const s32 offset = static_cast<s32>(((ratio-1) * 600.0f));

  TExPane *pane = console->mTelopWindow;
  pane->mRect.mX1 -= offset;
  pane->mRect.mX2 += offset;
  pane->mPane->mRect.mX1 -= offset;
  pane->mPane->mRect.mX2 += offset;
  reinterpret_cast<J2DWindow *>(pane->mPane)->mFillRect.mX2 += offset*2;
}
SME_PATCH_BL(SME_PORT_REGION(0x8014D8A4, 0, 0, 0), fixDEBSWideScreenPanel);


// 801711dc