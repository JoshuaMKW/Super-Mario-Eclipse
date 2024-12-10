#include <JSystem/JDrama/JDRNameRef.hxx>
#include <JSystem/JParticle/JPAResourceManager.hxx>

#include <SMS/GC2D/SelectMenu.hxx>
#include <SMS/MSound/MSound.hxx>
#include <SMS/MSound/MSoundSESystem.hxx>
#include <SMS/Manager/FlagManager.hxx>
#include <SMS/raw_fn.hxx>

#include <BetterSMS/libs/boundbox.hxx>
#include <BetterSMS/module.hxx>
#include <BetterSMS/objects/generic.hxx>

#include "globals.hxx"
#include "settings.hxx"
#include "stage.hxx"

extern bool gHadLuigiBefore;
extern bool gHadPiantissimoBefore;

bool gFromShineSelectForIntro = false;

static void setStageAfterShineSelect(TSelectMenu *menu) {
    gpApplication.mNextScene.mEpisodeID = menu->mEpisodeID;
    gFromShineSelectForIntro            = true;

    // Reset coins
    TFlagManager::smInstance->setFlag(0x40002, 0);

    // Reset balloons
    TFlagManager::smInstance->setFlag(0x30005, 0);

    switch (gpApplication.mPrevScene.mAreaID) {
    case TGameSequence::AREA_SIRENA: {
        if (gpApplication.mCurrentScene.mAreaID != TGameSequence::AREA_PINNABEACH) {
            break;
        }
        gpApplication.mNextScene.mAreaID = TGameSequence::AREA_PINNAPARCO;
        switch (menu->mEpisodeID) {
        case 0:
            gpApplication.mNextScene.mEpisodeID = 0;
            break;
        case 1:
        case 2:
            gpApplication.mNextScene.mEpisodeID = 1;
            break;
        case 3:
        case 4:
            gpApplication.mNextScene.mEpisodeID = 2;
            break;
        case 5:
            gpApplication.mNextScene.mEpisodeID = 3;
            break;
        case 6:
            gpApplication.mNextScene.mEpisodeID = 4;
            break;
        case 7:
            gpApplication.mNextScene.mEpisodeID = 5;
            break;
        }
        break;
    }
    default:
        break;
    }
}
SMS_PATCH_BL(0x80176038, setStageAfterShineSelect);
SMS_WRITE_32(0x80176044, 0x60000000);

#pragma region CruiserDelfino

static bool s_cruiser_checked = false;

BETTER_SMS_FOR_CALLBACK void resetCruiserUnlocked(TMarDirector *director) {
    s_cruiser_checked = false;
}

BETTER_SMS_FOR_CALLBACK void checkForCruiserUnlocked(TMarDirector *director) {
    if (s_cruiser_checked)
        return;

    s_cruiser_checked = true;

    bool post_corona = TFlagManager::smInstance->getBool(0x10077);  // Corona Mountain beaten

    {
        auto *nameRef = TMarNameRefGen::getInstance()->getRootNameRef();
        u16 keyCode   = JDrama::TNameRef::calcKeyCode("mini_cruiser");
        if (JDrama::TNameRef *p = nameRef->searchF(keyCode, "mini_cruiser")) {
            TGenericRailObj *cruiser = reinterpret_cast<TGenericRailObj *>(p);
            if (post_corona) {
                cruiser->makeObjAppeared();
            } else {
                cruiser->makeObjDead();
            }
        }
    }

    {
        auto *nameRef = TMarNameRefGen::getInstance()->getRootNameRef();
        u16 keyCode   = JDrama::TNameRef::calcKeyCode("brush_object");
        if (JDrama::TNameRef *p = nameRef->searchF(keyCode, "brush_object")) {
            TGenericRailObj *brush = reinterpret_cast<TGenericRailObj *>(p);
            if (post_corona) {
                brush->makeObjAppeared();
            } else {
                brush->makeObjDead();
            }
        }
    }
}

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

#pragma region OutsideDelfinoBehavior

