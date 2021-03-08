#include "CARD.h"
#include "sms/J2D.hxx"

#include "SME.hxx"

extern String *formatBMG(String *msg);

/*** Memory File Buffer ***/
#define MAXFILEBUFFER (1024 * 2048)	/*** 2MB Buffer ***/

class CardHandler
{
    void *mSysArea;
    CARDInfo *mCardInfo;

public:
    CARDInfo *getInfo()
    {
        return this->mCardInfo;
    }


};
//static u8 sSysArea[CARD_WORKAREA] __attribute__((aligned(32)));
//static CARDInfo sCardInfo;
//static int cardcount = 0;

s32 mountCard(TCardManager *cardManager, bool r4)
{
    s32 ret;
    bool tryMount = true;

    if (cardManager->mMounted)
    {
        tryMount = probe__12TCardManagerFv() < -1;
    }

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
kmCall(0x802B20F8, &mountCard);

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
kmCall(0x80163C40, &probeCard);

char *formatCardMessage(char *dst, const char *src, s32 len)
{
    String *fmtMessage = new String(src, len);
    formatBMG(fmtMessage);

    char *ret = strncpy(dst, fmtMessage->data(), len);
    delete fmtMessage;

    return ret;
}
kmCall(0x8015B2C0, &formatCardMessage);
kmCall(0x8015B2F8, &formatCardMessage);
kmCall(0x8015B5BC, &formatCardMessage);
kmCall(0x8015B5E4, &formatCardMessage);
kmCall(0x8015B638, &formatCardMessage);
kmCall(0x8015B660, &formatCardMessage);
kmCall(0x8015BCB8, &formatCardMessage);
kmCall(0x8015BCE0, &formatCardMessage);
kmCall(0x8015BE24, &formatCardMessage);
kmCall(0x8015BE4C, &formatCardMessage);
kmCall(0x8015BE9C, &formatCardMessage);
kmCall(0x8015BEC4, &formatCardMessage);
kmCall(0x8015C508, &formatCardMessage);
kmCall(0x8015D194, &formatCardMessage);
kmCall(0x8015D1CC, &formatCardMessage);
kmCall(0x8015DEFC, &formatCardMessage);
kmCall(0x8015DF34, &formatCardMessage);
kmCall(0x8015E34C, &formatCardMessage);
kmCall(0x8015E374, &formatCardMessage);
kmCall(0x8015E3C8, &formatCardMessage);
kmCall(0x8015E3F0, &formatCardMessage);
kmCall(0x8015F970, &formatCardMessage);
kmCall(0x8015F9A8, &formatCardMessage);
kmCall(0x80161320, &formatCardMessage);
kmCall(0x80161358, &formatCardMessage);
kmCall(0x8016889C, &formatCardMessage);
kmCall(0x801688D4, &formatCardMessage);
kmCall(0x80169224, &formatCardMessage);
kmCall(0x8016925C, &formatCardMessage);
kmCall(0x8016950C, &formatCardMessage);
kmCall(0x80169534, &formatCardMessage);
kmCall(0x8016958C, &formatCardMessage);
kmCall(0x801695B4, &formatCardMessage);
kmCall(0x80169ACC, &formatCardMessage);
kmCall(0x80169B04, &formatCardMessage);
kmCall(0x8016A02C, &formatCardMessage);
kmCall(0x8016A064, &formatCardMessage);
kmCall(0x8016A3D0, &formatCardMessage);
kmCall(0x8016A3F8, &formatCardMessage);
kmCall(0x8016A44C, &formatCardMessage);
kmCall(0x8016A474, &formatCardMessage);
kmCall(0x8016B19C, &formatCardMessage);
kmCall(0x8016B1D4, &formatCardMessage);