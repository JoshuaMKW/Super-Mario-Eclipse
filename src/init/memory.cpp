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

extern bool gInXYZMode;
extern bool gIsSwappingWarp;

constexpr size_t charactersize = 0x1A0000;
constexpr size_t globalsize = 0x8000;

// 0x802A750C
// extern -> SME.cpp
static JKRExpHeap *createGlobalHeaps(void *newHeap, size_t size,
                                     JKRHeap *rootHeap, bool unk_1) {
  SME_LOG("test\n");
#ifdef SME_DETACHED_HEAPS
  {
    JKRHeap *rootHeap = JKRHeap::sRootHeap;
    JKRHeap *systemHeap = JKRHeap::sSystemHeap;
    JKRHeap *currentHeap = JKRHeap::sCurrentHeap;
    JKRHeap::sRootHeap = nullptr; // Hack to make this heap disconnected

    void *heap = OSGetArenaHi();
    if (charactersize > sizeof(JKRExpHeap))
      TGlobals::sCharacterHeap = new (JKRExpHeap::sSystemHeap, 4)
          JKRExpHeap(heap, charactersize - sizeof(JKRExpHeap), nullptr, false);
    if (globalsize > sizeof(JKRExpHeap))
      TGlobals::sGlobalHeap = new (JKRExpHeap::sSystemHeap, 4)
          JKRExpHeap(heap, globalsize - sizeof(JKRExpHeap), nullptr, false);

    currentHeap->becomeCurrentHeap();
    systemHeap->becomeSystemHeap();

    // Restore heap pointers
    JKRHeap::sRootHeap = rootHeap;
  }
#else
  SME_LOG("test 3\n");
  {
    JKRHeap *systemHeap = JKRHeap::sSystemHeap;
    JKRHeap *currentHeap = JKRHeap::sCurrentHeap;

    // OSReport("isDebugMode = %s\n", SME::TGlobals::isDebugMode() ? "TRUE" :
    // "FALSE");
    SME_EVAL_LOG(charactersize > sizeof(JKRExpHeap));
    if (charactersize > sizeof(JKRExpHeap))
      TGlobals::sCharacterHeap = JKRExpHeap::create(
          charactersize - sizeof(JKRExpHeap), JKRHeap::sRootHeap, false);
    if (globalsize > sizeof(JKRExpHeap))
      TGlobals::sGlobalHeap = JKRExpHeap::create(
          globalsize - sizeof(JKRExpHeap), JKRHeap::sRootHeap, false);

    SME_ASSERT(TGlobals::sCharacterHeap,
               "Character heap failed to allocate at size %lu\n",
               charactersize);

    TGlobals::sCharacterHeap->alloc(TGlobals::sCharacterHeap->getFreeSize(), 4);

    TStageParams::sStageConfig =
        new (JKRHeap::sRootHeap, 4) TStageParams(nullptr);

    currentHeap->becomeCurrentHeap();
    systemHeap->becomeSystemHeap();
  }
  JKRExpHeap *gameHeap = JKRExpHeap::create(
      newHeap, size - (globalsize + charactersize), rootHeap, unk_1);
#endif
  return gameHeap;
}
SME_PATCH_BL(SME_PORT_REGION(0x802A750C, 0, 0, 0), createGlobalHeaps);

// 0x802A7140
// extern -> SME.cpp
static s32 setupMarioDatas(char *filepath) {
  TMarioGamePad *gpGamePad = gpApplication.mGamePad1;
#if SME_DEMO
  Enum::Player playerID = Enum::Player::MARIO;
#else
  Enum::Player playerID =
      Util::Mario::getPlayerIDFromInput(gpGamePad->mButtons.mInput);
#endif

  TGlobals::sCharacterIDList[0] = playerID;

  snprintf(filepath, 32, "/data/chr%d.szs", static_cast<u8>(playerID));
  return DVDConvertPathToEntrynum(filepath);
}
SME_PATCH_BL(SME_PORT_REGION(0x802A7140, 0, 0, 0), setupMarioDatas);

