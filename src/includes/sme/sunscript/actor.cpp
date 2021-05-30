#include "common.hxx"
#include "sms/JSystem/JDrama.hxx"

using namespace SME::Util;

void Spc::setActorPosToOther(TSpcInterp *interp, u32 argc) {
  SME_ASSERT(argc == 2, "Incorrect number of arguments passed to spc::setActorPosToOther (%lu args passed, 2 needed)", argc);
  JDrama::TActor *self =
      reinterpret_cast<JDrama::TActor *>(Spc::Stack::popItem(interp).mValue);
  JDrama::TActor *target =
      reinterpret_cast<JDrama::TActor *>(Spc::Stack::popItem(interp).mValue);
  JGeometry::TVec3<f32> pos;
  target->JSGGetTranslation(reinterpret_cast<Vec *>(&pos));
  self->JSGSetTranslation(reinterpret_cast<Vec &>(pos));
}

void Spc::setActorRotToOther(TSpcInterp *interp, u32 argc) {
  SME_ASSERT(argc == 2, "Incorrect number of arguments passed to spc::setActorRotToOther (%lu args passed, 2 needed)", argc);
  JDrama::TActor *self =
      reinterpret_cast<JDrama::TActor *>(Spc::Stack::popItem(interp).mValue);
  JDrama::TActor *target =
      reinterpret_cast<JDrama::TActor *>(Spc::Stack::popItem(interp).mValue);
  JGeometry::TVec3<f32> pos;
  target->JSGGetRotation(reinterpret_cast<Vec *>(&pos));
  self->JSGSetRotation(reinterpret_cast<Vec &>(pos));
}

void Spc::getActorPos(TSpcInterp *interp, u32 argc) {
  SME_ASSERT(argc == 1, "Incorrect number of arguments passed to spc::getActorPos (%lu args passed, 1 needed)", argc);
  JDrama::TActor *self =
      reinterpret_cast<JDrama::TActor *>(Spc::Stack::popItem(interp).mValue);
  JGeometry::TVec3<f32> pos;
  self->JSGGetTranslation(reinterpret_cast<Vec *>(&pos));
  Spc::Stack::pushItem(interp, reinterpret_cast<u32>(&pos), Spc::ValueType::INT); // Return a value
}

void Spc::setActorPos(TSpcInterp *interp, u32 argc) {
  SME_ASSERT(argc == 1, "Incorrect number of arguments passed to spc::setActorPos (%lu args passed, 1 needed)", argc);
  JDrama::TActor *self =
      reinterpret_cast<JDrama::TActor *>(Spc::Stack::popItem(interp).mValue);
  JGeometry::TVec3<f32> *pos =
      reinterpret_cast<JGeometry::TVec3<f32> *>(Spc::Stack::popItem(interp).mValue);
  self->JSGSetTranslation(reinterpret_cast<Vec &>(*pos));
}

void Spc::getActorRot(TSpcInterp *interp, u32 argc) {
  SME_ASSERT(argc == 1, "Incorrect number of arguments passed to spc::getActorRot (%lu args passed, 1 needed)", argc);
  JDrama::TActor *self =
      reinterpret_cast<JDrama::TActor *>(Spc::Stack::popItem(interp).mValue);
  JGeometry::TVec3<f32> pos;
  self->JSGGetRotation(reinterpret_cast<Vec *>(&pos));
  Spc::Stack::pushItem(interp, reinterpret_cast<u32>(&pos), Spc::ValueType::INT); // Return a value
}

void Spc::setActorRot(TSpcInterp *interp, u32 argc) {
  SME_ASSERT(argc == 1, "Incorrect number of arguments passed to spc::setActorRot (%lu args passed, 1 needed)", argc);
  JDrama::TActor *self =
      reinterpret_cast<JDrama::TActor *>(Spc::Stack::popItem(interp).mValue);
  JGeometry::TVec3<f32> *pos =
      reinterpret_cast<JGeometry::TVec3<f32> *>(Spc::Stack::popItem(interp).mValue);
  self->JSGSetRotation(reinterpret_cast<Vec &>(*pos));
}