#include "CARD.h"
#include "printf.h"

#include "SME.hxx"

String *formatBMG(String *msg)
{
    char date[16];
    char time[16];
    char buffer[32];

    OSCalendarTime *calendarTime = (OSCalendarTime *)Memory::malloc(sizeof(OSCalendarTime), 4);
    OSTicksToCalendarTime(OSGetTime(), calendarTime);

    MarioParams *playerData = gpMarioAddress->mCustomInfo;

    if (calendarTime->hour == 0)
        sprintf(time, "%lu:%02lu AM", calendarTime->hour + 12, calendarTime->min);
    else if (calendarTime->hour < 12)
        sprintf(time, "%lu:%02lu AM", calendarTime->hour % 13, calendarTime->min);
    else if (calendarTime->hour == 12)
        sprintf(time, "%lu:%02lu PM", calendarTime->hour, calendarTime->min);
    else
        sprintf(time, "%lu:%02lu PM", (calendarTime->hour + 1) % 13, calendarTime->min);
    
    sprintf(date, "%lu/%lu/%lu", calendarTime->mon + 1, calendarTime->mday, calendarTime->year);

    msg->replaceAll("%name%", playerData->getPlayerName());
    msg->replaceAll("%shine%", String::intToString(TFlagManager::smInstance->Type4Flag.mShineCount, buffer));
    msg->replaceAll("%bcoin%", String::intToString(TFlagManager::smInstance->Type4Flag.mBlueCoinCount, buffer));
    msg->replaceAll("%rcoin%", String::intToString(TFlagManager::smInstance->Type6Flag.mRedCoinCount, buffer));
    msg->replaceAll("%coin%", String::intToString(TFlagManager::smInstance->Type4Flag.mGoldCoinCount, buffer));
    msg->replaceAll("%time%", time);
    msg->replaceAll("%date%", date);
    msg->replaceAll("%bdate%", __DATE__);
    msg->replaceAll("%btime%", __TIME__);
    msg->replaceAll("%cardslot%", gpCardManager->mChannel == CARD_SLOTA ? "A" : "B");

    delete calendarTime;
    return msg;
}