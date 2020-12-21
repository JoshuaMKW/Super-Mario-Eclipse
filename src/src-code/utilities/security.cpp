#include "types.h"
#include "os.h"

#define MEM_MARR0 0xCC004000
#define MEM_MARR1 0xCC004004
#define MEM_MARR2 0xCC004008
#define MEM_MARR3 0xCC00400C
#define MEM_MARR_CONTROL 0xCC004010

enum ACCESS {
    DENIED,
    READ,
    WRITE,
    READWRITE
};

class MemProtectMap
{
    u16 _mStart;
    u16 _mEnd;

public:
    void *start()
    {
        return (void *)((this->_mStart << 10) | 0x80000000);
    }

    void *end()
    {
        return (void *)((this->_mEnd << 10) | 0x80000000);
    }

    s32 size()
    {
        return (this->_mEnd - this->_mStart) << 10;
    }

    void setStart(void *addr)
    {
        this->_mStart = ((u32)addr & 0x1FFFFFF) >> 10;
    }

    void setEnd(void *addr)
    {
        this->_mEnd = ((u32)addr & 0x1FFFFFF) >> 10;
    }
};

struct MemCtrl
{
    u8 _reserved;
    u8 mCh3 : 2;
    u8 mCh2 : 2;
    u8 mCh1 : 2;
    u8 mCh0 : 2;

    void setAccess(u8 index, u8 access)
    {
        switch (index)
        {
        case 0:
            this->mCh0 = access;
            break;
        case 1:
            this->mCh1 = access;
            break;
        case 2:
            this->mCh2 = access;
            break;
        case 3:
            this->mCh3 = access;
            break;
        }
    }
};

void protectMemRegion(u8 index, void *start, void *end, u8 access = READ)
{
    MemProtectMap *map = (MemProtectMap *)(MEM_MARR0 + (index << 2));
    MemCtrl *mapctrl = (MemCtrl *)MEM_MARR_CONTROL;

    map->setStart(start);
    map->setEnd(end);

    mapctrl->setAccess(index, access);
}

void initializeMemProtection()
{
    OSProtectRange(2, (void *)0x80003100, 0x36FF00, READ);
}

OSAlarm gctAlarm;

#ifdef SME_DEBUG
    OSStopwatch gctStopwatch;
#endif

static u32 enabledmagic = 0x00D0C0DE;
static u32 disabledmagic = 0x00D0DEAD;
static u32 *cachedAddr = NULL;

void *setUserCodes(OSAlarm *alarm, OSContext *context)
{
    #ifdef SME_DEBUG
        OSStartStopwatch(&gctStopwatch);
    #endif
    
    u32 *searchcontext = (u32 *)0x80001C00;
    u32 searchlength = (0x80003000 - 0x80001C00) >> 2;

    bool matched = false;

    if (!cachedAddr)
    {
        for (u32 i = 0; i < searchlength; ++i)
        {
            if (searchcontext[i] == enabledmagic && searchcontext[i + 1] == enabledmagic)
            {
                matched = true;
                searchcontext = &searchcontext[i];
                cachedAddr = searchcontext;
                break;
            }
        }
    }
    else
    {
        matched = true;
        searchcontext = cachedAddr;
    }

    if (matched)
    {
        searchcontext[0] = disabledmagic;
        searchcontext[1] = disabledmagic;
    }

    #ifdef SME_DEBUG
        OSStopStopwatch(&gctStopwatch);
        if (gctStopwatch.mHits % 60000 == 0) OSDumpStopwatch(&gctStopwatch);
    #endif

    return NULL;
}