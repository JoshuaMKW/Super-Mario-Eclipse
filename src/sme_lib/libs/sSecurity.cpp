#include "libs/sSecurity.hxx"
#include "Globals.hxx"

OSAlarm gctAlarm;

#if defined(SME_DEBUG) && !defined(SME_RELEASE)
OSStopwatch gctStopwatch;
#endif

static constexpr u32 sEnabledmagic = 0x00D0C0DE;
static u32 *sCachedAddr = nullptr;

using namespace SME;

bool Util::Security::areGeckoCodesPresent(void *handlerBuffer, size_t maxlen,
                                          u32 **addrStorage) {
  u32 *checkArea = static_cast<u32 *>(handlerBuffer);

  for (u32 i = 0; i < (maxlen >> 2) - 1; ++i) {
    if (checkArea[i] == sEnabledmagic && checkArea[i + 1] == sEnabledmagic) {
      if (addrStorage)
        *addrStorage = &checkArea[i];
      return true;
    }
  }
  return false;
}

void *Util::Security::checkUserCodes(OSAlarm *alarm, OSContext *context) {
#if defined(SME_DEBUG) && !defined(SME_RELEASE)
  OSStartStopwatch(&gctStopwatch);
#endif

  u32 *searchcontext = (u32 *)(0x80001C00);
  const size_t searchlength = 0x80003000 - 0x80001C00;

  if (sCachedAddr) {
    if (areGeckoCodesPresent(sCachedAddr, 8)) {
#if defined(SME_DEBUG) && !defined(SME_RELEASE)
      OSStopStopwatch(&gctStopwatch);
      // if (gctStopwatch.mHits % 60000 == 0)
      //    OSDumpStopwatch(&gctStopwatch);
#endif

      SME::TGlobals::sPlayerHasGeckoCodes = true;
      return nullptr;
    }
  }
  SME::TGlobals::sPlayerHasGeckoCodes = areGeckoCodesPresent(
      static_cast<void *>(searchcontext), searchlength, &sCachedAddr);

#if defined(SME_DEBUG) && !defined(SME_RELEASE)
  OSStopStopwatch(&gctStopwatch);
  // if (gctStopwatch.mHits % 60000 == 0) OSDumpStopwatch(&gctStopwatch);
#endif

  return nullptr;
}