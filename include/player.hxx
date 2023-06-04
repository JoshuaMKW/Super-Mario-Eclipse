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

namespace Player {

    constexpr const char *data_key = "eclipse_data";

	struct PlayerState {
        PlayerState() : mIsOnFire(false), mFireTimer(0), mFireTimerMax(0) {}

        bool mIsOnFire;
        s16 mFireTimer;
        s16 mFireTimerMax;
	};

// Set a player on fire
void setFire(TMario *);

// Extinguish the player if on fire
void extinguishFire(TMario *, bool);

}


} // namespace SME