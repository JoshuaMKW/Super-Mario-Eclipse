#include "Globals.hxx"

SME::Class::TPlayerParams *SME::TGlobals::getPlayerParams(u8 id) const {
    SME_DEBUG_ASSERT(id < SME_MAX_PLAYERS, "Invalid player index provided");
    return mPlayerCfgArray[id];
}

SME::Class::TPlayerParams *SME::TGlobals::getPlayerParams(TMario *player) const {
    for (u32 i = 0; i < SME_MAX_PLAYERS; ++i) {
        if (mPlayerCfgArray[i]->getPlayer() == player)
            return mPlayerCfgArray[i];
    }
    return nullptr;
}