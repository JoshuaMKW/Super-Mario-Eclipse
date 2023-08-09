#include <JSystem/JParticle/JPAResourceManager.hxx>
#include <SMS/MSound/MSound.hxx>
#include <SMS/MSound/MSoundSESystem.hxx>
#include <SMS/Manager/FlagManager.hxx>
#include <SMS/raw_fn.hxx>

#include <BetterSMS/module.hxx>

#include "settings.hxx"

static void checkIceStageSweat() {
    TMario *player;
    SMS_FROM_GPR(31, player);

    if (gpMarDirector->mAreaID == 13 && gpMarDirector->mEpisodeID == 0)
        return;

    *reinterpret_cast<u32 *>(&player->mAttributes) |= 0x20;
}
SMS_PATCH_BL(SMS_PORT_REGION(0x8024FC58, 0, 0, 0), checkIceStageSweat);

static void loadIceStageWalkSandA(JPAResourceManager *manager, const char *old_res, u16 id) {
    if ((gpMarDirector->mAreaID == 13 && gpMarDirector->mEpisodeID == 0) ||
        (gpMarDirector->mAreaID == 11 && gpMarDirector->mEpisodeID == 2))
        manager->load("ms_m_walksnow_a.jpa", id);
    else
        manager->load(old_res, id);
}
SMS_PATCH_BL(0x802B3ED0, loadIceStageWalkSandA);

static void loadIceStageWalkSandB(JPAResourceManager *manager, const char *old_res, u16 id) {
    if ((gpMarDirector->mAreaID == 13 && gpMarDirector->mEpisodeID == 0) ||
        (gpMarDirector->mAreaID == 11 && gpMarDirector->mEpisodeID == 2))
        manager->load("ms_m_walksnow_b.jpa", id);
    else
        manager->load(old_res, id);
}
SMS_PATCH_BL(0x802B3EF8, loadIceStageWalkSandA);

static void loadIceStageSlideSandA(JPAResourceManager *manager, const char *old_res, u16 id) {
    if ((gpMarDirector->mAreaID == 13 && gpMarDirector->mEpisodeID == 0) ||
        (gpMarDirector->mAreaID == 11 && gpMarDirector->mEpisodeID == 2))
        manager->load("ms_m_slidesnow_a.jpa", id);
    else
        manager->load(old_res, id);
}
SMS_PATCH_BL(0x802B4448, loadIceStageWalkSandA);

static void loadIceStageSlideSandB(JPAResourceManager *manager, const char *old_res, u16 id) {
    if ((gpMarDirector->mAreaID == 13 && gpMarDirector->mEpisodeID == 0) ||
        (gpMarDirector->mAreaID == 11 && gpMarDirector->mEpisodeID == 2))
        manager->load("ms_m_slidesnow_b.jpa", id);
    else
        manager->load(old_res, id);
}
SMS_PATCH_BL(0x802B4470, loadIceStageWalkSandA);

static bool checkSpecialBlueCoinSound(MSound *msound, u32 soundID) {
    return msound->gateCheck(TFlagManager::smInstance->getFlag(0x40001) >= 10 ? MSD_SE_SY_GET_SHINE
                                                                              : soundID);
}
SMS_PATCH_BL(SMS_PORT_REGION(0x8029A76C, 0, 0, 0), checkSpecialBlueCoinSound);

static bool sShineTradeReady = false;

static void makeSpecialBlueCoinSound(u32 soundID, const Vec *vec, u32 _0, JAISound **_1, u32 _2,
                                     u8 _3) {
    if (TFlagManager::smInstance->getFlag(0x40001) % 10 == 0) {
        MSoundSE::startSoundActor(MSD_SE_SY_GET_SHINE, vec, _0, _1, _2, _3);
        sShineTradeReady = true;
    } else {
        MSoundSE::startSoundActor(soundID, vec, _0, _1, _2, _3);
    }
}
SMS_PATCH_BL(SMS_PORT_REGION(0x8029A76C, 0, 0, 0), makeSpecialBlueCoinSound);

void checkForBlueCoinTrade(TMarDirector *director) {
    if (sShineTradeReady) {
        for (int id = 70; id < 80; ++id) {
            if (TFlagManager::smInstance->getShineFlag(id))
                continue;
            TFlagManager::smInstance->setShineFlag(id);
            break;
        }
        sShineTradeReady = false;
    }
}

// Disable guide menu
SMS_WRITE_32(SMS_PORT_REGION(0x80297A64, 0, 0, 0), 0x4800000C);

// Disable fludd balloons
//SMS_WRITE_32(0x8014A1EC, 0x4E800020);