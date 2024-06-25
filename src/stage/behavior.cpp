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
    if (gpMarDirector->mAreaID == 70 || gpMarDirector->mAreaID == 80 ||
        gpMarDirector->mAreaID == 86)
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

#pragma region OutsideDelfinoExBehavior

// This allows the redcoinswitch to be immediately pressed in extended stages
// and for the player to have Fludd, while still being an ex stage.
static u8 disableExBehaviorForCruiserWorlds() {
    if (gpApplication.mCurrentScene.mAreaID > 0x54) {
        return 0xFF;
    }
    return SMS_getShineIDofExStage__FUc(gpApplication.mCurrentScene.mAreaID);
}
SMS_PATCH_BL(0x801C08EC, disableExBehaviorForCruiserWorlds);
SMS_PATCH_BL(0x80298B64, disableExBehaviorForCruiserWorlds);
SMS_PATCH_BL(0x802A681C, disableExBehaviorForCruiserWorlds);

#pragma endregion

// Called when a fire is extinguished
static MSound *checkForFireIncrement() {
    TFlagManager::smInstance->incFlag(0x60020, 1);
    return gpMSound;
}
SMS_PATCH_BL(0x80037F04, checkForFireIncrement);

static void checkForSamboFlowerIncrement(MActor *actor, const char *hit_anm_name) {
    actor->setBck(hit_anm_name);
    TFlagManager::smInstance->incFlag(0x60021, 1);
}
SMS_PATCH_BL(0x800E4174, checkForSamboFlowerIncrement);

static void checkForSamboFlowerDecrement(MActor *actor, const char *hit_anm_name) {
    actor->checkCurAnm(hit_anm_name, MActor::BCK);
    TFlagManager::smInstance->incFlag(0x60021, -1);
}
SMS_PATCH_BL(0x800E3F9C, checkForSamboFlowerDecrement);

// Disable guide menu
SMS_WRITE_32(SMS_PORT_REGION(0x80297A64, 0, 0, 0), 0x4800000C);

// Disable fludd balloons
// SMS_WRITE_32(0x8014A1EC, 0x4E800020);