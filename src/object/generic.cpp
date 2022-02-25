#include "types.h"

#include "JDrama/JDRNameRef.hxx"
#include "obj/BlowWindObj.hxx"
#include "obj/TornadoObj.hxx"
#include "obj/WaterBalloon.hxx"
#include "sms/game/MarNameRefGen.hxx"
#include "sms/mapobj/MapObjInit.hxx"

#include "SME.hxx"

#if SME_EXTRA_OBJECTS

// ---------------------------------------- //

static constexpr size_t OBJNewCount = 3;

// ---------------------------------------- //

static constexpr size_t LoadAddrTableSize = 2;

// Locates instructions to patch, pointing to our table
static u16 *sObjLoadAddrTable[LoadAddrTableSize][2]{
    {(u16 *)SME_PORT_REGION(0x801B1772, 0, 0, 0),
     (u16 *)SME_PORT_REGION(0x801B178A, 0, 0, 0)},
    {(u16 *)SME_PORT_REGION(0x801B1AF2, 0, 0, 0),
     (u16 *)SME_PORT_REGION(0x801B1AFA, 0, 0, 0)}};

static ObjData *sObjDataTableNew[OBJDataTableSize + OBJNewCount];

// extern -> SME.cpp
void makeExtendedObjDataTable() {
  memcpy(sObjDataTableNew, sObjDataTable,
         sizeof(u32) * OBJDataTableSize - 1); // last entry is default null
  sObjDataTableNew[OBJDataTableSize - 1] = &waterBalloonData;
  sObjDataTableNew[OBJDataTableSize] = &blowWindData;
  sObjDataTableNew[OBJDataTableSize + 1] = &tornadoData;
  sObjDataTableNew[OBJDataTableSize + (OBJNewCount - 1)] = sObjDataTable[OBJDataTableSize - 1];
  {
    u32 addr = reinterpret_cast<u32>(&sObjDataTableNew);
    u16 lo = addr;
    u16 hi = (addr >> 16) + (lo >> 15);
    for (u32 i = 0; i < LoadAddrTableSize; ++i) {
      Memory::PPC::write<u16>(sObjLoadAddrTable[i][0], hi);
      Memory::PPC::write<u16>(sObjLoadAddrTable[i][1], lo);
    }
  }
}

#define GENERATE_OBJ(klass, objname, name) \
  if (strcmp(name, objname) == 0) {        \
    return new klass(name);                \
  }

static JDrama::TNameRef *makeExtendedMapObjFromRef(TMarNameRefGen *nameGen,
                                            const char *name) {
  JDrama::TNameRef *obj = nameGen->getNameRef_MapObj(name);
  if (obj)
    return obj;

  GENERATE_OBJ(TTornadoMapObj, "Tornado", name);

  return nullptr;
}
SME_PATCH_BL(SME_PORT_REGION(0x8029E120, 0, 0, 0), makeExtendedMapObjFromRef);

void objects_staticResetter() { }

#endif