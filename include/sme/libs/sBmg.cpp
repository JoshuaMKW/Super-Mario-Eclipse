#include "sBmg.hxx"
#include "SME.hxx"

using namespace SME;

#define SME_DEMO

String &Util::formatBMG(String &msg) {
    char buffer[32];

    OSCalendarTime calendarTime;
    OSTicksToCalendarTime(OSGetTime(), &calendarTime);

    SME::Class::TPlayerData *playerData =
        SME::TGlobals::getPlayerData(gpMarioAddress);

    msg.replaceAll("%name%", playerData->getPlayerName());
    msg.replaceAll("%shine%",
        String::intToString(TFlagManager::smInstance->Type4Flag.mShineCount, buffer));
    msg.replaceAll("%bcoin%",
        String::intToString(TFlagManager::smInstance->Type4Flag.mBlueCoinCount, buffer));
    msg.replaceAll("%rcoin%",
        String::intToString(TFlagManager::smInstance->Type6Flag.mRedCoinCount, buffer));
    msg.replaceAll("%coin%",
        String::intToString(TFlagManager::smInstance->Type4Flag.mGoldCoinCount, buffer));
    msg.replaceAll("%time%", SME::Util::Time::calendarToTime(calendarTime));
    msg.replaceAll("%date%", SME::Util::Time::calendarToDate(calendarTime));
    msg.replaceAll("%btime%", SME::Util::Time::buildTime());
    msg.replaceAll("%bdate%", SME::Util::Time::buildDate());
    msg.replaceAll("%cardslot%", gpCardManager->mChannel == CARD_SLOTA ? "A" : "B");
    #ifdef SME_DEMO
    msg.replaceAll("%c", gpCardManager->mChannel == CARD_SLOTA ? "A" : "B");
    #endif

    return msg;
}

// extern -> SME.cpp
char *Util::formatBMGRaw(char *dst, const char *src, size_t len)
{
    String msg(src, len);
    char buffer[32];

    OSCalendarTime calendarTime;
    OSTicksToCalendarTime(OSGetTime(), &calendarTime);

    SME::Class::TPlayerData *playerData =
        SME::TGlobals::getPlayerData(gpMarioAddress);

    msg.replaceAll("%name%", playerData->getPlayerName());
    msg.replaceAll("%shine%",
        String::intToString(TFlagManager::smInstance->Type4Flag.mShineCount, buffer));
    msg.replaceAll("%bcoin%",
        String::intToString(TFlagManager::smInstance->Type4Flag.mBlueCoinCount, buffer));
    msg.replaceAll("%rcoin%",
        String::intToString(TFlagManager::smInstance->Type6Flag.mRedCoinCount, buffer));
    msg.replaceAll("%coin%",
        String::intToString(TFlagManager::smInstance->Type4Flag.mGoldCoinCount, buffer));
    msg.replaceAll("%time%", SME::Util::Time::calendarToDate(calendarTime));
    msg.replaceAll("%date%", SME::Util::Time::calendarToTime(calendarTime));
    msg.replaceAll("%bdate%", SME::Util::Time::buildDate());
    msg.replaceAll("%btime%", SME::Util::Time::buildTime());
    msg.replaceAll("%cardslot%", gpCardManager->mChannel == CARD_SLOTA ? "A" : "B");
    #ifdef SME_DEMO
    msg.replaceAll("%c", gpCardManager->mChannel == CARD_SLOTA ? "A" : "B");
    #endif

    return strncpy(dst, msg.data(), len);
}

