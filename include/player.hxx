#pragma once

#include <SMS/Player/Mario.hxx>
#include <SMS/Camera/PolarSubCamera.hxx>

namespace SME {

enum class CharacterID : u8 {
  MARIO,
  LUIGI,
  PIANTISSIMO,
  SHADOW_MARIO
};

bool loadParams();
bool swapBinary(CharacterID id);
void switchCharacter(TMario *player, CharacterID id, bool is_in_stage);

} // namespace SME