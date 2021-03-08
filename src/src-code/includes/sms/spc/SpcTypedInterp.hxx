#pragma once

#include "types.h"
#include "sms/spc/SpcInterp.hxx"

template <typename T>
class TSpcTypedInterp : public TSpcInterp
{
    public:
    ~TSpcTypedInterp();

    void dispatchBuiltin(u32, u32);
};