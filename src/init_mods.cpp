#include "printf.h"
#include "sms/JSystem/JDrama.hxx"
#include "sms/JSystem/JKR/JKRDecomp.hxx"
#include "sms/JSystem/JKR/JKRHeap.hxx"
#include "sms/JSystem/JKR/JKRMemArchive.hxx"
#include "stdlib.h"

#include "SME.hxx"

using namespace SME::Util;

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
  constexpr size_t charactersize = 0x1A0000;
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
      SME::TGlobals::sGlobalHeap =
          JKRExpHeap::create(globalsize - sizeof(JKRExpHeap), JKRHeap::sRootHeap, false);

    SME::TGlobals::sCharacterHeap->alloc(
        SME::TGlobals::sCharacterHeap->getFreeSize(), 4);

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
  SME::Class::TSMEFile::sStageConfig->reset();

  if (SME::Class::TSMEFile::sStageConfig->load(
          SME::Util::getStageName(&gpApplication))) {
    if (characterID == SME::Enum::Player::UNKNOWN)
      characterID = SME::Class::TSMEFile::sStageConfig->GlobalFlags.mPlayerID;
  } else {
    if (characterID == SME::Enum::Player::UNKNOWN)
      characterID = SME::Enum::Player::MARIO;
  }

  SME::Util::Mario::swapBinary(characterID);
  SME::Util::Mario::loadParams();

  return director;
}

// 0x80280180
void SME::Patch::Init::initShineShadow() {
  if (SME::Class::TSMEFile::sStageConfig->mMagic !=
          SME::Class::TSMEFile::MAGIC ||
      !SME::Class::TSMEFile::sStageConfig->GlobalFlags.mIsShineShadow)
    return;

  if (TFlagManager::smInstance->Type4Flag.mShineCount < SME_MAX_SHINES) {
    SME::TGlobals::sLightData.mLightType =
        SME::Class::TSMEFile::sStageConfig->GlobalFlags.mShineShadowFlag;
    SME::TGlobals::sLightData.mShineShadowBase =
        SME::Class::TSMEFile::sStageConfig->Light.mSize;
    SME::TGlobals::sLightData.mPrevShineCount =
        TFlagManager::smInstance->Type4Flag.mShineCount;

    gpModelWaterManager->mColor =
        SME::Class::TSMEFile::sStageConfig->Light.mColor;
    gpModelWaterManager->mDarkLevel =
        SME::Class::TSMEFile::sStageConfig->Light.mDarkLevel;
    gpModelWaterManager->mLayerCount =
        SME::Class::TSMEFile::sStageConfig->Light.mLayerCount;
    gpModelWaterManager->mSphereStep =
        SME::Class::TSMEFile::sStageConfig->Light.mStep;
    gpModelWaterManager->mSize = SME::Class::TSMEFile::sStageConfig->Light.mSize;
    gpModelWaterManager->LightType.mMaskObjects = true;
    gpModelWaterManager->LightType.mShowShadow = true;

    if (SME::Class::TSMEFile::sStageConfig->GlobalFlags.mShineShadowFlag ==
        TLightContext::ActiveType::STATIC) {
      SME::TGlobals::sLightData.mNextSize =
          SME::Class::TSMEFile::sStageConfig->Light.mSize;
      for (u32 i = 0; i < TFlagManager::smInstance->Type4Flag.mShineCount;
           ++i) {
        SME::TGlobals::sLightData.mNextSize +=
            (10000.0f / (f32)SME_MAX_SHINES) + (f32)i * 2.0f;
      }
      gpModelWaterManager->mSize = SME::TGlobals::sLightData.mNextSize;
      gpModelWaterManager->mSphereStep = gpModelWaterManager->mSize / 2.0f;
    }
    SME::TGlobals::sLightData.mShineShadowCoordinates =
        SME::Class::TSMEFile::sStageConfig->Light.mCoordinates;
  } else {
    SME::TGlobals::sLightData.mLightType = TLightContext::ActiveType::DISABLED;
  }
}

// 0x802B7A4C
void SME::Patch::Init::initSoundBank(u8 areaID, u8 episodeID) {
  if (SME::Class::TSMEFile::sStageConfig->mMagic !=
          SME::Class::TSMEFile::MAGIC ||
      !SME::Class::TSMEFile::sStageConfig->GlobalFlags.mIsMusic)
    setMSoundEnterStage__10MSMainProcFUcUc(areaID, episodeID);
  else
    setMSoundEnterStage__10MSMainProcFUcUc(
        SME::Class::TSMEFile::sStageConfig->Music.mAreaID,
        SME::Class::TSMEFile::sStageConfig->Music.mEpisodeID);
}

