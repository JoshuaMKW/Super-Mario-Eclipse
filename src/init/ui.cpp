#include <Dolphin/DVD.h>

#include <SMS/GC2D/GCConsole2.hxx>
#include <SMS/Manager/FlagManager.hxx>

#include <BetterSMS/module.hxx>
#include <BetterSMS/stage.hxx>
#include <BetterSMS/libs/constmath.hxx>

using namespace BetterSMS;

// 0x8029CCB0
static void initCardColors() {
  auto *config = Stage::getStageConfiguration();

  if (!config->isCustomConfig())
    return;

  TGCConsole2 *gcConsole = gpMarDirector->mGCConsole;
  JUtility::TColor waterColor = config->mFluddWaterColor.get();

  if (config->mFluddShouldColorWater.get()) {
    gcConsole->mWaterLeftPanelColor         = waterColor;
    gcConsole->mWaterRightPanelColor.r      = lerp<u8>(0, waterColor.r, 0.8125f);
    gcConsole->mWaterRightPanelColor.g      = lerp<u8>(0, waterColor.g, 0.8125f);
    gcConsole->mWaterRightPanelColor.b      = lerp<u8>(0, waterColor.b, 0.8125f);
    gcConsole->mWaterRightPanelColor.a      = lerp<u8>(0, waterColor.a, 0.8125f);
    gcConsole->mWaterTopPanel->mColorMask.r = lerp<u8>(0, waterColor.r, 1.1875f);
    gcConsole->mWaterTopPanel->mColorMask.g = lerp<u8>(0, waterColor.g, 1.1875f);
    gcConsole->mWaterTopPanel->mColorMask.b = lerp<u8>(0, waterColor.b, 1.1875f);
    gcConsole->mWaterTopPanel->mColorMask.a = lerp<u8>(0, waterColor.a, 1.1875f);
  }
}
SMS_PATCH_B(SMS_PORT_REGION(0x8029CCB0,  0x80294B8C, 0, 0), initCardColors);

//// 0x802B57E4
//static void createUIHeap(u32 size, s32 alignment) {
//  gpMarDirector->mGame6Data = (u32 *)Memory::malloc(size, alignment);
//}
//// SME_PATCH_BL(SME_PORT_REGION(0x802B57E4, 0x802AD768, 0, 0), createUIHeap);
//
//// 0x802A72A4
//static u32 initHUDElements(char *filepath) {
//  char buffer[64];
//  s32 entrynum;
//
//  snprintf(buffer, 64, filepath,
//           TFlagManager::smInstance->Type6Flag.CustomFlags.mHUDElement);
//  entrynum = DVDConvertPathToEntrynum(buffer);
//
//  if (entrynum < 0)
//    snprintf(filepath, 64, filepath, 0);
//  else
//    strncpy(filepath, buffer, 64);
//
//  return DVDConvertPathToEntrynum(filepath);
//}
//// kmCall(0x802A72A4, &initHUDElements);
//
//static JKRMemArchive *switchArchive(char *curArchive, void *newArchive) {
//  auto *volume =
//      reinterpret_cast<JKRMemArchive *>(JKRFileLoader::getVolume(curArchive));
//  volume->unmountFixed();
//  volume->mountFixed(newArchive, JKRMemBreakFlag::UNK_0);
//  return volume;
//}
//
//// 0x80172440
//static JKRMemArchive *switchHUDOnStageLoad(char *curArchive, u32 *gameUI) {
//  char buffer[32];
//
//  if (gpApplication.mGamePad1->mButtons.mInput &
//      TMarioGamePad::EButtons::DPAD_UP)
//    TFlagManager::smInstance->Type6Flag.CustomFlags.mHUDElement = 1;
//  else if (gpApplication.mGamePad1->mButtons.mInput &
//           TMarioGamePad::EButtons::DPAD_DOWN)
//    TFlagManager::smInstance->Type6Flag.CustomFlags.mHUDElement = 0;
//
//  snprintf(buffer, 32, "/data/game_%d.arc",
//           TFlagManager::smInstance->Type6Flag.CustomFlags
//               .mHUDElement);                 //"/data/game_%d.arc"
//  strncpy(strstr(buffer, ".arc"), ".szs", 5); //".arc", ".szs"
//
//  if (DVDConvertPathToEntrynum(buffer) >= 0) {
//    Memory::free(gpMarDirector->mGame6Data);
//    *gameUI =
//        loadToMainRAM__12JKRDvdRipperFPCcPUc15JKRExpandSwitchUlP7JKRHeapQ212JKRDvdRipper15EAllocDirectionUlPi(
//            buffer, 0, 1, 0, gpMarDirector->mGame6Data, 1, 0, 0);
//  }
//
//  return switchArchive(curArchive, gameUI);
//}
//SMS_PATCH_BL(SMS_PORT_REGION(0x80172440, 0, 0, 0), &switchHUDOnStageLoad);
//
//// 0x802B57E4
//static JKRHeap *useCustomHUDHeap(u32 size, s32 alignment) {
//  return nullptr; // TGlobals::sGame6Heap;
//}
//// kmCall(0x802B57E4, &useCustomHUDHeap);