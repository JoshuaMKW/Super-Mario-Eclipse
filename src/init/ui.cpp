

#include "JDrama/JDRNameRef.hxx"
#include "JKR/JKRDecomp.hxx"
#include "JKR/JKRHeap.hxx"
#include "JKR/JKRMemArchive.hxx"
#include "printf.h"
#include "stdlib.h"

#include "SME.hxx"

using namespace SME;
using namespace Util;
using namespace Class;

// 0x8029CCB0
void Patch::Init::initCardColors() {
  TStageParams *config = TStageParams::sStageConfig;

  if (!config->isCustomConfig())
    return;

  TGCConsole2 *gcConsole = gpMarDirector->mGCConsole;
  JUtility::TColor waterColor = config->mFluddWaterColor.get();

  gcConsole->mJuiceCardOrangeColor = config->mYoshiColorOrange.get();
  gcConsole->mJuiceCardPurpleColor = config->mYoshiColorPurple.get();
  gcConsole->mJuiceCardPinkColor = config->mYoshiColorPink.get();

  if (config->mFluddShouldColorWater.get()) {
    gcConsole->mWaterLeftPanelColor = waterColor;
    gcConsole->mWaterRightPanelColor.r =
        Math::lerp<u8>(0, waterColor.r, 0.8125f);
    gcConsole->mWaterRightPanelColor.g =
        Math::lerp<u8>(0, waterColor.g, 0.8125f);
    gcConsole->mWaterRightPanelColor.b =
        Math::lerp<u8>(0, waterColor.b, 0.8125f);
    gcConsole->mWaterRightPanelColor.a =
        Math::lerp<u8>(0, waterColor.a, 0.8125f);
    gcConsole->mWaterTopPanel->mFillColor.r =
        Math::lerp<u8>(0, waterColor.r, 1.1875f);
    gcConsole->mWaterTopPanel->mFillColor.g =
        Math::lerp<u8>(0, waterColor.g, 1.1875f);
    gcConsole->mWaterTopPanel->mFillColor.b =
        Math::lerp<u8>(0, waterColor.b, 1.1875f);
    gcConsole->mWaterTopPanel->mFillColor.a =
        Math::lerp<u8>(0, waterColor.a, 1.1875f);
  }
}

// 0x802B57E4
void Patch::Init::createUIHeap(u32 size, s32 alignment) {
  gpMarDirector->mGame6Data = (u32 *)Memory::malloc(size, alignment);
}

// 0x802A72A4
u32 Patch::Init::initHUDElements(char *filepath) {
  char buffer[32];
  s32 entrynum;

  sprintf(buffer, filepath,
          TFlagManager::smInstance->Type6Flag.CustomFlags.mHUDElement);
  entrynum = DVDConvertPathToEntrynum(buffer);

  if (entrynum < 0)
    sprintf(filepath, filepath, 0);
  else
    strcpy(filepath, buffer);

  return DVDConvertPathToEntrynum(filepath);
}
// kmCall(0x802A72A4, &initHUDElements);

static JKRMemArchive *switchArchive(char *curArchive, void *newArchive) {
  auto *volume =
      reinterpret_cast<JKRMemArchive *>(JKRFileLoader::getVolume(curArchive));
  volume->unmountFixed();
  volume->mountFixed(newArchive, JKRMemBreakFlag::UNK_0);
  return volume;
}

// 0x80172440
JKRMemArchive *Patch::Init::switchHUDOnStageLoad(char *curArchive,
                                                 u32 *gameUI) {
  char buffer[32];

  if (gpApplication.mGamePad1->mButtons.mInput &
      TMarioGamePad::EButtons::DPAD_UP)
    TFlagManager::smInstance->Type6Flag.CustomFlags.mHUDElement = 1;
  else if (gpApplication.mGamePad1->mButtons.mInput &
           TMarioGamePad::EButtons::DPAD_DOWN)
    TFlagManager::smInstance->Type6Flag.CustomFlags.mHUDElement = 0;

  sprintf(buffer, "/data/game_%d.arc",
          TFlagManager::smInstance->Type6Flag.CustomFlags
              .mHUDElement);              //"/data/game_%d.arc"
  strcpy(strstr(buffer, ".arc"), ".szs"); //".arc", ".szs"

  if (DVDConvertPathToEntrynum(buffer) >= 0) {
    Memory::free(gpMarDirector->mGame6Data);
    *gameUI =
        loadToMainRAM__12JKRDvdRipperFPCcPUc15JKRExpandSwitchUlP7JKRHeapQ212JKRDvdRipper15EAllocDirectionUlPi(
            buffer, 0, 1, 0, gpMarDirector->mGame6Data, 1, 0, 0);
  }

  return switchArchive(curArchive, gameUI);
}
// kmCall(0x80172440, &switchHUDOnStageLoad);

// 0x802B57E4
JKRHeap *Patch::Init::useCustomHUDHeap(u32 size, s32 alignment) {
  return nullptr; // TGlobals::sGame6Heap;
}
// kmCall(0x802B57E4, &useCustomHUDHeap);