BETTER_SMS_FOR_CALLBACK void updateWarpStatesForCruiserCabin(TMarDirector *director) {
    if (director->mAreaID != SME::STAGE_CRUISER) {
        return;
    }

    size_t shine_count = TFlagManager::smInstance->getFlag(0x40000);

    J3DGXColor enabled_color  = {0, 255, 0, 255};
    J3DGXColor disabled_color = {200, 50, 50, 255};
    J3DGXColor off_color      = {0, 0, 0, 255};
    J3DGXColor on_color       = {255, 255, 255, 255};

    JDrama::TNameRef *name_ref = TMarNameRefGen::getInstance()->getRootNameRef();

    {
        bool is_enabled = shine_count >= 120;

        u16 key_code = JDrama::TNameRef::calcKeyCode("LanciaLight");
        if (JDrama::TNameRef *p = name_ref->searchF(key_code, "LanciaLight")) {
            TGenericRailObj *light_obj = reinterpret_cast<TGenericRailObj *>(p);
            J3DTevStage *tev_stage     = light_obj->mActorData->mModel->mModelData->mStages[0];
            J3DTevBlock *tev_block     = *(J3DTevBlock **)((u8 *)tev_stage + 0x28);
            tev_block->setTevKColor(0, is_enabled ? enabled_color : disabled_color);

            if (is_enabled &&
                PSVECDistance(gpMarioAddress->mTranslation, light_obj->mTranslation) < 250.0f) {
                director->setNextStage(SME::STAGE_LANCIA, nullptr);
            }
        }
    }
    {
        bool is_enabled = shine_count >= 100;

        u16 key_code = JDrama::TNameRef::calcKeyCode("VaporLight");
        if (JDrama::TNameRef *p = name_ref->searchF(key_code, "VaporLight")) {
            TGenericRailObj *light_obj = reinterpret_cast<TGenericRailObj *>(p);
            J3DTevStage *tev_stage     = light_obj->mActorData->mModel->mModelData->mStages[0];
            J3DTevBlock *tev_block     = *(J3DTevBlock **)((u8 *)tev_stage + 0x28);
            tev_block->setTevKColor(0, is_enabled ? on_color : off_color);

            if (is_enabled &&
                PSVECDistance(gpMarioAddress->mTranslation, light_obj->mTranslation) < 200.0f) {
                director->setNextStage(SME::STAGE_VAPORWAVE, nullptr);
            }
        }
    }

    {
        bool is_enabled = shine_count >= 140;

        u16 key_code = JDrama::TNameRef::calcKeyCode("CityLight");
        if (JDrama::TNameRef *p = name_ref->searchF(key_code, "CityLight")) {
            TGenericRailObj *light_obj = reinterpret_cast<TGenericRailObj *>(p);
            J3DTevStage *tev_stage     = light_obj->mActorData->mModel->mModelData->mStages[0];
            J3DTevBlock *tev_block     = *(J3DTevBlock **)((u8 *)tev_stage + 0x28);
            tev_block->setTevKColor(0, is_enabled ? enabled_color : disabled_color);

            if (is_enabled &&
                PSVECDistance(gpMarioAddress->mTranslation, light_obj->mTranslation) < 250.0f) {
                director->setNextStage(SME::STAGE_RED_LILY, nullptr);
            }
        }
    }

    {
        bool is_enabled = shine_count >= 180;

        u16 key_code = JDrama::TNameRef::calcKeyCode("YoshiLight");
        if (JDrama::TNameRef *p = name_ref->searchF(key_code, "YoshiLight")) {
            TGenericRailObj *light_obj = reinterpret_cast<TGenericRailObj *>(p);
            J3DTevStage *tev_stage     = light_obj->mActorData->mModel->mModelData->mStages[0];
            J3DTevBlock *tev_block     = *(J3DTevBlock **)((u8 *)tev_stage + 0x28);
            tev_block->setTevKColor(0, is_enabled ? enabled_color : disabled_color);

            if (is_enabled &&
                PSVECDistance(gpMarioAddress->mTranslation, light_obj->mTranslation) < 250.0f) {
                director->setNextStage(SME::STAGE_YOSHI_VILLAGE, nullptr);
            }
        }
    }

    {
        bool is_enabled = shine_count >= 160;

        u16 key_code = JDrama::TNameRef::calcKeyCode("PeachLight");
        if (JDrama::TNameRef *p = name_ref->searchF(key_code, "PeachLight")) {
            TGenericRailObj *light_obj = reinterpret_cast<TGenericRailObj *>(p);
            J3DTevStage *tev_stage     = light_obj->mActorData->mModel->mModelData->mStages[0];
            J3DTevBlock *tev_block     = *(J3DTevBlock **)((u8 *)tev_stage + 0x28);
            tev_block->setTevKColor(0, is_enabled ? enabled_color : disabled_color);

            if (is_enabled &&
                PSVECDistance(gpMarioAddress->mTranslation, light_obj->mTranslation) < 250.0f) {
                director->setNextStage(SME::STAGE_PEACH_BEACH, nullptr);
            }
        }
    }
}

