#ifndef GAMESEQUENCE_H
#define GAMESEQUENCE_H

#include "types.h"

struct AreaEpisodeArray
{
    u32 _00[0x10 / 4]; //0x0000
    u32 *startArray;   //0x0010
    u32 *endArray;     //0x0014
};

class TGameSequence
{

public:
    enum AREA
    {
        AIRPORT = 0x0,
        DOLPIC = 0x1,
        BIANCO = 0x2,
        RICCO = 0x3,
        MAMMA = 0x4,
        PINNABEACH = 0x5,
        SIRENA = 0x6,
        DELFINO = 0x7,
        MONTE = 0x8,
        MARE = 0x9,
        NONE = 0xA,
        SCALE = 0xB,
        TEST10 = 0xC,
        PINNAPARCO = 0xD,
        CASINO = 0xE,
        OPTION = 0xF,
        MAREUNDERSEA = 0x10,
        DOLPICEX0 = 0x14,
        DOLPICEX1 = 0x15,
        DOLPICEX2 = 0x16,
        DOLPICEX3 = 0x17,
        DOLPICEX4 = 0x18,
        BIANCOEX1 = 0x1D,
        RICOEX0 = 0x1E,
        RICOEX1 = 0x1F,
        MAMMAEX0 = 0x20,
        MAMMAEX1 = 0x21,
        SIRENAEX0 = 0x28,
        SIRENAEX1 = 0x29,
        MONTEEX0 = 0x2A,
        MAREEX0 = 0x2C,
        COROEX0 = 0x2E,
        COROEX1 = 0x2F,
        COROEX2 = 0x30,
        COROEX4 = 0x32,
        COROEX5 = 0x33,
        COROEX6 = 0x34,
        BIANCOBOSS = 0x37,
        DELFINOBOSS = 0x38,
        MAREBOSS = 0x39,
        PINNABOSS = 0x3A,
        RICCOBOSS = 0x3B,
        CORONABOSS = 0x3C
    };

    u8 mAreaID;
    u8 mEpisodeID;
    u16 _00;
};

#endif