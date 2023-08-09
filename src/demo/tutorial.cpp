#include <JSystem/JParticle/JPAResourceManager.hxx>
#include <SMS/MSound/MSound.hxx>
#include <SMS/MSound/MSoundSESystem.hxx>
#include <SMS/Manager/FlagManager.hxx>
#include <SMS/raw_fn.hxx>

#include <BetterSMS/libs/boundbox.hxx>
#include <BetterSMS/module.hxx>

#include "settings.hxx"

static BoundingBox sStartingPlatform = {
    {0,   110, 0  },
    {600, 300, 600},
    {0,   180, 0  },
};

static BoundingBox sIcePlatformBoundingBoxes[] = {
    {{-100, 2850, 14000}, {1600, 1000, 1500}, {0, 180, 0}},
    {{0, 11300, 9300},    {2700, 1000, 1500}, {0, 180, 0}},
    {{0, 13200, 6900},    {1400, 1000, 1200}, {0, 180, 0}},
    {{0, 14500, 1600},    {1100, 1500, 2600}, {0, 180, 0}},
    {{0, 14250, -4200},   {1700, 1000, 1200}, {0, 180, 0}},
    {{0, 11850, -13800},  {2800, 800, 3700},  {0, 180, 0}},
};

static int sIceFluddDialogues[] = {97, 98, 99, 103, -1, 100};

static int sIcePlatformIndex = -1;

static BoundingBox sCasinoPlatformBoundingBoxes[] = {
    {{18700, 66000, -18000},  {6000, 800, 6000}, {0, -90, 0}},
    {{-19100, 66000, -18000}, {6000, 800, 6000}, {0, 0, 0}  },
    {{-19100, 66000, 18000},  {6000, 800, 6000}, {0, 90, 0} },
    {{5200, 58100, 18000},    {7200, 800, 7200}, {0, 90, 0} },
    {{18800, 58100, 18000},   {6000, 800, 6000}, {0, 180, 0}},
    {{-4000, 48750, 11200},   {7200, 800, 7200}, {0, 270, 0}},
    {{-17000, 39050, -15500}, {7000, 800, 9000}, {0, 180, 0}},
};

static int sCasinoFluddDialogues[] = {93, 94, 95, -1, -1, -1, 96};

static int sCasinoPlatformIndex = -1;

static BoundingBox sPiantaPitPlatformBoundingBoxes[] = {
    {{-2000, -22400, -25000}, {3000, 800, 3000},  {0, -10, 0}},
    {{-2200, -23150, -17700}, {3000, 800, 1500},  {0, 50, 0} },
    {{5000, -22500, -10000},  {6000, 4000, 6000}, {0, 20, 0} },
    {{7000, -19300, -5800},   {3000, 3000, 5000}, {0, 200, 0}},
    {{3500, -17000, -4600},   {3600, 2000, 3600}, {0, 0, 0}  },
    {{0, -31500, 2500},       {8000, 3000, 8000}, {0, 0, 0}  },
};

static int sPiantaPitFluddDialogues[] = {-1, -1, 101, -1, -1, 102};

static int sPiantaPitPlatformIndex = -1;

static const int *sShouldFluddDialogue          = nullptr;
static const BoundingBox *sCurrentCheckpointBox = nullptr;
static int sActiveFluddDialogue                 = -1;

static inline bool playerIsGrounded(TMario &player, bool includeWater) {
    if (includeWater)
        return (player.mState & TMario::STATE_AIRBORN) == 0;
    else
        return (player.mState & TMario::STATE_AIRBORN) == 0 &&
               (player.mState & TMario::STATE_WATERBORN) == 0;
}

static inline void restoreFluddWater(TWaterGun &fludd) {
    fludd.mCurrentWater = fludd.mNozzleList[fludd.mCurrentNozzle]->mEmitParams.mAmountMax.get();
}

static const BoundingBox *getNearestCheckpointBox(const TVec3f &pos) {
    BoundingBox *nearestBox = nullptr;
    f32 minDist             = 1000000000000.0f;

    if (sIcePlatformIndex >= 0) {
        BoundingBox &box = sIcePlatformBoundingBoxes[sIcePlatformIndex];
        f32 dist         = PSVECSquareDistance(box.center, pos);
        if (dist < minDist) {
            minDist    = dist;
            nearestBox = &box;
        }
    }

    if (sCasinoPlatformIndex >= 0) {
        BoundingBox &box = sCasinoPlatformBoundingBoxes[sCasinoPlatformIndex];
        f32 dist         = PSVECSquareDistance(box.center, pos);
        if (dist < minDist) {
            minDist    = dist;
            nearestBox = &box;
        }
    }

    if (sPiantaPitPlatformIndex >= 0) {
        BoundingBox &box = sPiantaPitPlatformBoundingBoxes[sPiantaPitPlatformIndex];
        f32 dist         = PSVECSquareDistance(box.center, pos);
        if (dist < minDist) {
            minDist    = dist;
            nearestBox = &box;
        }
    }

    return nearestBox;
}

