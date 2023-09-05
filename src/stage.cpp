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
                exinfo->mParentStageID = 0x3D;
                exinfo->mShineID       = 0xA0;
                BetterSMS::Stage::registerExStageInfo(0xA0, exinfo);
            }

            {
                auto exinfo            = new BetterSMS::Stage::ExAreaInfo;
                exinfo->mParentStageID = 0x3D;
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
                exinfo->mParentStageID = 0x3E;
                exinfo->mShineID       = 0xA2;
                BetterSMS::Stage::registerExStageInfo(0xA2, exinfo);
            }

            {
                auto exinfo            = new BetterSMS::Stage::ExAreaInfo;
                exinfo->mParentStageID = 0x3E;
                exinfo->mShineID       = 0xA3;
                BetterSMS::Stage::registerExStageInfo(0xA3, exinfo);
            }
        }
        old_heap->becomeCurrentHeap();
    }
}

BETTER_SMS_FOR_CALLBACK void resetForExStage(TMarDirector *director) {
    if (!BetterSMS::Stage::isExStage(director->mAreaID, director->mEpisodeID))
        return;

    TFlagManager::smInstance->setFlag(0x40002, 0);
}