SME_PATCH_BL(SME_PORT_REGION(0x802B20F8, 0, 0, 0), Patch::Card::mountCard);
SME_PATCH_BL(SME_PORT_REGION(0x80163C40, 0, 0, 0), Patch::Card::probeCard);
SME_PATCH_BL(SME_PORT_REGION(0x8015B2C0, 0, 0, 0), Util::formatBMGRaw);
SME_PATCH_BL(SME_PORT_REGION(0x8015B2F8, 0, 0, 0), Util::formatBMGRaw);
SME_PATCH_BL(SME_PORT_REGION(0x8015B5BC, 0, 0, 0), Util::formatBMGRaw);
SME_PATCH_BL(SME_PORT_REGION(0x8015B5E4, 0, 0, 0), Util::formatBMGRaw);
SME_PATCH_BL(SME_PORT_REGION(0x8015B638, 0, 0, 0), Util::formatBMGRaw);
SME_PATCH_BL(SME_PORT_REGION(0x8015B660, 0, 0, 0), Util::formatBMGRaw);
SME_PATCH_BL(SME_PORT_REGION(0x8015BCB8, 0, 0, 0), Util::formatBMGRaw);
SME_PATCH_BL(SME_PORT_REGION(0x8015BCE0, 0, 0, 0), Util::formatBMGRaw);
SME_PATCH_BL(SME_PORT_REGION(0x8015BE24, 0, 0, 0), Util::formatBMGRaw);
SME_PATCH_BL(SME_PORT_REGION(0x8015BE4C, 0, 0, 0), Util::formatBMGRaw);
SME_PATCH_BL(SME_PORT_REGION(0x8015BE9C, 0, 0, 0), Util::formatBMGRaw);
SME_PATCH_BL(SME_PORT_REGION(0x8015BEC4, 0, 0, 0), Util::formatBMGRaw);
SME_PATCH_BL(SME_PORT_REGION(0x8015C508, 0, 0, 0), Util::formatBMGRaw);
SME_PATCH_BL(SME_PORT_REGION(0x8015D194, 0, 0, 0), Util::formatBMGRaw);
SME_PATCH_BL(SME_PORT_REGION(0x8015D1CC, 0, 0, 0), Util::formatBMGRaw);
SME_PATCH_BL(SME_PORT_REGION(0x8015DEFC, 0, 0, 0), Util::formatBMGRaw);
SME_PATCH_BL(SME_PORT_REGION(0x8015DF34, 0, 0, 0), Util::formatBMGRaw);
SME_PATCH_BL(SME_PORT_REGION(0x8015E34C, 0, 0, 0), Util::formatBMGRaw);
SME_PATCH_BL(SME_PORT_REGION(0x8015E374, 0, 0, 0), Util::formatBMGRaw);
SME_PATCH_BL(SME_PORT_REGION(0x8015E3C8, 0, 0, 0), Util::formatBMGRaw);
SME_PATCH_BL(SME_PORT_REGION(0x8015E3F0, 0, 0, 0), Util::formatBMGRaw);
SME_PATCH_BL(SME_PORT_REGION(0x8015F970, 0, 0, 0), Util::formatBMGRaw);
SME_PATCH_BL(SME_PORT_REGION(0x8015F9A8, 0, 0, 0), Util::formatBMGRaw);
SME_PATCH_BL(SME_PORT_REGION(0x80161320, 0, 0, 0), Util::formatBMGRaw);
SME_PATCH_BL(SME_PORT_REGION(0x80161358, 0, 0, 0), Util::formatBMGRaw);
SME_PATCH_BL(SME_PORT_REGION(0x80161380, 0, 0, 0), Util::formatBMGRaw);
SME_PATCH_BL(SME_PORT_REGION(0x801613A8, 0, 0, 0), Util::formatBMGRaw);
SME_PATCH_BL(SME_PORT_REGION(0x8016889C, 0, 0, 0), Util::formatBMGRaw);
SME_PATCH_BL(SME_PORT_REGION(0x801688D4, 0, 0, 0), Util::formatBMGRaw);
SME_PATCH_BL(SME_PORT_REGION(0x80169224, 0, 0, 0), Util::formatBMGRaw);
SME_PATCH_BL(SME_PORT_REGION(0x8016925C, 0, 0, 0), Util::formatBMGRaw);
SME_PATCH_BL(SME_PORT_REGION(0x8016950C, 0, 0, 0), Util::formatBMGRaw);
SME_PATCH_BL(SME_PORT_REGION(0x80169534, 0, 0, 0), Util::formatBMGRaw);
SME_PATCH_BL(SME_PORT_REGION(0x8016958C, 0, 0, 0), Util::formatBMGRaw);
SME_PATCH_BL(SME_PORT_REGION(0x801695B4, 0, 0, 0), Util::formatBMGRaw);
SME_PATCH_BL(SME_PORT_REGION(0x80169ACC, 0, 0, 0), Util::formatBMGRaw);
SME_PATCH_BL(SME_PORT_REGION(0x80169B04, 0, 0, 0), Util::formatBMGRaw);
SME_PATCH_BL(SME_PORT_REGION(0x8016A02C, 0, 0, 0), Util::formatBMGRaw);
SME_PATCH_BL(SME_PORT_REGION(0x8016A064, 0, 0, 0), Util::formatBMGRaw);
SME_PATCH_BL(SME_PORT_REGION(0x8016A3D0, 0, 0, 0), Util::formatBMGRaw);
SME_PATCH_BL(SME_PORT_REGION(0x8016A3F8, 0, 0, 0), Util::formatBMGRaw);
SME_PATCH_BL(SME_PORT_REGION(0x8016A44C, 0, 0, 0), Util::formatBMGRaw);
SME_PATCH_BL(SME_PORT_REGION(0x8016A474, 0, 0, 0), Util::formatBMGRaw);
SME_PATCH_BL(SME_PORT_REGION(0x8016B19C, 0, 0, 0), Util::formatBMGRaw);
SME_PATCH_BL(SME_PORT_REGION(0x8016B1D4, 0, 0, 0), Util::formatBMGRaw);