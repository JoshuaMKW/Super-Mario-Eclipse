#include <BetterSMS/libs/global_vector.hxx>

#include <BetterSMS/area.hxx>
#include <BetterSMS/module.hxx>
#include <BetterSMS/stage.hxx>
#include <SMS/Manager/FlagManager.hxx>

#include "stage.hxx"

using namespace BetterSMS;

struct SpawnInfo {
    s32 m_area_id    = -1;
    s32 m_episode_id = -1;
    TVec3f m_spawn_pos;
    TVec3f m_spawn_rot;
};

static TGlobalVector<SpawnInfo> s_area_spawn_map[256];

void SME::setSpawnTransRot(s32 stage, TVec3f &trans, TVec3f &rot, s32 from_stage,
                           s32 from_episode) {
    SpawnInfo info;
    info.m_area_id    = from_stage;
    info.m_episode_id = from_episode;
    info.m_spawn_pos  = trans;
    info.m_spawn_rot  = rot;
    s_area_spawn_map[stage].push_back(info);
}

bool SME::getSpawnTransRot(TApplication *app, TVec3f &trans, TVec3f &rot) {
    const TGlobalVector<SpawnInfo> &spawn_info = s_area_spawn_map[app->mCurrentScene.mAreaID];
    for (const SpawnInfo &info : spawn_info) {
        if (info.m_area_id == -1) {
            trans = info.m_spawn_pos;
            rot   = info.m_spawn_rot;
            return true;
        }

        if (info.m_area_id != app->mPrevScene.mAreaID) {
            continue;
        }

        if (info.m_episode_id == -1 || info.m_episode_id == app->mPrevScene.mEpisodeID) {
            trans = info.m_spawn_pos;
            rot   = info.m_spawn_rot;
            return true;
        }
    }
    return false;
}

