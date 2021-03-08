#include "sSecurity.hxx"

OSAlarm gctAlarm;

#ifdef SME_DEBUG
    OSStopwatch gctStopwatch;
#endif

static constexpr u32 sEnabledmagic = 0x00D0C0DE;
static constexpr u32 sDisabledmagic = 0x00D0DEAD;
static u32 *sCachedAddr = nullptr;
bool gEnableGCT;

void *setUserCodes(OSAlarm *alarm, OSContext *context)
{
    #ifdef SME_DEBUG
        OSStartStopwatch(&gctStopwatch);
    #endif
    
    u32 *searchcontext = (u32 *)0x80001C00;
    u32 searchlength = (0x80003000 - 0x80001C00) >> 2;

    bool matched = false;

    if (!sCachedAddr)
    {
        for (u32 i = 0; i < searchlength; ++i)
        {
            if (searchcontext[i] == sEnabledmagic && searchcontext[i + 1] == sEnabledmagic)
            {
                matched = true;
                searchcontext = &searchcontext[i];
                sCachedAddr = searchcontext;
                break;
            }
        }
    }
    else
    {
        matched = true;
        searchcontext = sCachedAddr;
    }

    if (matched)
    {
        if (gEnableGCT)
        {
            searchcontext[0] = sEnabledmagic;
            searchcontext[1] = sEnabledmagic;
        }
        else
        {
            searchcontext[0] = sDisabledmagic;
            searchcontext[1] = sDisabledmagic;
        }
    }

    #ifdef SME_DEBUG
        OSStopStopwatch(&gctStopwatch);
        if (gctStopwatch.mHits % 60000 == 0) OSDumpStopwatch(&gctStopwatch);
    #endif

    return nullptr;
}