#ifndef MAPOBJGENERAL_H
#define MAPOBJGENERAL_H

#include "types.h"
#include "sms/mapobj/MapObjBase.hxx"

class TMapObjGeneral : public TMapObjBase
{
public:
    u32 _01;   //0x0138
    u32 _02;   //0x013C
    f32 _03; //0x0140
    f32 _04; //0x0144
};

#endif