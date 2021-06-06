#include "common.hxx"
#include "Globals.hxx"
#include "libs/sTime.hxx"
#include "sms/SMS.hxx"
#include "sms/actor/Mario.hxx"
#include "sms/JSystem/JDrama.hxx"

using namespace SME::Util;

void Spc::getPlayerByIndex(TSpcInterp *interp, u32 argc) {
  SME_ASSERT(argc == 1, "Incorrect number of arguments passed to spc::getPlayerByIndex (%lu args passed, 1 needed)", argc);
  Spc::Stack::pushItem(interp, reinterpret_cast<u32>(SME::TGlobals::getPlayerByIndex(Spc::Stack::popItem(interp).mValue)), Spc::ValueType::INT); // Return a value
}

void Spc::getDateAsStr(TSpcInterp *interp, u32 argc) {
  SME_ASSERT(argc == 0, "Incorrect number of arguments passed to spc::getDateAsStr (%lu args passed, 0 needed)", argc);
  Spc::Stack::pushItem(interp, reinterpret_cast<u32>(SME::Util::Time::date()), Spc::ValueType::INT); // Return a value
}

void Spc::getTimeAsStr(TSpcInterp *interp, u32 argc) {
  SME_ASSERT(argc == 0, "Incorrect number of arguments passed to spc::getTimeAsStr (%lu args passed, 0 needed)", argc);
  Spc::Stack::pushItem(interp, reinterpret_cast<u32>(SME::Util::Time::time()), Spc::ValueType::INT); // Return a value
}

void Spc::getPlayerInputByIndex(TSpcInterp *interp, u32 argc) {
  SME_ASSERT(argc == 1 || argc == 2, "Incorrect number of arguments passed to spc::getPlayerInputByIndex (%lu args passed, 1-2 needed)", argc);
  TMarioGamePad *controller = nullptr;
  switch (Spc::Stack::popItem(interp).mValue) {
  case 0:
    controller = gpApplication.mGamePad1;
  case 1:
    controller = gpApplication.mGamePad2;
  case 2:
    controller = gpApplication.mGamePad3;
  case 3:
    controller = gpApplication.mGamePad4;
  }
  bool isFrameInput = static_cast<bool>(Spc::Stack::popItem(interp).mValue);
  if (controller) {
    if (isFrameInput)
        Spc::Stack::pushItem(interp, controller->mButtons.mFrameInput, Spc::ValueType::INT); // Return a value
    else
        Spc::Stack::pushItem(interp, controller->mButtons.mInput, Spc::ValueType::INT); // Return a value
  } else {
    SpcTrace("Controller not found!");
  }
}