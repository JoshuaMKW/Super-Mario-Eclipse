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
    bool emario_status;
    SMS_FROM_GPR(31, emario_status);

    if (soundId != 0x78FE) {
        auto *newSound = MSoundSESystem::MSoundSE::startSoundActorInner(soundId, sound, origin,
                                                                        param_4, param_5);
        if (!emario_status &&
            SME::TGlobals::getCharacterIDFromPlayer(gpMarioAddress) == SME::CharacterID::LUIGI) {
            newSound->setPitch(0.95, 3, 0);  // Down two semitones
            newSound->setTempoProportion(1.3, 5);
            newSound->setVolume(1.0, 0, 0);
        }
    }
}
SMS_PATCH_BL(SMS_PORT_REGION(0x80013dd4, 0, 0, 0), SoundSESystem_startSoundActorInner);

u32 playerVoiceProcess(TMario *player, MSound *sound, u32 soundID, s16 health, u8 status) {
    // Check for main player
    if (player->_388 == 0) {
        SME::CharacterID player_id = SME::TGlobals::getCharacterIDFromPlayer(player);
        if (player_id == SME::CharacterID::MARIO) {
            health = 8;
            switch (soundID) {
            case MSD_SE_MV41_JUMP_T_01: {
                soundID = MSD_SE_MV21_JUMP_SMALL_01;
                break;
            }
            case MSD_SE_MV41_JUMP_T_02: {
                soundID = MSD_SE_MV21_JUMP_SMALL_02;
                break;
            }
            case MSD_SE_MV41_JUMP_T_03: {
                soundID = MSD_SE_MV21_JUMP_SMALL_03;
                break;
            }
            case MSD_SE_MV34_ACTION_T_01: {
                soundID = MSD_SE_MV13_ACTION_SMALL_01;
                break;
            }
            case MSD_SE_MV34_ACTION_T_02: {
                soundID = MSD_SE_MV13_ACTION_SMALL_02;
                break;
            }
            case MSD_SE_MV34_ACTION_T_03: {
                soundID = MSD_SE_MV13_ACTION_SMALL_03;
                break;
            }
            case MSD_SE_MV42_JUMP_HUGE_T_01: {
                soundID = MSD_SE_MV25A_JUMP_HUGE_01;
                break;
            }
            case MSD_SE_MV42_JUMP_HUGE_T_02: {
                soundID = MSD_SE_MV25A_JUMP_HUGE_02;
                break;
            }
            case MSD_SE_MV46A_OPEN_DOOR_T_01: {
                soundID = MSD_SE_MV31_OPEN_DOOR_01;
                break;
            }
            case MSD_SE_MV46A_OPEN_DOOR_T_02: {
                soundID = MSD_SE_MV31_OPEN_DOOR_02;
                break;
            }
            case MSD_SE_MV46B_NEXT_STG_T_01:
            case MSD_SE_MV46B_NEXT_STG_T_02:
            case MSD_SE_MV46B_NEXT_STG_T_03: {
                soundID = -2;
                break;
            }
            case MSD_SE_MV38_EXERT_INST_T_01: {
                soundID = MSD_SE_MV15_EXERT_INST_01;
                break;
            }
            default:
                break;
            }
        } else if (player_id == SME::CharacterID::LUIGI) {
            // u32 soundInf     = (u32)((u8 *)gpMSound + 0xB0 + (0xC * ((status & 2) != 0)));
            u32 soundInf     = *(u32 *)((u8 *)sound + 0xB0);
            u32 jai_actor[4] = {
                soundInf,
                soundInf,
                soundInf,
                0,
            };
            switch (soundID) {
            case MSD_SE_MV21_JUMP_SMALL_01:
            case MSD_SE_MV24_JUMP_SPECIAL_01:
            case MSD_SE_MV25A_JUMP_HUGE_01: {
                if (sound->gateCheck(MSD_SE_MV41_JUMP_T_01)) {
                    JAISound **sounds = (JAISound **)((u8 *)sound + 0x8C);
                    JAISound *sound = MSoundSE::startSoundActorInner(MSD_SE_MV41_JUMP_T_01, sounds,
                                                                     (JAIActor *)jai_actor, 1, 4);
                    if (sound) {
                        sound->setPortData(11, 0);
                    }
                }
                *(u32 *)((u8 *)sound + 0x94) = MSD_SE_MV41_JUMP_T_01;
                return MSD_SE_MV41_JUMP_T_01;
            }
            case MSD_SE_MV13_ACTION_SMALL_01:
            case MSD_SE_MV22_JUMP_MID_01: {
                if (sound->gateCheck(MSD_SE_MV34_ACTION_T_01)) {
                    JAISound **sounds = (JAISound **)((u8 *)sound + 0x8C);
                    JAISound *sound   = MSoundSE::startSoundActorInner(
                        MSD_SE_MV34_ACTION_T_01, sounds, (JAIActor *)jai_actor, 1, 4);
                    if (sound) {
                        sound->setPortData(11, 0);
                    }
                }
                *(u32 *)((u8 *)sound + 0x94) = MSD_SE_MV34_ACTION_T_01;
                return MSD_SE_MV34_ACTION_T_01;
            }
            case MSD_SE_MV15_EXERT_INST_01:
            case MSD_SE_MV23_JUMP_LARGE_01: {
                if (sound->gateCheck(MSD_SE_MV38_EXERT_INST_T_01)) {
                    JAISound **sounds = (JAISound **)((u8 *)sound + 0x8C);
                    JAISound *sound   = MSoundSE::startSoundActorInner(
                        MSD_SE_MV38_EXERT_INST_T_01, sounds, (JAIActor *)jai_actor, 1, 4);
                    if (sound) {
                        sound->setPortData(11, 0);
                    }
                }
                *(u32 *)((u8 *)sound + 0x94) = MSD_SE_MV38_EXERT_INST_T_01;
                return MSD_SE_MV38_EXERT_INST_T_01;
            }
            default:
                break;
            }
        } else if (player_id == SME::CharacterID::PIANTISSIMO) {
            switch (soundID) {
            case MSD_SE_MV21_JUMP_SMALL_01:
            case MSD_SE_MV22_JUMP_MID_01:
            case MSD_SE_MV23_JUMP_LARGE_01:
            case MSD_SE_MV24_JUMP_SPECIAL_01:
            case MSD_SE_MV25A_JUMP_HUGE_01: {
                if (sound->gateCheck(MSD_SE_NPC_VA_MTMAN_JUMP1)) {
                    JAISound **sounds = (JAISound **)((u8 *)sound + 0x8C + (0x4 * 1));
                    MSoundSE::startSoundActor(MSD_SE_NPC_VA_MTMAN_JUMP1, player->mTranslation, 0,
                                              sounds, 1, 4);
                }
                return MSD_SE_NPC_VA_MTMAN_JUMP1;
            }
            default:
                return 0xFFFFFFFF;
            }
        }
    }

    /*  local_48 = *(JAISound **)(this + uVar2 * 0xc + 0xb0);
    iVar3    = uVar5 * 4;
    local_3c = 0;
    local_44 = local_48;
    local_40 = local_48;
    MSoundSESystem::MSoundSE::startSoundActorInner((MSoundSE *)param_1,
                                                   (ulong)(this + iVar3 + 0x8c), &local_48,
                                                   (JAIActor *)&DAT_00000001, 4, in_r8);*/

    return sound->startMarioVoice(soundID, health, status);
}

