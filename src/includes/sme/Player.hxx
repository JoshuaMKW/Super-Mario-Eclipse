#pragma once

#include "sms/actor/Mario.hxx"

namespace SME::Enum {
    enum class Player : u8 {
        MARIO,
        LUIGI,
        IL_PIANTISSIMO,
        SHADOW_MARIO,
        DRY_BONES,
        UNKNOWN = 0xFF
    };
}

namespace SME::Util::Mario {

bool loadParams();
bool swapBinary(SME::Enum::Player id);
void switchCharacter(TMario *player, SME::Enum::Player id, bool fadeInOut);
SME::Enum::Player getPlayerIDFromInput(u32 input);
SME::Enum::Player getPlayerIDFromInt(u8 id);

}