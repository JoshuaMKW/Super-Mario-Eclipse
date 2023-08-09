#include <SMS/Manager/FlagManager.hxx>
#include <BetterSMS/stage.hxx>
#include <BetterSMS/module.hxx>

BETTER_SMS_FOR_CALLBACK void resetForExStage(TMarDirector *director) {
    if (!BetterSMS::Stage::isExStage(director->mAreaID, director->mEpisodeID))
        return;

    TFlagManager::smInstance->setFlag(0x40002, 0);
}