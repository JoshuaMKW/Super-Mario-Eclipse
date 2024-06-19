#include <JSystem/JParticle/JPAResourceManager.hxx>
#include <SMS/MSound/MSound.hxx>
#include <SMS/MSound/MSoundSESystem.hxx>
#include <SMS/Manager/FlagManager.hxx>
#include <SMS/raw_fn.hxx>

#include <BetterSMS/module.hxx>

#include "settings.hxx"

#pragma region SnowParticles

static void checkIceStageSweat() {
    TMario *player;
    SMS_FROM_GPR(31, player);

    if (gpMarDirector->mAreaID == 70)
        return;

    *reinterpret_cast<u32 *>(&player->mAttributes) |= 0x20;
}
SMS_PATCH_BL(SMS_PORT_REGION(0x8024FC58, 0, 0, 0), checkIceStageSweat);

static void loadIceStageWalkSandA(JPAResourceManager *manager, const char *old_res, u16 id) {
    if (gpMarDirector->mAreaID == 70 || gpMarDirector->mAreaID == 80 || gpMarDirector->mAreaID == 86)
        manager->load("ms_m_walksnow_a.jpa", id);
    else
        manager->load(old_res, id);
}
SMS_PATCH_BL(0x802B3ED0, loadIceStageWalkSandA);

static void loadIceStageWalkSandB(JPAResourceManager *manager, const char *old_res, u16 id) {
    if (gpMarDirector->mAreaID == 70 || gpMarDirector->mAreaID == 80 ||
        gpMarDirector->mAreaID == 86)
        manager->load("ms_m_walksnow_b.jpa", id);
    else
        manager->load(old_res, id);
}
SMS_PATCH_BL(0x802B3EF8, loadIceStageWalkSandA);

static void loadIceStageSlideSandA(JPAResourceManager *manager, const char *old_res, u16 id) {
    if (gpMarDirector->mAreaID == 70 || gpMarDirector->mAreaID == 80 ||
        gpMarDirector->mAreaID == 86)
        manager->load("ms_m_slidesnow_a.jpa", id);
    else
        manager->load(old_res, id);
}
SMS_PATCH_BL(0x802B4448, loadIceStageWalkSandA);

static void loadIceStageSlideSandB(JPAResourceManager *manager, const char *old_res, u16 id) {
    if (gpMarDirector->mAreaID == 70 || gpMarDirector->mAreaID == 80 ||
        gpMarDirector->mAreaID == 86)
        manager->load("ms_m_slidesnow_b.jpa", id);
    else
        manager->load(old_res, id);
}
SMS_PATCH_BL(0x802B4470, loadIceStageWalkSandA);

#pragma endregion

// Disable guide menu
SMS_WRITE_32(SMS_PORT_REGION(0x80297A64, 0, 0, 0), 0x4800000C);

// Disable fludd balloons
// SMS_WRITE_32(0x8014A1EC, 0x4E800020);