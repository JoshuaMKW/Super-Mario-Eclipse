#include "common.hxx"
#include "Globals.hxx"
#include "sms/JSystem/JDrama.hxx"

using namespace SME::Util;

void Spc::isMultiplayerActive(TSpcInterp *interp, u32 argc) {
  SME_ASSERT(argc == 0, "Incorrect number of arguments passed to spc::isMultiplayerActive (%lu args passed, 0 needed)", argc);
  Spc::Stack::pushItem(interp, static_cast<u32>(SME::TGlobals::isMultiplayerActive()), Spc::ValueType::INT); // Return a value
}

void Spc::isFreePlayActive(TSpcInterp *interp, u32 argc) {
  SME_ASSERT(argc == 0, "Incorrect number of arguments passed to spc::isFreePlayActive (%lu args passed, 0 needed)", argc);
  Spc::Stack::pushItem(interp, static_cast<u32>(SME::TGlobals::isFreePlayMode()), Spc::ValueType::INT); // Return a value
}

void Spc::isCompletionRewardActive(TSpcInterp *interp, u32 argc) {
  SME_ASSERT(argc == 0, "Incorrect number of arguments passed to spc::isCompletionRewardActive (%lu args passed, 0 needed)", argc);
  Spc::Stack::pushItem(interp, static_cast<u32>(SME::TGlobals::isCompletionRewarded()), Spc::ValueType::INT); // Return a value
}

void Spc::getActivePlayers(TSpcInterp *interp, u32 argc) {
  SME_ASSERT(argc == 0, "Incorrect number of arguments passed to spc::getActivePlayers (%lu args passed, 0 needed)", argc);
  Spc::Stack::pushItem(interp, static_cast<u32>(SME::TGlobals::getActivePlayers()), Spc::ValueType::INT); // Return a value
}

void Spc::getMaxPlayers(TSpcInterp *interp, u32 argc) {
  SME_ASSERT(argc == 0, "Incorrect number of arguments passed to spc::getMaxPlayers (%lu args passed, 0 needed)", argc);
  Spc::Stack::pushItem(interp, static_cast<u32>(SME::TGlobals::getMaxPlayers()), Spc::ValueType::INT); // Return a value
}