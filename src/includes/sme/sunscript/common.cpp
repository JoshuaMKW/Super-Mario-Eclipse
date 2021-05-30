#include "common.hxx"

using namespace SME::Util;

TSpcSlice Spc::Stack::popItem(TSpcInterp *interp) {
  SME_ASSERT(interp->mSlicesCount > 0, "Can't pop item from empty SPC stack\n");
  interp->mSlicesCount -= 1;
  return interp->mSlices[interp->mSlicesCount];
}

void Spc::Stack::pushItem(TSpcInterp *interp, u32 value, Spc::ValueType type) {
  SME_ASSERT(interp->mSlicesCount < interp->mSlicesMax, "Can't pop item from empty SPC stack\n");
  TSpcSlice slice;
  slice.mValue = value;
  slice.mType = static_cast<u32>(type);
  interp->mSlices[interp->mSlicesCount] = slice;
  interp->mSlicesCount += 1;
}