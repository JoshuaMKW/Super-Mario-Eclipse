#include <BetterSMS/area.hxx>
#include <BetterSMS/module.hxx>
#include <BetterSMS/stage.hxx>
#include <SMS/Manager/FlagManager.hxx>

BETTER_SMS_FOR_CALLBACK void initializeStageInfo(TApplication *app) {
    {
        auto *old_heap = JKRHeap::sRootHeap->becomeCurrentHeap();

        // Register Erto Rock stage info
        {
            auto *info           = new BetterSMS::Stage::AreaInfo;
            info->mShineStageID  = 0x3D;
            info->mNormalStageID = 0x3D;
            info->mScenarioIDs.push_back(0x78);
            info->mScenarioIDs.push_back(0x79);
            info->mScenarioIDs.push_back(0x7A);
            info->mScenarioIDs.push_back(0x7B);
            info->mScenarioIDs.push_back(0x7C);
            info->mScenarioIDs.push_back(0x7D);
            info->mScenarioIDs.push_back(0x7E);
            info->mScenarioIDs.push_back(0x7F);
            info->mExScenarioIDs.push_back(0x100);
            info->mExScenarioIDs.push_back(0xE0);
            info->mExScenarioIDs.push_back(0xE1);
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
            BetterSMS::Stage::registerStageInfo(0x3D, info);

            {
                auto exinfo            = new BetterSMS::Stage::ExAreaInfo;
                exinfo->mShineStageID  = 0x3D;
                exinfo->mNormalStageID = 0x4D;
                exinfo->mShineID       = 0xA0;
                BetterSMS::Stage::registerExStageInfo(0xA0, exinfo);
            }

            {
                auto exinfo            = new BetterSMS::Stage::ExAreaInfo;
                exinfo->mShineStageID  = 0x3D;
                exinfo->mNormalStageID = 0x4D;
                exinfo->mShineID       = 0xA1;
                BetterSMS::Stage::registerExStageInfo(0xA1, exinfo);
            }
        }

        // Register Lighthouse Island stage info
        {
            auto *info           = new BetterSMS::Stage::AreaInfo;
            info->mShineStageID  = 0x3E;
            info->mNormalStageID = 0x3E;
            info->mScenarioIDs.push_back(0x80);
            info->mScenarioIDs.push_back(0x81);
            info->mScenarioIDs.push_back(0x82);
            info->mScenarioIDs.push_back(0x83);
            info->mScenarioIDs.push_back(0x84);
            info->mScenarioIDs.push_back(0x85);
            info->mScenarioIDs.push_back(0x86);
            info->mScenarioIDs.push_back(0x87);
            info->mExScenarioIDs.push_back(0x101);
            info->mExScenarioIDs.push_back(0xE2);
            info->mExScenarioIDs.push_back(0xE3);
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
            BetterSMS::Stage::registerStageInfo(0x3E, info);

            {
                auto exinfo            = new BetterSMS::Stage::ExAreaInfo;
                exinfo->mShineStageID  = 0x3E;
                exinfo->mNormalStageID = 0x4E;
                exinfo->mShineID       = 0xA2;
                BetterSMS::Stage::registerExStageInfo(0xA2, exinfo);
            }

            {
                auto exinfo            = new BetterSMS::Stage::ExAreaInfo;
                exinfo->mShineStageID  = 0x3E;
                exinfo->mNormalStageID = 0x4E;
                exinfo->mShineID       = 0xA3;
                BetterSMS::Stage::registerExStageInfo(0xA3, exinfo);
            }
        }

        // Register Warship Island stage info
        {
            auto *info           = new BetterSMS::Stage::AreaInfo;
            info->mShineStageID  = 0x3F;
            info->mNormalStageID = 0x3F;
            info->mScenarioIDs.push_back(0x88);
            info->mScenarioIDs.push_back(0x89);
            info->mScenarioIDs.push_back(0x8A);
            info->mScenarioIDs.push_back(0x8B);
            info->mScenarioIDs.push_back(0x8C);
            info->mScenarioIDs.push_back(0x8D);
            info->mScenarioIDs.push_back(0x8E);
            info->mScenarioIDs.push_back(0x8F);
            info->mExScenarioIDs.push_back(0x102);
            info->mExScenarioIDs.push_back(0xE4);
            info->mExScenarioIDs.push_back(0xE5);
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
            BetterSMS::Stage::registerStageInfo(0x3F, info);

            {
                auto exinfo            = new BetterSMS::Stage::ExAreaInfo;
                exinfo->mShineStageID  = 0x3F;
                exinfo->mNormalStageID = 0x4F;
                exinfo->mShineID       = 0xA4;
                BetterSMS::Stage::registerExStageInfo(0xA4, exinfo);
            }

            {
                auto exinfo            = new BetterSMS::Stage::ExAreaInfo;
                exinfo->mShineStageID  = 0x3F;
                exinfo->mNormalStageID = 0x4F;
                exinfo->mShineID       = 0xA5;
                BetterSMS::Stage::registerExStageInfo(0xA5, exinfo);
            }
        }

        // Register Hotel Lacrima stage info
        {
            auto *info           = new BetterSMS::Stage::AreaInfo;
            info->mShineStageID  = 0x40;
            info->mNormalStageID = 0x40;
            info->mScenarioIDs.push_back(0x90);
            info->mScenarioIDs.push_back(0x91);
            info->mScenarioIDs.push_back(0x92);
            info->mScenarioIDs.push_back(0x93);
            info->mScenarioIDs.push_back(0x94);
            info->mScenarioIDs.push_back(0x95);
            info->mScenarioIDs.push_back(0x96);
            info->mScenarioIDs.push_back(0x97);
            info->mExScenarioIDs.push_back(0x103);
            info->mExScenarioIDs.push_back(0xE6);
            info->mExScenarioIDs.push_back(0xE7);
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
            BetterSMS::Stage::registerStageInfo(0x40, info);

            {
                auto exinfo            = new BetterSMS::Stage::ExAreaInfo;
                exinfo->mShineStageID  = 0x40;
                exinfo->mNormalStageID = 0x50;
                exinfo->mShineID       = 0xA6;
                BetterSMS::Stage::registerExStageInfo(0xA7, exinfo);
            }

            {
                auto exinfo            = new BetterSMS::Stage::ExAreaInfo;
                exinfo->mShineStageID  = 0x40;
                exinfo->mNormalStageID = 0x50;
                exinfo->mShineID       = 0xA8;
                BetterSMS::Stage::registerExStageInfo(0xA8, exinfo);
            }
        }

        // Register Pianta Pit stage info
        {
            auto *info           = new BetterSMS::Stage::AreaInfo;
            info->mShineStageID  = 0x41;
            info->mNormalStageID = 0x41;
            info->mScenarioIDs.push_back(0x98);
            info->mScenarioIDs.push_back(0x99);
            info->mScenarioNameIDs.push_back(110);
            info->mScenarioNameIDs.push_back(111);
            info->mShineSelectPaneID = 'pp_0';
            BetterSMS::Stage::registerStageInfo(0x41, info);
        }

        // Register Daisy Cruiser stage info
        {
            auto *info           = new BetterSMS::Stage::AreaInfo;
            info->mShineStageID  = 0x42;
            info->mNormalStageID = 0x42;
            info->mScenarioIDs.push_back(0x9A);
            info->mScenarioIDs.push_back(0x9B);
            info->mScenarioIDs.push_back(0x9C);
            info->mScenarioIDs.push_back(0x9D);
            info->mScenarioIDs.push_back(0x9E);
            info->mScenarioIDs.push_back(0x9F);
            info->mExScenarioIDs.push_back(0x104);
            info->mExScenarioIDs.push_back(0xE8);
            info->mExScenarioIDs.push_back(0xE9);
            info->mExScenarioIDs.push_back(0x00);
            info->mScenarioNameIDs.push_back(112);
            info->mScenarioNameIDs.push_back(113);
            info->mScenarioNameIDs.push_back(114);
            info->mScenarioNameIDs.push_back(115);
            info->mScenarioNameIDs.push_back(116);
            info->mScenarioNameIDs.push_back(117);
            info->mExScenarioNameIDs.push_back(118);
            info->mExScenarioNameIDs.push_back(119);
            info->mShineSelectPaneID = 'dc_0';
            BetterSMS::Stage::registerStageInfo(0x42, info);
        }

        // Register Mario's Dream stage info
        {
            auto *info           = new BetterSMS::Stage::AreaInfo;
            info->mShineStageID  = 0x43;
            info->mNormalStageID = 0x43;
            info->mScenarioIDs.push_back(0x100);
            info->mScenarioIDs.push_back(0x101);
            info->mScenarioIDs.push_back(0x102);
            info->mScenarioIDs.push_back(0x103);
            info->mScenarioNameIDs.push_back(120);
            info->mScenarioNameIDs.push_back(121);
            info->mScenarioNameIDs.push_back(122);
            info->mScenarioNameIDs.push_back(123);
            info->mShineSelectPaneID = 'md_0';
            BetterSMS::Stage::registerStageInfo(0x43, info);
        }

        // Register Lancia Fredda stage info
        {
            auto *info           = new BetterSMS::Stage::AreaInfo;
            info->mShineStageID  = 0x44;
            info->mNormalStageID = 0x44;
            info->mScenarioIDs.push_back(0x104);
            info->mScenarioIDs.push_back(0x105);
            info->mScenarioIDs.push_back(0x106);
            info->mScenarioIDs.push_back(0x107);
            info->mExScenarioIDs.push_back(0x105);
            info->mExScenarioIDs.push_back(0xEA);
            info->mExScenarioIDs.push_back(0x00);
            info->mScenarioNameIDs.push_back(124);
            info->mScenarioNameIDs.push_back(125);
            info->mScenarioNameIDs.push_back(126);
            info->mScenarioNameIDs.push_back(127);
            info->mExScenarioNameIDs.push_back(128);
            info->mShineSelectPaneID = 'lf_0';
            BetterSMS::Stage::registerStageInfo(0x44, info);
        }

        // Register Vaporwave stage info
        {
            auto *info           = new BetterSMS::Stage::AreaInfo;
            info->mShineStageID  = 0x45;
            info->mNormalStageID = 0x45;
            info->mScenarioIDs.push_back(0x108);
            info->mScenarioIDs.push_back(0x109);
            info->mScenarioIDs.push_back(0x110);
            info->mScenarioIDs.push_back(0x111);
            info->mExScenarioIDs.push_back(0x106);
            info->mExScenarioIDs.push_back(0xEB);
            info->mExScenarioIDs.push_back(0x00);
            info->mScenarioNameIDs.push_back(129);
            info->mScenarioNameIDs.push_back(130);
            info->mScenarioNameIDs.push_back(131);
            info->mScenarioNameIDs.push_back(132);
            info->mExScenarioNameIDs.push_back(133);
            info->mShineSelectPaneID = 'vw_0';
            BetterSMS::Stage::registerStageInfo(0x45, info);
        }

        // Register Yoshi Village stage info
        {
            auto *info           = new BetterSMS::Stage::AreaInfo;
            info->mShineStageID  = 0x46;
            info->mNormalStageID = 0x46;
            info->mScenarioIDs.push_back(0x112);
            info->mScenarioIDs.push_back(0x113);
            info->mScenarioIDs.push_back(0x114);
            info->mScenarioIDs.push_back(0x115);
            info->mExScenarioIDs.push_back(0x107);
            info->mExScenarioIDs.push_back(0xEC);
            info->mExScenarioIDs.push_back(0x00);
            info->mScenarioNameIDs.push_back(134);
            info->mScenarioNameIDs.push_back(135);
            info->mScenarioNameIDs.push_back(136);
            info->mScenarioNameIDs.push_back(137);
            info->mExScenarioNameIDs.push_back(138);
            info->mShineSelectPaneID = 'yv_0';
            BetterSMS::Stage::registerStageInfo(0x46, info);
        }

        // Register Waterpark stage info
        {
            auto *info           = new BetterSMS::Stage::AreaInfo;
            info->mShineStageID  = 0x47;
            info->mNormalStageID = 0x47;
            info->mScenarioIDs.push_back(0x116);
            info->mScenarioIDs.push_back(0x117);
            info->mScenarioIDs.push_back(0x118);
            info->mScenarioIDs.push_back(0x119);
            info->mExScenarioIDs.push_back(0x108);
            info->mExScenarioIDs.push_back(0xED);
            info->mExScenarioIDs.push_back(0x00);
            info->mScenarioNameIDs.push_back(139);
            info->mScenarioNameIDs.push_back(140);
            info->mScenarioNameIDs.push_back(141);
            info->mScenarioNameIDs.push_back(142);
            info->mExScenarioNameIDs.push_back(143);
            info->mShineSelectPaneID = 'wp_0';
            BetterSMS::Stage::registerStageInfo(0x47, info);
        }

        // Register Red Lily City stage info
        {
            auto *info           = new BetterSMS::Stage::AreaInfo;
            info->mShineStageID  = 0x48;
            info->mNormalStageID = 0x48;
            info->mScenarioIDs.push_back(0x11A);
            info->mScenarioIDs.push_back(0x11B);
            info->mScenarioIDs.push_back(0x11C);
            info->mScenarioIDs.push_back(0x11D);
            info->mExScenarioIDs.push_back(0x109);
            info->mExScenarioIDs.push_back(0xEE);
            info->mExScenarioIDs.push_back(0x00);
            info->mScenarioNameIDs.push_back(144);
            info->mScenarioNameIDs.push_back(145);
            info->mScenarioNameIDs.push_back(146);
            info->mScenarioNameIDs.push_back(147);
            info->mExScenarioNameIDs.push_back(148);
            info->mShineSelectPaneID = 'rl_0';
            BetterSMS::Stage::registerStageInfo(0x48, info);
        }

        // Register Peach Beach stage info
        {
            auto *info           = new BetterSMS::Stage::AreaInfo;
            info->mShineStageID  = 0x49;
            info->mNormalStageID = 0x49;
            info->mScenarioIDs.push_back(0x11E);
            info->mScenarioIDs.push_back(0x11F);
            info->mScenarioIDs.push_back(0x120);
            info->mScenarioIDs.push_back(0x121);
            info->mExScenarioIDs.push_back(0x10A);
            info->mExScenarioIDs.push_back(0xEF);
            info->mExScenarioIDs.push_back(0x00);
            info->mScenarioNameIDs.push_back(149);
            info->mScenarioNameIDs.push_back(150);
            info->mScenarioNameIDs.push_back(151);
            info->mScenarioNameIDs.push_back(152);
            info->mExScenarioNameIDs.push_back(153);
            info->mShineSelectPaneID = 'pb_0';
            BetterSMS::Stage::registerStageInfo(0x49, info);
        }

        // Register Spettro Casino stage info
        {
            auto *info           = new BetterSMS::Stage::AreaInfo;
            info->mShineStageID  = 0x4A;
            info->mNormalStageID = 0x4A;
            info->mScenarioIDs.push_back(0x122);
            info->mScenarioIDs.push_back(0x123);
            info->mScenarioIDs.push_back(0x124);
            info->mScenarioIDs.push_back(0x125);
            info->mExScenarioIDs.push_back(0x10B);
            info->mExScenarioIDs.push_back(0xF0);
            info->mExScenarioIDs.push_back(0x00);
            info->mScenarioNameIDs.push_back(154);
            info->mScenarioNameIDs.push_back(155);
            info->mScenarioNameIDs.push_back(156);
            info->mScenarioNameIDs.push_back(157);
            info->mExScenarioNameIDs.push_back(158);
            info->mShineSelectPaneID = 'sp_0';
            BetterSMS::Stage::registerStageInfo(0x4A, info);
        }

        // Register Peach's Castle stage info
        {
            auto *info           = new BetterSMS::Stage::AreaInfo;
            info->mShineStageID  = 0x4B;
            info->mNormalStageID = 0x4B;
            info->mScenarioIDs.push_back(0x126);
            info->mExScenarioIDs.push_back(0x10C);
            info->mExScenarioIDs.push_back(0xF1);
            info->mExScenarioIDs.push_back(0xF2);
            info->mExScenarioIDs.push_back(0xF3);
            info->mExScenarioIDs.push_back(0xF4);
            info->mExScenarioIDs.push_back(0xF5);
            info->mExScenarioIDs.push_back(0xF6);
            info->mExScenarioIDs.push_back(0xF7);
            info->mExScenarioIDs.push_back(0xF8);
            info->mExScenarioIDs.push_back(0xF9);
            info->mExScenarioIDs.push_back(0xFA);
            info->mExScenarioIDs.push_back(0xFB);
            info->mExScenarioIDs.push_back(0xFC);
            info->mExScenarioIDs.push_back(0xFD);
            info->mExScenarioIDs.push_back(0xFE);
            info->mExScenarioIDs.push_back(0x00);
            info->mScenarioNameIDs.push_back(159);
            info->mExScenarioNameIDs.push_back(160);  // Block world
            info->mExScenarioNameIDs.push_back(161);  // Beta Bianco
            info->mExScenarioNameIDs.push_back(162);  // Secret Aquarium
            info->mExScenarioNameIDs.push_back(163);  // Battle Fort
            info->mExScenarioNameIDs.push_back(164);  // Sunshine Isles
            info->mExScenarioNameIDs.push_back(165);  // Starshine Beach
            info->mExScenarioNameIDs.push_back(166);  // Beta Ricco
            info->mExScenarioNameIDs.push_back(167);  // E3 Bianco
            info->mExScenarioNameIDs.push_back(168);  // E3 Dolpic
            info->mExScenarioNameIDs.push_back(169);  // Sweet Sweet Galaxy
            info->mExScenarioNameIDs.push_back(170);  // Zelda Windwaker
            info->mExScenarioNameIDs.push_back(171);  // Spaceworld Dolpic
            info->mExScenarioNameIDs.push_back(172);  // Beta Shell Secret
            info->mExScenarioNameIDs.push_back(173);  // Super Monkey Ball
            info->mShineSelectPaneID = 'pc_0';
            BetterSMS::Stage::registerStageInfo(0x4B, info);
        }

        // Register Island Junctions stage info
        {
            auto *info           = new BetterSMS::Stage::AreaInfo;
            info->mShineStageID  = 0x4C;
            info->mNormalStageID = 0x4C;
            info->mScenarioIDs.push_back(0x127);
            info->mScenarioIDs.push_back(0x128);
            info->mScenarioIDs.push_back(0x129);
            info->mScenarioIDs.push_back(0x12A);
            info->mScenarioIDs.push_back(0x12B);
            info->mScenarioIDs.push_back(0x12C);
            info->mScenarioIDs.push_back(0x12D);
            info->mScenarioNameIDs.push_back(174);  // Giovane Creek
            info->mScenarioNameIDs.push_back(175);  // Arido Trail
            info->mScenarioNameIDs.push_back(176);  // Boilente Crypt
            info->mScenarioNameIDs.push_back(177);  // Marea Crypt
            info->mScenarioNameIDs.push_back(178);  // Rugivosa Marsh
            info->mScenarioNameIDs.push_back(179);  // Sonnolento Grove
            info->mScenarioNameIDs.push_back(180);  // Arcobaleno Cove
            info->mShineSelectPaneID = 'ij_0';
            BetterSMS::Stage::registerStageInfo(0x4C, info);
        }

        // Register Final Corona Level
        {
            auto *info           = new BetterSMS::Stage::AreaInfo;
            info->mShineStageID  = 0x9;
            info->mNormalStageID = 0xA;
            info->mScenarioIDs.push_back(0x12E);
            info->mScenarioIDs.push_back(0x12F);
            info->mScenarioNameIDs.push_back(181);
            info->mScenarioNameIDs.push_back(182);
            info->mShineSelectPaneID = 'el_0';
            BetterSMS::Stage::registerStageInfo(0x9, info);
        }

        old_heap->becomeCurrentHeap();
    }
}

BETTER_SMS_FOR_CALLBACK void resetForExStage(TMarDirector *director) {
    if (!BetterSMS::Stage::isExStage(director->mAreaID, director->mEpisodeID))
        return;

    TFlagManager::smInstance->setFlag(0x40002, 0);
}