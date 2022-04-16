#include "types.h"

#include "JDrama/JDRNameRef.hxx"
#include "enemy/FireyPetey.hxx"
#include "obj/BlowWindObj.hxx"
#include "obj/TornadoObj.hxx"
#include "obj/WaterBalloon.hxx"
#include "sms/game/MarNameRefGen.hxx"
#include "sms/mapobj/MapObjInit.hxx"

#include "SME.hxx"
#include "enemy/DarkZhine.hxx"

#if SME_EXTRA_OBJECTS

// ---------------------------------------- //

static constexpr size_t OBJNewCount = 3;

// ---------------------------------------- //

static constexpr size_t LoadAddrTableSize = 2;

// Locates instructions to patch, pointing to our table
static u16 *sObjLoadAddrTable[LoadAddrTableSize][2]{
    {(u16 *)SME_PORT_REGION(0x801B1772, 0x801a962A, 0, 0),
     (u16 *)SME_PORT_REGION(0x801B178A, 0x801a9642, 0, 0)},
    {(u16 *)SME_PORT_REGION(0x801B1AF2, 0x801a99AA, 0, 0),
     (u16 *)SME_PORT_REGION(0x801B1AFA, 0x801A99B2, 0, 0)}};

static ObjData *sObjDataTableNew[OBJDataTableSize + OBJNewCount];

// extern -> SME.cpp
void makeExtendedObjDataTable() {
  memcpy(sObjDataTableNew, sObjDataTable,
         sizeof(u32) * OBJDataTableSize - 1); // last entry is default null
  sObjDataTableNew[OBJDataTableSize - 1] = &waterBalloonData;
  sObjDataTableNew[OBJDataTableSize] = &blowWindData;
  sObjDataTableNew[OBJDataTableSize + 1] = &tornadoData;
  sObjDataTableNew[OBJDataTableSize + (OBJNewCount - 1)] =
      sObjDataTable[OBJDataTableSize - 1];
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

#define GENERATE_OBJ(klass, objname, name, ...)                                \
  if (strcmp(name, objname) == 0) {                                            \
    return new klass(name, ##__VA_ARGS__);                                     \
  }

static JDrama::TNameRef *makeExtendedMapObjFromRef(TMarNameRefGen *nameGen,
                                                   const char *name) {
  JDrama::TNameRef *obj = nameGen->getNameRef_MapObj(name);
  if (obj)
    return obj;

  GENERATE_OBJ(TTornadoMapObj, "Tornado", name);

  return nullptr;
}
SME_PATCH_BL(SME_PORT_REGION(0x8029E120, 0x80295FFC, 0, 0),
             makeExtendedMapObjFromRef);

static JDrama::TNameRef *makeExtendedBossEnemyFromRef(TMarNameRefGen *nameGen,
                                                      const char *name) {
  JDrama::TNameRef *obj = nameGen->getNameRef_BossEnemy(name);
  if (obj)
    return obj;

  GENERATE_OBJ(TBossDarkZhine, "BossZhine", name)
  GENERATE_OBJ(TFireyPetey, "FireyPetey", name);

  return nullptr;
}
SME_PATCH_BL(SME_PORT_REGION(0x8029D2F4, 0x802951D0, 0, 0),
             makeExtendedBossEnemyFromRef);

static JDrama::TNameRef *makeExtendedGenericFromRef(TMarNameRefGen *nameGen,
                                                    const char *name) {
  JDrama::TNameRef *obj = reinterpret_cast<JDrama::TNameRef *>(
      getNameRef__Q26JDrama11TNameRefGenCFPCc(nameGen, name));
  if (obj)
    return obj;

  GENERATE_OBJ(TBossDarkZhineManager, "BossZhineManager", name);
  GENERATE_OBJ(TFireyPeteyManager, "FireyPeteyManager", "BossPakkunManager", 0);

  return nullptr;
}
SME_PATCH_BL(SME_PORT_REGION(0x8029EDD8, 0, 0, 0),
             makeExtendedGenericFromRef);

static THitActor **objectInteractionHandler() {
  TMario *player;
  int objIndex;
  SME_FROM_GPR(31, player);
  SME_FROM_GPR(29, objIndex);

  THitActor *obj = player->mCollidingObjs[objIndex >> 2];
  // keepDistance__6TMarioFRC9THitActorf(player, obj, 0.0f);

  switch (obj->mObjectID) {
  case 0x40000FFF:
    TWaterBalloon *balloon = reinterpret_cast<TWaterBalloon *>(obj);
    if (canTake__6TMarioFP9THitActor(player, balloon) &&
        !(balloon->mStateFlags.asU32 & 1)) {
      player->mGrabTarget = balloon;
      changePlayerStatus__6TMarioFUlUlb(player, 899, 0, 0);
    }
    break;
  }
  return player->mCollidingObjs;
}
SME_PATCH_BL(SME_PORT_REGION(0x80281510, 0x8027929C, 0, 0),
             objectInteractionHandler);

static THitActor *objGrabHandler() {
  TMario *player;
  SME_FROM_GPR(31, player);

  THitActor *obj = player->mHeldObject;
  if (!obj)
    return obj;

  switch (obj->mObjectID) {
  case 0x40000FFF:
    TWaterBalloon *balloon = reinterpret_cast<TWaterBalloon *>(obj);
    if (balloon->mStateFlags.asU32 & 1)
      player->mHeldObject = nullptr;
    break;
  }

  return obj;
}
SME_PATCH_BL(SME_PORT_REGION(0x80262400, 0x8025A18C, 0, 0), objGrabHandler);
SME_WRITE_32(SME_PORT_REGION(0x80262404, 0x8025A190, 0, 0), 0x2C030000);

void objects_staticResetter() {}

#endif