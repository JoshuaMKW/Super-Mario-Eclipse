#include <SMS/Manager/FlagManager.hxx>
#include <SMS/System/MarDirector.hxx>
#include <SMS/raw_fn.hxx>

#include <BetterSMS/logging.hxx>
#include <BetterSMS/module.hxx>

char *loadStageNameFromBMG(void *global_bmg, u32 index) {
    void *stage_bmg = JKRFileLoader::getGlbResource("/scene/map/stagename.bmg");
    char *message;
    if (stage_bmg) {
        BetterSMS::Console::log("[WARNING] /scene/map/stagename.bmg missing from archive, falling "
                                "back to default stage name loading\n");
        message = (char *)SMSGetMessageData__FPvUl(stage_bmg, 0);
    } else {
        message = (char *)SMSGetMessageData__FPvUl(global_bmg, index);
    }
    return message;
}

char *loadScenarioNameFromBMG(void *global_bmg, u32 index) {
    void *stage_bmg = JKRFileLoader::getGlbResource("/scene/map/stagename.bmg");

    char *message;
    if (stage_bmg) {
        message = (char *)SMSGetMessageData__FPvUl(stage_bmg, 1);
    } else {
        BetterSMS::Console::log("[WARNING] /scene/map/stagename.bmg missing from archive, falling "
                                "back to default scenario name loading\n");
        message = (char *)SMSGetMessageData__FPvUl(global_bmg, index);
    }
    return message;
}

SMS_PATCH_BL(SMS_PORT_REGION(0x80172704, 0x802A0C00, 0, 0), loadStageNameFromBMG);
SMS_PATCH_BL(SMS_PORT_REGION(0x801727A0, 0x802A0C00, 0, 0), loadScenarioNameFromBMG);

SMS_PATCH_BL(SMS_PORT_REGION(0x80156D2C, 0x802A0C00, 0, 0), loadStageNameFromBMG);
SMS_PATCH_BL(SMS_PORT_REGION(0x80156E04, 0x802A0C00, 0, 0), loadScenarioNameFromBMG);