BETTER_SMS_FOR_CALLBACK void initializeStageInfo(TApplication *app) {
    {
        auto *old_heap = JKRHeap::sRootHeap->becomeCurrentHeap();

        // Register Erto Rock stage info
        {
            Stage::ShineAreaInfo *info = new Stage::ShineAreaInfo(SME::STAGE_ERTO, 'er_0');
            info->addScenario(0x78, 72);
            info->addScenario(0x79, 73);
            info->addScenario(0x7A, 74);
            info->addScenario(0x7B, 75);
            info->addScenario(0x7C, 76);
            info->addScenario(0x7D, 77);
            info->addScenario(0x7E, 78);
            info->addScenario(0x7F, 79);
            info->addExScenario(0x80, 80);
            info->addExScenario(0x81, 81);
            Stage::registerShineStage(info);
            Stage::registerNormalStage(SME::STAGE_ERTO, info->getShineStageID());
            Stage::registerExStage(SME::STAGE_ERTO_EX, info->getShineStageID(), 0x7C);
        }

        // Register Lighthouse Island stage info
        {
            Stage::ShineAreaInfo *info = new Stage::ShineAreaInfo(SME::STAGE_LIGHTHOUSE, 'li_0');
            info->addScenario(0x82, 82);
            info->addScenario(0x83, 83);
            info->addScenario(0x84, 84);
            info->addScenario(0x85, 85);
            info->addScenario(0x86, 86);
            info->addScenario(0x87, 87);
            info->addScenario(0x88, 88);
            info->addScenario(0x89, 89);
            info->addExScenario(0x8A, 90);
            info->addExScenario(0x8B, 91);
            Stage::registerShineStage(info);
            Stage::registerNormalStage(SME::STAGE_LIGHTHOUSE, info->getShineStageID());
            Stage::registerNormalStage(SME::STAGE_LIGHTHOUSE_BOSS, info->getShineStageID());
            Stage::registerExStage(SME::STAGE_LIGHTHOUSE_EX, info->getShineStageID(), 0x85);
        }

        // Register Warship Island stage info
        {
            Stage::ShineAreaInfo *info = new Stage::ShineAreaInfo(SME::STAGE_WARSHIP, 'wr_0');
            info->addScenario(0x8C, 92);
            info->addScenario(0x8D, 93);
            info->addScenario(0x8E, 94);
            info->addScenario(0x8F, 95);
            info->addScenario(0x90, 96);
            info->addScenario(0x91, 97);
            info->addScenario(0x92, 98);
            info->addScenario(0x93, 99);
            info->addExScenario(0x94, 100);
            info->addExScenario(0x95, 101);
            Stage::registerShineStage(info);
            Stage::registerNormalStage(SME::STAGE_WARSHIP, info->getShineStageID());
            Stage::registerExStage(SME::STAGE_WARSHIP_EX, info->getShineStageID(), 0x8F);
        }

        // Register Hotel Lacrima stage info
        {
            Stage::ShineAreaInfo *info = new Stage::ShineAreaInfo(SME::STAGE_LACRIMA, 'hl_0');
            info->addScenario(0x96, 102);
            info->addScenario(0x97, 103);
            info->addScenario(0x98, 104);
            info->addScenario(0x99, 105);
            info->addScenario(0x9A, 106);
            info->addScenario(0x9B, 107);
            info->addScenario(0x9C, 108);
            info->addScenario(0x9D, 109);
            info->addExScenario(0x9E, 110);
            info->addExScenario(0x9F, 111);
            Stage::registerShineStage(info);
            Stage::registerNormalStage(SME::STAGE_LACRIMA, info->getShineStageID());
            Stage::registerNormalStage(SME::STAGE_LACRIMA_INSIDE, info->getShineStageID());
            Stage::registerNormalStage(SME::STAGE_LACRIMA_BACKHOUSE, info->getShineStageID());
            Stage::registerExStage(SME::STAGE_LACRIMA_EX0, info->getShineStageID(), 0x98);
            Stage::registerExStage(SME::STAGE_LACRIMA_EX1, info->getShineStageID(), 0x9B);
        }

        // Register Pianta Pit stage info
        {
            Stage::ShineAreaInfo *info =
                new Stage::ShineAreaInfo(SME::STAGE_PIANTA_PIT - 2, 'pp_0');
            info->addScenario(0xA0, 112);
            info->addScenario(0xA1, 113);
            info->addScenario(0xA2, 114);
            info->addScenario(0xA3, 115);
            info->addScenario(0xA4, 116);
            info->addScenario(0xA5, 117);
            info->addScenario(0xA6, 118);
            info->addScenario(0xA7, 119);
            Stage::registerShineStage(info);
            Stage::registerNormalStage(SME::STAGE_PIANTA_PIT, info->getShineStageID());
        }

        // Register Daisy Cruiser stage info
        {
            Stage::ShineAreaInfo *info = new Stage::ShineAreaInfo(SME::STAGE_CRUISER - 2, 'dc_0');
            info->addScenario(0xA8, 120);
            info->addScenario(0xA9, 121);
            info->addScenario(0xAA, 122);
            info->addScenario(0xAB, 123);
            info->addExScenario(0xAC, 124);
            Stage::registerShineStage(info);
            Stage::registerNormalStage(SME::STAGE_CRUISER, info->getShineStageID());
            Stage::registerExStage(SME::STAGE_CRUISER_EX, info->getShineStageID(), 0xAB);
        }

        // Register Mario's Dream stage info
        {
            Stage::ShineAreaInfo *info =
                new Stage::ShineAreaInfo(SME::STAGE_MARIO_DREAM - 2, 'md_0');
            info->addScenario(0xAD, 125);
            info->addScenario(0xAE, 126);
            info->addScenario(0xAF, 127);
            info->addScenario(0xB0, 128);
            Stage::registerShineStage(info);
            Stage::registerNormalStage(SME::STAGE_MARIO_DREAM, info->getShineStageID());
        }

        // Register Lancia Fredda stage info
        {
            Stage::ShineAreaInfo *info = new Stage::ShineAreaInfo(SME::STAGE_LANCIA - 2, 'lf_0');
            info->addScenario(0xB1, 129);
            info->addScenario(0xB2, 130);
            info->addScenario(0xB3, 131);
            info->addScenario(0xB4, 132);
            info->addExScenario(0xB5, 133);
            Stage::registerShineStage(info);
            Stage::registerNormalStage(SME::STAGE_LANCIA, info->getShineStageID());
            Stage::registerExStage(SME::STAGE_LANCIA_EX, info->getShineStageID(), 0xB4);
        }

        // Register Vaporwave stage info
        {
            Stage::ShineAreaInfo *info = new Stage::ShineAreaInfo(SME::STAGE_VAPORWAVE - 2, 'vw_0');
            info->addScenario(0xB6, 134);
            info->addScenario(0xB7, 135);
            info->addScenario(0xB8, 136);
            info->addScenario(0xB9, 137);
            info->addExScenario(0xBA, 138);
            Stage::registerShineStage(info);
            Stage::registerNormalStage(SME::STAGE_VAPORWAVE, info->getShineStageID());
            Stage::registerExStage(SME::STAGE_VAPORWAVE_EX, info->getShineStageID(), 0xB9);
        }

        // Register Yoshi Village stage info
        {
            Stage::ShineAreaInfo *info =
                new Stage::ShineAreaInfo(SME::STAGE_YOSHI_VILLAGE - 2, 'yv_0');
            info->addScenario(0xBB, 139);
            info->addScenario(0xBC, 140);
            info->addScenario(0xBD, 141);
            info->addScenario(0xBE, 142);
            info->addExScenario(0xBF, 143);
            Stage::registerShineStage(info);
            Stage::registerNormalStage(SME::STAGE_YOSHI_VILLAGE, info->getShineStageID());
            Stage::registerExStage(SME::STAGE_YOSHI_EX, info->getShineStageID(), 0xBE);
        }

        // Waterpark UNUSED
        // ****************

        // Register Red Lily City stage info
        {
            Stage::ShineAreaInfo *info = new Stage::ShineAreaInfo(SME::STAGE_RED_LILY - 2, 'rl_0');
            info->addScenario(0xC0, 144);
            info->addScenario(0xC1, 145);
            info->addScenario(0xC2, 146);
            info->addScenario(0xC3, 147);
            info->addExScenario(0xC5, 148);
            Stage::registerShineStage(info);
            Stage::registerNormalStage(SME::STAGE_RED_LILY, info->getShineStageID());
            Stage::registerExStage(SME::STAGE_RED_LILY_EX, info->getShineStageID(), 0xC3);
        }

        // Register Peach Beach stage info
        {
            Stage::ShineAreaInfo *info =
                new Stage::ShineAreaInfo(SME::STAGE_PEACH_BEACH - 2, 'pb_0');
            info->addScenario(0xC6, 149);
            info->addScenario(0xC7, 150);
            info->addScenario(0xC8, 151);
            info->addScenario(0xC9, 152);
            info->addExScenario(0xCA, 153);
            Stage::registerShineStage(info);
            Stage::registerNormalStage(SME::STAGE_PEACH_BEACH, info->getShineStageID());
            Stage::registerExStage(SME::STAGE_PEACH_BEACH_EX, info->getShineStageID(), 0xC9);
        }

        // Register Spettro Casino stage info
        {
            Stage::ShineAreaInfo *info =
                new Stage::ShineAreaInfo(SME::STAGE_SPETTRO_CASINO - 2, 'sp_0');
            info->addScenario(0xCB, 154);
            info->addScenario(0xCC, 155);
            info->addScenario(0xCD, 156);
            info->addScenario(0xCE, 157);
            info->addExScenario(0xCF, 158);
            Stage::registerShineStage(info);
            Stage::registerNormalStage(SME::STAGE_SPETTRO_CASINO, info->getShineStageID());
            Stage::registerExStage(SME::STAGE_SPETTRO_EX, info->getShineStageID(), 0xC9);
        }

        // Register Peach's Castle stage info
        {
            Stage::ShineAreaInfo *info =
                new Stage::ShineAreaInfo(SME::STAGE_PEACH_CASTLE - 2, 'pc_0');
            info->addScenario(-1, 159);
            info->addScenario(-1, 160);
            info->addScenario(-1, 161);
            info->addScenario(-1, 162);
            info->addScenario(-1, 163);
            // Painting episodes (not ex so they can have names)
            info->addScenario(-1, 164);
            info->addScenario(-1, 165);
            info->addScenario(-1, 166);
            info->addScenario(-1, 167);
            info->addScenario(-1, 168);
            info->addScenario(-1, 169);
            info->addScenario(-1, 170);
            info->addScenario(-1, 171);
            info->addScenario(-1, 172);
            info->addScenario(-1, 173);
            info->addScenario(-1, 174);
            info->addScenario(-1, 175);
            info->addScenario(-1, 176);
            info->addScenario(-1, 177);
            info->addScenario(-1, 178);
            info->addScenario(-1, 179);
            info->addScenario(-1, 180);
            Stage::registerShineStage(info);
            Stage::registerNormalStage(SME::STAGE_PEACH_CASTLE, info->getShineStageID());
        }

        // Register Island Junctions stage info
        {
            Stage::ShineAreaInfo *info =
                new Stage::ShineAreaInfo(SME::STAGE_ISLE_DELFINO - 2, 'ij_0');
            info->addScenario(0xE6, 181);  // Boilente Crypt
            info->addScenario(0xE7, 182);  // Arido Trail
            info->addScenario(0xE8, 183);  // Arcobaleno Cove
            info->addScenario(0xE9, 184);  // Sonnolento Grove
            info->addScenario(0xEA, 185);  // Rugivosa Marsh
            info->addScenario(0xEB, 186);  // Giovane Creek
            info->addScenario(0xEC, 187);  // Marea Reef
            info->addScenario(0xED, 188);  // Ricco Station
            info->addScenario(-1, 189);    // Train Car
            Stage::registerShineStage(info);
            Stage::registerNormalStage(SME::STAGE_ISLE_DELFINO, info->getShineStageID());
        }

        // Register Tutorial Stage Info
        {
            Stage::ShineAreaInfo *info = new Stage::ShineAreaInfo(SME::STAGE_TUTORIAL - 3);
            info->addScenario(-1, 190);
            Stage::registerShineStage(info);
            Stage::registerNormalStage(SME::STAGE_TUTORIAL, SME::STAGE_TUTORIAL - 3);
        }

        //// Register Final Corona Level
        //{
        //    auto *info           = new BetterSMS::Stage::AreaInfo;
        //    info->mShineStageID  = 0x9;
        //    info->mNormalStageID = 0xA;
        //    info->addScenario(0xED);
        //    info->addScenario(0xEE);
        //    info->mScenarioNameIDs.push_back(190);
        //    info->mScenarioNameIDs.push_back(191);
        //    info->mShineSelectPaneID = 'el_0';
        //    BetterSMS::Stage::registerStageInfo(info->mNormalStageID info);
        //}

        old_heap->becomeCurrentHeap();
    }
}

BETTER_SMS_FOR_CALLBACK void resetForExStage(TMarDirector *director) {
    if (!BetterSMS::Stage::isExStage(director->mAreaID, director->mEpisodeID))
        return;

    TFlagManager::smInstance->setFlag(0x40002, 0);
}