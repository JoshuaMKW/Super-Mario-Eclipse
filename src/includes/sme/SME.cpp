#include "SME.hxx"
#include "OS.h"

#include "Kernel.hxx"
#include "common_sdk.h"
#include "macros.h"
#include "sms/actor/Mario.hxx"

// mario.cpp
extern "C" s16 mario_shadowCrashPatch();

// shine.cpp
extern "C" void shine_animationFreezeCheck();
extern "C" void shine_thinkCloseCamera();

extern OSAlarm gctAlarm;

#define _SME_PATCH_RAM
#define _SME_EXECUTE_LOADS

#ifdef SME_DEBUG
extern OSStopwatch gctStopwatch;
#endif

using namespace SME;

#if defined(SME_BUILD_KURIBO)
#define SME_PATCH_B(source, target) KURIBO_PATCH_B(source, target)
#define SME_PATCH_BL(source, target) KURIBO_PATCH_BL(source, target)
#define SME_WRITE_8(source, value)                                             \
  SME::Util::Memory::PPC::write<u8>(reinterpet_cast<u8 *>(source), value)
#define SME_WRITE_16(source, value)                                            \
  SME::Util::Memory::PPC::write<u16>(reinterpet_cast<u16 *>(source), value)
#define SME_WRITE_32(source, value) kWrite32(source, value)
#elif defined(SME_BUILD_KAMEK) || defined(SME_BUILD_KAMEK_INLINE)
#define SME_PATCH_B(source, target) kmBranch(source, target)
#define SME_PATCH_BL(source, target) kmCall(source, target)
#define SME_WRITE_8(source, value) kmWrite8(source, value)
#define SME_WRITE_16(source, value) kmWrite16(source, value)
#define SME_WRITE_32(source, value) kmWrite32(source, value)
#else
#error                                                                         \
    "Build type unspecified. Define either SME_BUILD_KAMEK or SME_BUILD_KAMEK_INLINE or SME_BUILD_KURIBO"
#endif

#if defined(SME_BUILD_KURIBO) && !defined(SME_BUILD_KAMEK) &&                  \
    !defined(SME_BUILD_KAMEK_INLINE)
KURIBO_MODULE_BEGIN(SME_MODULE_NAME, SME_AUTHOR_NAME, SME_VERSION_TAG) {
  KURIBO_EXECUTE_ON_LOAD {
    SME_DEBUG_LOG(
        "Codeblocker - Creating OSAlarm at %p; Calls %p every %0.4f seconds\n",
        &gctAlarm, &SME::Util::Security::checkUserCodes, 0.001f);
#ifdef SME_DEBUG
    OSInitStopwatch(&gctStopwatch, "Codeblocker");
#endif
    OSCreateAlarm(&gctAlarm);
    OSSetPeriodicAlarm(
        &gctAlarm, OSGetTime(), OSMillisecondsToTicks(1),
        reinterpret_cast<OSAlarmHandler>(&SME::Util::Security::checkUserCodes));
    SME_DEBUG_LOG("Mario health offset = 0x%X\n", offsetof(TMario, mHealth));
    SME_DEBUG_LOG("J3DFrameCtrl offset = 0x%X\n", offsetof(J3DFrameCtrl, _04));
  }
  KURIBO_EXECUTE_ON_UNLOAD {
    SME_DEBUG_LOG("-- Destroying Module --\n");
#ifdef SME_DEBUG
    OSStopStopwatch(&gctStopwatch);
#endif
    OSCancelAlarm(&gctAlarm);
  }
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
  SME_DEBUG_LOG(
      "Codeblocker - Creating OSAlarm at %p; Calls %p every %0.4f seconds\n",
      &gctAlarm, &SME::Util::Security::checkUserCodes, 0.001f);
#ifdef SME_DEBUG
  OSInitStopwatch(&gctStopwatch, "Codeblocker");
#endif
  OSCreateAlarm(&gctAlarm);
  OSSetPeriodicAlarm(
      &gctAlarm, OSGetTime(), OSMillisecondsToTicks(1),
      reinterpret_cast<OSAlarmHandler>(&SME::Util::Security::checkUserCodes));
  SME_DEBUG_LOG("Mario health offset = %X\n", offsetof(TMario, mHealth));
  SME_DEBUG_LOG("J3DFrameCtrl offset = 0x%X\n", offsetof(J3DFrameCtrl, mFrameRate));
  Patch::Init::initCodeProtection();
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
            "~~~~~~~~~~~~~~~~~~~~~~~\n", );
    SME_DEBUG_LOG(
        "Codeblocker - Creating OSAlarm at %p; Calls %p every %0.4f seconds\n",
        &gctAlarm, &SME::Util::Security::checkUserCodes, 0.001f);
#ifdef SME_DEBUG
    OSInitStopwatch(&gctStopwatch, "Codeblocker");
#endif
    OSCreateAlarm(&gctAlarm);
    OSSetPeriodicAlarm(
        &gctAlarm, OSGetTime(), OSMillisecondsToTicks(1),
        reinterpret_cast<OSAlarmHandler>(&SME::Util::Security::checkUserCodes));
    SME_DEBUG_LOG("Mario health offset = %X\n", offsetof(TMario, mHealth));
  }
  Patch::Init::initCodeProtection();
}
SME_PATCH_BL(SME_PORT_REGION(0x802A744C, 0, 0, 0), moduleLoad);
#endif

