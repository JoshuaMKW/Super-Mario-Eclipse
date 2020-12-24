#ifndef NOZZLETRIGGER_H
#define NOZZLETRIGGER_H

#include "types.h"
#include "sms/nozzle/NozzleBase.hxx"

class TNozzleTrigger : public TNozzleBase
{

public:
    u8 _00;    //0x0384
    u8 _01;    //0x0385
    u16 _02;   //0x0386
    f32 _03; //0x0388
    u32 _04;   //0x038C
};


#endif