static int processCheckpoints(const BoundingBox *boxes, const int *dialogues, int checkpointCount,
                              int currentCheckpoint, bool includeWater, bool playSound,
                              TMario *player, bool isMario) {
    if (sShouldFluddDialogue && gpMarDirector->mGCConsole->mIsWaterCard) {
        startAppearBalloon__11TGCConsole2FUlb(gpMarDirector->mGCConsole, *sShouldFluddDialogue,
                                              false);
        sActiveFluddDialogue = *sShouldFluddDialogue;
        sShouldFluddDialogue = nullptr;
    }

    if (sActiveFluddDialogue != -1 && sCurrentCheckpointBox &&
        !sCurrentCheckpointBox->contains(player->mTranslation, 1.25f)) {
        startDisappearBalloon__11TGCConsole2FUlb(gpMarDirector->mGCConsole, sActiveFluddDialogue,
                                                 true);
        sActiveFluddDialogue = -1;
    }

    if (!isMario || currentCheckpoint >= checkpointCount || currentCheckpoint < -1)
        return -1;

    if (player->mFloorTriangle->mType == 3333) {
        if (player->mTranslation.y - player->mFloorBelow < 80.0f) {
            if (gpMSound->gateCheck(MSD_SE_SY_NOT_COLLECT_YOSHI)) {
                MSoundSE::startSoundSystemSE(MSD_SE_SY_NOT_COLLECT_YOSHI, 0, nullptr, 0);
            }

            /*const BoundingBox *box = getNearestCheckpointBox(player->mTranslation);
            if (!box)
                return currentCheckpoint;

            OSReport("Center: %f %f %f\n", box->center.x, box->center.y, box->center.z);*/

            auto box = sCurrentCheckpointBox;
            if (!box)
                return currentCheckpoint;

            player->warpRequest(box->center, box->rotation.y);
            player->mLastGroundedPos = box->center;
            if (player->mFludd) {
                restoreFluddWater(*player->mFludd);
            }
        }
        return currentCheckpoint;
    }

    if (playerIsGrounded(*player, includeWater)) {
        for (int i = currentCheckpoint + 1; i < checkpointCount; ++i) {
            if (boxes[i].contains(player->mTranslation)) {
                sCurrentCheckpointBox = &boxes[i];
                currentCheckpoint     = i;
                if (player->mFludd) {
                    restoreFluddWater(*player->mFludd);
                }
                if (dialogues[i] != -1) {
                    sShouldFluddDialogue = &dialogues[i];
                }
                if (i == checkpointCount - 1) {
                    if (playSound && gpMSound->gateCheck(MSD_SE_BS_UNG_TEATH_FLASH)) {
                        MSoundSE::startSoundSystemSE(MSD_SE_BS_UNG_TEATH_FLASH, 0, nullptr, 0);
                    }
                    break;
                } else if (checkpointCount > 0) {
                    if (playSound && gpMSound->gateCheck(MSD_SE_FGM_SOCCER_GOAL)) {
                        auto *sound =
                            MSoundSE::startSoundSystemSE(MSD_SE_FGM_SOCCER_GOAL, 0, nullptr, 0);
                        if (sound) {
                            sound->setPitch(1 + ((i - 1) * 0.1f), 0, 0);
                        }
                    }
                }
            }
        }
    }

    return currentCheckpoint;
}

BETTER_SMS_FOR_CALLBACK void resetTutorialIceStageCheckpoints(TMarDirector *director) {
    sIcePlatformIndex = -1;
}

BETTER_SMS_FOR_CALLBACK void checkTutorialIceStageCheckpoints(TMario *player, bool isMario) {
    if (gpMarDirector->mAreaID != 11 || gpMarDirector->mEpisodeID != 2)
        return;

    BoundingBox entrance = {
        {3000, 250, -615},
        {600,  600, 600 }
    };
    BoundingBox exit = {
        {0,   11950, -14950},
        {600, 300,   600   }
    };
    if (entrance.contains(player->mTranslation)) {
        BoundingBox &box = sIcePlatformBoundingBoxes[0];
        TVec3f target    = {box.center.x, box.center.y + 400, box.center.z};
        player->warpRequest(target, box.rotation.y);
        player->mAttributes.mHasFludd = true;
        player->mFludd->changeNozzle(TWaterGun::Rocket, false);
    } else if (exit.contains(player->mTranslation) && playerIsGrounded(*player, false)) {
        BoundingBox &box = sStartingPlatform;
        TVec3f target    = {box.center.x, box.center.y, box.center.z};
        player->warpRequest(target, box.rotation.y);
        player->mAttributes.mHasFludd = false;
    }
    sIcePlatformIndex =
        processCheckpoints(sIcePlatformBoundingBoxes, sIceFluddDialogues, 6, sIcePlatformIndex,
                           false, sIcePlatformIndex >= 0, player, isMario);
}

