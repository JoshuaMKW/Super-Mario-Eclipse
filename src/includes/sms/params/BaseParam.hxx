#pragma once

#include "types.h"
#include "sms/JSystem/JSU/JSUMemoryStream.hxx"

class TParams;

class TBaseParam
{
public:
    TBaseParam(TParams *parent, u16 keycode, const char *name);

    virtual void load(JSUMemoryInputStream &);

    u16 mKeyCode;
    char *mKeyName;
    TBaseParam *mNextParam;
};