// This allows the redcoinswitch to be immediately pressed in extended stages
// and for the player to have Fludd, while still being an ex stage.
static u8 disableExBehaviorForCruiserWorlds(u8 areaID) {
    if (areaID == SME::STAGE_VAPORWAVE_EX || areaID == SME::STAGE_YOSHI_EX ||
        areaID == SME::STAGE_PEACH_BEACH_EX || areaID == SME::STAGE_PEACH_CASTLE ||
        areaID == SME::STAGE_ISLE_DELFINO) {
        return 0xFF;
    }
    return SMS_getShineIDofExStage__FUc(gpApplication.mCurrentScene.mAreaID);
}
SMS_PATCH_BL(0x801C08EC, disableExBehaviorForCruiserWorlds);
SMS_PATCH_BL(0x80298B64, disableExBehaviorForCruiserWorlds);

// Cutscene to replace (ex cutscene: 5)
static u8 disableExCutsceneForAll(u8 areaID) { return 0xFF; }
SMS_PATCH_BL(0x802A681C, disableExCutsceneForAll);

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

static void checkHinoKuri2Level(void *hinokuri, int level) {
    if (gpMarDirector->mAreaID == SME::STAGE_LACRIMA_BACKHOUSE) {
        setLevel__10THinokuri2Fi(hinokuri, 2);
    } else {
        setLevel__10THinokuri2Fi(hinokuri, level);
    }
}
SMS_PATCH_BL(0x8005d5e4, checkHinoKuri2Level);
SMS_WRITE_32(0x8005b54c, 0x60000000);

#pragma region 2DWorldBehavior

static BoundingBox s_begin_bound_box = {
    {-14900, 24000, 0   },
    {6000,   16000, 4000},
    {0,      0,     0   }
};

static BoundingBox s_secret_a_bound_box = {
    {14900, 21300, 200 },
    {10000, 11000, 4000},
    {0,     0,     0   }
};

BETTER_SMS_FOR_CALLBACK void forcePlayerZOn2D(TMario *player, bool isMario) {
    if (gpMarDirector->mAreaID != 88) {
        return;
    }

    if (s_secret_a_bound_box.contains(player->mTranslation)) {
        player->mTranslation.z = clamp<f32>(player->mTranslation.z, 150.0f, 250.0f);
        return;
    }

    if (!s_begin_bound_box.contains(player->mTranslation)) {
        player->mTranslation.z = clamp<f32>(player->mTranslation.z, -50.0f, 50.0f);
        return;
    }
}

static void checkRideMovementCond(TMario *player) {
    if (gpMarDirector->mAreaID != SME::STAGE_YOSHI_EX) {
        player->checkRideMovement();
    }
}
SMS_PATCH_BL(0x8024DF54, checkRideMovementCond);

#pragma endregion

BETTER_SMS_FOR_CALLBACK void resetCoinsOnStageExit(TApplication *app) {
    // Set coins to 0
    if (SMS_getShineStage__FUc(gpApplication.mCurrentScene.mAreaID) !=
        SMS_getShineStage__FUc(gpApplication.mNextScene.mAreaID)) {
        TFlagManager::smInstance->setFlag(0x40002, 0);
    }
}

BETTER_SMS_FOR_CALLBACK void resetStateForStage(TMarDirector *director) {
    gHadLuigiBefore       = TFlagManager::smInstance->getBool(0x30018);
    gHadPiantissimoBefore = TFlagManager::smInstance->getBool(0x30019);

    if (director->mAreaID == TGameSequence::AREA_OPTION) {
        SME::TGlobals::sCharacterIDList[0] = SME::CharacterID::MARIO;
        SME::TGlobals::sCharacterIDList[1] = SME::CharacterID::LUIGI;
        SME::TGlobals::sCharacterIDList[2] = SME::CharacterID::PIANTISSIMO;
        SME::TGlobals::sCharacterIDList[3] = SME::CharacterID::SHADOW_MARIO;
        TFlagManager::smInstance->resetCard();
        memset(((u8 *)TFlagManager::smInstance) + 0xF4, 0, 0x18C);
        return;
    }

    if (director->mAreaID == TGameSequence::AREA_DOLPIC) {
        if (director->mEpisodeID == 4) {
            SME::TGlobals::sCharacterIDList[0] = SME::CharacterID::MARIO;
        }
    }

    // Reset shine select behavior flag
    TFlagManager::smInstance->setBool(false, 0x50010);

    if (SMS_getShineStage__FUc(gpApplication.mCurrentScene.mAreaID) !=
        SMS_getShineStage__FUc(gpApplication.mPrevScene.mAreaID)) {
        TFlagManager::smInstance->setFlag(0x40004, TWaterGun::Hover);
    }

    // Force unlock maregate at 239 shines or more
    if (TFlagManager::smInstance->getFlag(0x40000) >= 239) {
        TFlagManager::smInstance->setBool(true, 0x50004);
    }

    // Force unlock all gates, yoshi, and nozzles post-corona mountain
    if (TFlagManager::smInstance->getBool(0x10077)) {
        // Set the nozzle flags
        *((u8 *)TFlagManager::smInstance + 0x6C) |= 0xC0;
        *((u8 *)TFlagManager::smInstance + 0x6D) |= 0xFF;
        *((u8 *)TFlagManager::smInstance + 0x6E) |= 0x7F;

        TFlagManager::smInstance->setBool(true, 0x10384);  // Bianco gate
        TFlagManager::smInstance->setBool(true, 0x10385);  // M on statue
        TFlagManager::smInstance->setBool(true, 0x10386);  // Ricco gate
        TFlagManager::smInstance->setBool(true, 0x10387);  // Gelato gate
        TFlagManager::smInstance->setBool(true, 0x1038F);  // Yoshi unlocked
    }
}

