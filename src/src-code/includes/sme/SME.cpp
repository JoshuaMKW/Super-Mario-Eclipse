#include "SME.hxx"
#include "OS.h"

#include "kuribo_sdk.h"
#include "sms/actor/Mario.hxx"

// mario.cpp
extern "C" s16 mario_shadowCrashPatch();

// shine.cpp
extern "C" void shine_animationFreezeCheck();
extern "C" void shine_thinkCloseCamera();

extern OSAlarm gctAlarm;
extern void setUserCodes(OSAlarm *alarm, OSContext *context);

#ifdef SME_DEBUG
extern OSStopwatch gctStopwatch;
#endif

using namespace SME;

KURIBO_MODULE_BEGIN("Eclipse", "JoshuaMK",
                    __VERSION__ "[" SME_STRINGIZE(SME_MAX_SHINES) " Shines]") {
  KURIBO_EXECUTE_ON_LOAD {
    SME_DEBUG_LOG(
        "Codeblocker - Creating OSAlarm at %p; Calls %p every %0.4f seconds\n",
        &gctAlarm, &setUserCodes, 0.001f);
#ifdef SME_DEBUG
    OSInitStopwatch(&gctStopwatch, "Codeblocker");
#endif
    OSCreateAlarm(&gctAlarm);
    OSSetPeriodicAlarm(&gctAlarm, OSGetTime(), OSMillisecondsToTicks(1),
                       &setUserCodes);
    SME_DEBUG_LOG("Mario health offset = %X\n",
                  offsetof(TMario, mHealth));
  }
  KURIBO_EXECUTE_ON_UNLOAD {
    SME_DEBUG_LOG("-- Destroying Module --\n");
#ifdef SME_DEBUG
    OSStopStopwatch(&gctStopwatch);
#endif
    OSCancelAlarm(&gctAlarm);
  }

  /* -- HOOKS -- */

  // c_kit.cpp
  KURIBO_PATCH_BL(0x802998B8, Patch::CKit::onSetup);
  KURIBO_PATCH_BL(0x802A616C, Patch::CKit::onUpdate);
  KURIBO_PATCH_BL(0x80143F14, Patch::CKit::onDraw2D);
  KURIBO_PATCH_B(0x802A8B58, Util::SMS::isExMap);
  KURIBO_PATCH_B(0x802A8B30, Util::SMS::isMultiplayerMap);
  KURIBO_PATCH_B(0x802A8AFC, Util::SMS::isDivingMap);
  KURIBO_PATCH_B(0x802A8AE0, Util::SMS::isOptionMap);
  KURIBO_PATCH_BL(0x8027C6A4, Patch::CKit::manageLightSize);
  kWrite32(0x8027C6A8, 0x28030001);
  KURIBO_PATCH_BL(0x80153DE8, Patch::CKit::formatTalkMessage);
  KURIBO_PATCH_BL(0x80153E1C, Patch::CKit::formatTalkMessage);
  KURIBO_PATCH_BL(0x8024D3A8, Patch::CKit::realTimeCustomAttrsHandler);

  // camera.cpp
  KURIBO_PATCH_B(0x80027548, Patch::Camera::modifyCameraRangeToSize);

  // CheatCode.cpp
  KURIBO_PATCH_B(0x80295B6C, Patch::Cheat::handleDebugCheat);

  // collision.cpp
  KURIBO_PATCH_BL(0x80258334, Patch::Collision::checkIsGlideBounce);
  KURIBO_PATCH_BL(0x80264CFC, Patch::Collision::checkIsGlideBounce);
  KURIBO_PATCH_BL(0x8024C558, Patch::Collision::checkIsRestoreTypeNoFallDamage);
  KURIBO_PATCH_BL(0x802500B8, Patch::Collision::updateCollisionContext);
  KURIBO_PATCH_BL(0x80250CA0, Patch::Collision::masterGroundCollisionHandler);
  KURIBO_PATCH_BL(0x8025059C, Patch::Collision::masterAllCollisionHandler);

  // file_flags.cpp
  KURIBO_PATCH_BL(0x802B1794, Patch::Flag::setFileCompleteBool);

  // flagmanager.cpp
  KURIBO_PATCH_B(0x80294EF4, Patch::Flag::resetStage);

  // fludd.cpp
  KURIBO_PATCH_B(0x80248F14, Patch::Fludd::isPumpOK);
  kWrite32(0x803DCA02, 0x00300000 | TMarioAnimeData::FLUDD::FLUDD_ENABLED);
  KURIBO_PATCH_BL(0x8014206C, Patch::Fludd::hasWaterCardOpen);
  kWrite32(0x80142070, 0x28030000);
  KURIBO_PATCH_BL(0x80283058, Patch::Fludd::canCollectFluddItem);
  KURIBO_PATCH_BL(0x800678C4, Patch::Fludd::sprayGoopMapWrapGlobalMar);
  KURIBO_PATCH_BL(0x801A3ED0, Patch::Fludd::sprayGoopMapWrapGlobalMar);
  KURIBO_PATCH_BL(0x801B42D8, Patch::Fludd::sprayGoopMapWrapGlobalMar);
  KURIBO_PATCH_BL(0x8024E710, Patch::Fludd::sprayGoopMapWrapMar30);
  KURIBO_PATCH_BL(0x8027F7DC, Patch::Fludd::sprayGoopMapWrapGlobalMar);
  KURIBO_PATCH_BL(0x8027F94C, Patch::Fludd::sprayGoopMapWrapGlobalMar);
  KURIBO_PATCH_BL(0x800FED3C, Patch::Fludd::canCleanSeals);
  kWrite32(0x800FED40, 0x2C030000);
  KURIBO_PATCH_BL(0x8024D53C, Patch::Fludd::bindFluddtojoint);
  KURIBO_PATCH_BL(0x8024E548, Patch::Fludd::checkExecWaterGun);
  KURIBO_PATCH_BL(0x8026C370, Patch::Fludd::killTriggerNozzle);
  KURIBO_PATCH_BL(0x8026C018, Patch::Fludd::spamHoverWrapper);
  KURIBO_PATCH_BL(0x80262580, Patch::Fludd::checkAirNozzle);
  kWrite32(0x80262584, 0x2C030000);

  // fruit.cpp
  KURIBO_PATCH_BL(0x801E542C, Patch::Fruit::canFruitDieWater);
  kWrite32(0x801E5430, 0x2C030000);
  kWrite32(0x801E5434, 0x41820140);
  KURIBO_PATCH_BL(0x8023F964, Patch::Fruit::chooseGrabDistancing);
  KURIBO_PATCH_BL(0x8023F9DC, Patch::Fruit::isGrabWaitOver);

  // init_mods.cpp
  KURIBO_PATCH_BL(0x80005328, Patch::Init::initCodeProtection);
  KURIBO_PATCH_BL(0x802A750C, Patch::Init::createGlobalHeaps);
  KURIBO_PATCH_BL(0x802A7140, Patch::Init::setupMarioDatas);
  KURIBO_PATCH_BL(0x802A716C, Patch::Init::initFirstModel);
  KURIBO_PATCH_BL(0x802998B4, Patch::Init::initFileMods);
  KURIBO_PATCH_BL(0x80280180, Patch::Init::initShineShadow);
  KURIBO_PATCH_BL(0x802B7A4C, Patch::Init::initSoundBank);
  KURIBO_PATCH_BL(0x802983F0, Patch::Init::initMusicTrack);
  KURIBO_PATCH_BL(0x80298420, Patch::Init::initMusicTrack);
  kWrite32(0x80276C90, 0x60000000);
  KURIBO_PATCH_BL(0x80276C94, Patch::Init::fromMarioInit);
  KURIBO_PATCH_BL(0x800397DC, Patch::Init::fromShadowMarioInit);
  KURIBO_PATCH_BL(0x80271580, Patch::Init::initYoshi);
  KURIBO_PATCH_BL(0x8029CCB0, Patch::Init::initCardColors);
  KURIBO_PATCH_BL(0x802B8B20, Patch::Init::initCollisionWarpLinks);
  KURIBO_PATCH_BL(0x802B57E4, Patch::Init::createUIHeap);

  // mario.cpp
  KURIBO_PATCH_BL(0x802320E0, mario_shadowCrashPatch);
  KURIBO_PATCH_BL(0x802500B8, Patch::Mario::updateContexts);
  KURIBO_PATCH_BL(0x8029A87C, Patch::Mario::carryOrTalkNPC);
  KURIBO_PATCH_BL(0x802815F0, Patch::Mario::canGrabAtNPC);
  kWrite32(0x802815F4, 0x2C030000);
  KURIBO_PATCH_BL(0x80207430, Patch::Mario::canCarryNPC);
  kWrite32(0x80207434, 0x2C030000);

  // shine.cpp
  KURIBO_PATCH_BL(0x801BD664, Patch::Shine::manageShineVanish);
  kWrite32(0x801BD668, 0x48000568);
  KURIBO_PATCH_BL(0x802413E0, Patch::Shine::isKillEnemiesShine);
  KURIBO_PATCH_BL(0x802995BC, Patch::Shine::checkBootOut);
  kWrite32(0x80297BE8, 0x60848200);
  KURIBO_PATCH_BL(0x80293B10, Patch::Shine::extendShineIDLogic);
  KURIBO_PATCH_BL(0x801BCC98, Patch::Shine::shineObjectStringMod);
  kWrite32(0x803DEE50, Patch::Shine::shineFlagSetter);
  kWrite32(0x803DEE7C, Patch::Shine::shineFlagGetter);
  KURIBO_PATCH_BL(0x802946D4, Patch::Shine::shineSetClamper);
  kWrite32(0x80294744, 0x60000000);
  KURIBO_PATCH_BL(0x8029474C, Patch::Shine::shineGetClamper);
  /*Shine casts, fix light*/
  kWrite32(0x80412548, (f32)SME_MAX_SHINES);
  kWrite32(0x80293AF8, 0x3BFF03E7);
  kWrite32(0x802946B8, 0x280003E7);
  kWrite32(0x8017BE78, 0x5464037E);
  kWrite32(0x8017BEF4, 0x5464037E);
  kWrite32(0x8017BF34, 0x5464037E);
  kWrite32(0x801BCE30, 0x5464037E);
  kWrite32(0x801FF850, 0x5404037E);
  kWrite32(0x802946B4, 0x5480043E);
  kWrite32(0x80294730, 0x5480043E);
  kWrite32(0x80294734, 0x280003E7);
  kWrite32(0x80297BA0, 0x5404037E);
  KURIBO_PATCH_BL(0x80294334, Patch::Shine::extendFlagManagerLoad);
  kWrite32(0x80294338, 0x48000010);
  KURIBO_PATCH_BL(0x802939B8, Patch::Shine::extendFlagManagerSave);
  kWrite32(0x802939BC, 0x48000014);
  KURIBO_PATCH_BL(0x80297BD8, Patch::Shine::thinkSetBootFlag);
  KURIBO_PATCH_BL(0x801BCD20, Patch::Shine::loadAfterMaskState);
  kWrite32(0x801BCD24, 0x28030002);
  KURIBO_PATCH_BL(0x801BCEEC, Patch::Shine::setKillState);
  KURIBO_PATCH_BL(0x8029A590, shine_thinkCloseCamera);
  kWrite32(0x8029A594, 0x28000000);
  KURIBO_PATCH_BL(0x802999D8, shine_animationFreezeCheck);
  kWrite32(0x802999DC, 0x48000034);
  // Make Shines glow more
  kWrite32(0x803C9190, 0x3F19999A);

  // stage.cpp
  KURIBO_PATCH_BL(0x80299230, Patch::Stage::setStageOnExit);
  KURIBO_PATCH_BL(0x80175F58, Patch::Stage::startEpisodeSelect);

  // yoshi.cpp
  kWrite32(0x8026E068, 0x2C000001); // Turn green when out of juice
  kWrite32(0x8026E0A0, 0x60000000); // No flickering
  kWrite32(0x8026EE14, 0x4800020C); // Yoshi can't starve
  KURIBO_PATCH_BL(0x8026EB00, Patch::Yoshi::isYoshiDie);
  kWrite32(0x8026EB04, 0x2C030000);
  kWrite32(0x8026EB08, 0x41820518);

  KURIBO_PATCH_BL(0x8026EBFC, Patch::Yoshi::isYoshiDie);
  kWrite32(0x8026EC00, 0x2C030000);
  kWrite32(0x8026EC04, 0x4182041C);

  KURIBO_PATCH_BL(0x8026F218, Patch::Yoshi::isYoshiDie);
  kWrite32(0x8026F21C, 0x2C030000);
  kWrite32(0x8026F220, 0x41820164);
  KURIBO_PATCH_BL(0x801BC868, Patch::Yoshi::isYoshiEggNeedFruit);
  KURIBO_PATCH_BL(0x801BC8B4, Patch::Yoshi::isYoshiEggFree);
  kWrite32(0x801BC8B8, 0xA01E00FC);
  kWrite32(0x801BC8BC, 0x2C00000B);
  kWrite32(0x801BC8C0, 0x418200E4);
  kWrite32(0x801BC8C4, 0x2C030001);
  kWrite32(0x801BC8C8, 0x4182003C);
  kWrite32(0x801BC8CC, 0x418100D8);
  kWrite32(0x801BC8D0, 0x48000134);
  KURIBO_PATCH_BL(0x8024D68C, Patch::Yoshi::isYoshiMaintainFluddModel);
  kWrite32(0x8024D690, 0x2C030000);
  KURIBO_PATCH_BL(0x8024F240, Patch::Yoshi::maybeYoshiDrown);
  KURIBO_PATCH_BL(0x802810F8, Patch::Yoshi::canMountYoshi);
  KURIBO_PATCH_BL(0x80281148, Patch::Yoshi::getYoshiYPos_);
  kWrite32(0x802810FC, 0x2C030000);
  KURIBO_PATCH_BL(0x8026E810, Patch::Yoshi::fixYoshiJuiceDecrement);
  KURIBO_PATCH_BL(0x8024E58C, Patch::Yoshi::canYoshiSpray);
  KURIBO_PATCH_BL(0x80273198, Patch::Yoshi::calcYoshiSwimVelocity);
  KURIBO_PATCH_BL(0x80270078, Patch::Yoshi::isYoshiWaterFlutter);
  kWrite32(0x8027007C, 0x7C7E1B78);
  kWrite32(0x80270080, 0x60000000);
  kWrite32(0x80270084, 0x60000000);
  KURIBO_PATCH_BL(0x8024E788, Patch::Yoshi::isYoshiValidDrip);
  KURIBO_PATCH_B(0x801BC128, Patch::Yoshi::initFreeEggCard);
  KURIBO_PATCH_BL(0x801BC380, Patch::Yoshi::checkFreeEggCard);
  KURIBO_PATCH_BL(0x8028121C, Patch::Yoshi::saveNozzles);
  KURIBO_PATCH_BL(0x8024EC18, Patch::Yoshi::restoreNozzles);
  kWrite32(0x8024EC2C, 0x60000000);

  // utilities/card.cpp
  KURIBO_PATCH_BL(0x802B20F8, Patch::Card::mountCard);
  KURIBO_PATCH_BL(0x80163C40, Patch::Card::probeCard);
  KURIBO_PATCH_BL(0x8015B2C0, Patch::Card::formatCardMessage);
  KURIBO_PATCH_BL(0x8015B2F8, Patch::Card::formatCardMessage);
  KURIBO_PATCH_BL(0x8015B5BC, Patch::Card::formatCardMessage);
  KURIBO_PATCH_BL(0x8015B5E4, Patch::Card::formatCardMessage);
  KURIBO_PATCH_BL(0x8015B638, Patch::Card::formatCardMessage);
  KURIBO_PATCH_BL(0x8015B660, Patch::Card::formatCardMessage);
  KURIBO_PATCH_BL(0x8015BCB8, Patch::Card::formatCardMessage);
  KURIBO_PATCH_BL(0x8015BCE0, Patch::Card::formatCardMessage);
  KURIBO_PATCH_BL(0x8015BE24, Patch::Card::formatCardMessage);
  KURIBO_PATCH_BL(0x8015BE4C, Patch::Card::formatCardMessage);
  KURIBO_PATCH_BL(0x8015BE9C, Patch::Card::formatCardMessage);
  KURIBO_PATCH_BL(0x8015BEC4, Patch::Card::formatCardMessage);
  KURIBO_PATCH_BL(0x8015C508, Patch::Card::formatCardMessage);
  KURIBO_PATCH_BL(0x8015D194, Patch::Card::formatCardMessage);
  KURIBO_PATCH_BL(0x8015D1CC, Patch::Card::formatCardMessage);
  KURIBO_PATCH_BL(0x8015DEFC, Patch::Card::formatCardMessage);
  KURIBO_PATCH_BL(0x8015DF34, Patch::Card::formatCardMessage);
  KURIBO_PATCH_BL(0x8015E34C, Patch::Card::formatCardMessage);
  KURIBO_PATCH_BL(0x8015E374, Patch::Card::formatCardMessage);
  KURIBO_PATCH_BL(0x8015E3C8, Patch::Card::formatCardMessage);
  KURIBO_PATCH_BL(0x8015E3F0, Patch::Card::formatCardMessage);
  KURIBO_PATCH_BL(0x8015F970, Patch::Card::formatCardMessage);
  KURIBO_PATCH_BL(0x8015F9A8, Patch::Card::formatCardMessage);
  KURIBO_PATCH_BL(0x80161320, Patch::Card::formatCardMessage);
  KURIBO_PATCH_BL(0x80161358, Patch::Card::formatCardMessage);
  KURIBO_PATCH_BL(0x8016889C, Patch::Card::formatCardMessage);
  KURIBO_PATCH_BL(0x801688D4, Patch::Card::formatCardMessage);
  KURIBO_PATCH_BL(0x80169224, Patch::Card::formatCardMessage);
  KURIBO_PATCH_BL(0x8016925C, Patch::Card::formatCardMessage);
  KURIBO_PATCH_BL(0x8016950C, Patch::Card::formatCardMessage);
  KURIBO_PATCH_BL(0x80169534, Patch::Card::formatCardMessage);
  KURIBO_PATCH_BL(0x8016958C, Patch::Card::formatCardMessage);
  KURIBO_PATCH_BL(0x801695B4, Patch::Card::formatCardMessage);
  KURIBO_PATCH_BL(0x80169ACC, Patch::Card::formatCardMessage);
  KURIBO_PATCH_BL(0x80169B04, Patch::Card::formatCardMessage);
  KURIBO_PATCH_BL(0x8016A02C, Patch::Card::formatCardMessage);
  KURIBO_PATCH_BL(0x8016A064, Patch::Card::formatCardMessage);
  KURIBO_PATCH_BL(0x8016A3D0, Patch::Card::formatCardMessage);
  KURIBO_PATCH_BL(0x8016A3F8, Patch::Card::formatCardMessage);
  KURIBO_PATCH_BL(0x8016A44C, Patch::Card::formatCardMessage);
  KURIBO_PATCH_BL(0x8016A474, Patch::Card::formatCardMessage);
  KURIBO_PATCH_BL(0x8016B19C, Patch::Card::formatCardMessage);
  KURIBO_PATCH_BL(0x8016B1D4, Patch::Card::formatCardMessage);

  /* -- PATCHES -- */

  // Restore Chao Seed
  kWrite32(0x802FD1A0, 0x808D8C70);

  // Show Exception Handler
  kWrite32(0x8029D0BC, 0x60000000);

#ifdef SME_DEBUG
  // Skip FMVs
  kWrite32(0x802B5E8C, 0x38600001);
  kWrite32(0x802B5EF4, 0x38600001);
#endif

#ifdef SME_DEBUG
  // Level Select
  kWrite32(0x802A6788, 0x3BC00009);
#endif

  // Fix Infinte Flutter
  kWrite32(0x8028113C, 0xC002F824);

  // Remove Dive While Wall Sliding
  kWrite32(0x8024BC10, 0x48000068);

  // Flood Till Corona Beat
  kWrite32(0x8029961C, 0x38840077);

  // Map on D Pad down
  kWrite32(0x8017A830, 0x5400077B);
  kWrite32(0x80297A60, 0x5400077B);

  // Hover move on roofs
  kWrite32(0x802569BC, 0xC02300B0);

  // Global surfing bloopies
  kWrite32(0x801B74F8, 0x60000000);
  kWrite32(0x801B74FC, 0x60000000);
  kWrite32(0x801B7500, 0x60000000);
  kWrite32(0x801B7504, 0x60000000);
  kWrite32(0x801B7508, 0x60000000);
  kWrite32(0x801B750C, 0x60000000);
  kWrite32(0x801B7510, 0x387E0780);
  kWrite32(0x801B7514, 0x4810BA9D);
  kWrite32(0x801B7518, 0x28030000);
  kWrite32(0x801B751C, 0x418200A4);
}
KURIBO_MODULE_END();