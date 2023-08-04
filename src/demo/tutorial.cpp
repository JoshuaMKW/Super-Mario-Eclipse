#include <JSystem/JParticle/JPAResourceManager.hxx>
#include <SMS/MSound/MSound.hxx>
#include <SMS/MSound/MSoundSESystem.hxx>
#include <SMS/Manager/FlagManager.hxx>
#include <SMS/raw_fn.hxx>

#include <BetterSMS/libs/boundbox.hxx>
#include <BetterSMS/module.hxx>

#include "settings.hxx"

static BoundingBox sIcePlatformBoundingBoxes[] = {
    {{-100, 2800, 14000}, {1600, 300, 1500}},
    {{0, 11300, 9300},    {2700, 300, 1500}},
    {{0, 13200, 6900},    {1400, 300, 1200}},
    {{0, 14500, 1600},    {1100, 300, 2600}},
    {{0, 14250, -4200},   {1700, 300, 1200}},
    {{0, 11850, -13800},  {2800, 300, 3700}},
};

static int sIcePlatformIndex = 0;

BETTER_SMS_FOR_CALLBACK void resetTutorialIceStageCheckpoints(TMarDirector *director) {
    sIcePlatformIndex = 0;
}

BETTER_SMS_FOR_CALLBACK void checkTutorialIceStageCheckpoints(TMario *player, bool isMario) {
    if (!isMario)
        return;

    if (player->mFloorTriangle->mType == 3333) {
        if (player->mTranslation.y - player->mFloorBelow < 15.0f) {
            if (gpMSound->gateCheck(MSD_SE_SY_NOT_COLLECT_YOSHI)) {
                MSoundSE::startSoundSystemSE(MSD_SE_SY_NOT_COLLECT_YOSHI, 0, nullptr, 0);
            }
            player->warpRequest(sIcePlatformBoundingBoxes[sIcePlatformIndex].center, 180.0f);
        }
    }

    if (!(player->mState & TMario::STATE_AIRBORN) && !(player->mState & TMario::STATE_WATERBORN)) {
        for (int i = sIcePlatformIndex + 1; i < 6; ++i) {
            if (sIcePlatformBoundingBoxes[i].contains(player->mTranslation)) {
                sIcePlatformIndex = i;
                if (i == 0) {
                    continue;
                }
                if (i == 5) {
                    if (gpMSound->gateCheck(MSD_SE_SY_GET_SHINE)) {
                        MSoundSE::startSoundSystemSE(MSD_SE_SY_GET_SHINE, 0, nullptr, 0);
                    }
                    break;
                } else {
                    if (gpMSound->gateCheck(MSD_SE_FGM_SOCCER_GOAL)) {
                        auto *sound =
                            MSoundSE::startSoundSystemSE(MSD_SE_FGM_SOCCER_GOAL, 0, nullptr, 0);
                        if (sound) {
                            sound->setPitch(1 + ((i - 1) * 0.1f), 0, 0);
                        }
                    }
                }
                break;
            }
        }
    }
}