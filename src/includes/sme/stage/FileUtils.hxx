#pragma once

#include "sms/JSystem/JKR/JKRDvdRipper.hxx"
#include "sms/JSystem/JKR/JKRHeap.hxx"
#include "sms/JSystem/JUT/JUTGamePad.hxx"
#include "sms/game/Application.hxx"
#include "types.h"


#include "funcs.hxx"

namespace SME::Util {
const char *getStageName(TApplication *gpApplication);
void *loadArchive(const char *path, JKRHeap *heap,
                  JKRDvdRipper::EAllocDirection allocDirection =
                      JKRDvdRipper::EAllocDirection::HEAD);
} // namespace SME::Util