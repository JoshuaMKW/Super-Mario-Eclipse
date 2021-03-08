#pragma once

#include "types.h"

class TSpcBinary
{
    public:
    TSpcBinary(void *);
    virtual ~TSpcBinary();

    virtual void initUserBuiltin();

    void* getHeader() const;
    void bindSystemDataToSymbol(const char *, u32);
    u32* searchSymbol(const char *);
    void init();
    
    void* spcBinary; // _0
    // vtable _4
};
