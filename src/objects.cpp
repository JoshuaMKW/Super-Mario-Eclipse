#include "types.h"
#include "SME.hxx"

static constexpr size_t OBJDefaultCount = 360;
static constexpr size_t OBJNewCount = 2;

static constexpr size_t LoadAddrTableSize = 2;

static char *sObjDataTable[OBJDefaultCount + OBJNewCount];

static u16 *sObjLoadAddrTable[LoadAddrTableSize][2] {
    {(u16 *)SME_PORT_REGION(0x801B1772, 0, 0, 0), (u16 *)SME_PORT_REGION(0x801B178A, 0, 0, 0)},
    {(u16 *)SME_PORT_REGION(0x801B1AF2, 0, 0, 0), (u16 *)SME_PORT_REGION(0x801B1AFA, 0, 0, 0)}
};

#define OBJ_NAME_DEF(name) static const char name[] = #name;

OBJ_NAME_DEF(WaterBalloon);
OBJ_NAME_DEF(BlowWind);

#undef OBJ_NAME_DEF

// SME.cpp
extern void makeExtendedObjDataTable() {
    memcpy(sObjDataTable, (u32 *)SME_PORT_REGION(0x803C8580, 0, 0, 0), sizeof(u32) * OBJDefaultCount);
    sObjDataTable[OBJDefaultCount] = const_cast<char *>(WaterBalloon);
    sObjDataTable[OBJDefaultCount + 1] = const_cast<char *>(BlowWind);
    {
        u32 addr = reinterpret_cast<u32>(&sObjDataTable);
        u16 lo = addr;
        u16 hi = (addr >> 16) + (lo >> 15);
        for (u32 i = 0; i < LoadAddrTableSize; ++i) {
            Memory::PPC::write<u16>(sObjLoadAddrTable[i][0], hi);
            Memory::PPC::write<u16>(sObjLoadAddrTable[i][1], lo);
        }
    }
}

