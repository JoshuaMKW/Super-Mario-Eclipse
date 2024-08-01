#include "player.hxx"
#include <SMS/GC2D/SunGlass.hxx>
#include <SMS/MSound/MSound.hxx>
#include <SMS/Manager/MarioParticleManager.hxx>

static const char *sSunGlassRefName =
    "\x83\x54\x83\x93\x83\x4F\x83\x89\x83\x58\x83\x74\x83\x46\x81\x5B\x83\x5F";

static TSunGlass *sSunGlassRef = nullptr;
static bool sTriedToFind       = false;

void SME::Player::setCold(TMario *player, s16 timer, s16 frequency) {
    auto *data = getEclipseData(player);
    if (data->mColdTimer <= 0) {
        data->mAirDec = player->mSwimParams.mAirDec.get();
        data->mAirInc = player->mSwimParams.mAirInc.get();
        player->mSwimParams.mAirInc.set(player->mSwimParams.mAirInc.get() * 0.5f);
        player->mSwimParams.mAirDec.set(player->mSwimParams.mAirDec.get() * 1.5f);
        if (sSunGlassRef) {
            sSunGlassRef->mBlendFlags |= 2;
            sSunGlassRef->mColor      = {0, 120, 200, 0};
            sSunGlassRef->mToAlpha    = 80;
            sSunGlassRef->mFromAlpha  = 0;
            sSunGlassRef->mLerpTimer  = 0;
            sSunGlassRef->mIsUpdating = true;
        }
    }
    data->mColdDamageTimer = 1;
    data->mColdTimer       = timer;
    data->mColdTimerStart  = timer;
    data->mColdFrequency   = frequency;
}

BETTER_SMS_FOR_CALLBACK void initColdState(TMarDirector *director) {
    sSunGlassRef = nullptr;
    sTriedToFind = false;
}

BETTER_SMS_FOR_CALLBACK void processColdState(TMario *player, bool isMario) {
    if (!isMario) {
        return;
    }

    if (!sTriedToFind && !sSunGlassRef) {
        auto *nameRefRoot = TMarNameRefGen::getInstance()->getRootNameRef();
        sSunGlassRef      = reinterpret_cast<TSunGlass *>(nameRefRoot->searchF(
            JDrama::TNameRef::calcKeyCode(sSunGlassRefName), sSunGlassRefName));
        sTriedToFind      = true;
    }

    if ((player->mState & TMario::STATE_WATERBORN)) {
        if (gpMarDirector->mAreaID == 70) {
            SME::Player::setCold(player, 700, 300);
        }
    }

    auto *data = SME::Player::getEclipseData(player);
    if (data->mColdTimer > 0) {
        if ((data->mColdTimer % 60) == 0) {
            player->startVoiceIfNoVoice(MSD_SE_MV02_DAMAGE_MID_04);
        }
        auto *particle =
            gpMarioParticleManager->emitAndBindToPosPtr(344, &player->mTranslation, 1, player);
        if (particle) {
            particle->mSize1 = {0.7f, 0.7f, 0.7f};
            particle->mSize3 = {0.7f, 0.7f, 0.7f};
        }
        if (--data->mColdTimer == 0) {
            player->mSwimParams.mAirDec.set(data->mAirDec);
            player->mSwimParams.mAirInc.set(data->mAirInc);
            if (sSunGlassRef) {
                sSunGlassRef->mToAlpha    = 0;
                sSunGlassRef->mFromAlpha  = 80;
                sSunGlassRef->mLerpTimer  = 0;
                sSunGlassRef->mIsUpdating = true;
            }
        }
        if ((data->mColdDamageTimer++) % data->mColdFrequency == 0) {
            player->decHP(1);
            data->mColdDamageTimer = 1;
        }
    }
}