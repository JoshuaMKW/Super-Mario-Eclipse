#include "globals.hxx"

void *SME::TGlobals::sPRMFile = nullptr;

SME::CharacterID SME::TGlobals::sCharacterIDList[4] = {
    SME::CharacterID::MARIO, SME::CharacterID::LUIGI, SME::CharacterID::PIANTISSIMO,
    SME::CharacterID::SHADOW_MARIO};

bool SME::TGlobals::sIsFreePlay;