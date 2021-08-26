#include "Globals.hxx"
#include "SME.hxx"

using namespace SME;

#ifdef SME_DETACHED_HEAPS
static u8 gCharacterHeapBuffer[0x10000];
static u8 gGlobalBuffer[0x8000];

JKRExpHeap TGlobals::sCharacterHeap(gCharacterHeapBuffer,
                                    sizeof(gCharacterHeapBuffer), nullptr,
                                    false);
JKRExpHeap TGlobals::sGlobalHeap(gGlobalBuffer, sizeof(gGlobalBuffer), nullptr,
                                 false);
#else
JKRExpHeap *TGlobals::sCharacterHeap = nullptr;
JKRExpHeap *TGlobals::sGlobalHeap = nullptr;
#endif

Class::TLightContext TGlobals::sLightData = Class::TLightContext();

void *TGlobals::sPRMFile = nullptr;
SME::Class::TWarpCollisionList *TGlobals::sWarpColArray = nullptr;
SME::Class::TWarpCollisionList *TGlobals::sWarpColPreserveArray = nullptr;

SME::Class::TPlayerData *TGlobals::sPlayerCfgArray[] = {nullptr, nullptr,
                                                        nullptr, nullptr};
TMario *TGlobals::sPlayers[] = {nullptr, nullptr, nullptr, nullptr};
bool TGlobals::sPlayerHasGeckoCodes = false;
bool TGlobals::sIsAudioStreaming = false;
bool TGlobals::sIsAudioStreamAllowed = false;
bool TGlobals::sIsFreePlay = false;
u8 TGlobals::sActivePlayers = 0;
u8 TGlobals::sMaxPlayers = SME_MAX_PLAYERS;
Enum::Player TGlobals::sCharacterIDList[] = {
    Enum::Player::MARIO, Enum::Player::UNKNOWN, Enum::Player::UNKNOWN,
    Enum::Player::UNKNOWN};
f32 TGlobals::sScreenWidth = 700.0f;
f32 TGlobals::sFrameRate = 30.0f;
bool TGlobals::sIsVariableFrameRate = true;

TMario *TGlobals::getPlayerByIndex(u8 index) {
  SME_DEBUG_ASSERT(index < SME_MAX_PLAYERS, "Invalid player index provided");
  return TGlobals::sPlayers[index];
}

Class::TPlayerData *TGlobals::getPlayerParams(u8 id) {
  SME_DEBUG_ASSERT(id < SME_MAX_PLAYERS, "Invalid player index provided");
  return TGlobals::sPlayerCfgArray[id];
}

Class::TPlayerData *TGlobals::getPlayerParams(TMario *player) {
  Class::TPlayerData *cfg;
  for (u32 i = 0; i < SME_MAX_PLAYERS; ++i) {
    cfg = TGlobals::sPlayerCfgArray[i];
    if (!cfg)
      continue;
    else if (cfg->getPlayer() == player)
      return cfg;
  }
  return nullptr;
}

void TGlobals::setPlayerByIndex(u8 index, TMario *player) {
  SME_DEBUG_ASSERT(index < SME_MAX_PLAYERS, "Invalid player index provided");
  TGlobals::sPlayers[index] = player;
}

void TGlobals::registerPlayerParams(Class::TPlayerData *params) {
  for (u32 i = 0; i < SME_MAX_PLAYERS; ++i) {
    if (TGlobals::sPlayerCfgArray[i] == params)
      return;
    else if (TGlobals::sPlayerCfgArray[i] == nullptr) {
      TGlobals::sPlayerCfgArray[i] = params;
      return;
    }
  }
}

void TGlobals::deregisterPlayerParams(Class::TPlayerData *params) {
  for (u32 i = 0; i < SME_MAX_PLAYERS; ++i) {
    if (TGlobals::sPlayerCfgArray[i] == params) {
      TGlobals::sPlayerCfgArray[i] = nullptr;
      return;
    } else if (TGlobals::sPlayerCfgArray[i] == nullptr)
      return;
  }
}

void TGlobals::clearAllPlayerParams() {
  for (u32 i = 0; i < SME_MAX_PLAYERS; ++i) {
    TGlobals::sPlayerCfgArray[i] = nullptr;
  }
}