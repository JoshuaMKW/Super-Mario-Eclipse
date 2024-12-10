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
            info->addScenario(125, 72);
            info->addScenario(124, 76);
            info->addScenario(122, 74);
            info->addScenario(123, 75);
            info->addScenario(121, 73);
            info->addScenario(120, 77);
            info->addScenario(126, 78);
            info->addScenario(127, 79);
            info->addExScenario(207, 203);  // 100 coin shine
            info->addExScenario(128, 80);
            info->addExScenario(129, 81);
            Stage::registerShineStage(info);
            Stage::registerNormalStage(SME::STAGE_ERTO, info->getShineStageID());
            Stage::registerExStage(SME::STAGE_ERTO_EX, info->getShineStageID(), 124);

            /*SME::setSpawnTransRot(SME::STAGE_ERTO, TVec3f(-10000.0f, -110.0f, -2580.0f),
                                  TVec3f(0.0f, 100.0f, 0.0f), TGameSequence::AREA_DELFINO, -1);*/

            SME::setSpawnTransRot(SME::STAGE_ERTO, -1, TVec3f(9125.0f, 2774.0f, -13620.0f),
                                  TVec3f(0.0f, -45.0f, 0.0f), SME::STAGE_ISLE_DELFINO,
                                  1);  // Arido Trail

            SME::setSpawnTransRot(SME::STAGE_ERTO, -1, TVec3f(1830.0f, 2820.0f, -11400.0f),
                                  TVec3f(0.0f, 0.0f, 0.0f), SME::STAGE_ISLE_DELFINO,
                                  8);  // Delfino Express
        }

        // Register Lighthouse Island stage info
        {
            Stage::ShineAreaInfo *info = new Stage::ShineAreaInfo(SME::STAGE_LIGHTHOUSE, 'li_0');
            info->addScenario(130, 82);
            info->addScenario(131, 83);
            info->addScenario(132, 84);
            info->addScenario(133, 85);
            info->addScenario(134, 86);
            info->addScenario(135, 87);
            info->addScenario(136, 88);
            info->addScenario(137, 89);
            info->addExScenario(208, 203);  // 100 coin shine
            info->addExScenario(138, 90);
            info->addExScenario(139, 91);
            Stage::registerShineStage(info);
            Stage::registerNormalStage(SME::STAGE_LIGHTHOUSE, info->getShineStageID());
            Stage::registerNormalStage(SME::STAGE_LIGHTHOUSE_BOSS, info->getShineStageID());
            Stage::registerExStage(SME::STAGE_LIGHTHOUSE_EX, info->getShineStageID(), 133);

            SME::setSpawnTransRot(SME::STAGE_LIGHTHOUSE, -1, TVec3f(-1148.0f, 5506.0f, -1453.0f),
                                  TVec3f(0.0f, 0.0f, 0.0f), SME::STAGE_LIGHTHOUSE_BOSS, 1);

            SME::setSpawnTransRot(SME::STAGE_LIGHTHOUSE, -1, TVec3f(13620.0f, 750.0f, 3840.0f),
                                  TVec3f(0.0f, -90.0f, 0.0f), TGameSequence::AREA_RICCO, -1);

            SME::setSpawnTransRot(SME::STAGE_LIGHTHOUSE, -1, TVec3f(-10750.0f, 350.0f, 5400.0f),
                                  TVec3f(0.0f, 90.0f, 0.0f), TGameSequence::AREA_PINNABEACH, -1);

            SME::setSpawnTransRot(SME::STAGE_LIGHTHOUSE, -1, TVec3f(-270.0f, -80.0f, -16800.0f),
                                  TVec3f(0.0f, 0.0f, 0.0f), SME::STAGE_ISLE_DELFINO, 6);
        }

        // Register Warship Island stage info
        {
            Stage::ShineAreaInfo *info = new Stage::ShineAreaInfo(SME::STAGE_WARSHIP, 'wr_0');
            info->addScenario(140, 92);
            info->addScenario(141, 93);
            info->addScenario(143, 94);
            info->addScenario(142, 95);
            info->addScenario(144, 96);
            info->addScenario(145, 97);
            info->addScenario(146, 98);
            info->addScenario(147, 99);
            info->addExScenario(209, 203);  // 100 coin shine
            info->addExScenario(148, 100);
            info->addExScenario(149, 101);
            Stage::registerShineStage(info);
            Stage::registerNormalStage(SME::STAGE_WARSHIP, info->getShineStageID());
            Stage::registerExStage(SME::STAGE_WARSHIP_EX, info->getShineStageID(), 144);

            SME::setSpawnTransRot(SME::STAGE_WARSHIP, -1, TVec3f(-18000.0f, 180.0f, 13240.0f),
                                  TVec3f(0.0f, 100.0f, 0.0f), SME::STAGE_ISLE_DELFINO,
                                  4);  // Rugivosa Marsh
        }

        // Register Hotel Lacrima stage info
        {
            Stage::ShineAreaInfo *info = new Stage::ShineAreaInfo(SME::STAGE_LACRIMA, 'hl_0');
            info->addScenario(150, 102);
            info->addScenario(151, 103);
            info->addScenario(152, 104);
            info->addScenario(153, 105);
            info->addScenario(154, 106);
            info->addScenario(155, 107);
            info->addScenario(156, 108);
            info->addScenario(157, 109);
            info->addExScenario(210, 203);  // 100 coin shine
            info->addExScenario(158, 110);
            info->addExScenario(159, 111);
            Stage::registerShineStage(info);
            Stage::registerNormalStage(SME::STAGE_LACRIMA, info->getShineStageID());
            Stage::registerNormalStage(SME::STAGE_LACRIMA_INSIDE, info->getShineStageID());
            Stage::registerNormalStage(SME::STAGE_LACRIMA_BACKHOUSE, info->getShineStageID());
            Stage::registerExStage(SME::STAGE_LACRIMA_EX0, info->getShineStageID(), 152);
            Stage::registerExStage(SME::STAGE_LACRIMA_EX1, info->getShineStageID(), 154);

            SME::setSpawnTransRot(SME::STAGE_LACRIMA, -1, TVec3f(-1140.0f, 230.0f, 5400.0f),
                                  TVec3f(0.0f, -180.0f, 0.0f), SME::STAGE_LACRIMA_INSIDE, -1);

            SME::setSpawnTransRot(SME::STAGE_LACRIMA, -1, TVec3f(13300.0f, 2740.0f, 9400.0f),
                                  TVec3f(0.0f, 0.0f, 0.0f), SME::STAGE_ISLE_DELFINO,
                                  8);  // Delfino Express

            SME::setSpawnTransRot(SME::STAGE_LACRIMA, -1, TVec3f(-9000.0f, 270.0f, 4400.0f),
                                  TVec3f(0.0f, 145.0f, 0.0f), SME::STAGE_ISLE_DELFINO,
                                  3);  // Sonnolento Grove

            SME::setSpawnTransRot(SME::STAGE_LACRIMA_INSIDE, -1, TVec3f(-3300.0f, 235.0f, 9200.0f),
                                  TVec3f(0.0f, -160.0f, 0.0f), SME::STAGE_LACRIMA_BACKHOUSE, -1);
        }

        // Register Pianta Pit stage info
        {
            Stage::ShineAreaInfo *info =
                new Stage::ShineAreaInfo(SME::STAGE_PIANTA_PIT - 2, 'pp_0');
            info->addScenario(160, 112);
            info->addScenario(161, 113);
            info->addScenario(162, 114);
            info->addScenario(163, 115);
            info->addScenario(164, 116);
            info->addScenario(165, 117);
            info->addExScenario(211, 203);  // 100 coin shine
            Stage::registerShineStage(info);
            Stage::registerNormalStage(SME::STAGE_PIANTA_PIT, info->getShineStageID());
        }

        // Register Daisy Cruiser stage info
        {
            Stage::ShineAreaInfo *info = new Stage::ShineAreaInfo(SME::STAGE_CRUISER - 2, 'dc_0');
            info->addScenario(166, 120);
            info->addScenario(167, 121);
            info->addScenario(168, 122);
            info->addScenario(169, 123);
            info->addScenario(-1, 191);     // Cruiser cabin
            info->addExScenario(212, 203);  // 100 coin shine
            info->addExScenario(170, 124);
            Stage::registerShineStage(info);
            Stage::registerNormalStage(SME::STAGE_CRUISER, info->getShineStageID());
            Stage::registerExStage(SME::STAGE_CRUISER_EX, info->getShineStageID(), 169);

            SME::setSpawnTransRot(SME::STAGE_CRUISER, -1, TVec3f(8800.0f, 6610.0f, 0.0f),
                                  TVec3f(0.0f, -90.0f, 0.0f), SME::STAGE_CRUISER,
                                  4);  // Cruiser Cabin

            SME::setSpawnTransRot(SME::STAGE_CRUISER, -1, TVec3f(6060.0f, 6610.0f, -960.0f),
                                  TVec3f(0.0f, -60.0f, 0.0f), SME::STAGE_CRUISER_EX,
                                  8);  // Planes and Trains

            SME::setSpawnTransRot(SME::STAGE_CRUISER, -1, TVec3f(-17030.0f, 6520.0f, 3750.0f),
                                  TVec3f(0.0f, -180.0f, 0.0f), SME::STAGE_MARIO_DREAM,
                                  -1);  // Mario's Dream

            SME::setSpawnTransRot(SME::STAGE_CRUISER, -1, TVec3f(5500.0f, 1476.0f, -1530.0f),
                                  TVec3f(0.0f, 60.0f, 0.0f), SME::STAGE_SPETTRO_CASINO,
                                  -1);  // Spettro Casino
        }

        // Register Mario's Dream stage info
        {
            Stage::ShineAreaInfo *info =
                new Stage::ShineAreaInfo(SME::STAGE_MARIO_DREAM - 2, 'md_0');
            info->addScenario(171, 125);
            info->addScenario(172, 126);
            info->addScenario(173, 127);
            info->addScenario(174, 128);
            info->addExScenario(213, 203);  // 100 coin shine
            Stage::registerShineStage(info);
            Stage::registerNormalStage(SME::STAGE_MARIO_DREAM, info->getShineStageID());
        }

        // Register Lancia Fredda stage info
        {
            Stage::ShineAreaInfo *info = new Stage::ShineAreaInfo(SME::STAGE_LANCIA - 2, 'lf_0');
            info->addScenario(175, 129);
            info->addScenario(176, 130);
            info->addScenario(177, 131);
            info->addScenario(178, 132);
            info->addExScenario(214, 203);  // 100 coin shine
            Stage::registerShineStage(info);
            Stage::registerNormalStage(SME::STAGE_LANCIA, info->getShineStageID());
            Stage::registerExStage(SME::STAGE_LANCIA_EX, info->getShineStageID(), 178);
        }

        // Register Vaporwave stage info
        {
            Stage::ShineAreaInfo *info = new Stage::ShineAreaInfo(SME::STAGE_VAPORWAVE - 2, 'vw_0');
            info->addScenario(179, 134);
            info->addScenario(180, 135);
            info->addScenario(181, 136);
            info->addScenario(182, 137);
            info->addExScenario(215, 203);  // 100 coin shine
            Stage::registerShineStage(info);
            Stage::registerNormalStage(SME::STAGE_VAPORWAVE, info->getShineStageID());
            Stage::registerExStage(SME::STAGE_VAPORWAVE_EX, info->getShineStageID(), 182);
        }

        // Register Yoshi Village stage info
        {
            Stage::ShineAreaInfo *info =
                new Stage::ShineAreaInfo(SME::STAGE_YOSHI_VILLAGE - 2, 'yv_0');
            info->addScenario(183, 139);
            info->addScenario(184, 140);
            info->addScenario(185, 141);
            info->addScenario(186, 142);
            info->addExScenario(216, 203);  // 100 coin shine
            Stage::registerShineStage(info);
            Stage::registerNormalStage(SME::STAGE_YOSHI_VILLAGE, info->getShineStageID());
            Stage::registerExStage(SME::STAGE_YOSHI_EX, info->getShineStageID(), 186);
        }

        // Waterpark UNUSED
        // ****************

        // Register Red Lily City stage info
        {
            Stage::ShineAreaInfo *info = new Stage::ShineAreaInfo(SME::STAGE_RED_LILY - 2, 'rl_0');
            info->addScenario(187, 144);
            info->addScenario(188, 145);
            info->addScenario(189, 146);
            info->addScenario(190, 147);
            info->addExScenario(217, 203);  // 100 coin shine
            Stage::registerShineStage(info);
            Stage::registerNormalStage(SME::STAGE_RED_LILY, info->getShineStageID());
            Stage::registerExStage(SME::STAGE_RED_LILY_EX, info->getShineStageID(), 190);
        }

        // Register Peach Beach stage info
        {
            Stage::ShineAreaInfo *info =
                new Stage::ShineAreaInfo(SME::STAGE_PEACH_BEACH - 2, 'pb_0');
            info->addScenario(191, 149);
            info->addScenario(192, 150);
            info->addScenario(193, 151);
            info->addScenario(194, 152);
            info->addExScenario(218, 203);  // 100 coin shine
            Stage::registerShineStage(info);
            Stage::registerNormalStage(SME::STAGE_PEACH_BEACH, info->getShineStageID());
            Stage::registerExStage(SME::STAGE_PEACH_BEACH_EX, info->getShineStageID(), 194);
        }

        // Register Spettro Casino stage info
        {
            Stage::ShineAreaInfo *info =
                new Stage::ShineAreaInfo(SME::STAGE_SPETTRO_CASINO - 2, 'sp_0');
            info->addScenario(195, 154);
            info->addScenario(196, 155);
            info->addScenario(197, 156);
            info->addScenario(198, 157);
            info->addExScenario(219, 203);  // 100 coin shine
            Stage::registerShineStage(info);
            Stage::registerNormalStage(SME::STAGE_SPETTRO_CASINO, info->getShineStageID());
            Stage::registerExStage(SME::STAGE_SPETTRO_EX, info->getShineStageID(), 198);
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
            info->addScenario(-1, 192);
            info->addScenario(-1, 193);
            info->addScenario(-1, 194);
            info->addScenario(-1, 195);
            info->addScenario(-1, 196);
            info->addScenario(-1, 197);
            info->addScenario(-1, 198);
            info->addScenario(-1, 199);
            info->addScenario(-1, 200);
            info->addScenario(-1, 201);
            info->addScenario(-1, 202);
            Stage::registerShineStage(info);
            Stage::registerNormalStage(SME::STAGE_PEACH_CASTLE, info->getShineStageID());
        }

        // Register Island Junctions stage info
        {
            Stage::ShineAreaInfo *info =
                new Stage::ShineAreaInfo(SME::STAGE_ISLE_DELFINO - 2, 'ij_0');
            info->addScenario(199, 181);  // Boilente Crypt
            info->addScenario(200, 182);  // Arido Trail
            info->addScenario(201, 183);  // Arcobaleno Cove
            info->addScenario(202, 184);  // Sonnolento Grove
            info->addScenario(203, 185);  // Rugivosa Marsh
            info->addScenario(204, 186);  // Giovane Creek
            info->addScenario(205, 187);  // Marea Reef
            info->addScenario(206, 188);  // Ricco Station
            info->addScenario(220, 189);  // Train Car
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

        // Update Corona Stage Info
        { Stage::registerExStage(0x31, 9, -1); }

        // Set up warps for base stages
        {
            // Delfino Plaza
            {
                SME::setSpawnTransRot(TGameSequence::AREA_DOLPIC, -1,
                                      TVec3f(13700.0f, 300.0f, -12700.0f),
                                      TVec3f(0.0f, -50.0f, 0.0f), SME::STAGE_ERTO, -1);

                SME::setSpawnTransRot(TGameSequence::AREA_DOLPIC, -1,
                                      TVec3f(-7500.0f, 285.0f, 4700.0f), TVec3f(0.0f, 170.0f, 0.0f),
                                      TGameSequence::AREA_DOLPICEX0,
                                      0);  // Ricco Station
            }

            // Bianco Hills
            {
                SME::setSpawnTransRot(TGameSequence::AREA_BIANCO, -1,
                                      TVec3f(4370.0f, 2700.0f, 12050.0f),
                                      TVec3f(0.0f, 100.0f, 0.0f), SME::STAGE_ISLE_DELFINO,
                                      7);  // Ricco Station

                SME::setSpawnTransRot(TGameSequence::AREA_BIANCO, -1,
                                      TVec3f(11450.0f, 3670.0f, 10000.0f),
                                      TVec3f(0.0f, -135.0f, 0.0f), SME::STAGE_ISLE_DELFINO,
                                      0);  // Boilente Crypt

                SME::setSpawnTransRot(TGameSequence::AREA_BIANCO, -1,
                                      TVec3f(-7250.0f, 6600.0f, -15000.0f),
                                      TVec3f(0.0f, 20.0f, 0.0f), TGameSequence::AREA_MONTE, -1);

                SME::setSpawnTransRot(TGameSequence::AREA_BIANCO, -1,
                                      TVec3f(-14000.0f, 2250.0f, 5300.0f),
                                      TVec3f(0.0f, 120.0f, 0.0f), SME::STAGE_ISLE_DELFINO,
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
                                      TVec3f(-10310.0f, 4780.0f, -12090.0f),
                                      TVec3f(0.0f, 0.0f, 0.0f), TGameSequence::AREA_MONTE, -1);

                SME::setSpawnTransRot(TGameSequence::AREA_MAMMA, -1,
                                      TVec3f(1700.0f, -80.0f, 24000.0f),
                                      TVec3f(0.0f, -170.0f, 0.0f), TGameSequence::AREA_SIRENA, -1);

                SME::setSpawnTransRot(TGameSequence::AREA_MAMMA, -1,
                                      TVec3f(17000.0f, -80.0f, 9000.0f),
                                      TVec3f(0.0f, -110.0f, 0.0f), SME::STAGE_ISLE_DELFINO,
                                      6);  // Marea Reef
            }

            // Pinna Beach
            {
                // SME::setSpawnTransRot(TGameSequence::AREA_PINNABEACH, -1,
                //                       TVec3f(15500.0f, -80.0f, 8300.0f),
                //                       TVec3f(0.0f, -115.0f, 0.0f), SME::STAGE_LIGHTHOUSE, -1);

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

                SME::setSpawnTransRot(
                    TGameSequence::AREA_SIRENA, -1, TVec3f(-8100.0f, -80.0f, 15550.0f),
                    TVec3f(0.0f, 160.0f, 0.0f), TGameSequence::AREA_PINNAPARCO, -1);

                SME::setSpawnTransRot(TGameSequence::AREA_SIRENA, -1,
                                      TVec3f(10600.0f, -80.0f, 15900.0f),
                                      TVec3f(0.0f, -130.0f, 0.0f), TGameSequence::AREA_MAMMA, -1);

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
                                      TVec3f(-4350.0f, -2500.0f, 13770.0f),
                                      TVec3f(0.0f, 80.0f, 0.0f), SME::STAGE_PIANTA_PIT, -1);

                SME::setSpawnTransRot(
                    TGameSequence::AREA_MONTE, -1, TVec3f(23900.0f, -2310.0f, -16250.0f),
                    TVec3f(0.0f, -55.0f, 0.0f), SME::STAGE_ISLE_DELFINO, 4);  // Rugivosa Marsh

                SME::setSpawnTransRot(
                    TGameSequence::AREA_MONTE, -1, TVec3f(-17360.0f, -2290.0f, -21250.0f),
                    TVec3f(0.0f, 35.0f, 0.0f), SME::STAGE_ISLE_DELFINO, 3);  // Sonnolento Grove

                SME::setSpawnTransRot(TGameSequence::AREA_MONTE, -1,
                                      TVec3f(6000.0f, 200.0f, 30000.0f),
                                      TVec3f(0.0f, -150.0f, 0.0f), TGameSequence::AREA_BIANCO, -1);
            }

            // Noki Bay
            {
                SME::setSpawnTransRot(TGameSequence::AREA_MARE, -1,
                                      TVec3f(-12510.0f, 280.0f, 7000.0f),
                                      TVec3f(0.0f, 150.0f, 0.0f), SME::STAGE_ISLE_DELFINO,
                                      2);  // Arcobaleno Cove

                SME::setSpawnTransRot(TGameSequence::AREA_MARE, -1,
                                      TVec3f(7400.0f, 5000.0f, -4000.0f),
                                      TVec3f(0.0f, -120.0f, 0.0f), SME::STAGE_ISLE_DELFINO,
                                      3);  // Sonnolento Grove

                SME::setSpawnTransRot(TGameSequence::AREA_MARE, -1,
                                      TVec3f(3400.0f, 340.0f, 1700.0f), TVec3f(0.0f, -40.0f, 0.0f),
                                      TGameSequence::AREA_MAREUNDERSEA, -1);

                SME::setSpawnTransRot(TGameSequence::AREA_MARE, -1,
                                      TVec3f(3400.0f, 340.0f, 1700.0f), TVec3f(0.0f, -40.0f, 0.0f),
                                      TGameSequence::AREA_MAREBOSS, -1);

                /*SME::setSpawnTransRot(TGameSequence::AREA_MARE, -1,
                                      TVec3f(-7150.0f, -80.0f, -5150.0f), TVec3f(0.0f, 15.0f, 0.0f),
                                      TGameSequence::AREA_MAREEX0, -1);*/
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

TVec3f s_prev_player_pos = {0.0f, 0.0f, 0.0f};

BETTER_SMS_FOR_CALLBACK void warpContextUpdater(TMario *player, bool isMario) {
    if (isMario) {
        s_prev_player_pos = player->mTranslation;
    }
}

BETTER_SMS_FOR_CALLBACK void setPlayerStartPos(TMario *player) {
    if (player->_388 != 0) {
        return;
    }

    bool is_important_area = false;
    is_important_area |= (gpApplication.mCurrentScene.mAreaID == TGameSequence::AREA_SIRENA &&
                          gpApplication.mCurrentScene.mEpisodeID == 5);
    is_important_area |= (gpApplication.mCurrentScene.mAreaID == TGameSequence::AREA_MONTE &&
                          gpApplication.mCurrentScene.mEpisodeID == 5);

    TVec3f trans, rot;
    if (!is_important_area && SME::getSpawnTransRot(&gpApplication, trans, rot)) {
        player->mTranslation = trans;
        player->mRotation    = rot;
        player->mAngle.y     = convertAngleFloatToS16(rot.y);
    }

    if (gpApplication.mCurrentScene.mAreaID == SME::STAGE_PIANTA_PIT) {
        if (s_prev_player_pos.y <= -6500.0f &&
            gpApplication.mPrevScene.mAreaID == TGameSequence::AREA_MONTE) {
            player->mTranslation = TVec3f(-7420.0f, 8580.0f, 18670.0f);
            player->mRotation    = TVec3f(0.0f, -130.0f, 0.0f);
            player->mAngle.y     = convertAngleFloatToS16(-130.0f);
        }
    }

    if (gpApplication.mCurrentScene.mAreaID == SME::STAGE_PEACH_CASTLE &&
        gpApplication.mPrevScene.mAreaID == SME::STAGE_PEACH_CASTLE) {
        if (gpApplication.mCurrentScene.mEpisodeID == 0) {
            if (gpApplication.mPrevScene.mEpisodeID == 2) {
                if (s_prev_player_pos.y > 34000.0f && s_prev_player_pos.z < -5000.0f) {
                    player->mTranslation = TVec3f(-220.0f, 950.0f, 11200.0f);
                    player->mRotation    = TVec3f(0.0f, 0.0f, 0.0f);
                    player->mAngle.y     = convertAngleFloatToS16(0.0f);
                } else {
                    player->mTranslation = TVec3f(0.0f, 1470.0f, 2500.0f);
                    player->mRotation    = TVec3f(0.0f, 180.0f, 0.0f);
                    player->mAngle.y     = convertAngleFloatToS16(180.0f);
                }
            }
        } else if (gpApplication.mCurrentScene.mEpisodeID == 2) {
            if (gpApplication.mPrevScene.mEpisodeID == 0 && s_prev_player_pos.y > 900.0f &&
                s_prev_player_pos.z > 10000.0f) {
                player->mTranslation = TVec3f(0.0f, 35270.0f, -5600.0f);
                player->mRotation    = TVec3f(0.0f, 180.0f, 0.0f);
                player->mAngle.y     = convertAngleFloatToS16(180.0f);
            }
        }
    }
}

extern bool gHadLuigiBefore;
extern bool gHadPiantissimoBefore;

static void SME_extendedCorrectFlags(TFlagManager *manager) {
    manager->correctFlag();
    manager->setFlag(0x20012, 3080);  // Lighthouse Island
    manager->setFlag(0x20013, 2670);  // Red Lily City

    gHadLuigiBefore       = manager->getFlag(0x10077);
    gHadPiantissimoBefore = manager->getBool(0x10018) && manager->getBool(0x10041) &&
                            manager->getBool(0x10036) && manager->getShineFlag(135);

    manager->setFlag(0x30018, gHadLuigiBefore);        // Luigi
    manager->setFlag(0x30019, gHadPiantissimoBefore);  // Piantissimo

    // Normalize extra blue coin slots
    for (size_t i = 0x1033E; i < 0x10366; ++i) {
        manager->setFlag(i, 0);
    }
}
SMS_PATCH_BL(0x802954f4, SME_extendedCorrectFlags);

static size_t accumulateBlueCoinShinesForCalc() {
    size_t count = 0;

    {
        size_t i = 0x46;
        do {
            count += TFlagManager::smInstance->getShineFlag(i);
        } while (++i < 0x56);
    }

    {
        size_t i = 0x6C;
        do {
            count += TFlagManager::smInstance->getShineFlag(i);
        } while (++i < 0x74);
    }

    {
        size_t i = 221;
        do {
            count += TFlagManager::smInstance->getShineFlag(i);
        } while (++i < 239);
    }

    return count;
}
SMS_PATCH_BL(0x80147A80, accumulateBlueCoinShinesForCalc);
SMS_WRITE_32(0x80147A84, 0x7C7D1B78);
SMS_WRITE_32(0x80147A88, 0x4800004C);