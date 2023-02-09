#include <Dolphin/printf.h>
#include <Dolphin/stdlib.h>
#include <JSystem/JDrama/JDRNameRef.hxx>
#include <JSystem/JKernel/JKRDecomp.hxx>
#include <JSystem/JKernel/JKRHeap.hxx>
#include <JSystem/JKernel/JKRMemArchive.hxx>

#include <BetterSMS/module.hxx>

using namespace BetterSMS;

extern bool gIsSwappingWarp;

//constexpr size_t charactersize = 0x1A0000;
//constexpr size_t globalsize = 0x8000;
//
//// 0x802A750C
//// extern -> SME.cpp
//static JKRExpHeap *createGlobalHeaps(void *newHeap, size_t size,
//                                     JKRHeap *rootHeap, bool unk_1) {
//#ifdef SME_DETACHED_HEAPS
//  {
//    JKRHeap *rootHeap = JKRHeap::sRootHeap;
//    JKRHeap *systemHeap = JKRHeap::sSystemHeap;
//    JKRHeap *currentHeap = JKRHeap::sCurrentHeap;
//    JKRHeap::sRootHeap = nullptr; // Hack to make this heap disconnected
//
//    void *heap = OSGetArenaHi();
//    if (charactersize > sizeof(JKRExpHeap))
//      TGlobals::sCharacterHeap = new (JKRExpHeap::sSystemHeap, 4)
//          JKRExpHeap(heap, charactersize - sizeof(JKRExpHeap), nullptr, false);
//    if (globalsize > sizeof(JKRExpHeap))
//      TGlobals::sGlobalHeap = new (JKRExpHeap::sSystemHeap, 4)
//          JKRExpHeap(heap, globalsize - sizeof(JKRExpHeap), nullptr, false);
//
//    currentHeap->becomeCurrentHeap();
//    systemHeap->becomeSystemHeap();
//
//    // Restore heap pointers
//    JKRHeap::sRootHeap = rootHeap;
//  }
//#else
//  {
//    JKRHeap *systemHeap = JKRHeap::sSystemHeap;
//    JKRHeap *currentHeap = JKRHeap::sCurrentHeap;
//
//    if (charactersize > sizeof(JKRExpHeap))
//      TGlobals::sCharacterHeap = JKRExpHeap::create(
//          charactersize - sizeof(JKRExpHeap), JKRHeap::sRootHeap, false);
//    if (globalsize > sizeof(JKRExpHeap))
//      TGlobals::sGlobalHeap = JKRExpHeap::create(
//          globalsize - sizeof(JKRExpHeap), JKRHeap::sRootHeap, false);
//
//    SMS_ASSERT(TGlobals::sCharacterHeap,
//               "Character heap failed to allocate at size %lu\n",
//               charactersize);
//
//    TGlobals::sCharacterHeap->alloc(TGlobals::sCharacterHeap->getFreeSize(), 4);
//
//    currentHeap->becomeCurrentHeap();
//    systemHeap->becomeSystemHeap();
//  }
//  JKRExpHeap *gameHeap = JKRExpHeap::create(
//      newHeap, size - (globalsize + charactersize), rootHeap, unk_1);
//#endif
//  return gameHeap;
//}
//SMS_PATCH_BL(SMS_PORT_REGION(0x802A750C, 0x8029F588, 0, 0), createGlobalHeaps);
//
//// 0x802A7140
//// extern -> SME.cpp
//static s32 setupMarioDatas(char *filepath) {
//  TMarioGamePad *gpGamePad = gpApplication.mGamePad1;
//#if SME_DEMO
//  Enum::Player playerID = Enum::Player::MARIO;
//#else
//  Enum::Player playerID =
//      Util::Mario::getPlayerIDFromInput(gpGamePad->mButtons.mInput);
//#endif
//
//  TGlobals::sCharacterIDList[0] = playerID;
//
//  snprintf(filepath, 32, "/data/chr%d.szs", static_cast<u8>(playerID));
//  return DVDConvertPathToEntrynum(filepath);
//}
//SMS_PATCH_BL(SMS_PORT_REGION(0x802A7140, 0x8029F02C, 0, 0), setupMarioDatas);
//
//// 0x802A716C
//// extern -> SME.cpp
//static u32 *initFirstModel(const char *path, u32 unk_1, u32 unk_2, u32 unk_3,
//                           JKRHeap *heap,
//                           JKRDvdRipper::EAllocDirection direction, u32 unk_5,
//                           u32 unk_6) {
//
//  if (!TGlobals::sCharacterHeap) {
//    SME_LOG("Shouldn't reach here, character heap is nullptr?\n");
//  }
//#ifndef SME_DETACHED_HEAPS
//  TGlobals::sCharacterHeap->freeAll();
//#endif
//  u32 *archive = static_cast<u32 *>(
//      Util::loadArchive(path, TGlobals::sCharacterHeap, direction));
//#ifndef SME_DETACHED_HEAPS
//  TGlobals::sCharacterHeap->alloc(TGlobals::sCharacterHeap->getFreeSize(), 4);
//#endif
//  return archive;
//}
//SMS_PATCH_BL(SMS_PORT_REGION(0x802A716C, 0x8029F058, 0, 0), initFirstModel);

//// 0x802998B4
//void initFileMods(TMarDirector *director) {
//    TMarioGamePad *gpGamePad = gpApplication.mGamePad1;
//
//    TFlagManager::smInstance->setBool(true, 0x1038F); // Yosh
//    TFlagManager::smInstance->Type4Flag.mGoldCoinCount = 0;
//
//    OSReport("Hot swapping character model data...\n");
//    Util::Mario::swapBinary(characterID);
//
//    OSReport("Loading character specific params...\n");
//    Util::Mario::loadParams();
//
//    return director;
//}
//SMS_PATCH_BL(SMS_PORT_REGION(0x802998B4, 0x8029174C, 0, 0), initFileMods);