#include "OS.h"
#include "SME.hxx"


#include "Kernel.hxx"
#include "macros.h"
#include "sms/actor/Mario.hxx"

// mario.cpp
extern "C" s16 mario_shadowCrashPatch();

// shine.cpp
extern "C" void shine_animationFreezeCheck();
extern "C" void shine_thinkCloseCamera();

extern OSAlarm gctAlarm;

// ================================= //

#define _SME_PATCH_RAM
#define _SME_EXECUTE_LOADS

#if defined(SME_DEBUG) && !defined(SME_RELEASE)
extern OSStopwatch gctStopwatch;
#endif

using namespace SME;

extern void makeExtendedObjDataTable();

Memory::Protection::MemoryMap gCodeProtector;

static void initCodeProtection() {
#if SME_PROTECT_MEM
  gCodeProtector.setIndex(2);
  gCodeProtector.setStart(0x80003800);
  gCodeProtector.setEnd(0x80373000);
  gCodeProtector.setPermission(Memory::Protection::READ);
  gCodeProtector.activate();
#endif
}

#if SME_VARIABLE_FPS
static OSAlarm sFPSAlarm;

static void initVariableFPS() {
  extern void updateFPSAsync(OSAlarm *, OSContext *);
  OSCreateAlarm(&sFPSAlarm);
  OSSetPeriodicAlarm(&sFPSAlarm, OSGetTime(), OSMillisecondsToTicks(1),
                     &updateFPSAsync);
}
#else
static void initVariableFPS() {}
#endif

static void initMod() {
  SME_DEBUG_LOG(
      "Codeblocker - Creating OSAlarm at %p; Calls %p every %0.4f seconds\n",
      &gctAlarm, &SME::Util::Security::checkUserCodes, 0.001f);
#if defined(SME_DEBUG) && !defined(SME_RELEASE)
  OSInitStopwatch(&gctStopwatch, "Codeblocker");
#endif
  OSCreateAlarm(&gctAlarm);
  OSSetPeriodicAlarm(
      &gctAlarm, OSGetTime(), OSMillisecondsToTicks(1),
      reinterpret_cast<OSAlarmHandler>(&SME::Util::Security::checkUserCodes));
  SME_DEBUG_LOG("Registered checkUserCodes at 0x%X\n",
                &SME::Util::Security::checkUserCodes);

  makeExtendedObjDataTable();
  initCodeProtection();
  initVariableFPS();
}

static void destroyMod() {
  SME_DEBUG_LOG("-- Destroying Module --\n");
  // #if defined(SME_DEBUG) && !defined(SME_RELEASE)
  //   OSStopStopwatch(&gctStopwatch);
  // #endif
  //   OSCancelAlarm(&gctAlarm);
}

#if defined(SME_BUILD_KURIBO) && !defined(SME_BUILD_KAMEK) &&                  \
    !defined(SME_BUILD_KAMEK_INLINE)

pp::DefineModule(SME_MODULE_NAME, SME_AUTHOR_NAME, SME_VERSION_TAG);
pp::OnLoad(initMod);
pp::OnUnload(destroyMod);

#elif !defined(SME_BUILD_KURIBO) && !defined(SME_BUILD_KAMEK) &&               \
    defined(SME_BUILD_KAMEK_INLINE)

static void moduleLoad(int size, bool unk) {
  JKRExpHeap::createRoot(size, unk);
  SME_LOG("~~~~~~~~~~~~~~~~~~~~~~~\n"
          "[KAMEK] Game patched by"
          "         Name:     	" SME_MODULE_NAME "\n"
          "         Author:   	" SME_AUTHOR_NAME "\n"
          "         Version:  	" SME_VERSION_TAG "\n"
          "\n"
          "         Built:    	" __DATE__ " at " __TIME__ "\n"
          "         Compiler: 	" __VERSION__ "\n"
          "~~~~~~~~~~~~~~~~~~~~~~~\n");
  initMod();
}
SME_PATCH_BL(SME_PORT_REGION(0x802A744C, 0, 0, 0), moduleLoad);

#elif !defined(SME_BUILD_KURIBO) && defined(SME_BUILD_KAMEK) &&                \
    !defined(SME_BUILD_KAMEK_INLINE)

static void moduleLoad(int size, bool unk) {
  JKRExpHeap::createRoot(size, unk);
  if (Kamek::loadKamekBinaryFromDisc("/SME.kmk")) {
    SME_LOG("~~~~~~~~~~~~~~~~~~~~~~~\n"
            "[KAMEK] Game patched by"
            "         Name:     	" SME_MODULE_NAME "\n"
            "         Author:   	" SME_AUTHOR_NAME "\n"
            "         Version:  	" SME_VERSION_TAG "\n"
            "\n"
            "         Built:    	" __DATE__ " at " __TIME__ "\n"
            "         Compiler: 	" __VERSION__ "\n"
            "~~~~~~~~~~~~~~~~~~~~~~~\n");
    initMod();
  }
}
SME_PATCH_BL(SME_PORT_REGION(0x802A744C, 0, 0, 0), moduleLoad);

#endif

/* -- HOOKS -- */

// c_kit.cpp
SME_PATCH_BL(SME_PORT_REGION(0x802998B8, 0x80291750, 0, 0), Patch::CKit::onSetup);
SME_PATCH_BL(SME_PORT_REGION(0x802A616C, 0x8029E07C, 0, 0), Patch::CKit::onUpdate);
SME_PATCH_BL(SME_PORT_REGION(0x80143F14, 0x80138B50, 0, 0), Patch::CKit::onDraw2D);
SME_PATCH_B(SME_PORT_REGION(0x802A8B58, 0x802A0C00, 0, 0), Util::SMS::isExMap);
SME_PATCH_B(SME_PORT_REGION(0x802A8B30, 0x802A0BD8, 0, 0), Util::SMS::isMultiplayerMap);
SME_PATCH_B(SME_PORT_REGION(0x802A8AFC, 0x802A0BA4, 0, 0), Util::SMS::isDivingMap);
SME_PATCH_B(SME_PORT_REGION(0x802A8AE0, 0x802A0B88, 0, 0), Util::SMS::isOptionMap);
SME_PATCH_BL(SME_PORT_REGION(0x8027C6A4, 0x80274430, 0, 0),
             Patch::CKit::manageLightSize);
SME_WRITE_32(SME_PORT_REGION(0x8027C6A8, 0x80274434, 0, 0), 0x28030001);
//SME_PATCH_BL(SME_PORT_REGION(0x80153DE8, 0x80148D68, 0, 0),//              Patch::CKit::formatTalkMessage);
//SME_PATCH_BL(SME_PORT_REGION(0x80153E1C, 0x80148D9C, 0, 0),//              Patch::CKit::formatTalkMessage);
SME_PATCH_BL(SME_PORT_REGION(0x8024D3A8, 0x80245134, 0, 0),
             Patch::CKit::realTimeCustomAttrsHandler);

SME_PATCH_BL(SME_PORT_REGION(0x8003F8F0, 0x8003F740, 0, 0),
             Patch::CKit::realTimeCustomAttrsHandler);