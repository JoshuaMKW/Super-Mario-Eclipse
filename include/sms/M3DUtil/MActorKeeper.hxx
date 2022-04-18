#pragma once

#include "types.h"

#include "MActor.hxx"
#include "sms/manager/LiveManager.hxx"

class TMActorKeeper {
public:
    TMActorKeeper(TLiveManager *manager);
    TMActorKeeper(TLiveManager *manager, u16);
    ~TMActorKeeper() {}

    MActor *createAndRegister(void *, u32);
    MActor *createMActor(const char *, u32);
    MActor *createMActorFromAllBmd(u32);
    MActor *createMActorFromDefaultBmd(const char *, u32);
    MActor *createMActorFromNthData(int, u32);
    MActor *getMActor(const char *) const;
    MActorAnmData *getMActorAnmData() const;

    void *mModelKeeper;
    u16 _04;
    u16 _06;
    u32 *_08;
    MActorAnmData *mAnmData;
    u16 *_10;
    u32 _14;
};