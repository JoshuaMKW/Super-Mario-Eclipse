#include "Globals.hxx"
#include "SME.hxx"

using namespace SME;

TGlobals TGlobals::sGlobals = TGlobals();

TGlobals::TGlobals()
    : mStageConfig(nullptr), mIsCompletionRewards(false),
      mIsAudioStreaming(false), mIsAudioStreamAllowed(false),
      mIsFreePlay(false), mActivePlayers(0), mMaxPlayers(SME_MAX_PLAYERS),
      mPRMFile(nullptr), mWarpColArray(nullptr), mWarpColPreserveArray(nullptr),
      mCharacterHeap(nullptr), mGame6Heap(nullptr), mGlobalsHeap(nullptr),
      mPlayerHasGeckoCodes(false) {
  for (u32 i = 0; i < SME_MAX_PLAYERS; ++i) {
    mPlayers[i] = nullptr;
    mPlayerCfgArray[i] = nullptr;
  }

  mLightData.mShineShadowCoordinates.set(0.0f, 0.0f, 0.0f);
  mLightData.mPrevShineCount = 0;
  mLightData.mPrevSize = 0.0f;
  mLightData.mNextSize = 0.0f;
  mLightData.mShineShadowBase = 0.0f;
  mLightData.mStepContext = 0.0f;
  mLightData.mLightType = SME::Enum::LightContext::DISABLED;
  mLightData.mSizeMorphing = false;
}

TMario *TGlobals::getPlayerByIndex(u8 index) const {
  SME_DEBUG_ASSERT(index < SME_MAX_PLAYERS, "Invalid player index provided");
  return mPlayers[index];
}

Class::TPlayerParams *TGlobals::getPlayerParams(u8 id) const {
  SME_DEBUG_ASSERT(id < SME_MAX_PLAYERS, "Invalid player index provided");
  return mPlayerCfgArray[id];
}

Class::TPlayerParams *TGlobals::getPlayerParams(TMario *player) const {
  for (u32 i = 0; i < SME_MAX_PLAYERS; ++i) {
    if (mPlayerCfgArray[i]->getPlayer() == player)
      return mPlayerCfgArray[i];
  }
  return nullptr;
}

void TGlobals::setPlayerByIndex(u8 index, TMario *player) {
  SME_DEBUG_ASSERT(index < SME_MAX_PLAYERS, "Invalid player index provided");
  mPlayers[index] = player;
}

void TGlobals::registerPlayerParams(Class::TPlayerParams *params) {
  for (u32 i = 0; i < SME_MAX_PLAYERS; ++i) {
    if (mPlayerCfgArray[i] == params)
      return;
    else if (mPlayerCfgArray[i] == nullptr) {
      mPlayerCfgArray[i] = params;
      return;
    }
  }
}

void TGlobals::deregisterPlayerParams(Class::TPlayerParams *params) {
  for (u32 i = 0; i < SME_MAX_PLAYERS; ++i) {
    if (mPlayerCfgArray[i] == params) {
      mPlayerCfgArray[i] = nullptr;
      return;
    } else if (mPlayerCfgArray[i] == nullptr)
      return;
  }
}

void TGlobals::clearAllPlayerParams() {
  for (u32 i = 0; i < SME_MAX_PLAYERS; ++i) {
    mPlayerCfgArray[i] = nullptr;
  }
}