#ifdef _SME_PATCH_RAM

  /* -- HOOKS -- */

  // c_kit.cpp
  SME_PATCH_BL(SME_PORT_REGION(0x802998B8, 0x80291750, 0, 0), Patch::CKit::onSetup);
  SME_PATCH_BL(SME_PORT_REGION(0x802A616C, 0x8029e07c, 0, 0), Patch::CKit::onUpdate);
  SME_PATCH_BL(SME_PORT_REGION(0x80143F14, 0x80138b50, 0, 0), Patch::CKit::onDraw2D);
  // SME_PATCH_B(SME_PORT_REGION(0x802A8B58, 0x802a0c00, 0, 0), Util::SMS::isExMap);
  // SME_PATCH_B(SME_PORT_REGION(0x802A8B30, 0x802a0bd8, 0, 0),
  //             Util::SMS::isMultiplayerMap);
  // SME_PATCH_B(SME_PORT_REGION(0x802A8AFC, 0x802a0ba4, 0, 0), Util::SMS::isDivingMap);
  // SME_PATCH_B(SME_PORT_REGION(0x802A8AE0, 0x802a0b88, 0, 0), Util::SMS::isOptionMap);
  // SME_PATCH_BL(SME_PORT_REGION(0x8027C6A4, 0x80274430, 0, 0),
  //              Patch::CKit::manageLightSize);
  // SME_WRITE_32(SME_PORT_REGION(0x8027C6A8, 0x80274434, 0, 0), 0x28030001);
  SME_PATCH_BL(SME_PORT_REGION(0x80153DE8, 0x80148d68, 0, 0),
               Patch::CKit::formatTalkMessage);
  SME_PATCH_BL(SME_PORT_REGION(0x80153E1C, 0x80148d9c, 0, 0),
               Patch::CKit::formatTalkMessage);
  // SME_PATCH_BL(SME_PORT_REGION(0x8024D3A8, 0x80245134, 0, 0),
  //              Patch::CKit::realTimeCustomAttrsHandler);

  // camera.cpp
  // SME_PATCH_B(SME_PORT_REGION(0x80027548, 0x80245134, 0, 0),
  //             Patch::Camera::modifyCameraRangeToSize);

  // // CheatCode.cpp
  // SME_PATCH_B(SME_PORT_REGION(0x80295B6C, 0x8028da04, 0, 0),
  //             Patch::Cheat::handleDebugCheat);

  // collision.cpp
  SME_PATCH_BL(SME_PORT_REGION(0x80258334, 0x802500c0, 0, 0),
               Patch::Collision::checkIsGlideBounce);
  SME_PATCH_BL(SME_PORT_REGION(0x80264CFC, 0x8025ca88, 0, 0),
               Patch::Collision::checkIsGlideBounce);
  SME_PATCH_BL(SME_PORT_REGION(0x8024C558, 0x802442e4, 0, 0),
               Patch::Collision::checkIsRestoreTypeNoFallDamage);
  SME_PATCH_BL(SME_PORT_REGION(0x80250CA0, 0x80247e44, 0, 0),
               Patch::Collision::masterGroundCollisionHandler);
  SME_PATCH_BL(SME_PORT_REGION(0x8025059C, 0x80248a2c, 0, 0),
               Patch::Collision::masterAllCollisionHandler);

  // // debug.cpp
  // SME_PATCH_BL(SME_PORT_REGION(0x8024D194, 0x80244f20, 0, 0),
  //              Patch::Debug::xyzModifierMario);
  // SME_WRITE_32(SME_PORT_REGION(0x8024D198, 0x80244f24, 0, 0),
  //              0x2C030000);    

  // // file_flags.cpp
  // SME_PATCH_BL(SME_PORT_REGION(0x802B1794, 0x802a96a4, 0, 0),
  //              Patch::Flag::setFileCompleteBool);

  // // flagmanager.cpp
  // SME_PATCH_B(SME_PORT_REGION(0x80294EF4, 0x8028cd0c, 0, 0), Patch::Flag::resetStage);

  // // fludd.cpp
  // SME_PATCH_B(SME_PORT_REGION(0x80248F14, 0x80240ca0, 0, 0), Patch::Fludd::isPumpOK);
  // SME_WRITE_32(SME_PORT_REGION(0x803DCA00, 0x803d41e2, 0, 0),
  //              0x00300000 | TMarioAnimeData::FLUDD::FLUDD_ENABLED);
  // SME_PATCH_BL(SME_PORT_REGION(0x8014206C, 0x80136c80, 0, 0),
  //              Patch::Fludd::hasWaterCardOpen);
  // SME_WRITE_32(SME_PORT_REGION(0x80142070, 0x80136c84, 0, 0), 0x28030000);
  // SME_PATCH_BL(SME_PORT_REGION(0x80283058, 0x8027ade4, 0, 0),
  //              Patch::Fludd::canCollectFluddItem);
  // SME_PATCH_BL(SME_PORT_REGION(0x800678C4, 0x80060f64, 0, 0),
  //              Patch::Fludd::sprayGoopMap);
  // SME_PATCH_BL(SME_PORT_REGION(0x801A3ED0, 0x8019c758, 0, 0),
  //              Patch::Fludd::sprayGoopMap);
  // SME_PATCH_BL(SME_PORT_REGION(0x801B42D8, 0x801ac190, 0, 0),
  //              Patch::Fludd::sprayGoopMap);
  // SME_PATCH_BL(SME_PORT_REGION(0x8024E710, 0x8024649c, 0, 0),
  //              Patch::Fludd::sprayGoopMap);
  // SME_PATCH_BL(SME_PORT_REGION(0x8027F7DC, 0x80277568, 0, 0),
  //              Patch::Fludd::sprayGoopMap);
  // SME_PATCH_BL(SME_PORT_REGION(0x8027F94C, 0x802776d8, 0, 0),
  //              Patch::Fludd::sprayGoopMap);
  // SME_PATCH_BL(SME_PORT_REGION(0x800FED3C, 0x800f83dc, 0, 0),
  //              Patch::Fludd::canCleanSeals);
  // SME_WRITE_32(SME_PORT_REGION(0x800FED40, 0x800f83e0, 0, 0), 0x2C030000);
  // SME_PATCH_BL(SME_PORT_REGION(0x8024D53C, 0x802452c8, 0, 0),
  //              Patch::Fludd::bindFluddtojoint);
  // SME_PATCH_BL(SME_PORT_REGION(0x8024E548, 0x802462d4, 0, 0),
  //              Patch::Fludd::checkExecWaterGun);
  // SME_PATCH_BL(SME_PORT_REGION(0x8026C370, 0x802640fc, 0, 0),
  //              Patch::Fludd::killTriggerNozzle);
  // SME_PATCH_BL(SME_PORT_REGION(0x8026C018, 0x80263da4, 0, 0),
  //              Patch::Fludd::spamHoverWrapper);
  // SME_PATCH_BL(SME_PORT_REGION(0x80262580, 0x8025a30c, 0, 0),
  //              Patch::Fludd::checkAirNozzle);
  // SME_WRITE_32(SME_PORT_REGION(0x80262584, 0x8025a310, 0, 0), 0x2C030000);

  // fruit.cpp
  SME_PATCH_BL(SME_PORT_REGION(0x801E542C, 0x801dd304, 0, 0),
               Patch::Fruit::canFruitDieWater);
  SME_WRITE_32(SME_PORT_REGION(0x801E5430, 0x801dd308, 0, 0), 0x2C030000);
  SME_WRITE_32(SME_PORT_REGION(0x801E5434, 0x801dd30c, 0, 0), 0x41820140);
  SME_PATCH_BL(SME_PORT_REGION(0x8023F964, 0x802376f0, 0, 0),
               Patch::Fruit::chooseGrabDistancing);
  SME_PATCH_BL(SME_PORT_REGION(0x8023F9DC, 0x80237768, 0, 0),
               Patch::Fruit::isGrabWaitOver);

  // // init_mods.cpp
  // SME_PATCH_BL(SME_PORT_REGION(0x802A7140, 0x8029f02c, 0, 0),
  //              Patch::Init::setupMarioDatas);
               
  // SME_PATCH_BL(SME_PORT_REGION(0x802A750C, 0x8029f588, 0, 0),
  //              Patch::Init::createGlobalHeaps);
  // SME_PATCH_BL(SME_PORT_REGION(0x802A716C, 0x8029f058, 0, 0),
  //              Patch::Init::initFirstModel);
  // SME_PATCH_BL(SME_PORT_REGION(0x802998B4, 0x8029174c, 0, 0), Patch::Init::initFileMods);
  // SME_PATCH_B(SME_PORT_REGION(0x80280180, 0x80277f0c, 0, 0),
  //             Patch::Init::initShineShadow);
  // SME_PATCH_BL(SME_PORT_REGION(0x802B7A4C, 0x802afa1c, 0, 0),
  //              Patch::Init::initSoundBank);
  // SME_PATCH_BL(SME_PORT_REGION(0x802983F0, 0x80290288, 0, 0),
  //              Patch::Init::initMusicTrack);
  // SME_PATCH_BL(SME_PORT_REGION(0x80298420, 0x802902b8, 0, 0),
  //              Patch::Init::initMusicTrack);
  // SME_WRITE_32(SME_PORT_REGION(0x80276C90, 0x8026ea1c, 0, 0), 0x60000000);
  // SME_PATCH_BL(SME_PORT_REGION(0x80276C94, 0x8026ea20, 0, 0),
  //              Patch::Init::fromMarioInit);
  // SME_PATCH_BL(SME_PORT_REGION(0x800397DC, 0, 0, 0),
  //              Patch::Init::fromShadowMarioInit);
  // SME_PATCH_BL(SME_PORT_REGION(0x80271580, 0, 0, 0), Patch::Init::initYoshi);
  // SME_PATCH_B(SME_PORT_REGION(0x8029CCB0, 0, 0, 0),
  //             Patch::Init::initCardColors);
  // SME_PATCH_BL(SME_PORT_REGION(0x802B8B20, 0, 0, 0),
  //              Patch::Init::initCollisionWarpLinks);
  // SME_PATCH_BL(SME_PORT_REGION(0x802B57E4, 0, 0, 0), Patch::Init::createUIHeap);

  // // mario.cpp
  // SME_PATCH_BL(SME_PORT_REGION(0x802320E0, 0, 0, 0), mario_shadowCrashPatch);
  // SME_PATCH_BL(SME_PORT_REGION(0x802500B8, 0, 0, 0),
  //              Patch::Mario::updateContexts);
  // SME_PATCH_BL(SME_PORT_REGION(0x8029A87C, 0, 0, 0),
  //              Patch::Mario::carryOrTalkNPC);
  // SME_PATCH_BL(SME_PORT_REGION(0x802815F0, 0, 0, 0),
  //              Patch::Mario::canGrabAtNPC);
  // SME_WRITE_32(SME_PORT_REGION(0x802815F4, 0, 0, 0), 0x2C030000);
  // SME_PATCH_BL(SME_PORT_REGION(0x80207430, 0, 0, 0), Patch::Mario::canCarryNPC);
  // SME_WRITE_32(SME_PORT_REGION(0x80207434, 0, 0, 0), 0x2C030000);
  // SME_PATCH_BL(0x802145F0, Patch::Mario::scaleNPCThrowLength);
  // SME_WRITE_32(0x802145F4, 0xC002E5E0);
  // SME_WRITE_32(0x802145F8, 0xC0230034);
  // SME_WRITE_32(0x8021462C, 0xEC0B0032);
  // SME_WRITE_32(0x80214634, 0xEC2B0072);
  // SME_PATCH_BL(0x8021463C, Patch::Mario::scaleNPCThrowHeight);
  // #if 0
  // SME_PATCH_BL(0x80261C3C, Patch::Mario::getTreeClimbMinFall);
  // SME_WRITE_32(0x80261C40, 0xC05F038C);
  // SME_WRITE_32(0x80261C44, 0xFC020040);
  // SME_PATCH_BL(0x802619CC, Patch::Mario::getTreeClimbMaxFall);
  // SME_WRITE_32(0x802619D0, 0xC05F0014);
  // SME_PATCH_BL(0x80261CF4, Patch::Mario::scaleTreeSlideSpeed);
  // SME_WRITE_32(0x80261CF8, 0x2C030000);
  // SME_WRITE_32(0x80261CFC, 0x41820070);
  // SME_PATCH_BL(0x8025D588, Patch::Mario::getClimbingAnimSpd);
  // SME_PATCH_BL(0x8025D63C, Patch::Mario::getClimbingAnimSpd);
  // SME_PATCH_BL(0x8025D650, Patch::Mario::getClimbingAnimSpd);
  // SME_PATCH_BL(0x8025DBC4, Patch::Mario::getClimbingAnimSpd);
  // SME_PATCH_BL(0x8025E38C, Patch::Mario::getClimbingAnimSpd);
  // SME_PATCH_BL(0x802615AC, Patch::Mario::scaleHangSpeed);
  // SME_WRITE_32(0x802615B0, 0x60000000);
  // SME_PATCH_BL(0x8024E288, Patch::Mario::checkGraffitiAffected);
  // // SME_PATCH_BL(0x801E4118, Patch::Mario::rescaleHeldObj);
  // SME_PATCH_BL(0x802571F0, Patch::Mario::patchYStorage);
  // SME_PATCH_BL(0x8024E02C, Patch::Mario::manageExtraJumps);
  // SME_WRITE_32(0x8025B8BC, 0x60000000);
  // SME_PATCH_BL(0x8025B8C0, Patch::Mario::checkGroundSpeedLimit);
  // SME_WRITE_32(0x8025B8C4, 0xEFFF0072);
  // #endif
  // SME_PATCH_BL(0x8003FFEC, Patch::Mario::manageEMarioHealthWrapper);
  // SME_WRITE_32(0x8003FD94, 0x60000000);

  // multiplayer.cpp
  // SME_PATCH_B(SME_PORT_REGION(0x802EFAB4, 0, 0, 0),
  // Patch::Multiplayer::draw3DOverhaul);
  SME_PATCH_BL(SME_PORT_REGION(0x8029D7E8, 0, 0, 0),
               Patch::Multiplayer::makeMarios);
  // SME_PATCH_B(SME_PORT_REGION(0x80276BD0, 0, 0, 0),
  // Patch::Multiplayer::loadMarioTrickyOverhaul);
  // SME_PATCH_B(SME_PORT_REGION(0x8024D2A8, 0, 0, 0),
  // Patch::Multiplayer::performMarioTrickyOverhaul);
  // SME_PATCH_BL(SME_PORT_REGION(0x802983F8, 0, 0, 0),
  // Patch::Multiplayer::setMarioOverhaul);
  // SME_PATCH_BL(SME_PORT_REGION(0x80298428, 0, 0, 0),
  // Patch::Multiplayer::setMarioOverhaul);
  // SME_PATCH_BL(SME_PORT_REGION(0x802984D8, 0, 0, 0),
  // Patch::Multiplayer::setMarioOverhaul);
  // SME_PATCH_BL(SME_PORT_REGION(0x800397DC, 0x80039894, 0, 0),
  //              Patch::Init::fromShadowMarioInit);
  // SME_PATCH_BL(SME_PORT_REGION(0x80271580, 0x8026930c, 0, 0), Patch::Init::initYoshi);
  // SME_PATCH_B(SME_PORT_REGION(0x8029CCB0, 0x80294b8c, 0, 0),
  //             Patch::Init::initCardColors);
  // SME_PATCH_BL(SME_PORT_REGION(0x802B8B20, 0x802b0af0, 0, 0),
  //              Patch::Init::initCollisionWarpLinks);
  // SME_PATCH_BL(SME_PORT_REGION(0x802B57E4, 0x802ad768, 0, 0), Patch::Init::createUIHeap);


  // // mario.cpp
  // SME_PATCH_BL(SME_PORT_REGION(0x802320E0, 0x8022a034, 0, 0), mario_shadowCrashPatch);
  // SME_PATCH_BL(SME_PORT_REGION(0x802500B8, 0x80247e44, 0, 0),
  //              Patch::Mario::updateContexts);
  // SME_PATCH_BL(SME_PORT_REGION(0x8029A87C, 0x80292758, 0, 0),
  //              Patch::Mario::carryOrTalkNPC);
  // SME_PATCH_BL(SME_PORT_REGION(0x802815F0, 0x8027937c, 0, 0),
  //              Patch::Mario::canGrabAtNPC);
  // SME_WRITE_32(SME_PORT_REGION(0x802815F4, 0x80279380, 0, 0), 0x2C030000);
  // SME_PATCH_BL(SME_PORT_REGION(0x80207430, 0x801ff314, 0, 0), Patch::Mario::canCarryNPC);
  // SME_WRITE_32(SME_PORT_REGION(0x80207434, 0x801ff318, 0, 0), 0x2C030000);

  // // multiplayer.cpp
  // // SME_PATCH_B(SME_PORT_REGION(0x802EFAB4, 0, 0, 0),
  // // Patch::Multiplayer::draw3DOverhaul);
  // SME_PATCH_BL(SME_PORT_REGION(0x8029D7E8, 0x802956c4, 0, 0),
  //              Patch::Multiplayer::makeMarios);
  // // SME_PATCH_B(SME_PORT_REGION(0x80276BD0, 0, 0, 0),
  // // Patch::Multiplayer::loadMarioTrickyOverhaul);
  // // SME_PATCH_B(SME_PORT_REGION(0x8024D2A8, 0, 0, 0),
  // // Patch::Multiplayer::performMarioTrickyOverhaul);
  // // SME_PATCH_BL(SME_PORT_REGION(0x802983F8, 0, 0, 0),
  // // Patch::Multiplayer::setMarioOverhaul);
  // // SME_PATCH_BL(SME_PORT_REGION(0x80298428, 0, 0, 0),
  // // Patch::Multiplayer::setMarioOverhaul);
  // // SME_PATCH_BL(SME_PORT_REGION(0x802984D8, 0, 0, 0),
  // // Patch::Multiplayer::setMarioOverhaul);

  // music.cpp
  // SME_PATCH_BL(SME_PORT_REGION(0x80016998, 0x800169f4, 0, 0), Patch::Music::setIsValid);
  // SME_PATCH_B(SME_PORT_REGION(0x80016ABC, 0x80016b18, 0, 0), Patch::Music::initMusic);
  // SME_PATCH_B(SME_PORT_REGION(0x80016948, 0x800169a4, 0, 0),
  //             Patch::Music::stopMusicOnStop);
  // SME_PATCH_BL(SME_PORT_REGION(0x802A670C, 0x8029e664, 0, 0),
  //              Patch::Music::stopMusicOnStageExit);

  // // shine.cpp
  // SME_PATCH_BL(SME_PORT_REGION(0x801BD664, 0x801b551c, 0, 0),
  //              Patch::Shine::manageShineVanish);
  // SME_WRITE_32(SME_PORT_REGION(0x801BD668, 0x801b5520, 0, 0), 0x48000568);
  // SME_PATCH_BL(SME_PORT_REGION(0x802413E0, 0x8023916c, 0, 0),
  //              Patch::Shine::isKillEnemiesShine);
  // SME_PATCH_BL(SME_PORT_REGION(0x802995BC, 0x80291454, 0, 0),
  //              Patch::Shine::checkBootOut);
  // SME_WRITE_32(SME_PORT_REGION(0x80297BE8, 0x8028fa80, 0, 0), 0x60848200);
  // SME_PATCH_BL(SME_PORT_REGION(0x80293B10, 0x8028b928, 0, 0),
  //              Patch::Shine::extendShineIDLogic);
  // SME_PATCH_BL(SME_PORT_REGION(0x801BCC98, 0x801b4b50, 0, 0),
  //              Patch::Shine::shineObjectStringMod);
  // SME_WRITE_32(SME_PORT_REGION(0x803DEE50, 0x803d6630, 0, 0),
  //              Patch::Shine::shineFlagSetter);
  // SME_WRITE_32(SME_PORT_REGION(0x803DEE7C, 0x803d665c, 0, 0),
  //              Patch::Shine::shineFlagGetter);
  // SME_PATCH_BL(SME_PORT_REGION(0x802946D4, 0x8028c4ec, 0, 0),
  //              Patch::Shine::shineSetClamper);
  // SME_WRITE_32(SME_PORT_REGION(0x80294744, 0x8028c55c, 0, 0), 0x60000000);
  // SME_PATCH_BL(SME_PORT_REGION(0x8029474C, 0x8028c564, 0, 0),
  //              Patch::Shine::shineGetClamper);
  // /*Shine casts, fix light*/
  // SME_WRITE_32(SME_PORT_REGION(0x80412548, 0x80409a90, 0, 0), f32(SME_MAX_SHINES));
  // SME_WRITE_32(SME_PORT_REGION(0x80293AF8, 0x8028b910, 0, 0), 0x3BFF03E7);
  // SME_WRITE_32(SME_PORT_REGION(0x802946B8, 0x8028c4d0, 0, 0), 0x280003E7);
  // SME_WRITE_32(SME_PORT_REGION(0x8017BE78, 0x80171ea8, 0, 0), 0x5464037E);
  // SME_WRITE_32(SME_PORT_REGION(0x8017BEF4, 0x80171f24, 0, 0), 0x5464037E);
  // SME_WRITE_32(SME_PORT_REGION(0x8017BF34, 0x80171f6c, 0, 0), 0x5464037E);
  // SME_WRITE_32(SME_PORT_REGION(0x801BCE30, 0x801b4ce8, 0, 0), 0x5464037E);
  // SME_WRITE_32(SME_PORT_REGION(0x801FF850, 0x801f7734, 0, 0), 0x5404037E);
  // SME_WRITE_32(SME_PORT_REGION(0x802946B4, 0x8028c4cc, 0, 0), 0x5480043E);
  // SME_WRITE_32(SME_PORT_REGION(0x80294730, 0x8028c548, 0, 0), 0x5480043E);
  // SME_WRITE_32(SME_PORT_REGION(0x80294734, 0x8028c54c, 0, 0), 0x280003E7);
  // SME_WRITE_32(SME_PORT_REGION(0x80297BA0, 0x8028fa38, 0, 0), 0x5404037E);
  // SME_PATCH_BL(SME_PORT_REGION(0x80294334, 0x8028c14c, 0, 0),
  //              Patch::Shine::extendFlagManagerLoad);
  // SME_WRITE_32(SME_PORT_REGION(0x80294338, 0x8028c150, 0, 0), 0x48000010);
  // SME_PATCH_BL(SME_PORT_REGION(0x802939B8, 0x8028b7d0, 0, 0),
  //              Patch::Shine::extendFlagManagerSave);
  // SME_WRITE_32(SME_PORT_REGION(0x802939BC, 0x8028b7d4, 0, 0), 0x48000014);
  // SME_PATCH_BL(SME_PORT_REGION(0x80297BD8, 0x8028fa70, 0, 0),
  //              Patch::Shine::thinkSetBootFlag);
  // SME_PATCH_BL(SME_PORT_REGION(0x801BCD20, 0x801b4bd8, 0, 0),
  //              Patch::Shine::loadAfterMaskState);
  // SME_WRITE_32(SME_PORT_REGION(0x801BCD24, 0x801b4bdc, 0, 0), 0x28030002);
  // SME_WRITE_32(SME_PORT_REGION(0x801BCD40, 0, 0, 0), 0x28030001);
  // SME_PATCH_BL(SME_PORT_REGION(0x801BCEEC, 0x801b4da4, 0, 0),
  //              Patch::Shine::setKillState);
  // SME_PATCH_BL(SME_PORT_REGION(0x8029A590, 0x80292460, 0, 0), shine_thinkCloseCamera);
  // SME_WRITE_32(SME_PORT_REGION(0x8029A594, 0x80292464, 0, 0), 0x28000000);
  // SME_PATCH_BL(SME_PORT_REGION(0x802999D8, 0x80291870, 0, 0),
  //              shine_animationFreezeCheck);
  // SME_WRITE_32(SME_PORT_REGION(0x802999DC, 0x80291874, 0, 0), 0x48000034);
  // // Make Shines glow more
  // SME_WRITE_32(SME_PORT_REGION(0x803C9190, 0x803c0980, 0, 0), 0x3F19999A);

  // // stage.cpp
  // SME_PATCH_BL(SME_PORT_REGION(0x80299230, 0x802910c8, 0, 0),
  //              Patch::Stage::setStageOnExit);
  // SME_PATCH_BL(SME_PORT_REGION(0x80175F58, 0x8016befc, 0, 0),
  //              Patch::Stage::startEpisodeSelect);

  // // sunscript.cpp
  // SME_PATCH_BL(SME_PORT_REGION(0x80219380, 0x802112d4, 0, 0),
  //              Patch::Spc::initCustomFunctions);
  // SME_PATCH_BL(SME_PORT_REGION(0x80289920, 0x802816ac, 0, 0),
  //              Patch::Spc::initCustomFunctions);

  // // yoshi.cpp
  // SME_WRITE_32(SME_PORT_REGION(0x8026E068, 0x80265df4, 0, 0),
  //              0x2C000001); // Turn green when out of juice
  // SME_WRITE_32(SME_PORT_REGION(0x8026E0A0, 0x80265e28, 0, 0),
  //              0x60000000); // No flickering
  // SME_WRITE_32(SME_PORT_REGION(0x8026EE14, 0x80266ba0, 0, 0),
  //              0x4800020C); // Yoshi can't starve
  // SME_PATCH_BL(SME_PORT_REGION(0x8026EB00, 0x80266bdc, 0, 0), Patch::Yoshi::isYoshiDie);
  // SME_WRITE_32(SME_PORT_REGION(0x8026EB04, 0x80266be0, 0, 0), 0x2C030000);
  // SME_WRITE_32(SME_PORT_REGION(0x8026EB08, 0x80266be4, 0, 0), 0x41820518);

  // SME_PATCH_BL(SME_PORT_REGION(0x8026EBFC, 0x80266988, 0, 0), Patch::Yoshi::isYoshiDie);
  // SME_WRITE_32(SME_PORT_REGION(0x8026EC00, 0x8026698c, 0, 0), 0x2C030000);
  // SME_WRITE_32(SME_PORT_REGION(0x8026EC04, 0x80266990, 0, 0), 0x4182041C);

  // SME_PATCH_BL(SME_PORT_REGION(0x8026F218, 0x80266fa4, 0, 0), Patch::Yoshi::isYoshiDie);
  // SME_WRITE_32(SME_PORT_REGION(0x8026F21C, 0x80266fa8, 0, 0), 0x2C030000);
  // SME_WRITE_32(SME_PORT_REGION(0x8026F220, 0x80266fac, 0, 0), 0x41820164);
  // SME_PATCH_BL(SME_PORT_REGION(0x801BC868, 0x801b4720, 0, 0),
  //              Patch::Yoshi::isYoshiEggNeedFruit);
  // SME_PATCH_BL(SME_PORT_REGION(0x801BC8B4, 0x801b476c, 0, 0),
  //              Patch::Yoshi::isYoshiEggFree);
  // SME_WRITE_32(SME_PORT_REGION(0x801BC8B8, 0x801b4770, 0, 0), 0xA01E00FC);
  // SME_WRITE_32(SME_PORT_REGION(0x801BC8BC, 0x801b4774, 0, 0), 0x2C00000B);
  // SME_WRITE_32(SME_PORT_REGION(0x801BC8C0, 0x801b4778, 0, 0), 0x418200E4);
  // SME_WRITE_32(SME_PORT_REGION(0x801BC8C4, 0x801b477c, 0, 0), 0x2C030001);
  // SME_WRITE_32(SME_PORT_REGION(0x801BC8C8, 0x801b4780, 0, 0), 0x4182003C);
  // SME_WRITE_32(SME_PORT_REGION(0x801BC8CC, 0x801b4784, 0, 0), 0x418100D8);
  // SME_WRITE_32(SME_PORT_REGION(0x801BC8D0, 0x801b4788, 0, 0), 0x48000134);
  // SME_PATCH_BL(SME_PORT_REGION(0x8024D68C, 0x80245418, 0, 0),
  //              Patch::Yoshi::isYoshiMaintainFluddModel);
  // SME_WRITE_32(SME_PORT_REGION(0x8024D690, 0x8024541c, 0, 0), 0x2C030000);
  // SME_PATCH_BL(SME_PORT_REGION(0x8024F240, 0x80246fcc, 0, 0),
  //              Patch::Yoshi::maybeYoshiDrown);
  // SME_PATCH_BL(SME_PORT_REGION(0x802810F8, 0x80278e84, 0, 0),
  //              Patch::Yoshi::canMountYoshi);
  // SME_WRITE_32(SME_PORT_REGION(0x802810FC, 0x80278e88, 0, 0), 0x2C030000);
  // SME_WRITE_32(SME_PORT_REGION(0x80281100, 0x80278e8c, 0, 0), 0x807F03F0);
  // SME_WRITE_32(SME_PORT_REGION(0x80281104, 0x80278e90, 0, 0), 0x38830020);
  // SME_WRITE_32(SME_PORT_REGION(0x80281110, 0x80278e9c, 0, 0), 0x60000000);
  // SME_PATCH_BL(SME_PORT_REGION(0x80281148, 0x80278ed4, 0, 0),
  //              Patch::Yoshi::getYoshiYPos_);
  // SME_PATCH_BL(SME_PORT_REGION(0x8026E810, 0x8026659c, 0, 0),
  //              Patch::Yoshi::fixYoshiJuiceDecrement);
  // SME_PATCH_BL(SME_PORT_REGION(0x8024E58C, 0x80246318, 0, 0),
  //              Patch::Yoshi::canYoshiSpray);
  // SME_PATCH_BL(SME_PORT_REGION(0x80273198, 0x8026af24, 0, 0),
  //              Patch::Yoshi::calcYoshiSwimVelocity);
  // SME_PATCH_BL(SME_PORT_REGION(0x80270078, 0x80267e04, 0, 0),
  //              Patch::Yoshi::isYoshiWaterFlutter);
  // SME_WRITE_32(SME_PORT_REGION(0x8027007C, 0x80267e08, 0, 0), 0x7C7E1B78);
  // SME_WRITE_32(SME_PORT_REGION(0x80270080, 0x80267e0c, 0, 0), 0x60000000);
  // SME_WRITE_32(SME_PORT_REGION(0x80270084, 0x80267e10, 0, 0), 0x60000000);
  // SME_PATCH_BL(SME_PORT_REGION(0x8024E788, 0x80246514, 0, 0),
  //              Patch::Yoshi::isYoshiValidDrip);
  // SME_PATCH_B(SME_PORT_REGION(0x801BC128, 0x801b3fe0, 0, 0),
  //             Patch::Yoshi::initFreeEggCard);
  // SME_PATCH_BL(SME_PORT_REGION(0x801BC380, 0x801b4238, 0, 0),
  //              Patch::Yoshi::checkFreeEggCard);
  // SME_PATCH_BL(SME_PORT_REGION(0x8028121C, 0x80278fa8, 0, 0), Patch::Yoshi::saveNozzles);
  // SME_PATCH_BL(SME_PORT_REGION(0x8024EC18, 0x802469a4, 0, 0),
  //              Patch::Yoshi::restoreNozzles);
  // SME_WRITE_32(SME_PORT_REGION(0x8024EC2C, 0x802469b8, 0, 0), 0x60000000);

  // // utilities/card.cpp
  // SME_PATCH_BL(SME_PORT_REGION(0x802B20F8, 0x802aa008, 0, 0), Patch::Card::mountCard);
  // SME_PATCH_BL(SME_PORT_REGION(0x80163C40, 0x80158bf0, 0, 0), Patch::Card::probeCard);
  // SME_PATCH_BL(SME_PORT_REGION(0x8015B2C0, 0x8015030c, 0, 0), Util::formatBMGRaw);
  // SME_PATCH_BL(SME_PORT_REGION(0x8015B2F8, 0x80150344, 0, 0), Util::formatBMGRaw);
  // SME_PATCH_BL(SME_PORT_REGION(0x8015B5BC, 0x80150608, 0, 0), Util::formatBMGRaw);
  // SME_PATCH_BL(SME_PORT_REGION(0x8015B5E4, 0x80150630, 0, 0), Util::formatBMGRaw);
  // SME_PATCH_BL(SME_PORT_REGION(0x8015B638, 0x80150684, 0, 0), Util::formatBMGRaw);
  // SME_PATCH_BL(SME_PORT_REGION(0x8015B660, 0x801506ac, 0, 0), Util::formatBMGRaw);
  // SME_PATCH_BL(SME_PORT_REGION(0x8015BCB8, 0x80150d04, 0, 0), Util::formatBMGRaw);
  // SME_PATCH_BL(SME_PORT_REGION(0x8015BCE0, 0x80150d2c, 0, 0), Util::formatBMGRaw);
  // SME_PATCH_BL(SME_PORT_REGION(0x8015BE24, 0x80150e70, 0, 0), Util::formatBMGRaw);
  // SME_PATCH_BL(SME_PORT_REGION(0x8015BE4C, 0x80150e98, 0, 0), Util::formatBMGRaw);
  // SME_PATCH_BL(SME_PORT_REGION(0x8015BE9C, 0x80150ee8, 0, 0), Util::formatBMGRaw);
  // SME_PATCH_BL(SME_PORT_REGION(0x8015BEC4, 0x80150f10, 0, 0), Util::formatBMGRaw);
  // SME_PATCH_BL(SME_PORT_REGION(0x8015C508, 0x80151554, 0, 0), Util::formatBMGRaw);
  // SME_PATCH_BL(SME_PORT_REGION(0x8015D194, 0x801521e0, 0, 0), Util::formatBMGRaw);
  // SME_PATCH_BL(SME_PORT_REGION(0x8015D1CC, 0x80152218, 0, 0), Util::formatBMGRaw);
  // SME_PATCH_BL(SME_PORT_REGION(0x8015DEFC, 0x80152f48, 0, 0), Util::formatBMGRaw);
  // SME_PATCH_BL(SME_PORT_REGION(0x8015DF34, 0x80152f80, 0, 0), Util::formatBMGRaw);
  // SME_PATCH_BL(SME_PORT_REGION(0x8015E34C, 0x80153398, 0, 0), Util::formatBMGRaw);
  // SME_PATCH_BL(SME_PORT_REGION(0x8015E374, 0x801533c0, 0, 0), Util::formatBMGRaw);
  // SME_PATCH_BL(SME_PORT_REGION(0x8015E3C8, 0x80153414, 0, 0), Util::formatBMGRaw);
  // SME_PATCH_BL(SME_PORT_REGION(0x8015E3F0, 0x8015343c, 0, 0), Util::formatBMGRaw);
  // SME_PATCH_BL(SME_PORT_REGION(0x8015F970, 0x801549bc, 0, 0), Util::formatBMGRaw);
  // SME_PATCH_BL(SME_PORT_REGION(0x8015F9A8, 0x801549f4, 0, 0), Util::formatBMGRaw);
  // SME_PATCH_BL(SME_PORT_REGION(0x80161320, 0x8015636c, 0, 0), Util::formatBMGRaw);
  // SME_PATCH_BL(SME_PORT_REGION(0x80161358, 0x801563a4, 0, 0), Util::formatBMGRaw);
  // SME_PATCH_BL(SME_PORT_REGION(0x8016889C, 0x8015d954, 0, 0), Util::formatBMGRaw);
  // SME_PATCH_BL(SME_PORT_REGION(0x801688D4, 0x8015d98c, 0, 0), Util::formatBMGRaw);
  // SME_PATCH_BL(SME_PORT_REGION(0x80169224, 0x8015e31c, 0, 0), Util::formatBMGRaw);
  // SME_PATCH_BL(SME_PORT_REGION(0x8016925C, 0x8015e354, 0, 0), Util::formatBMGRaw);
  // SME_PATCH_BL(SME_PORT_REGION(0x8016950C, 0x8015e604, 0, 0), Util::formatBMGRaw);
  // SME_PATCH_BL(SME_PORT_REGION(0x80169534, 0x8015e62c, 0, 0), Util::formatBMGRaw);
  // SME_PATCH_BL(SME_PORT_REGION(0x8016958C, 0x8015e684, 0, 0), Util::formatBMGRaw);
  // SME_PATCH_BL(SME_PORT_REGION(0x801695B4, 0x8015e6ac, 0, 0), Util::formatBMGRaw);
  // SME_PATCH_BL(SME_PORT_REGION(0x80169ACC, 0x8015ebc4, 0, 0), Util::formatBMGRaw);
  // SME_PATCH_BL(SME_PORT_REGION(0x80169B04, 0x8015ebfc, 0, 0), Util::formatBMGRaw);
  // SME_PATCH_BL(SME_PORT_REGION(0x8016A02C, 0x8015f164, 0, 0), Util::formatBMGRaw);
  // SME_PATCH_BL(SME_PORT_REGION(0x8016A064, 0x8015f19c, 0, 0), Util::formatBMGRaw);
  // SME_PATCH_BL(SME_PORT_REGION(0x8016A3D0, 0x8015f508, 0, 0), Util::formatBMGRaw);
  // SME_PATCH_BL(SME_PORT_REGION(0x8016A3F8, 0x8015f530, 0, 0), Util::formatBMGRaw);
  // SME_PATCH_BL(SME_PORT_REGION(0x8016A44C, 0x8015f584, 0, 0), Util::formatBMGRaw);
  // SME_PATCH_BL(SME_PORT_REGION(0x8016A474, 0x8015f5ac, 0, 0), Util::formatBMGRaw);
  // SME_PATCH_BL(SME_PORT_REGION(0x8016B19C, 0x801602d4, 0, 0), Util::formatBMGRaw);
  // SME_PATCH_BL(SME_PORT_REGION(0x8016B1D4, 0x8016030c, 0, 0), Util::formatBMGRaw);

  /* -- PATCHES -- */

  // // Restore Chao Seed
  // SME_WRITE_32(SME_PORT_REGION(0x802FD1A0, 0x802f5330, 0, 0), 0x808D8C70);

  // // Show Exception Handler
  // SME_WRITE_32(SME_PORT_REGION(0x8029D0BC, 0x80294f98, 0, 0), 0x60000000);

  // // Extend Exception Handler
  // SME_WRITE_32(SME_PORT_REGION(0x802C7638, 0x802bf6cc, 0, 0), 0x60000000);
  // //SME_WRITE_32(SME_PORT_REGION(0x802C7690, 0, 0, 0), 0x60000000);

