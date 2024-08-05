// -----------------------------------------
// All credits go to TheAzack9 for this code
// Github: https://github.com/TheAzack9
// -----------------------------------------

#include <BetterSMS/module.hxx>
#include <SMS/MSound/MSoundSESystem.hxx>
#include <SMS/Player/Mario.hxx>
#include <SMS/raw_fn.hxx>

#include "globals.hxx"
#include "player.hxx"

using namespace SME::Player;

u8 playerIdMakingSound = 0;
void SoundSESystem_startSoundActorInner(u32 soundId, JAISound **sound, JAIActor *origin,
                                        u32 param_4, u8 param_5) {
    SME::CharacterID player_id = SME::TGlobals::getCharacterIDFromPlayer(gpMarioAddress);

    // 78FE - yawn, but replaced with luigi climb
    // 788F - climb ledge
    // 780A and 0x78B9 is spin
    u32 overridenSoundId = soundId;
    if (player_id == SME::CharacterID::LUIGI) {
        if (soundId == 0x78ab || soundId == 0x7803 || soundId == 0x7807 || soundId == 0x7800 ||
            soundId == 0x780A || soundId == 0x78B9) {
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

u32 checkOtherPlayerVoices(MSound *sound, u32 soundID, s16 health, u8 status) {
    TMario *player;
    SMS_FROM_GPR(29, player);

    SME::CharacterID player_id = SME::TGlobals::getCharacterIDFromPlayer(player);
    if (player_id == SME::CharacterID::PIANTISSIMO) {
        switch (soundID) {
        case MSD_SE_MV21_JUMP_SMALL_01:
        case MSD_SE_MV22_JUMP_MID_01:
        case MSD_SE_MV23_JUMP_LARGE_01:
        case MSD_SE_MV24_JUMP_SPECIAL_01:
        case MSD_SE_MV25A_JUMP_HUGE_01: {
            if (sound->gateCheck(MSD_SE_NPC_VA_MTMAN_JUMP1)) {
                JAISound **sounds = (JAISound **)((u8 *)gpMSound + 0x8C + (0x4 * 1));
                MSoundSE::startSoundActor(MSD_SE_NPC_VA_MTMAN_JUMP1, player->mTranslation, 0,
                                          sounds, 0, 4);
            }
            return MSD_SE_NPC_VA_MTMAN_JUMP1;
        }
        default:
            return 0xFFFFFFFF;
        }
    }

    return sound->startMarioVoice(soundID, health, status);
}
SMS_PATCH_BL(SMS_PORT_REGION(0x80012D38, 0, 0, 0), checkOtherPlayerVoices);
SMS_PATCH_BL(SMS_PORT_REGION(0x801E6DA0, 0, 0, 0), checkOtherPlayerVoices);
SMS_PATCH_BL(SMS_PORT_REGION(0x8026E90C, 0, 0, 0), checkOtherPlayerVoices);
SMS_PATCH_BL(SMS_PORT_REGION(0x80285354, 0, 0, 0), checkOtherPlayerVoices);
SMS_PATCH_BL(SMS_PORT_REGION(0x802853D8, 0, 0, 0), checkOtherPlayerVoices);
SMS_PATCH_BL(SMS_PORT_REGION(0x80285BE8, 0, 0, 0), checkOtherPlayerVoices);
SMS_PATCH_BL(SMS_PORT_REGION(0x80286014, 0, 0, 0), checkOtherPlayerVoices);
SMS_PATCH_BL(SMS_PORT_REGION(0x80286114, 0, 0, 0), checkOtherPlayerVoices);
SMS_PATCH_BL(SMS_PORT_REGION(0x802861B4, 0, 0, 0), checkOtherPlayerVoices);
SMS_PATCH_BL(SMS_PORT_REGION(0x80286348, 0, 0, 0), checkOtherPlayerVoices);
SMS_PATCH_BL(SMS_PORT_REGION(0x80286398, 0, 0, 0), checkOtherPlayerVoices);
SMS_PATCH_BL(SMS_PORT_REGION(0x80286774, 0, 0, 0), checkOtherPlayerVoices);
SMS_PATCH_BL(SMS_PORT_REGION(0x802867B4, 0, 0, 0), checkOtherPlayerVoices);
SMS_PATCH_BL(SMS_PORT_REGION(0x80286818, 0, 0, 0), checkOtherPlayerVoices);
SMS_PATCH_BL(SMS_PORT_REGION(0x80286858, 0, 0, 0), checkOtherPlayerVoices);
SMS_PATCH_BL(SMS_PORT_REGION(0x802868D4, 0, 0, 0), checkOtherPlayerVoices);
SMS_PATCH_BL(SMS_PORT_REGION(0x80286C24, 0, 0, 0), checkOtherPlayerVoices);
SMS_PATCH_BL(SMS_PORT_REGION(0x80286C64, 0, 0, 0), checkOtherPlayerVoices);
SMS_PATCH_BL(SMS_PORT_REGION(0x80286D08, 0, 0, 0), checkOtherPlayerVoices);
SMS_PATCH_BL(SMS_PORT_REGION(0x80286D48, 0, 0, 0), checkOtherPlayerVoices);
SMS_PATCH_BL(SMS_PORT_REGION(0x80286DA8, 0, 0, 0), checkOtherPlayerVoices);
SMS_PATCH_BL(SMS_PORT_REGION(0x80286DE8, 0, 0, 0), checkOtherPlayerVoices);
SMS_PATCH_BL(SMS_PORT_REGION(0x80286E40, 0, 0, 0), checkOtherPlayerVoices);
SMS_PATCH_BL(SMS_PORT_REGION(0x80286E94, 0, 0, 0), checkOtherPlayerVoices);
SMS_PATCH_BL(SMS_PORT_REGION(0x80286F2C, 0, 0, 0), checkOtherPlayerVoices);
SMS_PATCH_BL(SMS_PORT_REGION(0x80286F6C, 0, 0, 0), checkOtherPlayerVoices);
SMS_PATCH_BL(SMS_PORT_REGION(0x80286FC4, 0, 0, 0), checkOtherPlayerVoices);

// This is just for spinjump which crashes for some reason?
u32 muteOtherPlayerVoices(MSound *sound, u32 soundID, s16 health, u8 status) {
    TMario *player;
    SMS_FROM_GPR(29, player);

    SME::CharacterID player_id = SME::TGlobals::getCharacterIDFromPlayer(player);
    if (player_id == SME::CharacterID::PIANTISSIMO) {
        return 0xFFFFFFFF;
    }

    return sound->startMarioVoice(soundID, health, status);
}
SMS_PATCH_BL(SMS_PORT_REGION(0x80285CA0, 0, 0, 0), muteOtherPlayerVoices);
