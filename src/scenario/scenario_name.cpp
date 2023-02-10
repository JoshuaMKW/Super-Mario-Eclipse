#include <SMS/manager/FlagManager.hxx>
#include <SMS/System/MarDirector.hxx>
#include <SMS/raw_fn.hxx>

#include "sdk.h"
#include "BetterSMS/logging.hxx"


char *loadStageNameFromBMG(void *unk, u32 index)
{
    void *mapBMG = JKRFileLoader::getGlbResource("/scene/map/stagename.bmg");
    char *message;
    if (mapBMG)
    {
        BetterSMS::Console::log("[WARNING] /scene/map/stagename.bmg missing from archive, falling back to default stage name loading\n");
        message = (char *)SMSGetMessageData__FPvUl(mapBMG, 0);
    }
    else
    {
        message = (char *)SMSGetMessageData__FPvUl(unk, index);
    }
    return message;
}

char *loadScenarioNameFromBMG(void *unk, u32 index)
{
    void *mapBMG = JKRFileLoader::getGlbResource("/scene/map/stagename.bmg");

    char *message;
    if (mapBMG)
    {
        message = (char *)SMSGetMessageData__FPvUl(mapBMG, 1);
    }
    else
    {
        BetterSMS::Console::log("[WARNING] /scene/map/stagename.bmg missing from archive, falling back to default scenario name loading\n");
        message = (char *)SMSGetMessageData__FPvUl(unk, index);
    }
    return message;
}

SMS_PATCH_BL(SMS_PORT_REGION(0x80172704, 0x802A0C00, 0, 0), loadStageNameFromBMG);
SMS_PATCH_BL(SMS_PORT_REGION(0x801727a0, 0x802A0C00, 0, 0), loadScenarioNameFromBMG);

SMS_PATCH_BL(SMS_PORT_REGION(0x80156d2c, 0x802A0C00, 0, 0), loadStageNameFromBMG);
SMS_PATCH_BL(SMS_PORT_REGION(0x80156e04, 0x802A0C00, 0, 0), loadScenarioNameFromBMG);