BETTER_SMS_FOR_CALLBACK void resetTutorialCasinoStageCheckpoints(TMarDirector *director) {
    sCasinoPlatformIndex = -1;
}

BETTER_SMS_FOR_CALLBACK void checkTutorialCasinoStageCheckpoints(TMario *player, bool isMario) {
    if (gpMarDirector->mAreaID != 11 || gpMarDirector->mEpisodeID != 2)
        return;

    BoundingBox entrance = {
        {-3000, 250, -615},
        {600,   600, 600 }
    };
    BoundingBox exit = {
        {-16950, 38950, -19000},
        {600,    300,   600   }
    };
    if (entrance.contains(player->mTranslation)) {
        BoundingBox &box = sCasinoPlatformBoundingBoxes[0];
        TVec3f target    = {box.center.x, box.center.y + 400, box.center.z};
        player->warpRequest(target, box.rotation.y);
        player->mAttributes.mHasFludd = true;
        player->mFludd->changeNozzle(TWaterGun::Turbo, false);
    } else if (exit.contains(player->mTranslation, 1.0f, BoundingType::Spheroid) &&
               playerIsGrounded(*player, false)) {
        BoundingBox &box = sStartingPlatform;
        TVec3f target    = {box.center.x, box.center.y, box.center.z};
        player->warpRequest(target, box.rotation.y);
        player->mAttributes.mHasFludd = false;
    }
    sCasinoPlatformIndex =
        processCheckpoints(sCasinoPlatformBoundingBoxes, sCasinoFluddDialogues, 7,
                           sCasinoPlatformIndex, true, sCasinoPlatformIndex >= 0, player, isMario);
}

BETTER_SMS_FOR_CALLBACK void resetTutorialPiantaPitStageCheckpoints(TMarDirector *director) {
    sPiantaPitPlatformIndex = -1;
}

BETTER_SMS_FOR_CALLBACK void checkTutorialPiantaPitStageCheckpoints(TMario *player, bool isMario) {
    if (gpMarDirector->mAreaID != 11 || gpMarDirector->mEpisodeID != 2)
        return;

    BoundingBox entrance = {
        {0,   250, 2500},
        {600, 600, 600 }
    };
    BoundingBox exit = {
        {500, -31200, 6360},
        {600, 300,    600 }
    };
    if (entrance.contains(player->mTranslation)) {
        BoundingBox &box = sPiantaPitPlatformBoundingBoxes[0];
        TVec3f target    = {box.center.x, box.center.y + 400, box.center.z};
        player->warpRequest(target, box.rotation.y);
    } else if (exit.contains(player->mTranslation, 1.0f, BoundingType::Spheroid) &&
               playerIsGrounded(*player, false)) {
        BoundingBox &box = sStartingPlatform;
        TVec3f target    = {box.center.x, box.center.y, box.center.z};
        player->warpRequest(target, box.rotation.y);
        player->mAttributes.mHasFludd = false;
    }
    sPiantaPitPlatformIndex = processCheckpoints(
        sPiantaPitPlatformBoundingBoxes, sPiantaPitFluddDialogues, 6, sPiantaPitPlatformIndex,
        true, sPiantaPitPlatformIndex > 0, player, isMario);
}

BETTER_SMS_FOR_CALLBACK void checkTutorialCollisionRespawn(TMario *player, bool isMario) {
    if (!isMario)
        return;

    if (player->mFloorTriangle->mType != 3333)
        return;

    if (player->mTranslation.y - player->mFloorBelow > 80.0f)
        return;

    if (gpMSound->gateCheck(MSD_SE_SY_NOT_COLLECT_YOSHI)) {
        MSoundSE::startSoundSystemSE(MSD_SE_SY_NOT_COLLECT_YOSHI, 0, nullptr, 0);
    }

    const BoundingBox *box = getNearestCheckpointBox(player->mTranslation);
    if (!box)
        return;

    player->warpRequest(box->center, box->rotation.y);
    player->mLastGroundedPos = box->center;
    if (player->mFludd) {
        restoreFluddWater(*player->mFludd);
    }
}