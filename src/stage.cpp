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
            auto *info           = new BetterSMS::Stage::AreaInfo;
            info->mShineStageID  = SME::STAGE_ERTO;
            info->mNormalStageID = SME::STAGE_ERTO;
            info->mScenarioIDs.push_back(0x78);
            info->mScenarioIDs.push_back(0x79);
            info->mScenarioIDs.push_back(0x7A);
            info->mScenarioIDs.push_back(0x7B);
            info->mScenarioIDs.push_back(0x7C);
            info->mScenarioIDs.push_back(0x7D);
            info->mScenarioIDs.push_back(0x7E);
            info->mScenarioIDs.push_back(0x7F);
            info->mExScenarioIDs.push_back(0x200);
            info->mExScenarioIDs.push_back(0x80);
            info->mExScenarioIDs.push_back(0x81);
            info->mExScenarioIDs.push_back(0x00);
            info->mScenarioNameIDs.push_back(72);
            info->mScenarioNameIDs.push_back(73);
            info->mScenarioNameIDs.push_back(74);
            info->mScenarioNameIDs.push_back(75);
            info->mScenarioNameIDs.push_back(76);
            info->mScenarioNameIDs.push_back(77);
            info->mScenarioNameIDs.push_back(78);
            info->mScenarioNameIDs.push_back(79);
            info->mExScenarioNameIDs.push_back(80);
            info->mExScenarioNameIDs.push_back(81);
            info->mShineSelectPaneID = 'er_0';
            BetterSMS::Stage::registerStageInfo(info->mNormalStageID, info);

            {
                auto exinfo            = new BetterSMS::Stage::ExAreaInfo;
                exinfo->mShineStageID  = info->mShineStageID;
                exinfo->mNormalStageID = SME::STAGE_ERTO_EX;
                exinfo->mShineID       = 0xA0;
                BetterSMS::Stage::registerExStageInfo(exinfo->mNormalStageID, exinfo);
            }
        }

        // Register Lighthouse Island stage info
        {
            auto *info           = new BetterSMS::Stage::AreaInfo;
            info->mShineStageID  = SME::STAGE_LIGHTHOUSE;
            info->mNormalStageID = SME::STAGE_LIGHTHOUSE;
            info->mScenarioIDs.push_back(0x82);
            info->mScenarioIDs.push_back(0x83);
            info->mScenarioIDs.push_back(0x84);
            info->mScenarioIDs.push_back(0x85);
            info->mScenarioIDs.push_back(0x86);
            info->mScenarioIDs.push_back(0x87);
            info->mScenarioIDs.push_back(0x88);
            info->mScenarioIDs.push_back(0x89);
            info->mExScenarioIDs.push_back(0x201);
            info->mExScenarioIDs.push_back(0x8A);
            info->mExScenarioIDs.push_back(0x8B);
            info->mExScenarioIDs.push_back(0x00);
            info->mScenarioNameIDs.push_back(82);
            info->mScenarioNameIDs.push_back(83);
            info->mScenarioNameIDs.push_back(84);
            info->mScenarioNameIDs.push_back(85);
            info->mScenarioNameIDs.push_back(86);
            info->mScenarioNameIDs.push_back(87);
            info->mScenarioNameIDs.push_back(88);
            info->mScenarioNameIDs.push_back(89);
            info->mExScenarioNameIDs.push_back(90);
            info->mExScenarioNameIDs.push_back(91);
            info->mShineSelectPaneID = 'li_0';
            BetterSMS::Stage::registerStageInfo(info->mNormalStageID, info);

            {
                auto exinfo            = new BetterSMS::Stage::ExAreaInfo;
                exinfo->mShineStageID  = info->mShineStageID;
                exinfo->mNormalStageID = SME::STAGE_LIGHTHOUSE_EX;
                exinfo->mShineID       = 0xA2;
                BetterSMS::Stage::registerExStageInfo(exinfo->mNormalStageID, exinfo);
            }
        }

        // Register Warship Island stage info
        {
            auto *info           = new BetterSMS::Stage::AreaInfo;
            info->mShineStageID  = SME::STAGE_WARSHIP;
            info->mNormalStageID = SME::STAGE_WARSHIP;
            info->mScenarioIDs.push_back(0x8C);
            info->mScenarioIDs.push_back(0x8D);
            info->mScenarioIDs.push_back(0x8E);
            info->mScenarioIDs.push_back(0x8F);
            info->mScenarioIDs.push_back(0x90);
            info->mScenarioIDs.push_back(0x91);
            info->mScenarioIDs.push_back(0x92);
            info->mScenarioIDs.push_back(0x93);
            info->mExScenarioIDs.push_back(0x202);
            info->mExScenarioIDs.push_back(0x94);
            info->mExScenarioIDs.push_back(0x95);
            info->mExScenarioIDs.push_back(0x00);
            info->mScenarioNameIDs.push_back(92);
            info->mScenarioNameIDs.push_back(93);
            info->mScenarioNameIDs.push_back(94);
            info->mScenarioNameIDs.push_back(95);
            info->mScenarioNameIDs.push_back(96);
            info->mScenarioNameIDs.push_back(97);
            info->mScenarioNameIDs.push_back(98);
            info->mScenarioNameIDs.push_back(99);
            info->mExScenarioNameIDs.push_back(100);
            info->mExScenarioNameIDs.push_back(101);
            info->mShineSelectPaneID = 'wr_0';
            BetterSMS::Stage::registerStageInfo(info->mNormalStageID, info);

            {
                auto exinfo            = new BetterSMS::Stage::ExAreaInfo;
                exinfo->mShineStageID  = info->mShineStageID;
                exinfo->mNormalStageID = SME::STAGE_WARSHIP_EX;
                exinfo->mShineID       = 0xA4;
                BetterSMS::Stage::registerExStageInfo(exinfo->mNormalStageID, exinfo);
            }
        }

        // Register Hotel Lacrima stage info
        {
            auto *info           = new BetterSMS::Stage::AreaInfo;
            info->mShineStageID  = SME::STAGE_LACRIMA;
            info->mNormalStageID = SME::STAGE_LACRIMA;
            info->mScenarioIDs.push_back(0x96);
            info->mScenarioIDs.push_back(0x97);
            info->mScenarioIDs.push_back(0x98);
            info->mScenarioIDs.push_back(0x99);
            info->mScenarioIDs.push_back(0x9A);
            info->mScenarioIDs.push_back(0x9B);
            info->mScenarioIDs.push_back(0x9C);
            info->mScenarioIDs.push_back(0x9D);
            info->mExScenarioIDs.push_back(0x203);
            info->mExScenarioIDs.push_back(0x9E);
            info->mExScenarioIDs.push_back(0x9F);
            info->mExScenarioIDs.push_back(0x00);
            info->mScenarioNameIDs.push_back(102);
            info->mScenarioNameIDs.push_back(103);
            info->mScenarioNameIDs.push_back(104);
            info->mScenarioNameIDs.push_back(105);
            info->mScenarioNameIDs.push_back(106);
            info->mScenarioNameIDs.push_back(107);
            info->mScenarioNameIDs.push_back(108);
            info->mScenarioNameIDs.push_back(109);
            info->mExScenarioNameIDs.push_back(110);
            info->mExScenarioNameIDs.push_back(111);
            info->mShineSelectPaneID = 'hl_0';
            BetterSMS::Stage::registerStageInfo(info->mNormalStageID, info);

            {
                auto exinfo            = new BetterSMS::Stage::ExAreaInfo;
                exinfo->mShineStageID  = info->mShineStageID;
                exinfo->mNormalStageID = SME::STAGE_LACRIMA_EX0;
                exinfo->mShineID       = 0xA6;
                BetterSMS::Stage::registerExStageInfo(exinfo->mNormalStageID, exinfo);
            }

            {
                auto exinfo            = new BetterSMS::Stage::ExAreaInfo;
                exinfo->mShineStageID  = info->mShineStageID;
                exinfo->mNormalStageID = SME::STAGE_LACRIMA_EX1;
                exinfo->mShineID       = 0xA8;
                BetterSMS::Stage::registerExStageInfo(exinfo->mNormalStageID, exinfo);
            }
        }

        // Register Hotel Lacrima Inside info
        {
            auto *info           = new BetterSMS::Stage::AreaInfo;
            info->mShineStageID  = SME::STAGE_LACRIMA;
            info->mNormalStageID = SME::STAGE_LACRIMA_INSIDE;
            info->mScenarioIDs.push_back(0x96);
            info->mScenarioIDs.push_back(0x9C);
            info->mScenarioNameIDs.push_back(102);
            info->mScenarioNameIDs.push_back(108);
            BetterSMS::Stage::registerStageInfo(info->mNormalStageID, info);
        }

        // Register Hotel Lacrima Backhouse info
        {
            auto *info           = new BetterSMS::Stage::AreaInfo;
            info->mShineStageID  = SME::STAGE_LACRIMA;
            info->mNormalStageID = SME::STAGE_LACRIMA_BACKHOUSE;
            info->mScenarioIDs.push_back(0x96);
            info->mScenarioIDs.push_back(0x9C);
            info->mScenarioNameIDs.push_back(102);
            info->mScenarioNameIDs.push_back(108);
            BetterSMS::Stage::registerStageInfo(info->mNormalStageID, info);
        }

        // Register Pianta Pit stage info
        {
            auto *info           = new BetterSMS::Stage::AreaInfo;
            info->mShineStageID  = SME::STAGE_PIANTA_PIT - 2;
            info->mNormalStageID = SME::STAGE_PIANTA_PIT;
            info->mScenarioIDs.push_back(0xA0);
            info->mScenarioIDs.push_back(0xA1);
            info->mScenarioNameIDs.push_back(112);
            info->mScenarioNameIDs.push_back(113);
            info->mShineSelectPaneID = 'pp_0';
            BetterSMS::Stage::registerStageInfo(info->mNormalStageID, info);
        }

        // Register Daisy Cruiser stage info
        {
            auto *info           = new BetterSMS::Stage::AreaInfo;
            info->mShineStageID  = SME::STAGE_CRUISER - 2;
            info->mNormalStageID = SME::STAGE_CRUISER;
            info->mScenarioIDs.push_back(0xA2);
            info->mScenarioIDs.push_back(0xA3);
            info->mScenarioIDs.push_back(0xA4);
            info->mScenarioIDs.push_back(0xA5);
            info->mExScenarioIDs.push_back(0x204);
            info->mExScenarioIDs.push_back(0xA6);
            info->mExScenarioIDs.push_back(0xA7);
            info->mExScenarioIDs.push_back(0x00);
            info->mScenarioNameIDs.push_back(114);
            info->mScenarioNameIDs.push_back(115);
            info->mScenarioNameIDs.push_back(116);
            info->mScenarioNameIDs.push_back(117);
            info->mExScenarioNameIDs.push_back(118);
            info->mExScenarioNameIDs.push_back(119);
            info->mShineSelectPaneID = 'dc_0';
            BetterSMS::Stage::registerStageInfo(info->mNormalStageID, info);

            {
                auto exinfo            = new BetterSMS::Stage::ExAreaInfo;
                exinfo->mShineStageID  = info->mShineStageID;
                exinfo->mNormalStageID = SME::STAGE_CRUISER_EX;
                exinfo->mShineID       = 0xA9;
                BetterSMS::Stage::registerExStageInfo(exinfo->mNormalStageID, exinfo);
            }
        }

        // Register Mario's Dream stage info
        {
            auto *info           = new BetterSMS::Stage::AreaInfo;
            info->mShineStageID  = SME::STAGE_MARIO_DREAM - 2;
            info->mNormalStageID = SME::STAGE_MARIO_DREAM;
            info->mScenarioIDs.push_back(0xA8);
            info->mScenarioIDs.push_back(0xA9);
            info->mScenarioIDs.push_back(0xAA);
            info->mScenarioIDs.push_back(0xAB);
            info->mScenarioNameIDs.push_back(120);
            info->mScenarioNameIDs.push_back(121);
            info->mScenarioNameIDs.push_back(122);
            info->mScenarioNameIDs.push_back(123);
            info->mShineSelectPaneID = 'md_0';
            BetterSMS::Stage::registerStageInfo(info->mNormalStageID, info);
        }

        // Register Lancia Fredda stage info
        {
            auto *info           = new BetterSMS::Stage::AreaInfo;
            info->mShineStageID  = SME::STAGE_LANCIA - 2;
            info->mNormalStageID = SME::STAGE_LANCIA;
            info->mScenarioIDs.push_back(0xAC);
            info->mScenarioIDs.push_back(0xAD);
            info->mScenarioIDs.push_back(0xAE);
            info->mScenarioIDs.push_back(0xAF);
            info->mExScenarioIDs.push_back(0x205);
            info->mExScenarioIDs.push_back(0xB0);
            info->mExScenarioIDs.push_back(0xB1);
            info->mScenarioNameIDs.push_back(124);
            info->mScenarioNameIDs.push_back(125);
            info->mScenarioNameIDs.push_back(126);
            info->mScenarioNameIDs.push_back(127);
            info->mExScenarioNameIDs.push_back(128);
            info->mShineSelectPaneID = 'lf_0';
            BetterSMS::Stage::registerStageInfo(info->mNormalStageID, info);

            {
                auto exinfo            = new BetterSMS::Stage::ExAreaInfo;
                exinfo->mShineStageID  = info->mShineStageID;
                exinfo->mNormalStageID = SME::STAGE_LANCIA_EX;
                exinfo->mShineID       = 0xAB;
                BetterSMS::Stage::registerExStageInfo(exinfo->mNormalStageID, exinfo);
            }
        }

        // Register Vaporwave stage info
        {
            auto *info           = new BetterSMS::Stage::AreaInfo;
            info->mShineStageID  = SME::STAGE_VAPORWAVE - 2;
            info->mNormalStageID = SME::STAGE_VAPORWAVE;
            info->mScenarioIDs.push_back(0xB2);
            info->mScenarioIDs.push_back(0xB3);
            info->mScenarioIDs.push_back(0xB4);
            info->mScenarioIDs.push_back(0xB5);
            info->mExScenarioIDs.push_back(0x206);
            info->mExScenarioIDs.push_back(0xB6);
            info->mExScenarioIDs.push_back(0x00);
            info->mScenarioNameIDs.push_back(129);
            info->mScenarioNameIDs.push_back(130);
            info->mScenarioNameIDs.push_back(131);
            info->mScenarioNameIDs.push_back(132);
            info->mExScenarioNameIDs.push_back(133);
            info->mShineSelectPaneID = 'vw_0';
            BetterSMS::Stage::registerStageInfo(info->mNormalStageID, info);

            {
                auto exinfo            = new BetterSMS::Stage::ExAreaInfo;
                exinfo->mShineStageID  = 0x45;
                exinfo->mNormalStageID = 0x57;
                exinfo->mShineID       = 0xAD;
                BetterSMS::Stage::registerExStageInfo(exinfo->mNormalStageID, exinfo);
            }
        }

        // Register Yoshi Village stage info
        {
            auto *info           = new BetterSMS::Stage::AreaInfo;
            info->mShineStageID  = SME::STAGE_YOSHI_VILLAGE - 2;
            info->mNormalStageID = SME::STAGE_YOSHI_VILLAGE;
            info->mScenarioIDs.push_back(0xB7);
            info->mScenarioIDs.push_back(0xB8);
            info->mScenarioIDs.push_back(0xB9);
            info->mScenarioIDs.push_back(0xBA);
            info->mExScenarioIDs.push_back(0x207);
            info->mExScenarioIDs.push_back(0xBB);
            info->mExScenarioIDs.push_back(0x00);
            info->mScenarioNameIDs.push_back(134);
            info->mScenarioNameIDs.push_back(135);
            info->mScenarioNameIDs.push_back(136);
            info->mScenarioNameIDs.push_back(137);
            info->mExScenarioNameIDs.push_back(138);
            info->mShineSelectPaneID = 'yv_0';
            BetterSMS::Stage::registerStageInfo(info->mNormalStageID, info);

            {
                auto exinfo            = new BetterSMS::Stage::ExAreaInfo;
                exinfo->mShineStageID  = info->mShineStageID;
                exinfo->mNormalStageID = SME::STAGE_YOSHI_EX;
                exinfo->mShineID       = 0xAF;
                BetterSMS::Stage::registerExStageInfo(exinfo->mNormalStageID, exinfo);
            }
        }

        // Waterpark UNUSED
        // ****************

        // Register Red Lily City stage info
        {
            auto *info           = new BetterSMS::Stage::AreaInfo;
            info->mShineStageID  = SME::STAGE_RED_LILY - 2;
            info->mNormalStageID = SME::STAGE_RED_LILY;
            info->mScenarioIDs.push_back(0xC1);
            info->mScenarioIDs.push_back(0xC2);
            info->mScenarioIDs.push_back(0xC3);
            info->mScenarioIDs.push_back(0xC4);
            info->mExScenarioIDs.push_back(0x209);
            info->mExScenarioIDs.push_back(0xC5);
            info->mExScenarioIDs.push_back(0x00);
            info->mScenarioNameIDs.push_back(144);
            info->mScenarioNameIDs.push_back(145);
            info->mScenarioNameIDs.push_back(146);
            info->mScenarioNameIDs.push_back(147);
            info->mExScenarioNameIDs.push_back(148);
            info->mShineSelectPaneID = 'rl_0';
            BetterSMS::Stage::registerStageInfo(info->mNormalStageID, info);

            {
                auto exinfo            = new BetterSMS::Stage::ExAreaInfo;
                exinfo->mShineStageID  = info->mShineStageID;
                exinfo->mNormalStageID = SME::STAGE_RED_LILY_EX;
                exinfo->mShineID       = 0xB1;
                BetterSMS::Stage::registerExStageInfo(exinfo->mNormalStageID, exinfo);
            }
        }

        // Register Peach Beach stage info
        {
            auto *info           = new BetterSMS::Stage::AreaInfo;
            info->mShineStageID  = SME::STAGE_PEACH_BEACH - 2;
            info->mNormalStageID = SME::STAGE_PEACH_BEACH;
            info->mScenarioIDs.push_back(0xC6);
            info->mScenarioIDs.push_back(0xC7);
            info->mScenarioIDs.push_back(0xC8);
            info->mScenarioIDs.push_back(0xC9);
            info->mExScenarioIDs.push_back(0x20A);
            info->mExScenarioIDs.push_back(0xCA);
            info->mExScenarioIDs.push_back(0x00);
            info->mScenarioNameIDs.push_back(149);
            info->mScenarioNameIDs.push_back(150);
            info->mScenarioNameIDs.push_back(151);
            info->mScenarioNameIDs.push_back(152);
            info->mExScenarioNameIDs.push_back(153);
            info->mShineSelectPaneID = 'pb_0';
            BetterSMS::Stage::registerStageInfo(info->mNormalStageID, info);

            {
                auto exinfo            = new BetterSMS::Stage::ExAreaInfo;
                exinfo->mShineStageID  = info->mShineStageID;
                exinfo->mNormalStageID = SME::STAGE_PEACH_BEACH_EX;
                exinfo->mShineID       = 0xB3;
                BetterSMS::Stage::registerExStageInfo(exinfo->mNormalStageID, exinfo);
            }
        }

        // Register Spettro Casino stage info
        {
            auto *info           = new BetterSMS::Stage::AreaInfo;
            info->mShineStageID  = SME::STAGE_SPETTRO_CASINO - 2;
            info->mNormalStageID = SME::STAGE_SPETTRO_CASINO;
            info->mScenarioIDs.push_back(0xCB);
            info->mScenarioIDs.push_back(0xCC);
            info->mScenarioIDs.push_back(0xCD);
            info->mScenarioIDs.push_back(0xCE);
            info->mExScenarioIDs.push_back(0x20B);
            info->mExScenarioIDs.push_back(0xCF);
            info->mExScenarioIDs.push_back(0x00);
            info->mScenarioNameIDs.push_back(154);
            info->mScenarioNameIDs.push_back(155);
            info->mScenarioNameIDs.push_back(156);
            info->mScenarioNameIDs.push_back(157);
            info->mExScenarioNameIDs.push_back(158);
            info->mShineSelectPaneID = 'sp_0';
            BetterSMS::Stage::registerStageInfo(info->mNormalStageID, info);

            {
                auto exinfo            = new BetterSMS::Stage::ExAreaInfo;
                exinfo->mShineStageID  = info->mShineStageID;
                exinfo->mNormalStageID = SME::STAGE_SPETTRO_EX;
                exinfo->mShineID       = 0xB5;
                BetterSMS::Stage::registerExStageInfo(exinfo->mNormalStageID, exinfo);
            }
        }

        // Register Peach's Castle stage info
        {
            auto *info           = new BetterSMS::Stage::AreaInfo;
            info->mShineStageID  = SME::STAGE_PEACH_CASTLE - 2;
            info->mNormalStageID = SME::STAGE_PEACH_CASTLE;
            info->mScenarioIDs.push_back(0xD0);
            info->mScenarioIDs.push_back(0xD1);
            info->mScenarioIDs.push_back(0xD2);
            info->mScenarioIDs.push_back(0xD3);
            info->mScenarioIDs.push_back(0xD4);
            info->mExScenarioIDs.push_back(0x20C);
            info->mExScenarioIDs.push_back(0xD5);
            info->mExScenarioIDs.push_back(0xD6);
            info->mExScenarioIDs.push_back(0xD7);
            info->mExScenarioIDs.push_back(0xD8);
            info->mExScenarioIDs.push_back(0xD9);
            info->mExScenarioIDs.push_back(0xDA);
            info->mExScenarioIDs.push_back(0xDB);
            info->mExScenarioIDs.push_back(0xDC);
            info->mExScenarioIDs.push_back(0xDD);
            info->mExScenarioIDs.push_back(0xDE);
            info->mExScenarioIDs.push_back(0xDF);
            info->mExScenarioIDs.push_back(0xE0);
            info->mExScenarioIDs.push_back(0xE1);
            info->mExScenarioIDs.push_back(0xE2);
            info->mExScenarioIDs.push_back(0xE3);
            info->mExScenarioIDs.push_back(0xE4);
            info->mExScenarioIDs.push_back(0xE5);
            info->mExScenarioIDs.push_back(0x00);
            info->mScenarioNameIDs.push_back(159);
            info->mScenarioNameIDs.push_back(160);
            info->mScenarioNameIDs.push_back(161);
            info->mScenarioNameIDs.push_back(162);
            info->mScenarioNameIDs.push_back(163);
            info->mExScenarioNameIDs.push_back(164);  // Block world
            info->mExScenarioNameIDs.push_back(165);  // Beta Bianco
            info->mExScenarioNameIDs.push_back(166);  // Secret Aquarium
            info->mExScenarioNameIDs.push_back(167);  // Battle Fort
            info->mExScenarioNameIDs.push_back(168);  // Sunshine Isles
            info->mExScenarioNameIDs.push_back(169);  // Starshine Beach
            info->mExScenarioNameIDs.push_back(170);  // Beta Ricco
            info->mExScenarioNameIDs.push_back(171);  // E3 Bianco
            info->mExScenarioNameIDs.push_back(172);  // E3 Dolpic
            info->mExScenarioNameIDs.push_back(173);  // Dolpic Town
            info->mExScenarioNameIDs.push_back(174);  // Super Monkey Ball
            info->mExScenarioNameIDs.push_back(175);  // Beta Shell Secret
            info->mExScenarioNameIDs.push_back(176);  // Wade Stage
            info->mExScenarioNameIDs.push_back(177);  // Spaceworld Bianco
            info->mExScenarioNameIDs.push_back(178);  // Starshine Beach
            info->mExScenarioNameIDs.push_back(179);  // Windfall Island
            info->mExScenarioNameIDs.push_back(180);  // Old Lighthouse Island
            info->mShineSelectPaneID = 'pc_0';
            BetterSMS::Stage::registerStageInfo(info->mNormalStageID, info);

            {
                auto exinfo            = new BetterSMS::Stage::ExAreaInfo;
                exinfo->mShineStageID  = info->mShineStageID;
                exinfo->mNormalStageID = SME::STAGE_PEACH_CASTLE_EX0;
                exinfo->mShineID       = 0xB7;
                BetterSMS::Stage::registerExStageInfo(exinfo->mNormalStageID, exinfo);
            }

            {
                auto exinfo            = new BetterSMS::Stage::ExAreaInfo;
                exinfo->mShineStageID  = info->mShineStageID;
                exinfo->mNormalStageID = SME::STAGE_PEACH_CASTLE_EX1;
                exinfo->mShineID       = 0xB8;
                BetterSMS::Stage::registerExStageInfo(exinfo->mNormalStageID, exinfo);
            }

            {
                auto exinfo            = new BetterSMS::Stage::ExAreaInfo;
                exinfo->mShineStageID  = info->mShineStageID;
                exinfo->mNormalStageID = SME::STAGE_PEACH_CASTLE_EX2;
                exinfo->mShineID       = 0xB9;
                BetterSMS::Stage::registerExStageInfo(exinfo->mNormalStageID, exinfo);
            }

            {
                auto exinfo            = new BetterSMS::Stage::ExAreaInfo;
                exinfo->mShineStageID  = info->mShineStageID;
                exinfo->mNormalStageID = SME::STAGE_PEACH_CASTLE_EX3;
                exinfo->mShineID       = 0xBA;
                BetterSMS::Stage::registerExStageInfo(exinfo->mNormalStageID, exinfo);
            }

            {
                auto exinfo            = new BetterSMS::Stage::ExAreaInfo;
                exinfo->mShineStageID  = info->mShineStageID;
                exinfo->mNormalStageID = SME::STAGE_PEACH_CASTLE_EX4;
                exinfo->mShineID       = 0xBB;
                BetterSMS::Stage::registerExStageInfo(exinfo->mNormalStageID, exinfo);
            }

            {
                auto exinfo            = new BetterSMS::Stage::ExAreaInfo;
                exinfo->mShineStageID  = info->mShineStageID;
                exinfo->mNormalStageID = SME::STAGE_PEACH_CASTLE_EX5;
                exinfo->mShineID       = 0xBC;
                BetterSMS::Stage::registerExStageInfo(exinfo->mNormalStageID, exinfo);
            }

            {
                auto exinfo            = new BetterSMS::Stage::ExAreaInfo;
                exinfo->mShineStageID  = info->mShineStageID;
                exinfo->mNormalStageID = SME::STAGE_PEACH_CASTLE_EX6;
                exinfo->mShineID       = 0xBD;
                BetterSMS::Stage::registerExStageInfo(exinfo->mNormalStageID, exinfo);
            }

            {
                auto exinfo            = new BetterSMS::Stage::ExAreaInfo;
                exinfo->mShineStageID  = info->mShineStageID;
                exinfo->mNormalStageID = SME::STAGE_PEACH_CASTLE_EX7;
                exinfo->mShineID       = 0xBE;
                BetterSMS::Stage::registerExStageInfo(exinfo->mNormalStageID, exinfo);
            }

            {
                auto exinfo            = new BetterSMS::Stage::ExAreaInfo;
                exinfo->mShineStageID  = info->mShineStageID;
                exinfo->mNormalStageID = SME::STAGE_PEACH_CASTLE_EX8;
                exinfo->mShineID       = 0xBF;
                BetterSMS::Stage::registerExStageInfo(exinfo->mNormalStageID, exinfo);
            }

            {
                auto exinfo            = new BetterSMS::Stage::ExAreaInfo;
                exinfo->mShineStageID  = info->mShineStageID;
                exinfo->mNormalStageID = SME::STAGE_PEACH_CASTLE_EX9;
                exinfo->mShineID       = 0xC0;
                BetterSMS::Stage::registerExStageInfo(exinfo->mNormalStageID, exinfo);
            }

            {
                auto exinfo            = new BetterSMS::Stage::ExAreaInfo;
                exinfo->mShineStageID  = info->mShineStageID;
                exinfo->mNormalStageID = SME::STAGE_PEACH_CASTLE_EX10;
                exinfo->mShineID       = 0xC1;
                BetterSMS::Stage::registerExStageInfo(exinfo->mNormalStageID, exinfo);
            }

            {
                auto exinfo            = new BetterSMS::Stage::ExAreaInfo;
                exinfo->mShineStageID  = info->mShineStageID;
                exinfo->mNormalStageID = SME::STAGE_PEACH_CASTLE_EX11;
                exinfo->mShineID       = 0xC2;
                BetterSMS::Stage::registerExStageInfo(exinfo->mNormalStageID, exinfo);
            }

            {
                auto exinfo            = new BetterSMS::Stage::ExAreaInfo;
                exinfo->mShineStageID  = info->mShineStageID;
                exinfo->mNormalStageID = SME::STAGE_PEACH_CASTLE_EX12;
                exinfo->mShineID       = 0xC3;
                BetterSMS::Stage::registerExStageInfo(exinfo->mNormalStageID, exinfo);
            }

            {
                auto exinfo            = new BetterSMS::Stage::ExAreaInfo;
                exinfo->mShineStageID  = info->mShineStageID;
                exinfo->mNormalStageID = SME::STAGE_PEACH_CASTLE_EX13;
                exinfo->mShineID       = 0xC4;
                BetterSMS::Stage::registerExStageInfo(exinfo->mNormalStageID, exinfo);
            }

            {
                auto exinfo            = new BetterSMS::Stage::ExAreaInfo;
                exinfo->mShineStageID  = info->mShineStageID;
                exinfo->mNormalStageID = SME::STAGE_PEACH_CASTLE_EX14;
                exinfo->mShineID       = 0xC5;
                BetterSMS::Stage::registerExStageInfo(exinfo->mNormalStageID, exinfo);
            }

            {
                auto exinfo            = new BetterSMS::Stage::ExAreaInfo;
                exinfo->mShineStageID  = info->mShineStageID;
                exinfo->mNormalStageID = SME::STAGE_PEACH_CASTLE_EX15;
                exinfo->mShineID       = 0xC6;
                BetterSMS::Stage::registerExStageInfo(exinfo->mNormalStageID, exinfo);
            }

            {
                auto exinfo            = new BetterSMS::Stage::ExAreaInfo;
                exinfo->mShineStageID  = info->mShineStageID;
                exinfo->mNormalStageID = SME::STAGE_PEACH_CASTLE_EX16;
                exinfo->mShineID       = 0xC7;
                BetterSMS::Stage::registerExStageInfo(exinfo->mNormalStageID, exinfo);
            }
        }

        // Register Island Junctions stage info
        {
            auto *info           = new BetterSMS::Stage::AreaInfo;
            info->mShineStageID  = SME::STAGE_VAPORWAVE - 2;
            info->mNormalStageID = SME::STAGE_VAPORWAVE;
            info->mScenarioIDs.push_back(0xE6);
            info->mScenarioIDs.push_back(0xE7);
            info->mScenarioIDs.push_back(0xE8);
            info->mScenarioIDs.push_back(0xE9);
            info->mScenarioIDs.push_back(0xEA);
            info->mScenarioIDs.push_back(0xEB);
            info->mScenarioIDs.push_back(0xEC);
            info->mScenarioIDs.push_back(0xED);
            info->mScenarioIDs.push_back(0xF0);
            info->mScenarioNameIDs.push_back(181);  // Boilente Crypt
            info->mScenarioNameIDs.push_back(182);  // Arido Trail
            info->mScenarioNameIDs.push_back(183);  // Arcobaleno Cove
            info->mScenarioNameIDs.push_back(184);  // Sonnolento Grove
            info->mScenarioNameIDs.push_back(185);  // Rugivosa Marsh
            info->mScenarioNameIDs.push_back(186);  // Giovane Creek
            info->mScenarioNameIDs.push_back(187);  // Marea Reef
            info->mScenarioNameIDs.push_back(188);  // Ricco Station
            info->mScenarioNameIDs.push_back(189);  // Train Car
            info->mShineSelectPaneID = 'ij_0';
            BetterSMS::Stage::registerStageInfo(info->mNormalStageID, info);
        }

        // Register Lighthouse Island Boss
        {
            auto *info           = new BetterSMS::Stage::AreaInfo;
            info->mShineStageID  = SME::STAGE_LIGHTHOUSE;
            info->mNormalStageID = SME::STAGE_LIGHTHOUSE_BOSS;
            info->mScenarioIDs.push_back(0x82);
            info->mScenarioNameIDs.push_back(82);
            BetterSMS::Stage::registerStageInfo(info->mNormalStageID, info);
        }

        // Register Tutorial Stage Info
        {
            auto *info           = new BetterSMS::Stage::AreaInfo;
            info->mShineStageID  = SME::STAGE_TUTORIAL - 2;
            info->mNormalStageID = SME::STAGE_TUTORIAL;
            info->mScenarioIDs.push_back(0xF1);
            info->mScenarioNameIDs.push_back(190);
            BetterSMS::Stage::registerStageInfo(info->mNormalStageID, info);
        }

        //// Register Final Corona Level
        //{
        //    auto *info           = new BetterSMS::Stage::AreaInfo;
        //    info->mShineStageID  = 0x9;
        //    info->mNormalStageID = 0xA;
        //    info->mScenarioIDs.push_back(0xED);
        //    info->mScenarioIDs.push_back(0xEE);
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