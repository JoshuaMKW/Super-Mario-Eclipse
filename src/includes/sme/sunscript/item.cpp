#include "sms/actor/Item.hxx"
#include "common.hxx"
#include "sms/JSystem/JDrama.hxx"
#include "sms/actor/item/Coin.hxx"
#include "sms/actor/item/Shine.hxx"
#include "sms/manager/ItemManager.hxx"

using namespace SME::Util;

void Spc::spawnObjByID(TSpcInterp *interp, u32 argc) {
  const u32 id = Spc::Stack::popItem(interp).mValue;
  TMapObjBase *obj = TItemManager::newAndRegisterObjByEventID(id, "");
  Spc::Stack::pushItem(interp, reinterpret_cast<u32>(obj), Spc::ValueType::INT);
  if (!obj) {
    SpcTrace("Item could not be registered. ID(%lu)", id);
    return;
  }

  if (argc == 4) {
    JGeometry::TVec3<f32> pos = *reinterpret_cast<JGeometry::TVec3<f32> *>(
        Spc::Stack::popItem(interp).mValue);
    JGeometry::TVec3<f32> rot = *reinterpret_cast<JGeometry::TVec3<f32> *>(
        Spc::Stack::popItem(interp).mValue);
    JGeometry::TVec3<f32> spd = *reinterpret_cast<JGeometry::TVec3<f32> *>(
        Spc::Stack::popItem(interp).mValue);
    obj->JSGSetTranslation(reinterpret_cast<Vec &>(pos));
    obj->JSGSetRotation(reinterpret_cast<Vec &>(rot));
    obj->mSpeed = spd;
  } else if (argc != 1) {
    SpcTrace("Incorrect number of arguments passed to spc::setActorRotToOther "
             "(%lu args passed, 1 or 4 needed)",
             argc);
  }
}