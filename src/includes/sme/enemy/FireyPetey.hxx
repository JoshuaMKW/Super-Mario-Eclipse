#pragma once

#include "types.h"
#include "sms/enemy/BossPakkun.hxx"



class TFireyPetey : public TBossPakkun
{
    public:
    TFireyPetey();
    TFireyPetey(const char *);
    ~TFireyPetey();

    virtual void perform(u32, JDrama::TGraphics *) override;
};


