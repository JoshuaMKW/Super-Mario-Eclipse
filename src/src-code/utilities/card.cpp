#include "CARD.h"
#include "SME.hxx"

extern String *formatBMG(String *msg);

/*** Memory File Buffer ***/
#define MAXFILEBUFFER (1024 * 2048)	/*** 2MB Buffer ***/

class CardHandler
{
    void *mSysArea;
    CARDInfo *mCardInfo;

public:
    CARDInfo *getInfo() const { return this->mCardInfo; }
    void *getSysArea() const { return this->mSysArea; }

};
//static u8 sSysArea[CARD_WORKAREA] __attribute__((aligned(32)));
//static CARDInfo sCardInfo;
//static int cardcount = 0;

// extern -> SME.cpp
s32 mountCard(TCardManager *cardManager, bool r4)
{
    s32 ret;
    bool tryMount = true;

    if (cardManager->mMounted)
        tryMount = probe__12TCardManagerFv() < -1;

    if (tryMount)
    {
        cardManager->mChannel = CARD_SLOTA;

        ret = mount___12TCardManagerFb(cardManager, r4);
        if (ret == 0)
            return ret;

        cardManager->mChannel = CARD_SLOTB;

        ret = mount___12TCardManagerFb(cardManager, r4);
        if (ret == 0)
            return ret;

        cardManager->mChannel = CARD_SLOTA;
        return ret;
    }

    return 0;
}

// extern -> SME.cpp
s32 probeCard(TCardManager *cardManager)
{
    s32 ret;

    ret = probe__12TCardManagerFv();
    if (ret == CARD_ERROR_NOCARD)
    {
        unmount__12TCardManagerFv(cardManager);
        cardManager->mLastStatus = mountCard(cardManager, true);
    }
    return ret;
}

// extern -> SME.cpp
char *formatCardMessage(char *dst, const char *src, s32 len)
{
    String *fmtMessage = new String(src, len);
    formatBMG(fmtMessage);

    char *ret = strncpy(dst, fmtMessage->data(), len);
    delete fmtMessage;

    return ret;
}