#pragma once

#include "types.h"

// I've only seen this used as u32 and TSpcSlice
template <typename T>
class TSpcStack
{
    public:
    virtual ~TSpcStack();

    u32 _0;
    u32 _4;
    u32 *_8; // some array
    u32 *vtable; // _C
};