#include "globals.hxx"

void *SME::TGlobals::sPRMFile = nullptr;

SME::CharacterID SME::TGlobals::sCharacterIDList[4] = {
    SME::CharacterID::MARIO, SME::CharacterID::LUIGI, SME::CharacterID::PIANTISSIMO,
    SME::CharacterID::SHADOW_MARIO};

u8 SME::TGlobals::sMinDarkness;

bool SME::TGlobals::sIsFreePlay;