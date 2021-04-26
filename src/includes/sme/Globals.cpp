#include "Globals.hxx"
#include "SME.hxx"

using namespace SME;

TGlobals TGlobals::sGlobals = TGlobals();

TMario *TGlobals::getPlayerByIndex(u8 index) const {
  SME_DEBUG_ASSERT(index < SME_MAX_PLAYERS, "Invalid player index provided");
  return mPlayers[index];
}

Class::TPlayerParams *TGlobals::getPlayerParams(u8 id) const {
  SME_DEBUG_ASSERT(id < SME_MAX_PLAYERS, "Invalid player index provided");
  return mPlayerCfgArray[id];
}

Class::TPlayerParams *
TGlobals::getPlayerParams(TMario *player) const {
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