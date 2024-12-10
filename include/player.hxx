#pragma once

#include <SMS/Camera/PolarSubCamera.hxx>
#include <SMS/Player/Mario.hxx>

#include <BetterSMS/player.hxx>

namespace SME {

    enum class CharacterID : u8 { MARIO, LUIGI, PIANTISSIMO, SHADOW_MARIO };

    bool loadParams();
    bool swapBinary(CharacterID id);
    void switchCharacter(TMario *player, CharacterID id, bool is_in_stage);

    namespace Player {

        constexpr const char *data_key = "eclipse_data";

        struct PlayerState {
            PlayerState() {}

            bool mIsHoldingKey   = false;
            bool mIsOnFire       = false;
            s16 mFireTimer       = 0;
            s16 mFireTimerMax    = 0;
            s16 mColdTimer       = 0;
            s16 mColdDamageTimer = 0;
            s16 mColdTimerStart  = 0;
            s16 mColdFrequency   = 0;
            f32 mAirDec          = 0;
            f32 mAirInc          = 0;
            int mRocketTimer     = 0;
        };

        inline PlayerState *getEclipseData(TMario *player) {
            return reinterpret_cast<PlayerState *>(
                BetterSMS::Player::getRegisteredData(player, data_key));
        }

        // Set a player on fire
        void setFire(TMario *);

        // Extinguish the player if on fire
        void extinguishFire(TMario *, bool);

        // Set a player on cold
        void setCold(TMario *, s16 timer, s16 frequency);

    }  // namespace Player

}  // namespace SME