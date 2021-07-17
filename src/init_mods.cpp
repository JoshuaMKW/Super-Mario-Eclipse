#include "printf.h"
#include "sms/JSystem/JDrama.hxx"
#include "sms/JSystem/JKR/JKRDecomp.hxx"
#include "sms/JSystem/JKR/JKRHeap.hxx"
#include "sms/JSystem/JKR/JKRMemArchive.hxx"
#include "stdlib.h"

#include "SME.hxx"

using namespace SME;
using namespace SME::Util;
using namespace SME::Class;

extern bool gInXYZMode;

Memory::Protection::MemoryMap gCodeProtector;

// 0x80005328
// extern -> SME.cpp
void SME::Patch::Init::initCodeProtection() {
  OSInit();
  gCodeProtector.setIndex(2);
  gCodeProtector.setStart(0x80003800);
  gCodeProtector.setEnd(0x80373000);
  gCodeProtector.setPermission(Memory::Protection::READ);
  gCodeProtector.activate();
}

// 0x802A750C
// extern -> SME.cpp
JKRExpHeap *SME::Patch::Init::createGlobalHeaps(void *newHeap, size_t size,
                                                JKRHeap *rootHeap, bool unk_1) {
  constexpr size_t charactersize = 0x1C0000;
  constexpr size_t globalsize = 0x8000;

#ifdef SME_DETACHED_HEAPS
  {
    JKRHeap *rootHeap = JKRHeap::sRootHeap;
    JKRHeap *systemHeap = JKRHeap::sSystemHeap;
    JKRHeap *currentHeap = JKRHeap::sCurrentHeap;
    JKRHeap::sRootHeap = nullptr; // Hack to make this heap disconnected

    void *heap = OSGetArenaHi();
    if (charactersize > sizeof(JKRExpHeap))
      SME::TGlobals::sCharacterHeap = new (JKRExpHeap::sSystemHeap, 4)
          JKRExpHeap(heap, charactersize - sizeof(JKRExpHeap), nullptr, false);
    if (globalsize > sizeof(JKRExpHeap))
      SME::TGlobals::sGlobalHeap = new (JKRExpHeap::sSystemHeap, 4)
          JKRExpHeap(heap, globalsize - sizeof(JKRExpHeap), nullptr, false);

    currentHeap->becomeCurrentHeap();
    systemHeap->becomeSystemHeap();

    // Restore heap pointers
    JKRHeap::sRootHeap = rootHeap;
  }
#else
  {
    JKRHeap *systemHeap = JKRHeap::sSystemHeap;
    JKRHeap *currentHeap = JKRHeap::sCurrentHeap;

    if (charactersize > sizeof(JKRExpHeap))
      SME::TGlobals::sCharacterHeap = JKRExpHeap::create(
          charactersize - sizeof(JKRExpHeap), JKRHeap::sRootHeap, false);
    if (globalsize > sizeof(JKRExpHeap))
      SME::TGlobals::sGlobalHeap = JKRExpHeap::create(
          globalsize - sizeof(JKRExpHeap), JKRHeap::sRootHeap, false);

    SME::TGlobals::sCharacterHeap->alloc(
        SME::TGlobals::sCharacterHeap->getFreeSize(), 4);

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

// 0x802A7140
// extern -> SME.cpp
s32 SME::Patch::Init::setupMarioDatas(char *filepath) {
  TMarioGamePad *gpGamePad = gpApplication.mGamePad1;
  SME::Enum::Player playerID =
      SME::Util::Mario::getPlayerIDFromInput(gpGamePad->mButtons.mInput);

  sprintf(filepath, "/data/chr%d.szs", static_cast<u8>(playerID));
  return DVDConvertPathToEntrynum(filepath);
}

// 0x802A716C
// extern -> SME.cpp
u32 *SME::Patch::Init::initFirstModel(char *path, u32 unk_1, u32 unk_2,
                                      u32 unk_3, JKRHeap *heap,
                                      JKRDvdRipper::EAllocDirection direction,
                                      u32 unk_5, u32 unk_6) {
#ifndef SME_DETACHED_HEAPS
  SME::TGlobals::sCharacterHeap->freeAll();
#endif
  u32 *archive = static_cast<u32 *>(
      SME::Util::loadArchive(path, SME::TGlobals::sCharacterHeap, direction));
#ifndef SME_DETACHED_HEAPS
  SME::TGlobals::sCharacterHeap->alloc(
      SME::TGlobals::sCharacterHeap->getFreeSize(), 4);
#endif
  return archive;
}

static void resetGlobalValues() {
  gModelWaterManagerWaterColor.set(0x3C, 0x46, 0x78, 0x14); // Water rgba
  gYoshiJuiceColor[0].set(0xFE, 0xA8, 0x02, 0x6E);          // Yoshi Juice rgba
  gYoshiJuiceColor[1].set(0x9B, 0x01, 0xFD, 0x6E);
  gYoshiJuiceColor[2].set(0xFD, 0x62, 0xA7, 0x6E);
  gYoshiBodyColor[0].set(0x40, 0xA1, 0x24, 0xFF); // Yoshi rgba
  gYoshiBodyColor[1].set(0xFF, 0x8C, 0x1C, 0xFF);
  gYoshiBodyColor[2].set(0xAA, 0x4C, 0xFF, 0xFF);
  gYoshiBodyColor[3].set(0xFF, 0xA0, 0xBE, 0xFF);
  *(f32 *)0x80415CA8 = 0.25f; // Mario overall speed
  gAudioVolume = 0.75f;
  gAudioPitch = 1.0f;
  gAudioSpeed = 1.0f;
}

static Enum::Player gCharacterID;

// 0x802998B4
TMarDirector *SME::Patch::Init::initFileMods() {
  TMarDirector *director;
  SME_FROM_GPR(26, director);

  TMarioGamePad *gpGamePad = gpApplication.mGamePad1;

#ifdef SME_DEBUG
  SME::Enum::Player characterID =
      SME::Util::Mario::getPlayerIDFromInput(gpGamePad->mButtons.mInput);
#else
  SME::Enum::Player characterID = SME::Enum::Player::UNKNOWN;
#endif

  resetGlobalValues();
  SME::TGlobals::clearAllPlayerParams();
  TStageParams::sStageConfig->reset();
  TStageParams::sStageConfig->load(SME::Util::getStageName(&gpApplication));

  gCharacterID = characterID;
  gInXYZMode = false;

  SME::Util::Mario::swapBinary(characterID);
  SME::Util::Mario::loadParams();

  return director;
}

// 0x80280180
void SME::Patch::Init::initShineShadow() {
  TStageParams *config = TStageParams::sStageConfig;
  if (config->mLightType.get() == TLightContext::ActiveType::DISABLED)
    return;

  Class::TLightContext &LightContext = SME::TGlobals::sLightData;

  s32 &CurrentShineCount = TFlagManager::smInstance->Type4Flag.mShineCount;
  if (CurrentShineCount < SME_MAX_SHINES) {
    LightContext.mLightType = config->mLightType.get();
    LightContext.mShineShadowBase = config->mLightSize.get();
    LightContext.mPrevShineCount = CurrentShineCount;
    {
      JGeometry::TVec3<f32> coordinates(config->mLightPosX.get(),
                                        config->mLightPosY.get(),
                                        config->mLightPosZ.get());
      LightContext.mShineShadowCoordinates = coordinates;
    }

    gpModelWaterManager->mColor = config->mLightColor.get();
    gpModelWaterManager->mDarkLevel = config->mLightDarkLevel.get();
    gpModelWaterManager->mLayerCount = config->mLightLayerCount.get();
    gpModelWaterManager->mSphereStep = config->mLightStep.get();
    gpModelWaterManager->mSize = config->mLightSize.get();
    gpModelWaterManager->LightType.mMaskObjects = true;
    gpModelWaterManager->LightType.mShowShadow = true;

    if (LightContext.mLightType == TLightContext::ActiveType::STATIC) {
      LightContext.mNextSize =
          LightContext.mShineShadowBase +
          powf(((1350.0f / SME_MAX_SHINES) * CurrentShineCount), 1.5f);

      if (config->mLightDarkLevel.get() == 255)
        gpModelWaterManager->mDarkLevel =
            Util::Math::lerp<u8>(30, 190,
                                 static_cast<f32>(CurrentShineCount) /
                                     static_cast<f32>(SME_MAX_SHINES));
      else
        gpModelWaterManager->mDarkLevel = config->mLightDarkLevel.get();

      gpModelWaterManager->mSize = LightContext.mNextSize;
      gpModelWaterManager->mSphereStep = gpModelWaterManager->mSize / 2.0f;
    }
  } else {
    LightContext.mLightType = TLightContext::ActiveType::DISABLED;
  }
}

// for future intro sound patch
static u8 gOldAreaID = 0;
static u8 gOldEpisodeID = 0;

// 0x802B7A4C
void SME::Patch::Init::initSoundBank(u8 areaID, u8 episodeID) {
  TStageParams *config = TStageParams::sStageConfig;

  gOldAreaID = areaID;
  gOldEpisodeID = episodeID;
  if (config->mMusicSetCustom.get()) {
    areaID = config->mMusicAreaID.get();
    episodeID = config->mMusicEpisodeID.get();
  }
  setMSoundEnterStage__10MSMainProcFUcUc(areaID, episodeID);
}

// 0x802983F0
// 0x80298420
void SME::Patch::Init::initMusicTrack() {
  TStageParams *config = TStageParams::sStageConfig;

  if (config->mMusicSetCustom.get())
    gStageBGM = 0x80010000 | config->mMusicID.get();
  gAudioSpeed = config->mMusicSpeed.get();
  gAudioPitch = config->mMusicPitch.get();
  gAudioVolume = Max(Min(config->mMusicVolume.get(), 1), 0);

  if (config->mMusicEnabled.get())
    startStageBGM__10MSMainProcFUcUc();
}

static void initFludd(TMario *player, TPlayerData *params) {
  SME_ASSERT(params, "Can't init fludd with non existant params!");
  TStageParams *config = TStageParams::sStageConfig;

  if (!params->isMario())
    return;

  if (!params->canUseNozzle(
          static_cast<TWaterGun::NozzleType>(player->mFludd->mCurrentNozzle))) {
    for (u8 i = 0; i < 7; ++i) {
      if (params->canUseNozzle(static_cast<TWaterGun::NozzleType>(i))) {
        player->mAttributes.mHasFludd = params->getCanUseFludd();
        player->mFludd->mCurrentNozzle = i;
        player->mFludd->mCurrentWater =
            player->mFludd->mNozzleList[(u8)player->mFludd->mCurrentNozzle]
                ->mMaxWater;
        break;
      } else if (i == 6) {
        player->mAttributes.mHasFludd = false;
        params->setCanUseFludd(false);
      }
    }
  }

  if (!params->canUseNozzle(
          static_cast<TWaterGun::NozzleType>(player->mFludd->mSecondNozzle))) {
    for (u8 i = 0; i < 7; ++i) {
      if (params->canUseNozzle(static_cast<TWaterGun::NozzleType>(i))) {
        player->mAttributes.mHasFludd = params->getCanUseFludd();
        player->mFludd->mSecondNozzle = i;
        break;
      }
      player->mFludd->mSecondNozzle = player->mFludd->mCurrentNozzle;
    }
  }

  if (config->mFluddShouldColorWater.get())
    gModelWaterManagerWaterColor = config->mFluddWaterColor.get();

  player->mFludd->mCurrentNozzle = config->mFluddPrimary.get();
  player->mFludd->mSecondNozzle = config->mFluddSecondary.get();

  player->mFludd->mCurrentWater =
      player->mFludd->mNozzleList[(u8)player->mFludd->mCurrentNozzle]
          ->mMaxWater;
}

static void initMario(TMario *player, bool isMario) {
  TStageParams *config = TStageParams::sStageConfig;

  TPlayerData *params = new TPlayerData(player, nullptr, isMario);
  TGlobals::registerPlayerParams(params);

  params->scalePlayerAttrs(config->mPlayerSizeMultiplier.get());

  bool isGlasses = false;

  if (config->isCustomConfig()) {
    params->setPlayerID(isMario ? gCharacterID : Enum::Player::SHADOW_MARIO);
    player->mHealth = config->mPlayerHealth.get();
    player->mDeParams.mHPMax.set(config->mPlayerMaxHealth.get());
    player->mJumpParams.mGravity.set(config->mGravityMultiplier.get());

    if (isMario) {
      player->mAttributes.mGainHelmet = config->mPlayerHasHelmet.get();
      player->mAttributes.mHasFludd =
          config->mPlayerHasFludd.get();
      player->mAttributes.mIsShineShirt = config->mPlayerHasShirt.get();
    }

    isGlasses = config->mPlayerHasGlasses.get();
  }

  if (isMario) {
    player->mAttributes.mGainHelmet = params->getParams()->mPlayerHasHelmet.get();
    player->mAttributes.mHasFludd =
        params->getParams()->mCanUseFludd.get();
    player->mAttributes.mIsShineShirt = params->getParams()->mPlayerHasShirt.get();
    isGlasses = params->getParams()->mPlayerHasGlasses.get();

    initFludd(player, params);
  }

  if (isGlasses)
    wearGlass__6TMarioFv(player);
}

// 0x80276C94
TMario *SME::Patch::Init::fromMarioInit(TMario *player) {
  player->initValues();
  initMario(player, true);
  return player;
}

// 0x800397DC
bool SME::Patch::Init::fromShadowMarioInit() {
  TMario *player;
  asm volatile("lwz %0, 0x150 (31)" : "=r"(player));
  initMario(player, false);
  return SMS_isMultiPlayerMap__Fv();
}

// 0x80271580
void SME::Patch::Init::initYoshi(MAnmSound *anmSound, void *r4, u32 r5,
                                 f32 f1) {
  initAnmSound__9MAnmSoundFPvUlf(anmSound, r4, r5, f1);

  TYoshi *yoshi;
  SME_FROM_GPR(31, yoshi);

  TStageParams *config = TStageParams::sStageConfig;
  if (!config->isCustomConfig())
    return;

  gYoshiJuiceColor[0] = config->mYoshiColorOrange.get();
  gYoshiJuiceColor[1] = config->mYoshiColorPurple.get();
  gYoshiJuiceColor[2] = config->mYoshiColorPink.get();

  gYoshiBodyColor[0] = config->mYoshiColorGreen.get();
  gYoshiBodyColor[1] = config->mYoshiColorOrange.get();
  gYoshiBodyColor[2] = config->mYoshiColorPurple.get();
  gYoshiBodyColor[3] = config->mYoshiColorPink.get();

  yoshi->mMaxJuice = config->mYoshiMaxJuice.get();
  yoshi->mMaxVSpdStartFlutter = config->mYoshiMaxVSpdStartFlutter.get();
  yoshi->mFlutterAcceleration = config->mYoshiFlutterAcceleration.get();
  yoshi->mMaxFlutterTimer = config->mYoshiMaxFlutterTimer.get();
}

// 0x8029CCB0
void SME::Patch::Init::initCardColors() {
  TStageParams *config = TStageParams::sStageConfig;

  if (!config->isCustomConfig())
    return;

  JUtility::TColor waterColor = config->mFluddWaterColor.get();

  gpMarDirector->mGCConsole->mJuiceCardOrange = config->mYoshiColorOrange.get();
  gpMarDirector->mGCConsole->mJuiceCardPurple = config->mYoshiColorPurple.get();
  gpMarDirector->mGCConsole->mJuiceCardPink = config->mYoshiColorPink.get();

  if (config->mFluddShouldColorWater.get()) {
    gpMarDirector->mGCConsole->mWaterLeftPanel = waterColor;
    gpMarDirector->mGCConsole->mWaterRightPanel.r =
        Math::lerp<u8>(0, waterColor.r, 0.8125f);
    gpMarDirector->mGCConsole->mWaterRightPanel.g =
        Math::lerp<u8>(0, waterColor.g, 0.8125f);
    gpMarDirector->mGCConsole->mWaterRightPanel.b =
        Math::lerp<u8>(0, waterColor.b, 0.8125f);
    gpMarDirector->mGCConsole->mWaterRightPanel.a =
        Math::lerp<u8>(0, waterColor.a, 0.8125f);
  }
}

/*This works by taking the target id and matching it to the
/ ID of the first entry to have the same home ID as the target.
/
/ This makes a half decent linking system for the collision
/ triangles for functionalities like linked warping!
*/

static void parseWarpLinks(TMapCollisionData *col, TWarpCollisionList *links,
                           u32 validID, u32 idGroupSize = 0) {
  u32 curDataIndex = 0;

  for (u32 i = 0; i < col->mFloorArraySize; ++i) {
    if (((col->mColTable[i].mCollisionType & 0x7FFF) - validID) <=
            idGroupSize ||
        ((col->mColTable[i].mCollisionType & 0x7FFF) - validID) <=
            idGroupSize + 1000) {

      links->mColList[curDataIndex] = TCollisionLink(
          &col->mColTable[i], (u8)(col->mColTable[i].mValue4 >> 8),
          (u8)col->mColTable[i].mValue4);
      if (curDataIndex >= 0xFF)
        break;
      ++curDataIndex;
    }
  }
  links->mArrayLength = curDataIndex;
}

// 0x802B8B20
u32 SME::Patch::Init::initCollisionWarpLinks(const char *name) {
  TWarpCollisionList *warpDataArray = new TWarpCollisionList();
  TWarpCollisionList *warpDataPreserveArray = new TWarpCollisionList();
  SME::TGlobals::sWarpColArray = warpDataArray;
  SME::TGlobals::sWarpColPreserveArray = warpDataPreserveArray;

  if (warpDataArray) {
    parseWarpLinks(gpMapCollisionData, warpDataArray, 16040);
    parseWarpLinks(gpMapCollisionData, warpDataPreserveArray, 16041, 1);
  }

  return JDrama::TNameRef::calcKeyCode(name);
}

// 0x802B57E4
void SME::Patch::Init::createUIHeap(u32 size, s32 alignment) {
  gpMarDirector->mGame6Data = (u32 *)Memory::malloc(size, alignment);
}

// 0x802A72A4
u32 SME::Patch::Init::initHUDElements(char *filepath) {
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
JKRMemArchive *SME::Patch::Init::switchHUDOnStageLoad(char *curArchive,
                                                      u32 *gameUI) {
  char buffer[32];

  if (gpApplication.mGamePad1->mButtons.mInput &
      TMarioGamePad::Buttons::DPAD_UP)
    TFlagManager::smInstance->Type6Flag.CustomFlags.mHUDElement = 1;
  else if (gpApplication.mGamePad1->mButtons.mInput &
           TMarioGamePad::Buttons::DPAD_DOWN)
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
JKRHeap *SME::Patch::Init::useCustomHUDHeap(u32 size, s32 alignment) {
  return nullptr; // SME::TGlobals::sGame6Heap;
}
// kmCall(0x802B57E4, &useCustomHUDHeap);