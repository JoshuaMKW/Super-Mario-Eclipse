#include "common.hxx"
#include "libs/sBmg.hxx"
#include "libs/sMemory.hxx"
#include "libs/sString.hxx"
#include "libs/sTime.hxx"
#include "sms/JSystem/JDrama.hxx"

using namespace SME::Util;

void Spc::formatStrBySpec(TSpcInterp *interp, u32 argc) {
  SME_ASSERT(argc == 1, "Incorrect number of arguments passed to spc::getPlayerByIndex (%lu args passed, 1 needed)", argc);
  char *newstr = reinterpret_cast<char *>(SME::Util::Memory::calloc(1024, 4));
  formatBMGRaw(newstr, reinterpret_cast<char *>(Spc::Stack::popItem(interp).mValue), 1024);
  Spc::Stack::pushItem(interp, reinterpret_cast<u32>(newstr), Spc::ValueType::INT); // Return a value
}