// 0x802983F0
// 0x80298420
void SME::Patch::Init::initMusicTrack() {
  if (SME::Class::TSMEFile::sStageConfig->Music.mPlayMusic) {
    gStageBGM = 0x80010000 | SME::Class::TSMEFile::sStageConfig->Music.mMusicID;
    gAudioSpeed = SME::Class::TSMEFile::sStageConfig->Music.mSpeed;
    gAudioPitch = SME::Class::TSMEFile::sStageConfig->Music.mPitch;
    gAudioVolume =
        Max(Min(SME::Class::TSMEFile::sStageConfig->Music.mVolume, 1), 0);
  }
  startStageBGM__10MSMainProcFUcUc();
}

static void initFludd(TMario *player, SME::Class::TPlayerData *params) {
  SME_ASSERT(params != nullptr, "Can't init fludd with non existant params!");
  if (params) {
    gModelWaterManagerWaterColor = params->getParams()->mWaterColor.get();

    if (!params->canUseNozzle(static_cast<TWaterGun::NozzleType>(
            player->mFludd->mCurrentNozzle))) {
      for (u8 i = 0; i < 7; ++i) {
        if (params->canUseNozzle(static_cast<TWaterGun::NozzleType>(i))) {
          player->mFludd->mCurrentNozzle = i;
          player->mAttributes.mHasFludd = params->getCanUseFludd();
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

    if (!params->canUseNozzle(static_cast<TWaterGun::NozzleType>(
            player->mFludd->mSecondNozzle))) {
      for (u8 i = 0; i < 7; ++i) {
        if (params->canUseNozzle(static_cast<TWaterGun::NozzleType>(i))) {
          player->mFludd->mSecondNozzle = i;
          player->mAttributes.mHasFludd = params->getCanUseFludd();
          break;
        }
        player->mFludd->mSecondNozzle = player->mFludd->mCurrentNozzle;
      }
    }
  }

  if (SME::Class::TSMEFile::sStageConfig->mMagic ==
          SME::Class::TSMEFile::MAGIC &&
      SME::Class::TSMEFile::sStageConfig->GlobalFlags.mIsFludd) {
    if (SME::Class::TSMEFile::sStageConfig->Fludd.mIsColorWater) {
      gModelWaterManagerWaterColor =
          SME::Class::TSMEFile::sStageConfig->Fludd.mWaterColor;
    }
    player->mFludd->mCurrentNozzle =
        SME::Class::TSMEFile::sStageConfig->Fludd.mPrimaryNozzle;
    player->mFludd->mSecondNozzle =
        SME::Class::TSMEFile::sStageConfig->Fludd.mSecondaryNozzle;

    player->mFludd->mCurrentWater =
        player->mFludd->mNozzleList[(u8)player->mFludd->mCurrentNozzle]
            ->mMaxWater;
  }
}

static void initMario(TMario *player, bool isMario) {
  CPolarSubCamera *camera = new CPolarSubCamera("<CPolarSubCamera>");
  SME::Class::TPlayerData *params =
      new SME::Class::TPlayerData(player, camera, isMario);
  SME::TGlobals::registerPlayerParams(params);

  if (SME::Class::TSMEFile::sStageConfig->mMagic ==
          SME::Class::TSMEFile::MAGIC &&
      SME::Class::TSMEFile::sStageConfig->GlobalFlags.mIsMario) {
    params->setPlayerID(
        SME::Class::TSMEFile::sStageConfig->GlobalFlags.mPlayerID);
    player->mHealth = SME::Class::TSMEFile::sStageConfig->Mario.mHealth;
    player->mDeParams.mTramplePowStep1.set(
        SME::Class::TSMEFile::sStageConfig->Mario.mBaseBounce1);
    player->mDeParams.mTramplePowStep2.set(
        SME::Class::TSMEFile::sStageConfig->Mario.mBaseBounce2);
    player->mDeParams.mTramplePowStep3.set(
        SME::Class::TSMEFile::sStageConfig->Mario.mBaseBounce3);
    player->mDeParams.mDamageFallHeight.set(
        SME::Class::TSMEFile::sStageConfig->Mario.mMaxFallNoDamage);
    player->mDeParams.mHPMax.set(
        SME::Class::TSMEFile::sStageConfig->Mario.mMaxHealth);
    player->mDeParams.mIllegalPlaneCtInc.set(
        SME::Class::TSMEFile::sStageConfig->Mario.mOBStep);
    player->mDeParams.mIllegalPlaneTime.set(
        SME::Class::TSMEFile::sStageConfig->Mario.mOBMax);
    player->mJumpParams.mGravity.set(
        SME::Class::TSMEFile::sStageConfig->Mario.mGravity);
    player->mAttributes.mGainHelmet =
        SME::Class::TSMEFile::sStageConfig->GlobalFlags.MarioStates
            .mMarioHasHelmet;
    player->mAttributes.mHasFludd = SME::Class::TSMEFile::sStageConfig
                                        ->GlobalFlags.MarioStates.mMarioHasFludd;
    player->mAttributes.mIsShineShirt =
        SME::Class::TSMEFile::sStageConfig->GlobalFlags.MarioStates
            .mMarioHasShirt;

    if (SME::Class::TSMEFile::sStageConfig->GlobalFlags.MarioStates
            .mMarioHasGlasses)
      wearGlass__6TMarioFv(player);
  }

  if (isMario)
    initFludd(player, params);
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

  if (SME::Class::TSMEFile::sStageConfig->mMagic !=
          SME::Class::TSMEFile::MAGIC ||
      !SME::Class::TSMEFile::sStageConfig->GlobalFlags.mIsYoshi)
    return;

  gYoshiJuiceColor[0] = SME::Class::TSMEFile::sStageConfig->Yoshi.mOrangeYoshi;
  gYoshiJuiceColor[1] = SME::Class::TSMEFile::sStageConfig->Yoshi.mPurpleYoshi;
  gYoshiJuiceColor[2] = SME::Class::TSMEFile::sStageConfig->Yoshi.mPinkYoshi;

  gYoshiBodyColor[0] = SME::Class::TSMEFile::sStageConfig->Yoshi.mGreenYoshi;
  gYoshiBodyColor[1] = SME::Class::TSMEFile::sStageConfig->Yoshi.mOrangeYoshi;
  gYoshiBodyColor[2] = SME::Class::TSMEFile::sStageConfig->Yoshi.mPurpleYoshi;
  gYoshiBodyColor[3] = SME::Class::TSMEFile::sStageConfig->Yoshi.mPinkYoshi;

  yoshi->mMaxJuice = SME::Class::TSMEFile::sStageConfig->Yoshi.mMaxJuice;
  yoshi->mMaxVSpdStartFlutter =
      SME::Class::TSMEFile::sStageConfig->Yoshi.mMaxVSpdStartFlutter;
  yoshi->mFlutterAcceleration =
      SME::Class::TSMEFile::sStageConfig->Yoshi.mFlutterAcceleration;
  yoshi->mMaxFlutterTimer =
      SME::Class::TSMEFile::sStageConfig->Yoshi.mMaxFlutterTimer;
}

// 0x8029CCB0
void SME::Patch::Init::initCardColors() {
  if (SME::Class::TSMEFile::sStageConfig->mMagic ==
          SME::Class::TSMEFile::MAGIC &&
      SME::Class::TSMEFile::sStageConfig->GlobalFlags.mIsYoshi) {
    gpMarDirector->mGCConsole->mJuiceCardOrange =
        SME::Class::TSMEFile::sStageConfig->Yoshi.mOrangeYoshi;
    gpMarDirector->mGCConsole->mJuiceCardPurple =
        SME::Class::TSMEFile::sStageConfig->Yoshi.mPurpleYoshi;
    gpMarDirector->mGCConsole->mJuiceCardPink =
        SME::Class::TSMEFile::sStageConfig->Yoshi.mPinkYoshi;
  }

  if (SME::Class::TSMEFile::sStageConfig->mMagic ==
          SME::Class::TSMEFile::MAGIC &&
      SME::Class::TSMEFile::sStageConfig->GlobalFlags.mIsFludd &&
      SME::Class::TSMEFile::sStageConfig->Fludd.mIsColorWater) {
    gpMarDirector->mGCConsole->mWaterLeftPanel =
        SME::Class::TSMEFile::sStageConfig->Fludd.mWaterColor;
    gpMarDirector->mGCConsole->mWaterRightPanel.r = Math::lerp<u8>(
        0, SME::Class::TSMEFile::sStageConfig->Fludd.mWaterColor.r, 0.8125);
    gpMarDirector->mGCConsole->mWaterRightPanel.g = Math::lerp<u8>(
        0, SME::Class::TSMEFile::sStageConfig->Fludd.mWaterColor.g, 0.8125);
    gpMarDirector->mGCConsole->mWaterRightPanel.b = Math::lerp<u8>(
        0, SME::Class::TSMEFile::sStageConfig->Fludd.mWaterColor.b, 0.8125);
    gpMarDirector->mGCConsole->mWaterRightPanel.a = Math::lerp<u8>(
        0, SME::Class::TSMEFile::sStageConfig->Fludd.mWaterColor.a, 0.8125);
  }
}

/*This works by taking the target id and matching it to the
/ ID of the first entry to have the same home ID as the target.
/
/ This makes a half decent linking system for the collision
/ triangles for functionalities like linked warping!
*/

static void parseWarpLinks(TMapCollisionData *col,
                           SME::Class::TWarpCollisionList *links, u32 validID,
                           u32 idGroupSize = 0) {
  u32 curDataIndex = 0;

  for (u32 i = 0; i < col->mFloorArraySize; ++i) {
    if (((col->mColTable[i].mCollisionType & 0x7FFF) - validID) <=
            idGroupSize ||
        ((col->mColTable[i].mCollisionType & 0x7FFF) - validID) <=
            idGroupSize + 1000) {

      links->mColList[curDataIndex] = SME::Class::TCollisionLink(
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
  SME::Class::TWarpCollisionList *warpDataArray =
      new (32) SME::Class::TWarpCollisionList();
  SME::Class::TWarpCollisionList *warpDataPreserveArray =
      new (32) SME::Class::TWarpCollisionList();
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