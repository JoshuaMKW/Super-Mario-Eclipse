#ifndef ITEM_H
#define ITEM_H

#include "types.h"
#include "sms/mapobj/MapObjGeneral.hxx"

class TItem : public TMapObjGeneral
{
public:
    u32 *_148;
    s32 mKillTimer; // _14C
    u32 _150;
};

#endif