u32 r29PlayerVoices(MSound *sound, u32 soundID, s16 health, u8 status) {
    TMario *player;
    SMS_FROM_GPR(29, player);

    return playerVoiceProcess(player, sound, soundID, health, status);
}
SMS_PATCH_BL(SMS_PORT_REGION(0x80285354, 0, 0, 0), r29PlayerVoices);
SMS_PATCH_BL(SMS_PORT_REGION(0x802853D8, 0, 0, 0), r29PlayerVoices);

u32 r31PlayerVoices(MSound *sound, u32 soundID, s16 health, u8 status) {
    TMario *player;
    SMS_FROM_GPR(31, player);

    return playerVoiceProcess(player, sound, soundID, health, status);
}
SMS_PATCH_BL(SMS_PORT_REGION(0x80285BE8, 0, 0, 0), r31PlayerVoices);
SMS_PATCH_BL(SMS_PORT_REGION(0x80285CA0, 0, 0, 0), r31PlayerVoices);
SMS_PATCH_BL(SMS_PORT_REGION(0x80286014, 0, 0, 0), r31PlayerVoices);
SMS_PATCH_BL(SMS_PORT_REGION(0x80286114, 0, 0, 0), r31PlayerVoices);
SMS_PATCH_BL(SMS_PORT_REGION(0x802861B4, 0, 0, 0), r31PlayerVoices);
SMS_PATCH_BL(SMS_PORT_REGION(0x80286348, 0, 0, 0), r31PlayerVoices);
SMS_PATCH_BL(SMS_PORT_REGION(0x80286398, 0, 0, 0), r31PlayerVoices);
SMS_PATCH_BL(SMS_PORT_REGION(0x80286774, 0, 0, 0), r31PlayerVoices);
SMS_PATCH_BL(SMS_PORT_REGION(0x802867B4, 0, 0, 0), r31PlayerVoices);
SMS_PATCH_BL(SMS_PORT_REGION(0x80286818, 0, 0, 0), r31PlayerVoices);
SMS_PATCH_BL(SMS_PORT_REGION(0x80286858, 0, 0, 0), r31PlayerVoices);
SMS_PATCH_BL(SMS_PORT_REGION(0x802868D4, 0, 0, 0), r31PlayerVoices);
SMS_PATCH_BL(SMS_PORT_REGION(0x80286C24, 0, 0, 0), r31PlayerVoices);
SMS_PATCH_BL(SMS_PORT_REGION(0x80286C64, 0, 0, 0), r31PlayerVoices);
SMS_PATCH_BL(SMS_PORT_REGION(0x80286D08, 0, 0, 0), r31PlayerVoices);
SMS_PATCH_BL(SMS_PORT_REGION(0x80286D48, 0, 0, 0), r31PlayerVoices);
SMS_PATCH_BL(SMS_PORT_REGION(0x80286DA8, 0, 0, 0), r31PlayerVoices);
SMS_PATCH_BL(SMS_PORT_REGION(0x80286DE8, 0, 0, 0), r31PlayerVoices);
SMS_PATCH_BL(SMS_PORT_REGION(0x80286E40, 0, 0, 0), r31PlayerVoices);
SMS_PATCH_BL(SMS_PORT_REGION(0x80286E94, 0, 0, 0), r31PlayerVoices);
SMS_PATCH_BL(SMS_PORT_REGION(0x80286F2C, 0, 0, 0), r31PlayerVoices);
SMS_PATCH_BL(SMS_PORT_REGION(0x80286F6C, 0, 0, 0), r31PlayerVoices);
SMS_PATCH_BL(SMS_PORT_REGION(0x80286FC4, 0, 0, 0), r31PlayerVoices);

u32 omitGenericPlayerVoices(MSound *sound, u32 soundID, s16 health, u8 status) {
    SME::CharacterID player_id = SME::TGlobals::getCharacterIDFromPlayer(gpMarioAddress);
    if (player_id == SME::CharacterID::PIANTISSIMO) {
        if (soundID != MSD_SE_MA_VO_JUMP_BIG_0) {
            return 0xFFFFFFFF;
        }
        soundID = MSD_SE_NPC_VA_MTMAN_JUMP2;  // Shine Get
    }
    return sound->startMarioVoice(soundID, health, status);
}
SMS_PATCH_BL(SMS_PORT_REGION(0x80012D38, 0, 0, 0), omitGenericPlayerVoices);
SMS_PATCH_BL(SMS_PORT_REGION(0x801E6DA0, 0, 0, 0), omitGenericPlayerVoices);
