#include <BetterSMS/module.hxx>

#include <SMS/MSound/MSound.hxx>
#include <SMS/MSound/MSoundSESystem.hxx>

#include "stage.hxx"

static void startSoundActorInnerForNPC(u32 soundId, JAISound **sound, JAIActor *origin, u32 param_4,
                                       u8 param_5) {
    if (gpMarDirector->mAreaID == SME::STAGE_YOSHI_VILLAGE) {
        soundId = MSD_SE_YV_YOSHI2;
    }
    MSoundSE::startSoundActorInner(soundId, sound, origin, param_4, param_5);
}
SMS_PATCH_BL(SMS_PORT_REGION(0x80017F9C, 0, 0, 0), startSoundActorInnerForNPC);

static void startSoundActorInnerForNPCTalk(u32 soundId, u32 param_2, JAISound **sound,
                                           u32 param_4) {
    if (gpMarDirector->mAreaID == SME::STAGE_YOSHI_VILLAGE && soundId != MSD_SE_SY_TALK_MODE_IN) {
        soundId = MSD_SE_YV_YOSHI1;
    }

    OSReport("startSoundActorInnerForNPCTalk: %X\n", soundId);

    if (gpMarDirector->mAreaID == TGameSequence::AREA_DOLPIC && gpMarDirector->mEpisodeID == 4) {
        if (soundId == MSD_SE_NPC_VM_SUNFP_JOY) {
            soundId = MSD_SE_BS_KOOPA_VO_LAUGH;
        } else if (soundId == MSD_SE_NPC_VM_SUNFP_SAD) {
            soundId = MSD_SE_BS_KOOPA_VO_GROAN_0;
        }
    }

    MSoundSE::startSoundSystemSE(soundId, param_2, sound, param_4);
}
SMS_PATCH_BL(SMS_PORT_REGION(0x80153E90, 0, 0, 0), startSoundActorInnerForNPCTalk);