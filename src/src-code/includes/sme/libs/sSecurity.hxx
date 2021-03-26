#pragma once

#include "types.h"
#include "OS.h"

namespace SME::Util::Security
{
bool areGeckoCodesPresent(void *handlerBuffer, size_t maxlen, u32 **addrStorage = nullptr);
void *setUserCodes(OSAlarm *alarm, OSContext *context);
}