// #ifdef SME_DEBUG
//   // Skip FMVs
//   SME_WRITE_32(SME_PORT_REGION(0x802B5E8C, 0, 0, 0), 0x38600001);
//   SME_WRITE_32(SME_PORT_REGION(0x802B5EF4, 0, 0, 0), 0x38600001);
// #endif

// #ifdef SME_DEBUG
//   // Level Select
//   SME_WRITE_32(SME_PORT_REGION(0x802A6788, 0, 0, 0), 0x3BC00009);
// #endif

//   // Fix Infinte Flutter
//   #if defined(PAL)
//   kWrite32(SME_PORT_REGION(0x80278ec8, 0x80278ec8, 0, 0), 0xC002F69C);
//   #else
//   kWrite32(SME_PORT_REGION(0x8028113C, 0, 0, 0), 0xC002F824);
//   #endif

  // // Remove Dive While Wall Sliding
  // kWrite32(SME_PORT_REGION(0x8024BC10, 0x8024399c, 0, 0), 0x48000068);

  // // Flood Till Corona Beat
  // kWrite32(SME_PORT_REGION(0x8029961C, 0x802914b4, 0, 0), 0x38840077);

  // // Map on D Pad down
  // kWrite32(SME_PORT_REGION(0x8017A830, 0x801706f4, 0, 0), 0x5400077B);
  // kWrite32(SME_PORT_REGION(0x80297A60, 0x8028f8f8, 0, 0), 0x5400077B);

  // // Hover move on roofs
  // kWrite32(SME_PORT_REGION(0x802569BC, 0x8024e748, 0, 0), 0xC02300B0);

  // Global surfing bloopies
  // kWrite32(SME_PORT_REGION(0x801B74F8, 0x801af3b0, 0, 0), 0x60000000);
  // kWrite32(SME_PORT_REGION(0x801B74FC, 0x801af3b4, 0, 0), 0x60000000);
  // kWrite32(SME_PORT_REGION(0x801B7500, 0x801af3b8, 0, 0), 0x60000000);
  // kWrite32(SME_PORT_REGION(0x801B7504, 0x801af3bc, 0, 0), 0x60000000);
  // kWrite32(SME_PORT_REGION(0x801B7508, 0x801af3c0, 0, 0), 0x60000000);
  // kWrite32(SME_PORT_REGION(0x801B750C, 0x801af3c4, 0, 0), 0x60000000);
  // kWrite32(SME_PORT_REGION(0x801B7510, 0x801af3c8, 0, 0), 0x387E0780);
  // kWrite32(SME_PORT_REGION(0x801B7514, 0x801af3cc, 0, 0), 0x4810BA9D);
  // kWrite32(SME_PORT_REGION(0x801B7518, 0x801af3d0, 0, 0), 0x28030000);
  // kWrite32(SME_PORT_REGION(0x801B751C, 0x801af3d4, 0, 0), 0x418200A4);

  // // Sunscript logging restoration
  // SME_WRITE_32(SME_PORT_REGION(0x8003DB3C, 0x8003d98c, 0, 0), 0x48306B08);

#if defined(SME_BUILD_KURIBO) && !defined(SME_BUILD_KAMEK) &&                  \
    !defined(SME_BUILD_KAMEK_INLINE)
  Patch::Init::initCodeProtection();
#endif

#endif

#if defined(SME_BUILD_KURIBO) && !defined(SME_BUILD_KAMEK) &&                  \
    !defined(SME_BUILD_KAMEK_INLINE)
}
KURIBO_MODULE_END();
#endif