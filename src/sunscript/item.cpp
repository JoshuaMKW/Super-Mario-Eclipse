#include "sms/actor/Item.hxx"
#include "common.hxx"
#include "sms/actor/item/Coin.hxx"
#include "sms/actor/item/Shine.hxx"
#include "sms/manager/ItemManager.hxx"

using namespace SME::Util;

void Spc::spawnObjByID(TSpcInterp *interp, u32 argc) {
  const u32 id = Stack::popItem(interp).mValue;
  TMapObjBase *obj = TItemManager::newAndRegisterObjByEventID(id, "");
  Stack::pushItem(interp, reinterpret_cast<u32>(obj), ValueType::INT);
  if (!obj) {
    SpcTrace("Item could not be registered. ID(%lu)", id);
    return;
  }

  if (argc == 4) {
    TVec3f pos = *reinterpret_cast<TVec3f *>(
        Stack::popItem(interp).mValue);
    TVec3f rot = *reinterpret_cast<TVec3f *>(
        Stack::popItem(interp).mValue);
    TVec3f spd = *reinterpret_cast<TVec3f *>(
        Stack::popItem(interp).mValue);
    obj->JSGSetTranslation(reinterpret_cast<Vec &>(pos));
    obj->JSGSetRotation(reinterpret_cast<Vec &>(rot));
    obj->mSpeed = spd;
  } else {
    interp->verifyArgNum(1, &argc);
  }
}