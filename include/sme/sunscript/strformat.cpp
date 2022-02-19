#include "common.hxx"
#include "libs/sBmg.hxx"
#include "libs/sMemory.hxx"
#include "libs/sString.hxx"
#include "libs/sTime.hxx"

using namespace SME::Util;

void Spc::formatStrBySpec(TSpcInterp *interp, u32 argc) {
  interp->verifyArgNum(1, &argc);
  char *newstr = reinterpret_cast<char *>(Memory::calloc(1024, 4));
  formatBMGRaw(newstr, reinterpret_cast<char *>(Stack::popItem(interp).mValue),
               1024);
  Stack::pushItem(interp, reinterpret_cast<u32>(newstr),
                  ValueType::STRING); // Return a value
}