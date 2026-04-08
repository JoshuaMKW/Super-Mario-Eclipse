#include <smparticle.hxx>
#include <SMS/MSound/MSound.hxx>
#include <SMS/MSound/MSoundSESystem.hxx>
#include <SMS/rand.h>

void smPlayerInit(TMario *player) {
    SME::CharacterID charID = SME::TGlobals::getCharacterIDFromPlayer(gpMarioAddress);
    if (charID == SME::CharacterID::SHADOW_MARIO) {
        Player::registerData(player, "SMPData", new SMParticleData);
    }
}

void smParticleInit(JPAResourceManager *manager, const char *poggies, u8 numbah) {
    gpResourceManager->load(poggies, numbah);
    gpResourceManager->load("ms_kgm_move_a.jpa", 426);
    gpResourceManager->load("ms_kgm_move_b.jpa", 427);
    gpResourceManager->load("ms_kgm_change.jpa", 237);
}
SMS_PATCH_BL(0x802b44e8, smParticleInit);

void doSMParticle(TMario *player, bool cool) {
    SME::CharacterID charID = SME::TGlobals::getCharacterIDFromPlayer(gpMarioAddress);
    if (charID != SME::CharacterID::SHADOW_MARIO) {
        return;
    }

    SMParticleData *data = (SMParticleData *)Player::getRegisteredData(player, "SMPData");
    SME::TGlobals::getCharacterIDFromPlayer(gpMarioAddress) == SME::CharacterID::SHADOW_MARIO;

    const TVec3f yOffset  = {0.0f, 50.0f, 0.0f};
    data->mParticleOffset = player->mTranslation + yOffset;

    bool mIsLongJump = (player->mState == 0x2000880) && (player->mAnimationID == 246) && !player->mFludd->isEmitting();

    if (mIsLongJump) {
        data->mParticle1 =
            gpMarioParticleManager->emitAndBindToPosPtr(426, &data->mParticleOffset, 1, nullptr);
        data->mParticle2 =
            gpMarioParticleManager->emitAndBindToPosPtr(427, &data->mParticleOffset, 1, nullptr);
        player->mAttributes.mIsVisible = true; //mIsVisible is a bad name i hate it
    } else if (!mIsLongJump) {
        player->mAttributes.mIsVisible = false;
    }
    if (!data->mLastLongJump && mIsLongJump) { //Start
        gpMarioParticleManager->emit(237, &player->mTranslation, 0, nullptr);
        JAISound *sound = MSoundSE::startSoundActor(MSD_SE_MA_BOTTLE_WIND, player->mTranslation, 0, nullptr, 0x0,
                                  4);
        if (sound) {
            sound->setPitch(0.9f + 0.2f * ((f32)rand() / 32767.0f), 3, 0);
            sound->setTempoProportion(1.5f, 3);
        }
    }
    if (data->mLastLongJump && !mIsLongJump) { //End
        gpMarioParticleManager->emit(237, &player->mTranslation, 0, nullptr);
    }
    data->mLastLongJump = mIsLongJump;
}