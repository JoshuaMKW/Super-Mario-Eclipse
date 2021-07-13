#include "SME.hxx"
#include "OS.h"

#include "Kernel.hxx"
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

static void initMod() {
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
  Patch::Init::initCodeProtection();
}

static void destroyMod() {
  SME_DEBUG_LOG("-- Destroying Module --\n");
#ifdef SME_DEBUG
  OSStopStopwatch(&gctStopwatch);
#endif
  OSCancelAlarm(&gctAlarm);
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
SME_PATCH_BL(SME_PORT_REGION(0x802998B8, 0, 0, 0), Patch::CKit::onSetup);
SME_PATCH_BL(SME_PORT_REGION(0x802A616C, 0, 0, 0), Patch::CKit::onUpdate);
SME_PATCH_BL(SME_PORT_REGION(0x80143F14, 0, 0, 0), Patch::CKit::onDraw2D);
SME_PATCH_B(SME_PORT_REGION(0x802A8B58, 0, 0, 0), Util::SMS::isExMap);
SME_PATCH_B(SME_PORT_REGION(0x802A8B30, 0, 0, 0), Util::SMS::isMultiplayerMap);
SME_PATCH_B(SME_PORT_REGION(0x802A8AFC, 0, 0, 0), Util::SMS::isDivingMap);
SME_PATCH_B(SME_PORT_REGION(0x802A8AE0, 0, 0, 0), Util::SMS::isOptionMap);
SME_PATCH_BL(SME_PORT_REGION(0x8027C6A4, 0, 0, 0),
             Patch::CKit::manageLightSize);
SME_WRITE_32(SME_PORT_REGION(0x8027C6A8, 0, 0, 0), 0x28030001);
SME_PATCH_BL(SME_PORT_REGION(0x80153DE8, 0, 0, 0),
             Patch::CKit::formatTalkMessage);
SME_PATCH_BL(SME_PORT_REGION(0x80153E1C, 0, 0, 0),
             Patch::CKit::formatTalkMessage);
SME_PATCH_BL(SME_PORT_REGION(0x8024D3A8, 0, 0, 0),
             Patch::CKit::realTimeCustomAttrsHandler);
SME_PATCH_BL(SME_PORT_REGION(0x8003F8F0, 0, 0, 0),
             Patch::CKit::realTimeCustomAttrsHandler);

// camera.cpp
SME_PATCH_B(SME_PORT_REGION(0x80027548, 0, 0, 0),
            Patch::Camera::modifyCameraRangeToSize);

// CheatCode.cpp
SME_PATCH_B(SME_PORT_REGION(0x80295B6C, 0, 0, 0),
            Patch::Cheat::handleDebugCheat);

// collision.cpp
SME_PATCH_BL(SME_PORT_REGION(0x80258334, 0, 0, 0),
             Patch::Collision::checkIsGlideBounce);
SME_PATCH_BL(SME_PORT_REGION(0x80264CFC, 0, 0, 0),
             Patch::Collision::checkIsGlideBounce);
SME_PATCH_BL(SME_PORT_REGION(0x8024C558, 0, 0, 0),
             Patch::Collision::checkIsRestoreTypeNoFallDamage);
SME_PATCH_BL(SME_PORT_REGION(0x80250CA0, 0, 0, 0),
             Patch::Collision::masterGroundCollisionHandler);
SME_PATCH_BL(SME_PORT_REGION(0x8025059C, 0, 0, 0),
             Patch::Collision::masterAllCollisionHandler);

// debug.cpp
SME_PATCH_BL(SME_PORT_REGION(0x8024D194, 0, 0, 0),
             Patch::Debug::xyzModifierMario);
SME_WRITE_32(SME_PORT_REGION(0x8024D198, 0, 0, 0), 0x2C030000);

// file_flags.cpp
SME_PATCH_BL(SME_PORT_REGION(0x802B1794, 0, 0, 0),
             Patch::Flag::setFileCompleteBool);

// flagmanager.cpp
SME_PATCH_B(SME_PORT_REGION(0x80294EF4, 0, 0, 0), Patch::Flag::resetStage);

// fludd.cpp
SME_PATCH_B(SME_PORT_REGION(0x80248F14, 0, 0, 0), Patch::Fludd::isPumpOK);
SME_WRITE_32(SME_PORT_REGION(0x803DCA00, 0, 0, 0),
             0x00300000 | TMarioAnimeData::FLUDD::FLUDD_ENABLED);
SME_PATCH_BL(SME_PORT_REGION(0x8014206C, 0, 0, 0),
             Patch::Fludd::hasWaterCardOpen);
SME_WRITE_32(SME_PORT_REGION(0x80142070, 0, 0, 0), 0x28030000);
SME_PATCH_BL(SME_PORT_REGION(0x80283058, 0, 0, 0),
             Patch::Fludd::canCollectFluddItem);
SME_PATCH_BL(SME_PORT_REGION(0x800678C4, 0, 0, 0), Patch::Fludd::sprayGoopMap);
SME_PATCH_BL(SME_PORT_REGION(0x801A3ED0, 0, 0, 0), Patch::Fludd::sprayGoopMap);
SME_PATCH_BL(SME_PORT_REGION(0x801B42D8, 0, 0, 0), Patch::Fludd::sprayGoopMap);
SME_PATCH_BL(SME_PORT_REGION(0x8024E710, 0, 0, 0), Patch::Fludd::sprayGoopMap);
SME_PATCH_BL(SME_PORT_REGION(0x8027F7DC, 0, 0, 0), Patch::Fludd::sprayGoopMap);
SME_PATCH_BL(SME_PORT_REGION(0x8027F94C, 0, 0, 0), Patch::Fludd::sprayGoopMap);
SME_PATCH_BL(SME_PORT_REGION(0x800FED3C, 0, 0, 0), Patch::Fludd::canCleanSeals);
SME_WRITE_32(SME_PORT_REGION(0x800FED40, 0, 0, 0), 0x2C030000);
SME_PATCH_BL(SME_PORT_REGION(0x8024D53C, 0, 0, 0),
             Patch::Fludd::bindFluddtojoint);
SME_PATCH_BL(SME_PORT_REGION(0x8024E548, 0, 0, 0),
             Patch::Fludd::checkExecWaterGun);
SME_PATCH_BL(SME_PORT_REGION(0x8026C370, 0, 0, 0),
             Patch::Fludd::killTriggerNozzle);
SME_PATCH_BL(SME_PORT_REGION(0x8026C018, 0, 0, 0),
             Patch::Fludd::spamHoverWrapper);
SME_PATCH_BL(SME_PORT_REGION(0x80262580, 0, 0, 0),
             Patch::Fludd::checkAirNozzle);
SME_WRITE_32(SME_PORT_REGION(0x80262584, 0, 0, 0), 0x2C030000);

// fruit.cpp
SME_PATCH_BL(SME_PORT_REGION(0x801E542C, 0, 0, 0),
             Patch::Fruit::canFruitDieWater);
SME_WRITE_32(SME_PORT_REGION(0x801E5430, 0, 0, 0), 0x2C030000);
SME_WRITE_32(SME_PORT_REGION(0x801E5434, 0, 0, 0), 0x41820140);
SME_PATCH_BL(SME_PORT_REGION(0x8023F964, 0, 0, 0),
             Patch::Fruit::chooseGrabDistancing);
SME_PATCH_BL(SME_PORT_REGION(0x8023F9DC, 0, 0, 0),
             Patch::Fruit::isGrabWaitOver);

// init_mods.cpp
SME_PATCH_BL(SME_PORT_REGION(0x802A7140, 0, 0, 0),
             Patch::Init::setupMarioDatas);

SME_PATCH_BL(SME_PORT_REGION(0x802A750C, 0, 0, 0),
             Patch::Init::createGlobalHeaps);
SME_PATCH_BL(SME_PORT_REGION(0x802A716C, 0, 0, 0), Patch::Init::initFirstModel);
SME_PATCH_BL(SME_PORT_REGION(0x802998B4, 0, 0, 0), Patch::Init::initFileMods);
SME_PATCH_B(SME_PORT_REGION(0x80280180, 0, 0, 0), Patch::Init::initShineShadow);
SME_PATCH_BL(SME_PORT_REGION(0x802B7A4C, 0, 0, 0), Patch::Init::initSoundBank);
SME_PATCH_BL(SME_PORT_REGION(0x802983F0, 0, 0, 0), Patch::Init::initMusicTrack);
SME_PATCH_BL(SME_PORT_REGION(0x80298420, 0, 0, 0), Patch::Init::initMusicTrack);
SME_WRITE_32(SME_PORT_REGION(0x80276C90, 0, 0, 0), 0x60000000);
SME_PATCH_BL(SME_PORT_REGION(0x80276C94, 0, 0, 0), Patch::Init::fromMarioInit);
SME_PATCH_BL(SME_PORT_REGION(0x800397DC, 0, 0, 0),
             Patch::Init::fromShadowMarioInit);
SME_PATCH_BL(SME_PORT_REGION(0x80271580, 0, 0, 0), Patch::Init::initYoshi);
SME_PATCH_B(SME_PORT_REGION(0x8029CCB0, 0, 0, 0), Patch::Init::initCardColors);
SME_PATCH_BL(SME_PORT_REGION(0x802B8B20, 0, 0, 0),
             Patch::Init::initCollisionWarpLinks);
SME_PATCH_BL(SME_PORT_REGION(0x802B57E4, 0, 0, 0), Patch::Init::createUIHeap);

// mario.cpp
SME_PATCH_B(SME_PORT_REGION(0x802558A4, 0, 0, 0), Patch::Mario::addVelocity);
SME_PATCH_BL(SME_PORT_REGION(0x802500B8, 0, 0, 0),
             Patch::Mario::updateContexts);
SME_PATCH_BL(SME_PORT_REGION(0x8029A87C, 0, 0, 0),
             Patch::Mario::carryOrTalkNPC);
SME_PATCH_BL(SME_PORT_REGION(0x802815F0, 0, 0, 0), Patch::Mario::canGrabAtNPC);
SME_WRITE_32(SME_PORT_REGION(0x802815F4, 0, 0, 0), 0x2C030000);
SME_PATCH_BL(SME_PORT_REGION(0x80207430, 0, 0, 0), Patch::Mario::canCarryNPC);
SME_WRITE_32(SME_PORT_REGION(0x80207434, 0, 0, 0), 0x2C030000);
SME_PATCH_BL(SME_PORT_REGION(0x80213314, 0, 0, 0), Patch::Mario::scaleNPCTalkRadius);
SME_PATCH_BL(SME_PORT_REGION(0x80261C3C, 0, 0, 0), Patch::Mario::getTreeClimbMinFall);
SME_WRITE_32(SME_PORT_REGION(0x80261C40, 0, 0, 0), 0xC05F038C);
SME_WRITE_32(SME_PORT_REGION(0x80261C44, 0, 0, 0), 0xFC020040);
SME_PATCH_BL(SME_PORT_REGION(0x802619CC, 0, 0, 0), Patch::Mario::getTreeClimbMaxFall);
SME_WRITE_32(SME_PORT_REGION(0x802619D0, 0, 0, 0), 0x60000000);
#if 0
  SME_PATCH_BL(SME_PORT_REGION(0x802145F0, 0, 0, 0),
               Patch::Mario::scaleNPCThrowLength);
  SME_WRITE_32(SME_PORT_REGION(0x802145F4, 0, 0, 0), 0xC002E5E0);
  SME_WRITE_32(SME_PORT_REGION(0x802145F8, 0, 0, 0), 0xC0230034);
  SME_WRITE_32(SME_PORT_REGION(0x8021462C, 0, 0, 0), 0xEC0B0032);
  SME_WRITE_32(SME_PORT_REGION(0x80214634, 0, 0, 0), 0xEC2B0072);
  SME_PATCH_BL(SME_PORT_REGION(0x8021463C, 0, 0, 0),
               Patch::Mario::scaleNPCThrowHeight);
  SME_PATCH_BL(SME_PORT_REGION(0x80261CF4, 0, 0, 0), Patch::Mario::scaleTreeSlideSpeed);
  SME_WRITE_32(SME_PORT_REGION(0x80261CF8, 0, 0, 0), 0x2C030000);
  SME_WRITE_32(SME_PORT_REGION(0x80261CFC, 0, 0, 0), 0x41820070);
  SME_PATCH_BL(SME_PORT_REGION(0x8025D588, 0, 0, 0), Patch::Mario::getClimbingAnimSpd);
  SME_PATCH_BL(SME_PORT_REGION(0x8025D63C, 0, 0, 0), Patch::Mario::getClimbingAnimSpd);
  SME_PATCH_BL(SME_PORT_REGION(0x8025D650, 0, 0, 0), Patch::Mario::getClimbingAnimSpd);
  SME_PATCH_BL(SME_PORT_REGION(0x8025DBC4, 0, 0, 0), Patch::Mario::getClimbingAnimSpd);
  SME_PATCH_BL(SME_PORT_REGION(0x8025E38C, 0, 0, 0), Patch::Mario::getClimbingAnimSpd);
  SME_PATCH_BL(SME_PORT_REGION(0x802615AC, 0, 0, 0), Patch::Mario::scaleHangSpeed);
  SME_WRITE_32(SME_PORT_REGION(0x802615B0, 0, 0, 0), 0x60000000);
  SME_PATCH_BL(SME_PORT_REGION(0x8024E288, 0, 0, 0), Patch::Mario::checkGraffitiAffected);
  // SME_PATCH_BL(0x801E4118, Patch::Mario::rescaleHeldObj);
#endif
SME_PATCH_BL(SME_PORT_REGION(0x8024E02C, 0, 0, 0),
             Patch::Mario::manageExtraJumps);
SME_PATCH_BL(SME_PORT_REGION(0x80254534, 0, 0, 0),
             Patch::Mario::normJumpMultiplier);
SME_WRITE_32(SME_PORT_REGION(0x80254538, 0, 0, 0), 0x60000000);
SME_WRITE_32(SME_PORT_REGION(0x8025453C, 0, 0, 0), 0x60000000);
SME_WRITE_32(SME_PORT_REGION(0x80254540, 0, 0, 0), 0x60000000);
SME_WRITE_32(SME_PORT_REGION(0x80254544, 0, 0, 0), 0x60000000);
SME_PATCH_BL(SME_PORT_REGION(0x80256678, 0, 0, 0),
             Patch::Mario::checkYSpdForTerminalVelocity);
SME_WRITE_32(SME_PORT_REGION(0x8025667C, 0, 0, 0), 0x60000000);
SME_WRITE_32(0x8025B8BC, 0x60000000);
SME_PATCH_BL(0x8025B8C0, Patch::Mario::checkGroundSpeedLimit);
SME_WRITE_32(0x8025B8C4, 0xEFFF0072);
SME_WRITE_32(0x8024CC60, 0x60000000);
SME_WRITE_32(0x8024CC64, 0x60000000);
SME_WRITE_32(0x8024CC68, 0x60000000);
SME_PATCH_BL(0x8024CC6C, Patch::Mario::checkJumpSpeedLimit);
SME_PATCH_BL(0x8024CC2C, Patch::Mario::checkJumpSpeedMulti);
SME_WRITE_32(0x8024CC30, 0x57C5043E);

SME_PATCH_BL(SME_PORT_REGION(0x8003FDAC, 0, 0, 0),
             Patch::Mario::manageEMarioHealthWrapper);
SME_WRITE_32(SME_PORT_REGION(0x8003FD94, 0, 0, 0), 0x60000000);

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

// music.cpp
SME_PATCH_BL(SME_PORT_REGION(0x80016998, 0, 0, 0), Patch::Music::setIsValid);
SME_PATCH_B(SME_PORT_REGION(0x80016ABC, 0, 0, 0), Patch::Music::initMusic);
SME_PATCH_B(SME_PORT_REGION(0x80016948, 0, 0, 0),
            Patch::Music::stopMusicOnStop);
SME_PATCH_BL(SME_PORT_REGION(0x802A670C, 0, 0, 0),
             Patch::Music::stopMusicOnStageExit);

// patches.cpp
SME_PATCH_BL(SME_PORT_REGION(0x802320E0, 0, 0, 0),
             Patch::Fixes::shadowCrashPatch);
SME_PATCH_BL(SME_PORT_REGION(0x802571F0, 0, 0, 0), Patch::Fixes::patchYStorage);

// shine.cpp
SME_PATCH_BL(SME_PORT_REGION(0x801BD664, 0, 0, 0),
             Patch::Shine::manageShineVanish);
SME_WRITE_32(SME_PORT_REGION(0x801BD668, 0, 0, 0), 0x48000568);
SME_PATCH_BL(SME_PORT_REGION(0x802413E0, 0, 0, 0),
             Patch::Shine::isKillEnemiesShine);
SME_PATCH_BL(SME_PORT_REGION(0x802995BC, 0, 0, 0), Patch::Shine::checkBootOut);
SME_WRITE_32(SME_PORT_REGION(0x80297BE8, 0, 0, 0), 0x60848200);
SME_PATCH_BL(SME_PORT_REGION(0x80293B10, 0, 0, 0),
             Patch::Shine::extendShineIDLogic);
SME_PATCH_BL(SME_PORT_REGION(0x801BCC98, 0, 0, 0),
             Patch::Shine::shineObjectStringMod);
SME_WRITE_32(SME_PORT_REGION(0x803DEE50, 0, 0, 0),
             Patch::Shine::shineFlagSetter);
SME_WRITE_32(SME_PORT_REGION(0x803DEE7C, 0, 0, 0),
             Patch::Shine::shineFlagGetter);
SME_PATCH_BL(SME_PORT_REGION(0x802946D4, 0, 0, 0),
             Patch::Shine::shineSetClamper);
SME_WRITE_32(SME_PORT_REGION(0x80294744, 0, 0, 0), 0x60000000);
SME_PATCH_BL(SME_PORT_REGION(0x8029474C, 0, 0, 0),
             Patch::Shine::shineGetClamper);
/*Shine casts, fix light*/
SME_WRITE_32(SME_PORT_REGION(0x80412548, 0, 0, 0), f32(SME_MAX_SHINES));
SME_WRITE_32(SME_PORT_REGION(0x80293AF8, 0, 0, 0), 0x3BFF03E7);
SME_WRITE_32(SME_PORT_REGION(0x802946B8, 0, 0, 0), 0x280003E7);
SME_WRITE_32(SME_PORT_REGION(0x8017BE78, 0, 0, 0), 0x5464037E);
SME_WRITE_32(SME_PORT_REGION(0x8017BEF4, 0, 0, 0), 0x5464037E);
SME_WRITE_32(SME_PORT_REGION(0x8017BF34, 0, 0, 0), 0x5464037E);
SME_WRITE_32(SME_PORT_REGION(0x801BCE30, 0, 0, 0), 0x5464037E);
SME_WRITE_32(SME_PORT_REGION(0x801FF850, 0, 0, 0), 0x5404037E);
SME_WRITE_32(SME_PORT_REGION(0x802946B4, 0, 0, 0), 0x5480043E);
SME_WRITE_32(SME_PORT_REGION(0x80294730, 0, 0, 0), 0x5480043E);
SME_WRITE_32(SME_PORT_REGION(0x80294734, 0, 0, 0), 0x280003E7);
SME_WRITE_32(SME_PORT_REGION(0x80297BA0, 0, 0, 0), 0x5404037E);
SME_PATCH_BL(SME_PORT_REGION(0x80294334, 0, 0, 0),
             Patch::Shine::extendFlagManagerLoad);
SME_WRITE_32(SME_PORT_REGION(0x80294338, 0, 0, 0), 0x48000010);
SME_PATCH_BL(SME_PORT_REGION(0x802939B8, 0, 0, 0),
             Patch::Shine::extendFlagManagerSave);
SME_WRITE_32(SME_PORT_REGION(0x802939BC, 0, 0, 0), 0x48000014);
SME_PATCH_BL(SME_PORT_REGION(0x80297BD8, 0, 0, 0),
             Patch::Shine::thinkSetBootFlag);
SME_PATCH_BL(SME_PORT_REGION(0x801BCD20, 0, 0, 0),
             Patch::Shine::loadAfterMaskState);
SME_WRITE_32(SME_PORT_REGION(0x801BCD24, 0, 0, 0), 0x28030002);
SME_WRITE_32(SME_PORT_REGION(0x801BCD40, 0, 0, 0), 0x28030001);
SME_PATCH_BL(SME_PORT_REGION(0x801BCEEC, 0, 0, 0), Patch::Shine::setKillState);
SME_PATCH_BL(SME_PORT_REGION(0x8029A590, 0, 0, 0),
             Patch::Shine::thinkCloseCamera);
SME_WRITE_32(SME_PORT_REGION(0x8029A594, 0, 0, 0), 0x28000000);
SME_PATCH_BL(SME_PORT_REGION(0x802999D8, 0, 0, 0),
             Patch::Shine::animationFreezeCheck);
SME_WRITE_32(SME_PORT_REGION(0x802999DC, 0, 0, 0), 0x48000034);
// Make Shines glow more
SME_WRITE_32(SME_PORT_REGION(0x803C9190, 0, 0, 0), 0x3F19999A);

// stage.cpp
SME_PATCH_BL(SME_PORT_REGION(0x80299230, 0, 0, 0),
             Patch::Stage::setStageOnExit);
SME_PATCH_BL(SME_PORT_REGION(0x80175F58, 0, 0, 0),
             Patch::Stage::startEpisodeSelect);

// sunscript.cpp
SME_PATCH_BL(SME_PORT_REGION(0x80219380, 0, 0, 0),
             Patch::Spc::initCustomFunctions);
SME_PATCH_BL(SME_PORT_REGION(0x80289920, 0, 0, 0),
             Patch::Spc::initCustomFunctions);

// yoshi.cpp
SME_WRITE_32(SME_PORT_REGION(0x8026E068, 0, 0, 0),
             0x2C000001); // Turn green when out of juice
SME_WRITE_32(SME_PORT_REGION(0x8026E0A0, 0, 0, 0),
             0x60000000); // No flickering
SME_WRITE_32(SME_PORT_REGION(0x8026EE14, 0, 0, 0),
             0x4800020C); // Yoshi can't starve
SME_PATCH_BL(SME_PORT_REGION(0x8026EB00, 0, 0, 0), Patch::Yoshi::isYoshiDie);
SME_WRITE_32(SME_PORT_REGION(0x8026EB04, 0, 0, 0), 0x2C030000);
SME_WRITE_32(SME_PORT_REGION(0x8026EB08, 0, 0, 0), 0x41820518);

SME_PATCH_BL(SME_PORT_REGION(0x8026EBFC, 0, 0, 0), Patch::Yoshi::isYoshiDie);
SME_WRITE_32(SME_PORT_REGION(0x8026EC00, 0, 0, 0), 0x2C030000);
SME_WRITE_32(SME_PORT_REGION(0x8026EC04, 0, 0, 0), 0x4182041C);

SME_PATCH_BL(SME_PORT_REGION(0x8026F218, 0, 0, 0), Patch::Yoshi::isYoshiDie);
SME_WRITE_32(SME_PORT_REGION(0x8026F21C, 0, 0, 0), 0x2C030000);
SME_WRITE_32(SME_PORT_REGION(0x8026F220, 0, 0, 0), 0x41820164);
SME_PATCH_BL(SME_PORT_REGION(0x801BC868, 0, 0, 0),
             Patch::Yoshi::isYoshiEggNeedFruit);
SME_PATCH_BL(SME_PORT_REGION(0x801BC8B4, 0, 0, 0),
             Patch::Yoshi::isYoshiEggFree);
SME_WRITE_32(SME_PORT_REGION(0x801BC8B8, 0, 0, 0), 0xA01E00FC);
SME_WRITE_32(SME_PORT_REGION(0x801BC8BC, 0, 0, 0), 0x2C00000B);
SME_WRITE_32(SME_PORT_REGION(0x801BC8C0, 0, 0, 0), 0x418200E4);
SME_WRITE_32(SME_PORT_REGION(0x801BC8C4, 0, 0, 0), 0x2C030001);
SME_WRITE_32(SME_PORT_REGION(0x801BC8C8, 0, 0, 0), 0x4182003C);
SME_WRITE_32(SME_PORT_REGION(0x801BC8CC, 0, 0, 0), 0x418100D8);
SME_WRITE_32(SME_PORT_REGION(0x801BC8D0, 0, 0, 0), 0x48000134);
SME_PATCH_BL(SME_PORT_REGION(0x8024D68C, 0, 0, 0),
             Patch::Yoshi::isYoshiMaintainFluddModel);
SME_WRITE_32(SME_PORT_REGION(0x8024D690, 0, 0, 0), 0x2C030000);
SME_PATCH_BL(SME_PORT_REGION(0x8024F240, 0, 0, 0),
             Patch::Yoshi::maybeYoshiDrown);
SME_PATCH_BL(SME_PORT_REGION(0x802810F8, 0, 0, 0), Patch::Yoshi::canMountYoshi);
SME_WRITE_32(SME_PORT_REGION(0x802810FC, 0, 0, 0), 0x2C030000);
SME_WRITE_32(SME_PORT_REGION(0x80281100, 0, 0, 0), 0x807F03F0);
SME_WRITE_32(SME_PORT_REGION(0x80281104, 0, 0, 0), 0x38830020);
SME_WRITE_32(SME_PORT_REGION(0x80281110, 0, 0, 0), 0x60000000);
SME_PATCH_BL(SME_PORT_REGION(0x80281148, 0, 0, 0), Patch::Yoshi::getYoshiYPos_);
SME_PATCH_BL(SME_PORT_REGION(0x8026E810, 0, 0, 0),
             Patch::Yoshi::fixYoshiJuiceDecrement);
SME_PATCH_BL(SME_PORT_REGION(0x8024E58C, 0, 0, 0), Patch::Yoshi::canYoshiSpray);
SME_PATCH_BL(SME_PORT_REGION(0x80273198, 0, 0, 0),
             Patch::Yoshi::calcYoshiSwimVelocity);
SME_PATCH_BL(SME_PORT_REGION(0x80270078, 0, 0, 0),
             Patch::Yoshi::isYoshiWaterFlutter);
SME_WRITE_32(SME_PORT_REGION(0x8027007C, 0, 0, 0), 0x7C7E1B78);
SME_WRITE_32(SME_PORT_REGION(0x80270080, 0, 0, 0), 0x60000000);
SME_WRITE_32(SME_PORT_REGION(0x80270084, 0, 0, 0), 0x60000000);
SME_PATCH_BL(SME_PORT_REGION(0x8024E788, 0, 0, 0),
             Patch::Yoshi::isYoshiValidDrip);
SME_PATCH_B(SME_PORT_REGION(0x801BC128, 0, 0, 0),
            Patch::Yoshi::initFreeEggCard);
SME_PATCH_BL(SME_PORT_REGION(0x801BC380, 0, 0, 0),
             Patch::Yoshi::checkFreeEggCard);
SME_PATCH_BL(SME_PORT_REGION(0x8028121C, 0, 0, 0), Patch::Yoshi::saveNozzles);
SME_PATCH_BL(SME_PORT_REGION(0x8024EC18, 0, 0, 0),
             Patch::Yoshi::restoreNozzles);
SME_WRITE_32(SME_PORT_REGION(0x8024EC2C, 0, 0, 0), 0x60000000);

// utilities/card.cpp
SME_PATCH_BL(SME_PORT_REGION(0x802B20F8, 0, 0, 0), Patch::Card::mountCard);
SME_PATCH_BL(SME_PORT_REGION(0x80163C40, 0, 0, 0), Patch::Card::probeCard);
SME_PATCH_BL(SME_PORT_REGION(0x8015B2C0, 0, 0, 0), Util::formatBMGRaw);
SME_PATCH_BL(SME_PORT_REGION(0x8015B2F8, 0, 0, 0), Util::formatBMGRaw);
SME_PATCH_BL(SME_PORT_REGION(0x8015B5BC, 0, 0, 0), Util::formatBMGRaw);
SME_PATCH_BL(SME_PORT_REGION(0x8015B5E4, 0, 0, 0), Util::formatBMGRaw);
SME_PATCH_BL(SME_PORT_REGION(0x8015B638, 0, 0, 0), Util::formatBMGRaw);
SME_PATCH_BL(SME_PORT_REGION(0x8015B660, 0, 0, 0), Util::formatBMGRaw);
SME_PATCH_BL(SME_PORT_REGION(0x8015BCB8, 0, 0, 0), Util::formatBMGRaw);
SME_PATCH_BL(SME_PORT_REGION(0x8015BCE0, 0, 0, 0), Util::formatBMGRaw);
SME_PATCH_BL(SME_PORT_REGION(0x8015BE24, 0, 0, 0), Util::formatBMGRaw);
SME_PATCH_BL(SME_PORT_REGION(0x8015BE4C, 0, 0, 0), Util::formatBMGRaw);
SME_PATCH_BL(SME_PORT_REGION(0x8015BE9C, 0, 0, 0), Util::formatBMGRaw);
SME_PATCH_BL(SME_PORT_REGION(0x8015BEC4, 0, 0, 0), Util::formatBMGRaw);
SME_PATCH_BL(SME_PORT_REGION(0x8015C508, 0, 0, 0), Util::formatBMGRaw);
SME_PATCH_BL(SME_PORT_REGION(0x8015D194, 0, 0, 0), Util::formatBMGRaw);
SME_PATCH_BL(SME_PORT_REGION(0x8015D1CC, 0, 0, 0), Util::formatBMGRaw);
SME_PATCH_BL(SME_PORT_REGION(0x8015DEFC, 0, 0, 0), Util::formatBMGRaw);
SME_PATCH_BL(SME_PORT_REGION(0x8015DF34, 0, 0, 0), Util::formatBMGRaw);
SME_PATCH_BL(SME_PORT_REGION(0x8015E34C, 0, 0, 0), Util::formatBMGRaw);
SME_PATCH_BL(SME_PORT_REGION(0x8015E374, 0, 0, 0), Util::formatBMGRaw);
SME_PATCH_BL(SME_PORT_REGION(0x8015E3C8, 0, 0, 0), Util::formatBMGRaw);
SME_PATCH_BL(SME_PORT_REGION(0x8015E3F0, 0, 0, 0), Util::formatBMGRaw);
SME_PATCH_BL(SME_PORT_REGION(0x8015F970, 0, 0, 0), Util::formatBMGRaw);
SME_PATCH_BL(SME_PORT_REGION(0x8015F9A8, 0, 0, 0), Util::formatBMGRaw);
SME_PATCH_BL(SME_PORT_REGION(0x80161320, 0, 0, 0), Util::formatBMGRaw);
SME_PATCH_BL(SME_PORT_REGION(0x80161358, 0, 0, 0), Util::formatBMGRaw);
SME_PATCH_BL(SME_PORT_REGION(0x8016889C, 0, 0, 0), Util::formatBMGRaw);
SME_PATCH_BL(SME_PORT_REGION(0x801688D4, 0, 0, 0), Util::formatBMGRaw);
SME_PATCH_BL(SME_PORT_REGION(0x80169224, 0, 0, 0), Util::formatBMGRaw);
SME_PATCH_BL(SME_PORT_REGION(0x8016925C, 0, 0, 0), Util::formatBMGRaw);
SME_PATCH_BL(SME_PORT_REGION(0x8016950C, 0, 0, 0), Util::formatBMGRaw);
SME_PATCH_BL(SME_PORT_REGION(0x80169534, 0, 0, 0), Util::formatBMGRaw);
SME_PATCH_BL(SME_PORT_REGION(0x8016958C, 0, 0, 0), Util::formatBMGRaw);
SME_PATCH_BL(SME_PORT_REGION(0x801695B4, 0, 0, 0), Util::formatBMGRaw);
SME_PATCH_BL(SME_PORT_REGION(0x80169ACC, 0, 0, 0), Util::formatBMGRaw);
SME_PATCH_BL(SME_PORT_REGION(0x80169B04, 0, 0, 0), Util::formatBMGRaw);
SME_PATCH_BL(SME_PORT_REGION(0x8016A02C, 0, 0, 0), Util::formatBMGRaw);
SME_PATCH_BL(SME_PORT_REGION(0x8016A064, 0, 0, 0), Util::formatBMGRaw);
SME_PATCH_BL(SME_PORT_REGION(0x8016A3D0, 0, 0, 0), Util::formatBMGRaw);
SME_PATCH_BL(SME_PORT_REGION(0x8016A3F8, 0, 0, 0), Util::formatBMGRaw);
SME_PATCH_BL(SME_PORT_REGION(0x8016A44C, 0, 0, 0), Util::formatBMGRaw);
SME_PATCH_BL(SME_PORT_REGION(0x8016A474, 0, 0, 0), Util::formatBMGRaw);
SME_PATCH_BL(SME_PORT_REGION(0x8016B19C, 0, 0, 0), Util::formatBMGRaw);
SME_PATCH_BL(SME_PORT_REGION(0x8016B1D4, 0, 0, 0), Util::formatBMGRaw);

/* -- PATCHES -- */

// Restore Chao Seed
SME_WRITE_32(SME_PORT_REGION(0x802FD1A0, 0, 0, 0), 0x808D8C70);

// Show Exception Handler
SME_WRITE_32(SME_PORT_REGION(0x8029D0BC, 0, 0, 0), 0x60000000);

// Extend Exception Handler
SME_WRITE_32(SME_PORT_REGION(0x802C7638, 0, 0, 0), 0x60000000); // gpr info
// SME_WRITE_32(SME_PORT_REGION(0x802C7690, 0, 0, 0), 0x60000000); // float
// info

#ifdef SME_DEBUG
// Skip FMVs
SME_WRITE_32(SME_PORT_REGION(0x802B5E8C, 0, 0, 0), 0x38600001);
SME_WRITE_32(SME_PORT_REGION(0x802B5EF4, 0, 0, 0), 0x38600001);
#endif

#ifdef SME_DEBUG
// Level Select
SME_WRITE_32(SME_PORT_REGION(0x802A6788, 0, 0, 0), 0x3BC00009);
#endif

// Fix Infinte Flutter
SME_WRITE_32(SME_PORT_REGION(0x8028113C, 0, 0, 0), 0xC002F824);

// Fix Health Meter Not Rising Underwater
SME_WRITE_32(SME_PORT_REGION(0x801410E4, 0, 0, 0), 0x60000000);

// Remove Dive While Wall Sliding
SME_WRITE_32(SME_PORT_REGION(0x8024BC10, 0, 0, 0), 0x48000068);

// Flood Till Corona Beat
SME_WRITE_32(SME_PORT_REGION(0x8029961C, 0, 0, 0), 0x38840077);

// Map on D Pad down
SME_WRITE_32(SME_PORT_REGION(0x8017A830, 0, 0, 0), 0x5400077B);
SME_WRITE_32(SME_PORT_REGION(0x80297A60, 0, 0, 0), 0x5400077B);

// Hover move on roofs
SME_WRITE_32(SME_PORT_REGION(0x802569BC, 0, 0, 0), 0xC02300B0);

// Global surfing bloopies
SME_WRITE_32(SME_PORT_REGION(0x801B74F8, 0, 0, 0), 0x60000000);
SME_WRITE_32(SME_PORT_REGION(0x801B74FC, 0, 0, 0), 0x60000000);
SME_WRITE_32(SME_PORT_REGION(0x801B7500, 0, 0, 0), 0x60000000);
SME_WRITE_32(SME_PORT_REGION(0x801B7504, 0, 0, 0), 0x60000000);
SME_WRITE_32(SME_PORT_REGION(0x801B7508, 0, 0, 0), 0x60000000);
SME_WRITE_32(SME_PORT_REGION(0x801B750C, 0, 0, 0), 0x60000000);
SME_WRITE_32(SME_PORT_REGION(0x801B7510, 0, 0, 0), 0x387E0780);
SME_WRITE_32(SME_PORT_REGION(0x801B7514, 0, 0, 0), 0x4810BA9D);
SME_WRITE_32(SME_PORT_REGION(0x801B7518, 0, 0, 0), 0x28030000);
SME_WRITE_32(SME_PORT_REGION(0x801B751C, 0, 0, 0), 0x418200A4);

// Sunscript logging restoration
SME_WRITE_32(SME_PORT_REGION(0x8003DB3C, 0, 0, 0), 0x48306B08);