// Disable guide menu
SMS_WRITE_32(SMS_PORT_REGION(0x80297A64, 0, 0, 0), 0x4800000C);

// Disable fludd balloons
// SMS_WRITE_32(0x8014A1EC, 0x4E800020);

static bool isMareGateVisible() {
    if (gpMarDirector->mAreaID == TGameSequence::AREA_DOLPIC && gpMarDirector->mEpisodeID == 3) {
        return true;
    }
    if (gpMarDirector->mAreaID == TGameSequence::AREA_DOLPIC) {
        if (gpMarDirector->mEpisodeID == 4 || gpMarDirector->mEpisodeID == 9) {
            return false;
        }
    }
    return TFlagManager::smInstance->getBool(0x50004);
}
SMS_PATCH_BL(0x801E7124, isMareGateVisible);

static u32 getScenarioForLateDolpic() {
    size_t shine_count = TFlagManager::smInstance->getFlag(0x40000);
    if (shine_count >= MaxShineCount) {
        return 3;
    } else if (shine_count == MaxShineCount - 1) {
        return 4;
    }
    return 2;
}
SMS_PATCH_BL(0x8029962C, getScenarioForLateDolpic);

static u32 s_shadow_mario_shines[] = {0x6, 0x10, 0x1A, 0x24, 0x2E, 0x38, 0x42, 126, 136, 146, 156};

static u32 checkIsFloodedPlazaCriteria() {
    for (u32 shine_id : s_shadow_mario_shines) {
        if (!TFlagManager::smInstance->getShineFlag(shine_id)) {
            return false;
        }
    }
    return true;
}
SMS_PATCH_BL(0x80299644, checkIsFloodedPlazaCriteria);
SMS_PATCH_B(0x8029964C, 0x80299670);

// DEBS 80142a00

s32 s_dolpic_11_debs_list[]  = {22, 23, 24, -1};
s32 s_final_boss_debs_list[] = {20, 21, -1};

SMS_ASM_FUNC static void setDolpic11DEBSList(TGCConsole2 *console2) {
    SMS_ASM_BLOCK("lis 3, s_dolpic_11_debs_list@h      \r\t"
                  "ori 3, 3, s_dolpic_11_debs_list@l   \r\t"
                  "stw 3, 0x574 (30)                   \r\t"
                  "lis 3, 0x8014                       \r\t"
                  "ori 3, 3, 0x5D04                    \r\t"
                  "mtctr 3                             \r\t"
                  "bctr                                \r\t");
}
SMS_WRITE_32(0x803C0360, setDolpic11DEBSList);

SMS_ASM_FUNC static void setFinalBossDEBSList(TGCConsole2 *console2) {
    SMS_ASM_BLOCK("lis 3, s_final_boss_debs_list@h      \r\t"
                  "ori 3, 3, s_final_boss_debs_list@l   \r\t"
                  "stw 3, 0x574 (30)                    \r\t"
                  "lis 3, 0x8014                        \r\t"
                  "ori 3, 3, 0x5D04                     \r\t"
                  "mtctr 3                              \r\t"
                  "bctr                                 \r\t");
}
SMS_WRITE_32(0x803C0364, setFinalBossDEBSList);

static bool checkForMareGate(JDrama::TNameRef *actor, u16 keycode, const char *name) {
    if (gpMarDirector->mAreaID == TGameSequence::AREA_DOLPIC && gpMarDirector->mEpisodeID == 3) {
        TFlagManager::smInstance->setBool(true, 0x50004);
    }
    if (gpMarDirector->mAreaID == TGameSequence::AREA_DOLPIC && gpMarDirector->mEpisodeID == 4) {
        return false;
    }
    return TFlagManager::smInstance->getBool(0x50004);
}
SMS_PATCH_BL(0x8002e5d0, checkForMareGate);

// Jump table for episodes: 803c0354
// 80145cf0 is null