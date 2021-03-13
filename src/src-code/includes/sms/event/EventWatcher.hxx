#pragma once

#include "types.h"
#include "sms/JSystem/JDrama.hxx"
#include "sms/spc/SpcTypedBinary.hxx"

class TEventWatcher : public JDrama::TViewObj
{
    public:
    TEventWatcher(char const *, char const *);
    TEventWatcher(char const *);
    virtual ~TEventWatcher();

    virtual void perform(u32, JDrama::TGraphics *);
    virtual void launchScript(char const *);

    TSpcTypedBinary<TEventWatcher>* _10;
    TSpcTypedBinary<TEventWatcher>* _14;
};

char* sShineViewObjName;