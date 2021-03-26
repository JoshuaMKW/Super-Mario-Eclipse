#include "Globals.hxx"

SME::Class::TPlayerParams *SME::TGlobals::getPlayerParams(u8 id) const {
  SME_DEBUG_ASSERT(id < SME_MAX_PLAYERS, "Invalid player index provided");
  return mPlayerCfgArray[id];
}

SME::Class::TPlayerParams *
SME::TGlobals::getPlayerParams(TMario *player) const {
  for (u32 i = 0; i < SME_MAX_PLAYERS; ++i) {
    if (mPlayerCfgArray[i]->getPlayer() == player)
      return mPlayerCfgArray[i];
  }
  return nullptr;
}

void SME::TGlobals::registerPlayerParams(SME::Class::TPlayerParams *params) {
  for (u32 i = 0; i < SME_MAX_PLAYERS; ++i) {
    if (mPlayerCfgArray[i] == params)
      return;
    else if (mPlayerCfgArray[i] == nullptr) {
      mPlayerCfgArray[i] = params;
      return;
    }
  }
}

void SME::TGlobals::deregisterPlayerParams(SME::Class::TPlayerParams *params) {
  for (u32 i = 0; i < SME_MAX_PLAYERS; ++i) {
    if (mPlayerCfgArray[i] == params) {
      mPlayerCfgArray[i] = nullptr;
      return;
    } else if (mPlayerCfgArray[i] == nullptr)
      return;
  }
}