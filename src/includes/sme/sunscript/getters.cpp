#include "common.hxx"
#include "Globals.hxx"
#include "libs/sTime.hxx"
#include "sms/SMS.hxx"
#include "sms/actor/Mario.hxx"

using namespace SME::Util;

void Spc::getPlayerByIndex(TSpcInterp *interp, u32 argc) {
  interp->verifyArgNum(1, &argc);
  Stack::pushItem(interp, reinterpret_cast<u32>(SME::TGlobals::getPlayerByIndex(Stack::popItem(interp).mValue)), ValueType::INT); // Return a value
}

void Spc::getDateAsStr(TSpcInterp *interp, u32 argc) {
  interp->verifyArgNum(0, &argc);
  Stack::pushItem(interp, reinterpret_cast<u32>(Time::date()), ValueType::INT); // Return a value
}

void Spc::getTimeAsStr(TSpcInterp *interp, u32 argc) {
  interp->verifyArgNum(0, &argc);
  Stack::pushItem(interp, reinterpret_cast<u32>(Time::time()), ValueType::INT); // Return a value
}

void Spc::getPlayerInputByIndex(TSpcInterp *interp, u32 argc) {
  interp->verifyArgNum(2, &argc);
  TMarioGamePad *controller = nullptr;
  switch (Stack::popItem(interp).mValue) {
  case 0:
    controller = gpApplication.mGamePad1;
  case 1:
    controller = gpApplication.mGamePad2;
  case 2:
    controller = gpApplication.mGamePad3;
  case 3:
    controller = gpApplication.mGamePad4;
  }
  bool isFrameInput = static_cast<bool>(Stack::popItem(interp).mValue);
  if (controller) {
    if (isFrameInput)
        Stack::pushItem(interp, controller->mButtons.mFrameInput, ValueType::INT); // Return a value
    else
        Stack::pushItem(interp, controller->mButtons.mInput, ValueType::INT); // Return a value
  } else {
    SpcTrace("Controller not found!\n");
  }
}