// 0x802A716C
// extern -> SME.cpp
static u32 *initFirstModel(const char *path, u32 unk_1, u32 unk_2, u32 unk_3,
                           JKRHeap *heap,
                           JKRDvdRipper::EAllocDirection direction, u32 unk_5,
                           u32 unk_6) {

  SME_LOG("Instruction at 0x802A750C = %X\n", *(u32 *)0x802A750C);
  if (!TGlobals::sCharacterHeap) {
    SME_LOG("Shouldn't reach here, character heap is nullptr?\n");
  }
#ifndef SME_DETACHED_HEAPS
  TGlobals::sCharacterHeap->freeAll();
#endif
  u32 *archive = static_cast<u32 *>(
      Util::loadArchive(path, TGlobals::sCharacterHeap, direction));
#ifndef SME_DETACHED_HEAPS
  TGlobals::sCharacterHeap->alloc(TGlobals::sCharacterHeap->getFreeSize(), 4);
#endif
  return archive;
}
SME_PATCH_BL(SME_PORT_REGION(0x802A716C, 0, 0, 0), initFirstModel);

static void resetGlobalValues() {
  gModelWaterManagerWaterColor.set(0x3C, 0x46, 0x78, 0x14); // Water rgba
  gYoshiJuiceColor[0].set(0xFE, 0xA8, 0x02, 0x6E);          // Yoshi Juice rgba
  gYoshiJuiceColor[1].set(0x9B, 0x01, 0xFD, 0x6E);
  gYoshiJuiceColor[2].set(0xFD, 0x62, 0xA7, 0x6E);
  gYoshiBodyColor[0].set(0x40, 0xA1, 0x24, 0xFF); // Yoshi rgba
  gYoshiBodyColor[1].set(0xFF, 0x8C, 0x1C, 0xFF);
  gYoshiBodyColor[2].set(0xAA, 0x4C, 0xFF, 0xFF);
  gYoshiBodyColor[3].set(0xFF, 0xA0, 0xBE, 0xFF);
  gAudioVolume = 0.75f;
  gAudioPitch = 1.0f;
  gAudioSpeed = 1.0f;
}

static bool isMario = true;

extern void patches_staticResetter();
extern void objects_staticResetter();

// 0x802998B4
static TMarDirector *initFileMods() {
  TMarDirector *director;
  SME_FROM_GPR(26, director);

  TMarioGamePad *gpGamePad = gpApplication.mGamePad1;

#if defined(SME_DEMO)
  Enum::Player characterID = TGlobals::sCharacterIDList[0];
  if (gIsSwappingWarp) {
    if (isMario) {
      characterID = Enum::Player::IL_PIANTISSIMO;
    } else {
      characterID = Enum::Player::MARIO;
    }
    isMario = !isMario;
    gIsSwappingWarp = false;
  }
#elif defined(SME_DEBUG)
  Enum::Player characterID =
      Util::Mario::getPlayerIDFromInput(gpGamePad->mButtons.mInput);
#else
  Enum::Player characterID = Enum::Player::UNKNOWN;
#endif

  resetGlobalValues();
  objects_staticResetter();
  patches_staticResetter();
  TGlobals::clearAllPlayerParams();
  TStageParams::sStageConfig->reset();
  TStageParams::sStageConfig->load(Util::getStageName(&gpApplication));

  TFlagManager::smInstance->setBool(true, 0x1038F); // Yosh
  TFlagManager::smInstance->Type4Flag.mGoldCoinCount = 0;

#ifdef CHARACTER_SELECT
  // ...
#else
  TGlobals::sCharacterIDList[0] = characterID;
#endif
  gInXYZMode = false;

  Util::Mario::swapBinary(characterID);
  Util::Mario::loadParams();

  return director;
}
SME_PATCH_BL(SME_PORT_REGION(0x802998B4, 0, 0, 0), initFileMods);