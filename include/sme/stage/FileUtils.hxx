#pragma once

#include "JKR/JKRDvdRipper.hxx"
#include "JKR/JKRHeap.hxx"
#include "JUT/JUTGamePad.hxx"
#include "sms/game/Application.hxx"
#include "types.h"


#include "funcs.hxx"

namespace SME::Util {
const char *getStageName(TApplication *gpApplication);
void *loadArchive(const char *path, JKRHeap *heap,
                  JKRDvdRipper::EAllocDirection allocDirection =
                      JKRDvdRipper::EAllocDirection::HEAD);
} // namespace SME::Util