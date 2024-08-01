// -----------------------------------------
// All credits go to TheAzack9 for this code
// Github: https://github.com/TheAzack9
// -----------------------------------------

#include <SMS/MSound/MSoundSESystem.hxx>
#include <SMS/Player/Mario.hxx>
#include <SMS/raw_fn.hxx>
#include <BetterSMS/module.hxx>

#include "globals.hxx"
#include "player.hxx"

using namespace SME::Player;

// void JAISound_setPitch(JAISound* sound, f32 pitch, u32 interpolation, u8 slot) {
//	sound->setPitch(pitch/2.0, interpolation, slot);
// }
// SMS_PATCH_BL(SMS_PORT_REGION(0x80013e18, 0, 0, 0), JAISound_setPitch);
//
u8 playerIdMakingSound = 0;
void SoundSESystem_startSoundActorInner(u32 soundId, JAISound **sound, JAIActor *origin,
                                        u32 param_4, u8 param_5) {
    SME::CharacterID player_id = SME::TGlobals::getCharacterIDFromPlayer(gpMarioAddress);

    // 78FE - yawn, but replaced with luigi climb
    // 788F - climb ledge
    // 780A and 0x78B9 is spin
    u32 overridenSoundId = soundId;
    if (player_id == SME::CharacterID::LUIGI) {
        if (soundId == 0x78ab || soundId == 0x7803 || soundId == 0x7807 ||
            soundId == 0x7800 || soundId == 0x780A || soundId == 0x78B9) {
            overridenSoundId = 0x7901;
        }
        if (soundId == 0x7884 || soundId == 0x78b1) {
            overridenSoundId = 0x78fb;
        }
        if (soundId == 0x788F || soundId == 0x78b6) {
            overridenSoundId = 0x78FE;
        }
    } else {
        if (soundId == 0x7901) {
            overridenSoundId = 0x78ab;
        }
        if (soundId == 0x78fb) {
            overridenSoundId = 0x7884;
        }
    }
    if (soundId != 0x78FE) {
        auto *newSound = MSoundSESystem::MSoundSE::startSoundActorInner(overridenSoundId, sound,
                                                                        origin, param_4, param_5);
        if (newSound && soundId == overridenSoundId && player_id == SME::CharacterID::LUIGI) {
            newSound->setPitch(0.891251, 3, 0);  // Down two semitones
            newSound->setTempoProportion(1.3, 5);
            newSound->setVolume(1.0, 0, 0);
        }
    }
}
SMS_PATCH_BL(SMS_PORT_REGION(0x80013dd4, 0, 0, 0), SoundSESystem_startSoundActorInner);
