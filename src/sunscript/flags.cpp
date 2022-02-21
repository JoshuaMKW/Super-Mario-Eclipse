#include "common.hxx"
#include "Globals.hxx"

using namespace SME::Util;

void Spc::isMultiplayerActive(TSpcInterp *interp, u32 argc) {
  interp->verifyArgNum(0, &argc);
  Spc::Stack::pushItem(interp, static_cast<u32>(SME::TGlobals::isMultiplayerActive()), Spc::ValueType::INT); // Return a value
}

void Spc::isFreePlayActive(TSpcInterp *interp, u32 argc) {
  interp->verifyArgNum(0, &argc);
  Spc::Stack::pushItem(interp, static_cast<u32>(SME::TGlobals::isFreePlayMode()), Spc::ValueType::INT); // Return a value
}

void Spc::isCompletionRewardActive(TSpcInterp *interp, u32 argc) {
  interp->verifyArgNum(0, &argc);
  Spc::Stack::pushItem(interp, static_cast<u32>(SME::TGlobals::isCompletionRewarded()), Spc::ValueType::INT); // Return a value
}

void Spc::isMirrorModeActive(TSpcInterp *interp, u32 argc) {
  interp->verifyArgNum(0, &argc);
  Spc::Stack::pushItem(interp, static_cast<u32>(SME::TGlobals::isMirrorMode()), Spc::ValueType::INT); // Return a value
}

void Spc::getActivePlayers(TSpcInterp *interp, u32 argc) {
  interp->verifyArgNum(0, &argc);
  Spc::Stack::pushItem(interp, static_cast<u32>(SME::TGlobals::getActivePlayers()), Spc::ValueType::INT); // Return a value
}

void Spc::getMaxPlayers(TSpcInterp *interp, u32 argc) {
  interp->verifyArgNum(0, &argc);
  Spc::Stack::pushItem(interp, static_cast<u32>(SME::TGlobals::getMaxPlayers()), Spc::ValueType::INT); // Return a value
}