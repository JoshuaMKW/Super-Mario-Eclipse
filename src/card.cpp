#include "CARD.h"
#include "SME.hxx"

using namespace SME;

/*** Memory File Buffer ***/
#define MAXFILEBUFFER (1024 * 2048)	/*** 2MB Buffer ***/

/*
class CardHandler
{
    void *mSysArea;
    CARDInfo *mCardInfo;

public:
    CARDInfo *getInfo() const { return this->mCardInfo; }
    void *getSysArea() const { return this->mSysArea; }

};
*/
//static u8 sSysArea[CARD_WORKAREA] __attribute__((aligned(32)));
//static CARDInfo sCardInfo;
//static int cardcount = 0;

// extern -> SME.cpp
s32 Patch::Card::mountCard(TCardManager *cardManager, bool r4)
{
    bool tryMount = true;
    s32 ret = 0;

    if (cardManager->mMounted)
        tryMount = cardManager->probe() < -1;

    if (tryMount)
    {
        cardManager->mChannel = CARD_SLOTA;

        ret = cardManager->mount_(r4);
        if (ret == 0)
            return ret;

        cardManager->mChannel = CARD_SLOTB;

        ret = cardManager->mount_(r4);
        if (ret == 0)
            return ret;

        cardManager->mChannel = CARD_SLOTA;
        return ret;
    }

    return ret;
}

// extern -> SME.cpp
s32 Patch::Card::probeCard(TCardManager *cardManager)
{
    s32 ret = cardManager->probe();
    if (ret == CARD_ERROR_NOCARD)
    {
        cardManager->unmount();
        cardManager->mLastStatus = mountCard(cardManager, true);
    }
    return ret;
}

// extern -> SME.cpp
char *Patch::Card::formatCardMessage(char *dst, const char *src, s32 len)
{
    String fmtMessage(src, len);
    Util::formatBMG(fmtMessage);
    return strncpy(dst, fmtMessage.data(), len);;
}