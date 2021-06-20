#include "Globals.hxx"
#include "SME.hxx"
#include "OS.h"

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

TMario *TGlobals::getPlayerByIndex(u8 index) {
  SME_DEBUG_ASSERT(index < SME_MAX_PLAYERS, "Invalid player index provided");
  return sPlayers[index];
}

Class::TPlayerData *TGlobals::getPlayerParams(u8 id) {
  SME_DEBUG_ASSERT(id < SME_MAX_PLAYERS, "Invalid player index provided");
  return sPlayerCfgArray[id];
}

Class::TPlayerData *TGlobals::getPlayerParams(TMario *player) {
  Class::TPlayerData *cfg;
  for (u32 i = 0; i < SME_MAX_PLAYERS; ++i) {
    cfg = sPlayerCfgArray[i];
    if (!cfg)
      continue;
    else if (cfg->getPlayer() == player){
      return cfg;
    }
  }
  SME_DEBUG_LOG("Player data not found");
  return nullptr;
}

void TGlobals::setPlayerByIndex(u8 index, TMario *player) {
  SME_DEBUG_ASSERT(index < SME_MAX_PLAYERS, "Invalid player index provided");
  sPlayers[index] = player;
}

void TGlobals::registerPlayerParams(Class::TPlayerData *params) {
  for (u32 i = 0; i < SME_MAX_PLAYERS; ++i) {
    if (sPlayerCfgArray[i] == params)
      return;
    else if (sPlayerCfgArray[i] == nullptr) {
      sPlayerCfgArray[i] = params;
      return;
    }
  }
}

void TGlobals::deregisterPlayerParams(Class::TPlayerData *params) {
  for (u32 i = 0; i < SME_MAX_PLAYERS; ++i) {
    if (sPlayerCfgArray[i] == params) {
      sPlayerCfgArray[i] = nullptr;
      return;
    } else if (sPlayerCfgArray[i] == nullptr)
      return;
  }
}

void TGlobals::clearAllPlayerParams() {
  for (u32 i = 0; i < SME_MAX_PLAYERS; ++i) {
    sPlayerCfgArray[i] = nullptr;
  }
}