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
            BetterSMS::Stage::registerStageInfo(0x3D, info);

            {
                auto exinfo            = new BetterSMS::Stage::ExAreaInfo;
                exinfo->mShineStageID  = 0x3D;
                exinfo->mNormalStageID = 0x51;
                exinfo->mShineID = 0xA0;
                BetterSMS::Stage::registerExStageInfo(0x51, exinfo);
            }
        }

        // Register Lighthouse Island stage info
        {
            auto *info           = new BetterSMS::Stage::AreaInfo;
            info->mShineStageID  = 0x3E;
            info->mNormalStageID = 0x3E;
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
            BetterSMS::Stage::registerStageInfo(0x3E, info);

            {
                auto exinfo            = new BetterSMS::Stage::ExAreaInfo;
                exinfo->mShineStageID  = 0x3E;
                exinfo->mNormalStageID = 0x52;
                exinfo->mShineID = 0xA2;
                BetterSMS::Stage::registerExStageInfo(0x52, exinfo);
            }
        }

        // Register Warship Island stage info
        {
            auto *info           = new BetterSMS::Stage::AreaInfo;
            info->mShineStageID  = 0x3F;
            info->mNormalStageID = 0x3F;
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
            BetterSMS::Stage::registerStageInfo(0x3F, info);

            {
                auto exinfo            = new BetterSMS::Stage::ExAreaInfo;
                exinfo->mShineStageID  = 0x3F;
                exinfo->mNormalStageID = 0x53;
                exinfo->mShineID       = 0xA4;
                BetterSMS::Stage::registerExStageInfo(0x53, exinfo);
            }
        }

        // Register Hotel Lacrima stage info
        {
            auto *info           = new BetterSMS::Stage::AreaInfo;
            info->mShineStageID  = 0x40;
            info->mNormalStageID = 0x40;
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
            BetterSMS::Stage::registerStageInfo(0x40, info);

            {
                auto exinfo            = new BetterSMS::Stage::ExAreaInfo;
                exinfo->mShineStageID  = 0x40;
                exinfo->mNormalStageID = 0x54;
                exinfo->mShineID       = 0xA6;
                BetterSMS::Stage::registerExStageInfo(0x54, exinfo);
            }
        }

        // Register Hotel Lacrima Inside info
        {
            auto *info           = new BetterSMS::Stage::AreaInfo;
            info->mShineStageID  = 0x40;
            info->mNormalStageID = 0x41;
            info->mScenarioIDs.push_back(0x96);
            info->mScenarioIDs.push_back(0x9C);
            info->mScenarioNameIDs.push_back(102);
            info->mScenarioNameIDs.push_back(108);
            BetterSMS::Stage::registerStageInfo(0x41, info);
        }

        // Register Hotel Lacrima Backhouse info
        {
            auto *info           = new BetterSMS::Stage::AreaInfo;
            info->mShineStageID  = 0x40;
            info->mNormalStageID = 0x42;
            info->mScenarioIDs.push_back(0x96);
            info->mScenarioIDs.push_back(0x9C);
            info->mScenarioNameIDs.push_back(102);
            info->mScenarioNameIDs.push_back(108);
            BetterSMS::Stage::registerStageInfo(0x42, info);
        }

        // Register Pianta Pit stage info
        {
            auto *info           = new BetterSMS::Stage::AreaInfo;
            info->mShineStageID  = 0x41;
            info->mNormalStageID = 0x43;
            info->mScenarioIDs.push_back(0xA0);
            info->mScenarioIDs.push_back(0xA1);
            info->mScenarioNameIDs.push_back(112);
            info->mScenarioNameIDs.push_back(113);
            info->mShineSelectPaneID = 'pp_0';
            BetterSMS::Stage::registerStageInfo(0x43, info);
        }

        // Register Daisy Cruiser stage info
        {
            auto *info           = new BetterSMS::Stage::AreaInfo;
            info->mShineStageID  = 0x42;
            info->mNormalStageID = 0x44;
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
            BetterSMS::Stage::registerStageInfo(0x44, info);

            {
                auto exinfo            = new BetterSMS::Stage::ExAreaInfo;
                exinfo->mShineStageID  = 0x42;
                exinfo->mNormalStageID = 0x55;
                exinfo->mShineID       = 0xA8;
                BetterSMS::Stage::registerExStageInfo(0x55, exinfo);
            }
        }

        // Register Mario's Dream stage info
        {
            auto *info           = new BetterSMS::Stage::AreaInfo;
            info->mShineStageID  = 0x43;
            info->mNormalStageID = 0x45;
            info->mScenarioIDs.push_back(0xA8);
            info->mScenarioIDs.push_back(0xA9);
            info->mScenarioIDs.push_back(0xAA);
            info->mScenarioIDs.push_back(0xAB);
            info->mScenarioNameIDs.push_back(120);
            info->mScenarioNameIDs.push_back(121);
            info->mScenarioNameIDs.push_back(122);
            info->mScenarioNameIDs.push_back(123);
            info->mShineSelectPaneID = 'md_0';
            BetterSMS::Stage::registerStageInfo(0x45, info);
        }

        // Register Lancia Fredda stage info
        {
            auto *info           = new BetterSMS::Stage::AreaInfo;
            info->mShineStageID  = 0x44;
            info->mNormalStageID = 0x46;
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
            BetterSMS::Stage::registerStageInfo(0x46, info);

            {
                auto exinfo            = new BetterSMS::Stage::ExAreaInfo;
                exinfo->mShineStageID  = 0x44;
                exinfo->mNormalStageID = 0x56;
                exinfo->mShineID       = 0xAA;
                BetterSMS::Stage::registerExStageInfo(0x56, exinfo);
            }
        }

        // Register Vaporwave stage info
        {
            auto *info           = new BetterSMS::Stage::AreaInfo;
            info->mShineStageID  = 0x45;
            info->mNormalStageID = 0x47;
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
            BetterSMS::Stage::registerStageInfo(0x47, info);

            {
                auto exinfo            = new BetterSMS::Stage::ExAreaInfo;
                exinfo->mShineStageID  = 0x45;
                exinfo->mNormalStageID = 0x57;
                exinfo->mShineID       = 0xAC;
                BetterSMS::Stage::registerExStageInfo(0x57, exinfo);
            }
        }

        // Register Yoshi Village stage info
        {
            auto *info           = new BetterSMS::Stage::AreaInfo;
            info->mShineStageID  = 0x46;
            info->mNormalStageID = 0x48;
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
            BetterSMS::Stage::registerStageInfo(0x48, info);

            {
                auto exinfo            = new BetterSMS::Stage::ExAreaInfo;
                exinfo->mShineStageID  = 0x46;
                exinfo->mNormalStageID = 0x58;
                exinfo->mShineID       = 0xAE;
                BetterSMS::Stage::registerExStageInfo(0x58, exinfo);
            }
        }

        // Register Waterpark stage info
        {
            auto *info           = new BetterSMS::Stage::AreaInfo;
            info->mShineStageID  = 0x47;
            info->mNormalStageID = 0x49;
            info->mScenarioIDs.push_back(0xBC);
            info->mScenarioIDs.push_back(0xBD);
            info->mScenarioIDs.push_back(0xBE);
            info->mScenarioIDs.push_back(0xBF);
            info->mExScenarioIDs.push_back(0x208);
            info->mExScenarioIDs.push_back(0xC0);
            info->mExScenarioIDs.push_back(0x00);
            info->mScenarioNameIDs.push_back(139);
            info->mScenarioNameIDs.push_back(140);
            info->mScenarioNameIDs.push_back(141);
            info->mScenarioNameIDs.push_back(142);
            info->mExScenarioNameIDs.push_back(143);
            info->mShineSelectPaneID = 'wp_0';
            BetterSMS::Stage::registerStageInfo(0x49, info);

            {
                auto exinfo            = new BetterSMS::Stage::ExAreaInfo;
                exinfo->mShineStageID  = 0x47;
                exinfo->mNormalStageID = 0x59;
                exinfo->mShineID       = 0xB0;
                BetterSMS::Stage::registerExStageInfo(0x59, exinfo);
            }
        }

        // Register Red Lily City stage info
        {
            auto *info           = new BetterSMS::Stage::AreaInfo;
            info->mShineStageID  = 0x48;
            info->mNormalStageID = 0x4A;
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
            BetterSMS::Stage::registerStageInfo(0x4A, info);

            {
                auto exinfo            = new BetterSMS::Stage::ExAreaInfo;
                exinfo->mShineStageID  = 0x48;
                exinfo->mNormalStageID = 0x5A;
                exinfo->mShineID       = 0xB2;
                BetterSMS::Stage::registerExStageInfo(0x5A, exinfo);
            }
        }

        // Register Peach Beach stage info
        {
            auto *info           = new BetterSMS::Stage::AreaInfo;
            info->mShineStageID  = 0x49;
            info->mNormalStageID = 0x4B;
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
            BetterSMS::Stage::registerStageInfo(0x4B, info);

            {
                auto exinfo            = new BetterSMS::Stage::ExAreaInfo;
                exinfo->mShineStageID  = 0x49;
                exinfo->mNormalStageID = 0x5B;
                exinfo->mShineID       = 0xB4;
                BetterSMS::Stage::registerExStageInfo(0x5B, exinfo);
            }
        }

        // Register Spettro Casino stage info
        {
            auto *info           = new BetterSMS::Stage::AreaInfo;
            info->mShineStageID  = 0x4A;
            info->mNormalStageID = 0x4C;
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
            BetterSMS::Stage::registerStageInfo(0x4C, info);

            {
                auto exinfo            = new BetterSMS::Stage::ExAreaInfo;
                exinfo->mShineStageID  = 0x4A;
                exinfo->mNormalStageID = 0x5C;
                exinfo->mShineID       = 0xB6;
                BetterSMS::Stage::registerExStageInfo(0x5C, exinfo);
            }
        }

        // Register Peach's Castle stage info
        {
            auto *info           = new BetterSMS::Stage::AreaInfo;
            info->mShineStageID  = 0x4B;
            info->mNormalStageID = 0x4D;
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
            BetterSMS::Stage::registerStageInfo(0x4D, info);

            {
                auto exinfo            = new BetterSMS::Stage::ExAreaInfo;
                exinfo->mShineStageID  = 0x4B;
                exinfo->mNormalStageID = 0x5D;
                exinfo->mShineID       = 0xB8;
                BetterSMS::Stage::registerExStageInfo(0x5D, exinfo);
            }
        }

        // Register Island Junctions stage info
        {
            auto *info           = new BetterSMS::Stage::AreaInfo;
            info->mShineStageID  = 0x4C;
            info->mNormalStageID = 0x4E;
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
            BetterSMS::Stage::registerStageInfo(0x4E, info);
        }

        // Register Lighthouse Island Boss
        {
            auto *info           = new BetterSMS::Stage::AreaInfo;
            info->mShineStageID  = 0x3E;
            info->mNormalStageID = 0x4F;
            info->mScenarioIDs.push_back(0x82);
            info->mScenarioNameIDs.push_back(82);
            BetterSMS::Stage::registerStageInfo(0x4F, info);
        }

        // Register Tutorial Stage Info
        {
            auto *info           = new BetterSMS::Stage::AreaInfo;
            info->mShineStageID  = 0x4D;
            info->mNormalStageID = 0x50;
            info->mScenarioIDs.push_back(0xF1);
            info->mScenarioNameIDs.push_back(190);
            BetterSMS::Stage::registerStageInfo(0x50, info);
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
        //    BetterSMS::Stage::registerStageInfo(0xA, info);
        //}

        old_heap->becomeCurrentHeap();
    }
}

BETTER_SMS_FOR_CALLBACK void resetForExStage(TMarDirector *director) {
    if (!BetterSMS::Stage::isExStage(director->mAreaID, director->mEpisodeID))
        return;

    TFlagManager::smInstance->setFlag(0x40002, 0);
}