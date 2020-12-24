#ifndef RESETFRUIT_H
#define RESETFRUIT_H

#include "types.h"
#include "sms/mapobj/MapObjBall.hxx"

class TResetFruit : public TMapObjBall
{
public:
    enum Fruit
    {
        COCONUT = 0x40000390,
        PAPAYA = 0x40000391,
        PINEAPPLE = 0x40000392,
        DURIAN = 0x40000393,
        BANANA = 0x40000394
    };
};

#endif