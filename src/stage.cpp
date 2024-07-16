#include <BetterSMS/libs/global_vector.hxx>

#include <BetterSMS/area.hxx>
#include <BetterSMS/module.hxx>
#include <BetterSMS/stage.hxx>
#include <SMS/Manager/FlagManager.hxx>
#include <SMS/System/GameSequence.hxx>

#include "stage.hxx"
#include <BetterSMS/libs/constmath.hxx>

using namespace BetterSMS;

struct SpawnInfo {
    s32 m_to_episode_id   = -1;
    s32 m_from_area_id    = -1;
    s32 m_from_episode_id = -1;
    TVec3f m_spawn_pos;
    TVec3f m_spawn_rot;
};

static TGlobalVector<SpawnInfo> s_area_spawn_map[256];

void SME::setSpawnTransRot(s32 stage, s32 episode, const TVec3f &trans, const TVec3f &rot,
                           s32 from_stage, s32 from_episode) {
    SpawnInfo info;
    info.m_to_episode_id   = episode;
    info.m_from_area_id    = from_stage;
    info.m_from_episode_id = from_episode;
    info.m_spawn_pos       = trans;
    info.m_spawn_rot       = rot;
    s_area_spawn_map[stage].push_back(info);
}

bool SME::getSpawnTransRot(TApplication *app, TVec3f &trans, TVec3f &rot) {
    const TGlobalVector<SpawnInfo> &spawn_info = s_area_spawn_map[app->mCurrentScene.mAreaID];
    for (const SpawnInfo &info : spawn_info) {
        if (info.m_to_episode_id != -1 && info.m_to_episode_id != app->mCurrentScene.mEpisodeID) {
            continue;
        }

        if (info.m_from_area_id == -1) {
            trans = info.m_spawn_pos;
            rot   = info.m_spawn_rot;
            return true;
        }

        if (info.m_from_area_id != app->mPrevScene.mAreaID) {
            continue;
        }

        if (info.m_from_episode_id == -1 || info.m_from_episode_id == app->mPrevScene.mEpisodeID) {
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

            /*SME::setSpawnTransRot(SME::STAGE_ERTO, TVec3f(-10000.0f, -110.0f, -2580.0f),
                                  TVec3f(0.0f, 100.0f, 0.0f), TGameSequence::AREA_DELFINO, -1);*/

            SME::setSpawnTransRot(SME::STAGE_ERTO, -1, TVec3f(9910.0f, 2500.0f, -15000.0f),
                                  TVec3f(0.0f, -45.0f, 0.0f), SME::STAGE_ISLE_DELFINO, 1);  // Arido Trail

            SME::setSpawnTransRot(SME::STAGE_ERTO, -1, TVec3f(1830.0f, 2820.0f, -11400.0f),
                                  TVec3f(0.0f, 0.0f, 0.0f), SME::STAGE_ISLE_DELFINO,
                                  8);  // Delfino Express
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

            SME::setSpawnTransRot(SME::STAGE_LIGHTHOUSE, -1, TVec3f(13620.0f, -750.0f, 3840.0f),
                                  TVec3f(0.0f, -90.0f, 0.0f), TGameSequence::AREA_RICCO, -1);

            SME::setSpawnTransRot(SME::STAGE_LIGHTHOUSE, -1, TVec3f(-10750.0f, -500.0f, 5400.0f),
                                  TVec3f(0.0f, 90.0f, 0.0f), TGameSequence::AREA_PINNABEACH, -1);

            SME::setSpawnTransRot(SME::STAGE_LIGHTHOUSE, -1, TVec3f(-270.0f, -1080.0f, -16800.0f),
                                  TVec3f(0.0f, 0.0f, 0.0f), SME::STAGE_ISLE_DELFINO, 6);
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

            SME::setSpawnTransRot(SME::STAGE_WARSHIP, -1, TVec3f(-18000.0f, 180.0f, 13240.0f),
                                  TVec3f(0.0f, 100.0f, 0.0f), SME::STAGE_ISLE_DELFINO,
                                  5);  // Rugivosa Marsh
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

            SME::setSpawnTransRot(SME::STAGE_LACRIMA, -1, TVec3f(-1140.0f, 230.0f, 5400.0f),
                                  TVec3f(0.0f, -180.0f, 0.0f), SME::STAGE_LACRIMA_INSIDE, -1);

            SME::setSpawnTransRot(SME::STAGE_LACRIMA, -1, TVec3f(13300.0f, 2740.0f, 9400.0f),
                                  TVec3f(0.0f, 0.0f, 0.0f), SME::STAGE_ISLE_DELFINO,
                                  8);  // Delfino Express

            SME::setSpawnTransRot(SME::STAGE_LACRIMA, -1, TVec3f(-9000.0f, 270.0f, 4400.0f),
                                  TVec3f(0.0f, 145.0f, 0.0f), SME::STAGE_ISLE_DELFINO,
                                  4);  // Sonnolento Grove
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

            // Boilente Crypt
            SME::setSpawnTransRot(SME::STAGE_ISLE_DELFINO, 0, TVec3f(1800.0f, 30.0f, -4300.0f),
                                  TVec3f(0.0f, -10.0f, 0.0f), TGameSequence::AREA_BIANCO, -1);

            // Arido Trail
            SME::setSpawnTransRot(SME::STAGE_ISLE_DELFINO, 1, TVec3f(4500.0f, 340.0f, 2027.0f),
                                  TVec3f(0.0f, -120.0f, 0.0f), SME::STAGE_ERTO, -1);

            // Arcobaleno Cove
            SME::setSpawnTransRot(SME::STAGE_ISLE_DELFINO, 2, TVec3f(-1400.0f, 120.0f, -1900.0f),
                                  TVec3f(0.0f, -170.0f, 0.0f), TGameSequence::AREA_SIRENA, -1);

            // Sonnolento Grove
            {
                SME::setSpawnTransRot(SME::STAGE_ISLE_DELFINO, 3, TVec3f(1000.0f, -10.0f, -1470.0f),
                                      TVec3f(0.0f, -35.0f, 0.0f), SME::STAGE_LACRIMA, -1);

                SME::setSpawnTransRot(SME::STAGE_ISLE_DELFINO, 3, TVec3f(-1900.0f, -40.0f, -600.0f),
                                      TVec3f(0.0f, 70.0f, 0.0f), TGameSequence::AREA_MARE, -1);
            }

            // Rugivosa Marsh
            {
                SME::setSpawnTransRot(SME::STAGE_ISLE_DELFINO, 4, TVec3f(3950.0f, 140.0f, 140.0f),
                                      TVec3f(0.0f, 0.0f, 0.0f), SME::STAGE_WARSHIP, -1);

                SME::setSpawnTransRot(SME::STAGE_ISLE_DELFINO, 4,
                                      TVec3f(-3800.0f, 190.0f, -1175.0f), TVec3f(0.0f, 90.0f, 0.0f),
                                      SME::STAGE_LACRIMA, -1);
            }

            // Giovane Creek
            {
                SME::setSpawnTransRot(SME::STAGE_ISLE_DELFINO, 5, TVec3f(235.0f, 270.0f, 920.0f),
                                      TVec3f(0.0f, -155.0f, 0.0f), TGameSequence::AREA_RICCO, -1);

                SME::setSpawnTransRot(SME::STAGE_ISLE_DELFINO, 5,
                                      TVec3f(-2950.0f, 270.0f, -2030.0f), TVec3f(0.0f, 72.0f, 0.0f),
                                      TGameSequence::AREA_MAMMA, -1);
            }

            // Marea Reef
            {
                SME::setSpawnTransRot(SME::STAGE_ISLE_DELFINO, 6, TVec3f(770.0f, -80.0f, -1070.0f),
                                      TVec3f(0.0f, -30.0f, 0.0f), TGameSequence::AREA_MAMMA, -1);

                SME::setSpawnTransRot(SME::STAGE_ISLE_DELFINO, 6, TVec3f(-1240.0f, -80.0f, -650.0f),
                                      TVec3f(0.0f, 56.0f, 0.0f), TGameSequence::AREA_SIRENA, -1);
            }

            // Ricco Station
            {
                SME::setSpawnTransRot(SME::STAGE_ISLE_DELFINO, 7, TVec3f(0.0f, 0.0f, 0.0f),
                                      TVec3f(0.0f, 0.0f, 0.0f), SME::STAGE_PIANTA_PIT, -1);

                SME::setSpawnTransRot(SME::STAGE_ISLE_DELFINO, 7, TVec3f(0.0f, 0.0f, 0.0f),
                                      TVec3f(0.0f, 0.0f, 0.0f), SME::STAGE_CRUISER, -1);
            }
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

        // Set up warps for base stages
        {
            // Delfino Plaza
            {
                SME::setSpawnTransRot(TGameSequence::AREA_DOLPIC, -1,
                                      TVec3f(13700.0f, 300.0f, -12700.0f), TVec3f(0.0f, -50.0f, 0.0f),
                                      SME::STAGE_ERTO, -1);
            }

            // Bianco Hills
            {
                SME::setSpawnTransRot(TGameSequence::AREA_BIANCO, -1,
                                      TVec3f(4370.0f, 2700.0f, 12050.0f), TVec3f(0.0f, 100.0f, 0.0f),
                                      SME::STAGE_ISLE_DELFINO,
                                      7);  // Ricco Station

                SME::setSpawnTransRot(TGameSequence::AREA_BIANCO, -1,
                                      TVec3f(11450.0f, 3670.0f, 10000.0f), TVec3f(0.0f, -135.0f, 0.0f),
                                      SME::STAGE_ISLE_DELFINO,
                                      0);  // Boilente Crypt

                SME::setSpawnTransRot(TGameSequence::AREA_BIANCO, -1,
                                      TVec3f(-7250.0f, 6600.0f, -15000.0f), TVec3f(0.0f, 20.0f, 0.0f),
                                      TGameSequence::AREA_MONTE, -1);

                SME::setSpawnTransRot(TGameSequence::AREA_BIANCO, -1,
                                      TVec3f(-14000.0f, 2250.0f, 5300.0f), TVec3f(0.0f, 120.0f, 0.0f),
                                      SME::STAGE_ISLE_DELFINO,
                                      5);  // Giovane Creek
            }

            // Ricco Harbor
            {
                SME::setSpawnTransRot(TGameSequence::AREA_RICCO, -1,
                                      TVec3f(-18500.0f, 300.0f, 860.0f), TVec3f(0.0f, 73.0f, 0.0f),
                                      SME::STAGE_ISLE_DELFINO,
                                      5);  // Giovane Creek

                SME::setSpawnTransRot(TGameSequence::AREA_RICCO, -1,
                                      TVec3f(9100.0f, 1500.0f, 1780.0f), TVec3f(0.0f, 90.0f, 0.0f),
                                      SME::STAGE_ISLE_DELFINO,
                                      7);  // Ricco Station
            }

            // Gelato Beach
            {
                SME::setSpawnTransRot(TGameSequence::AREA_MAMMA, -1,
                                      TVec3f(9450.0f, 415.0f, -8550.0f), TVec3f(0.0f, -80.0f, 0.0f),
                                      SME::STAGE_ISLE_DELFINO,
                                      5);  // Giovane Creek

                SME::setSpawnTransRot(TGameSequence::AREA_MAMMA, -1,
                                      TVec3f(-10310.0f, 4780.0f, -12090.0f), TVec3f(0.0f, 0.0f, 0.0f),
                                      TGameSequence::AREA_MONTE, -1);

                SME::setSpawnTransRot(TGameSequence::AREA_MAMMA, -1,
                                      TVec3f(1700.0f, -80.0f, 24000.0f), TVec3f(0.0f, -170.0f, 0.0f),
                                      TGameSequence::AREA_SIRENA, -1);

                SME::setSpawnTransRot(TGameSequence::AREA_MAMMA, -1,
                                      TVec3f(27000.0f, -80.0f, 10100.0f), TVec3f(0.0f, 90.0f, 0.0f),
                                      SME::STAGE_ISLE_DELFINO,
                                      6);  // Marea Reef
            }

            // Pinna Beach
            {
                SME::setSpawnTransRot(TGameSequence::AREA_PINNABEACH, -1,
                                      TVec3f(-3500.0f, 120.0f, 12670.0f), TVec3f(0.0f, 115.0f, 0.0f),
                                      SME::STAGE_LIGHTHOUSE, -1);

                SME::setSpawnTransRot(TGameSequence::AREA_PINNABEACH, -1,
                                      TVec3f(-3500.0f, 400.0f, 2800.0f), TVec3f(0.0f, 0.0f, 0.0f),
                                      TGameSequence::AREA_PINNAPARCO, -1);
            }

            // Pinna Park
            {
                SME::setSpawnTransRot(TGameSequence::AREA_PINNAPARCO, -1,
                                      TVec3f(-6280.0f, 40.0f, -13700.0f), TVec3f(0.0f, 40.0f, 0.0f),
                                      TGameSequence::AREA_SIRENA, -1);
            }

            // Sirena Beach
            {
                SME::setSpawnTransRot(TGameSequence::AREA_SIRENA, -1,
                                      TVec3f(8150.0f, 990.0f, 650.0f), TVec3f(0.0f, -85.0f, 0.0f),
                                      SME::STAGE_ISLE_DELFINO,
                                      8);  // Delfino Express

                SME::setSpawnTransRot(TGameSequence::AREA_SIRENA, -1,
                                      TVec3f(-8200.0f, 760.0f, 9270.0f), TVec3f(0.0f, 145.0f, 0.0f),
                                      SME::STAGE_ISLE_DELFINO,
                                      2);  // Arcobaleno Cove

                SME::setSpawnTransRot(TGameSequence::AREA_SIRENA, -1,
                                      TVec3f(-8100.0f, -80.0f, 15550.0f), TVec3f(0.0f, 160.0f, 0.0f),
                                      TGameSequence::AREA_PINNAPARCO, -1);
                 
                SME::setSpawnTransRot(TGameSequence::AREA_SIRENA, -1,
                                      TVec3f(10600.0f, -80.0f, 15900.0f), TVec3f(0.0f, -130.0f, 0.0f),
                                      TGameSequence::AREA_MAMMA, -1);

                SME::setSpawnTransRot(TGameSequence::AREA_SIRENA, -1,
                                      TVec3f(-50.0f, 900.0f, -1770.0f), TVec3f(0.0f, 0.0f, 0.0f),
                                      TGameSequence::AREA_DELFINO,
                                      -1);  // Hotel
            }

            // Sirena Hotel
            {
                SME::setSpawnTransRot(TGameSequence::AREA_DELFINO, -1,
                                      TVec3f(-2450.0f, 0.0f, -1250.0f), TVec3f(0.0f, 90.0f, 0.0f),
                                      TGameSequence::AREA_CASINO, -1);
            }

            // Pianta Village
            {
                SME::setSpawnTransRot(TGameSequence::AREA_MONTE, -1,
                                      TVec3f(-4350.0f, -2500.0f, 13770.0f), TVec3f(0.0f, 80.0f, 0.0f),
                                      SME::STAGE_PIANTA_PIT, -1);

                SME::setSpawnTransRot(TGameSequence::AREA_MONTE, -1,
                                      TVec3f(23900.0f, -2310.0f, -16250.0f), TVec3f(0.0f, -55.0f, 0.0f),
                                      SME::STAGE_ISLE_DELFINO, 4);  // Rugivosa Marsh

                SME::setSpawnTransRot(TGameSequence::AREA_MONTE, -1,
                                      TVec3f(-17360.0f, -2290.0f, -21250.0f), TVec3f(0.0f, 35.0f, 0.0f),
                                      SME::STAGE_ISLE_DELFINO, 3);  // Sonnolento Grove

                SME::setSpawnTransRot(TGameSequence::AREA_MONTE, -1,
                                      TVec3f(6000.0f, 200.0f, 30000.0f), TVec3f(0.0f, -150.0f, 0.0f),
                                      TGameSequence::AREA_BIANCO, -1);
            }

            // Noki Bay
            {
                SME::setSpawnTransRot(TGameSequence::AREA_MARE, -1,
                                      TVec3f(-12510.0f, 280.0f, 7000.0f), TVec3f(0.0f, 150.0f, 0.0f),
                                      SME::STAGE_ISLE_DELFINO,
                                      2);  // Arcobaleno Cove

                SME::setSpawnTransRot(TGameSequence::AREA_MARE, -1,
                                      TVec3f(7400.0f, 5000.0f, -4000.0f), TVec3f(0.0f, -120.0f, 0.0f),
                                      SME::STAGE_ISLE_DELFINO,
                                      3);  // Sonnolento Grove

                SME::setSpawnTransRot(TGameSequence::AREA_MARE, -1,
                                      TVec3f(3400.0f, 340.0f, 1700.0f), TVec3f(0.0f, -40.0f, 0.0f),
                                      TGameSequence::AREA_MAREUNDERSEA, -1);

                SME::setSpawnTransRot(TGameSequence::AREA_MARE, -1,
                                      TVec3f(3400.0f, 340.0f, 1700.0f), TVec3f(0.0f, -40.0f, 0.0f),
                                      TGameSequence::AREA_MAREBOSS, -1);

                SME::setSpawnTransRot(TGameSequence::AREA_MARE, -1,
                                      TVec3f(-120.0f, -80.0f, -2000.0f), TVec3f(0.0f, -30.0f, 0.0f),
                                      TGameSequence::AREA_MAREEX0, -1);
            }

            // Corona Mountain
            {
                SME::setSpawnTransRot(TGameSequence::AREA_COROEX6, -1,
                                      TVec3f(-3000.0f, 610.0f, 22960.0f), TVec3f(0.0f, 90.0f, 0.0f),
                                      SME::STAGE_ISLE_DELFINO, 0);  // Boilente Crypt
            }
        }

        //

        old_heap->becomeCurrentHeap();
    }
}

BETTER_SMS_FOR_CALLBACK void resetForExStage(TMarDirector *director) {
    if (!BetterSMS::Stage::isExStage(director->mAreaID, director->mEpisodeID))
        return;

    TFlagManager::smInstance->setFlag(0x40002, 0);
}

BETTER_SMS_FOR_CALLBACK void setPlayerStartPos(TMario *player) {
    TVec3f trans, rot;
    if (SME::getSpawnTransRot(&gpApplication, trans, rot)) {
        player->mTranslation = trans;
        player->mRotation    = rot;
        player->mAngle.y     = convertAngleFloatToS16(rot.y);
    }
}

void SME_extendedCorrectFlags(TFlagManager *manager) {
    manager->correctFlag();
    manager->setFlag(0x20013, 2670);
}
SMS_PATCH_BL(0x802954f4, SME_extendedCorrectFlags);