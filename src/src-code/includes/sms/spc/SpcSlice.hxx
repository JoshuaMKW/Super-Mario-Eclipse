#pragma once

#include "types.h"

class TSpcSlice
{
    public:
    TSpcSlice();

    u32 getDataInt() const;
    f32 getDataFloat() const;
    
    bool operator !=(const TSpcSlice &) const;
    bool operator ==(const TSpcSlice &) const;

    u32 _0;
    u32 _4;
};