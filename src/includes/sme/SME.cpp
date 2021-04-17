#include "SME.hxx"
#include "OS.h"

#include "kuribo_sdk.h"
#include "sms/actor/Mario.hxx"
#include "macros.h"

// mario.cpp
extern "C" s16 mario_shadowCrashPatch();

// shine.cpp
extern "C" void shine_animationFreezeCheck();
extern "C" void shine_thinkCloseCamera();

extern OSAlarm gctAlarm;

#ifdef SME_DEBUG
extern OSStopwatch gctStopwatch;
#endif

using namespace SME;

KURIBO_MODULE_BEGIN("Eclipse", "JoshuaMK",
                    __VERSION__ "[" SME_STRINGIZE(SME_MAX_SHINES) " Shines]") {
  KURIBO_EXECUTE_ON_LOAD {
    SME_DEBUG_LOG(
        "Codeblocker - Creating OSAlarm at %p; Calls %p every %0.4f seconds\n",
        &gctAlarm, &SME::Util::Security::checkUserCodes, 0.001f);
#ifdef SME_DEBUG
    OSInitStopwatch(&gctStopwatch, "Codeblocker");
#endif
    OSCreateAlarm(&gctAlarm);
    OSSetPeriodicAlarm(&gctAlarm, OSGetTime(), OSMillisecondsToTicks(1),
                       &SME::Util::Security::checkUserCodes);
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
  KURIBO_PATCH_BL(SME_PORT_REGION(0x802998B8, 0, 0, 0), Patch::CKit::onSetup);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x802A616C, 0, 0, 0), Patch::CKit::onUpdate);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x80143F14, 0, 0, 0), Patch::CKit::onDraw2D);
  KURIBO_PATCH_B(SME_PORT_REGION(0x802A8B58, 0, 0, 0), Util::SMS::isExMap);
  KURIBO_PATCH_B(SME_PORT_REGION(0x802A8B30, 0, 0, 0), Util::SMS::isMultiplayerMap);
  KURIBO_PATCH_B(SME_PORT_REGION(0x802A8AFC, 0, 0, 0), Util::SMS::isDivingMap);
  KURIBO_PATCH_B(SME_PORT_REGION(0x802A8AE0, 0, 0, 0), Util::SMS::isOptionMap);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x8027C6A4, 0, 0, 0), Patch::CKit::manageLightSize);
  kWrite32(SME_PORT_REGION(0x8027C6A8, 0, 0, 0), 0x28030001);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x80153DE8, 0, 0, 0), Patch::CKit::formatTalkMessage);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x80153E1C, 0, 0, 0), Patch::CKit::formatTalkMessage);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x8024D3A8, 0, 0, 0), Patch::CKit::realTimeCustomAttrsHandler);

  // camera.cpp
  KURIBO_PATCH_B(SME_PORT_REGION(0x80027548, 0, 0, 0), Patch::Camera::modifyCameraRangeToSize);

  // CheatCode.cpp
  KURIBO_PATCH_B(SME_PORT_REGION(0x80295B6C, 0, 0, 0), Patch::Cheat::handleDebugCheat);

  // collision.cpp
  KURIBO_PATCH_BL(SME_PORT_REGION(0x80258334, 0, 0, 0), Patch::Collision::checkIsGlideBounce);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x80264CFC, 0, 0, 0), Patch::Collision::checkIsGlideBounce);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x8024C558, 0, 0, 0), Patch::Collision::checkIsRestoreTypeNoFallDamage);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x802500B8, 0, 0, 0), Patch::Collision::updateCollisionContext);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x80250CA0, 0, 0, 0), Patch::Collision::masterGroundCollisionHandler);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x8025059C, 0, 0, 0), Patch::Collision::masterAllCollisionHandler);

  // file_flags.cpp
  KURIBO_PATCH_BL(SME_PORT_REGION(0x802B1794, 0, 0, 0), Patch::Flag::setFileCompleteBool);

  // flagmanager.cpp
  KURIBO_PATCH_B(SME_PORT_REGION(0x80294EF4, 0, 0, 0), Patch::Flag::resetStage);

  // fludd.cpp
  KURIBO_PATCH_B(SME_PORT_REGION(0x80248F14, 0, 0, 0), Patch::Fludd::isPumpOK);
  kWrite32(SME_PORT_REGION(0x803DCA02, 0, 0, 0), 0x00300000 | TMarioAnimeData::FLUDD::FLUDD_ENABLED);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x8014206C, 0, 0, 0), Patch::Fludd::hasWaterCardOpen);
  kWrite32(SME_PORT_REGION(0x80142070, 0, 0, 0), 0x28030000);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x80283058, 0, 0, 0), Patch::Fludd::canCollectFluddItem);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x800678C4, 0, 0, 0), Patch::Fludd::sprayGoopMapWrapGlobalMar);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x801A3ED0, 0, 0, 0), Patch::Fludd::sprayGoopMapWrapGlobalMar);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x801B42D8, 0, 0, 0), Patch::Fludd::sprayGoopMapWrapGlobalMar);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x8024E710, 0, 0, 0), Patch::Fludd::sprayGoopMapWrapMar30);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x8027F7DC, 0, 0, 0), Patch::Fludd::sprayGoopMapWrapGlobalMar);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x8027F94C, 0, 0, 0), Patch::Fludd::sprayGoopMapWrapGlobalMar);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x800FED3C, 0, 0, 0), Patch::Fludd::canCleanSeals);
  kWrite32(SME_PORT_REGION(0x800FED40, 0, 0, 0), 0x2C030000);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x8024D53C, 0, 0, 0), Patch::Fludd::bindFluddtojoint);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x8024E548, 0, 0, 0), Patch::Fludd::checkExecWaterGun);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x8026C370, 0, 0, 0), Patch::Fludd::killTriggerNozzle);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x8026C018, 0, 0, 0), Patch::Fludd::spamHoverWrapper);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x80262580, 0, 0, 0), Patch::Fludd::checkAirNozzle);
  kWrite32(SME_PORT_REGION(0x80262584, 0, 0, 0), 0x2C030000);

  // fruit.cpp
  KURIBO_PATCH_BL(SME_PORT_REGION(0x801E542C, 0, 0, 0), Patch::Fruit::canFruitDieWater);
  kWrite32(SME_PORT_REGION(0x801E5430, 0, 0, 0), 0x2C030000);
  kWrite32(SME_PORT_REGION(0x801E5434, 0, 0, 0), 0x41820140);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x8023F964, 0, 0, 0), Patch::Fruit::chooseGrabDistancing);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x8023F9DC, 0, 0, 0), Patch::Fruit::isGrabWaitOver);

  // init_mods.cpp
  KURIBO_PATCH_BL(SME_PORT_REGION(0x80005328, 0, 0, 0), Patch::Init::initCodeProtection);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x802A750C, 0, 0, 0), Patch::Init::createGlobalHeaps);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x802A7140, 0, 0, 0), Patch::Init::setupMarioDatas);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x802A716C, 0, 0, 0), Patch::Init::initFirstModel);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x802998B4, 0, 0, 0), Patch::Init::initFileMods);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x80280180, 0, 0, 0), Patch::Init::initShineShadow);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x802B7A4C, 0, 0, 0), Patch::Init::initSoundBank);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x802983F0, 0, 0, 0), Patch::Init::initMusicTrack);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x80298420, 0, 0, 0), Patch::Init::initMusicTrack);
  kWrite32(SME_PORT_REGION(0x80276C90, 0, 0, 0), 0x60000000);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x80276C94, 0, 0, 0), Patch::Init::fromMarioInit);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x800397DC, 0, 0, 0), Patch::Init::fromShadowMarioInit);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x80271580, 0, 0, 0), Patch::Init::initYoshi);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x8029CCB0, 0, 0, 0), Patch::Init::initCardColors);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x802B8B20, 0, 0, 0), Patch::Init::initCollisionWarpLinks);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x802B57E4, 0, 0, 0), Patch::Init::createUIHeap);

  // mario.cpp
  KURIBO_PATCH_BL(SME_PORT_REGION(0x802320E0, 0, 0, 0), mario_shadowCrashPatch);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x802500B8, 0, 0, 0), Patch::Mario::updateContexts);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x8029A87C, 0, 0, 0), Patch::Mario::carryOrTalkNPC);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x802815F0, 0, 0, 0), Patch::Mario::canGrabAtNPC);
  kWrite32(SME_PORT_REGION(0x802815F4, 0, 0, 0), 0x2C030000);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x80207430, 0, 0, 0), Patch::Mario::canCarryNPC);
  kWrite32(SME_PORT_REGION(0x80207434, 0, 0, 0), 0x2C030000);

  // shine.cpp
  KURIBO_PATCH_BL(SME_PORT_REGION(0x801BD664, 0, 0, 0), Patch::Shine::manageShineVanish);
  kWrite32(SME_PORT_REGION(0x801BD668, 0, 0, 0), 0x48000568);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x802413E0, 0, 0, 0), Patch::Shine::isKillEnemiesShine);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x802995BC, 0, 0, 0), Patch::Shine::checkBootOut);
  kWrite32(SME_PORT_REGION(0x80297BE8, 0, 0, 0), 0x60848200);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x80293B10, 0, 0, 0), Patch::Shine::extendShineIDLogic);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x801BCC98, 0, 0, 0), Patch::Shine::shineObjectStringMod);
  kWrite32(SME_PORT_REGION(0x803DEE50, 0, 0, 0), Patch::Shine::shineFlagSetter);
  kWrite32(SME_PORT_REGION(0x803DEE7C, 0, 0, 0), Patch::Shine::shineFlagGetter);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x802946D4, 0, 0, 0), Patch::Shine::shineSetClamper);
  kWrite32(SME_PORT_REGION(0x80294744, 0, 0, 0), 0x60000000);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x8029474C, 0, 0, 0), Patch::Shine::shineGetClamper);
  /*Shine casts, fix light*/
  kWrite32(SME_PORT_REGION(0x80412548, 0, 0, 0), (f32)SME_MAX_SHINES);
  kWrite32(SME_PORT_REGION(0x80293AF8, 0, 0, 0), 0x3BFF03E7);
  kWrite32(SME_PORT_REGION(0x802946B8, 0, 0, 0), 0x280003E7);
  kWrite32(SME_PORT_REGION(0x8017BE78, 0, 0, 0), 0x5464037E);
  kWrite32(SME_PORT_REGION(0x8017BEF4, 0, 0, 0), 0x5464037E);
  kWrite32(SME_PORT_REGION(0x8017BF34, 0, 0, 0), 0x5464037E);
  kWrite32(SME_PORT_REGION(0x801BCE30, 0, 0, 0), 0x5464037E);
  kWrite32(SME_PORT_REGION(0x801FF850, 0, 0, 0), 0x5404037E);
  kWrite32(SME_PORT_REGION(0x802946B4, 0, 0, 0), 0x5480043E);
  kWrite32(SME_PORT_REGION(0x80294730, 0, 0, 0), 0x5480043E);
  kWrite32(SME_PORT_REGION(0x80294734, 0, 0, 0), 0x280003E7);
  kWrite32(SME_PORT_REGION(0x80297BA0, 0, 0, 0), 0x5404037E);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x80294334, 0, 0, 0), Patch::Shine::extendFlagManagerLoad);
  kWrite32(SME_PORT_REGION(0x80294338, 0, 0, 0), 0x48000010);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x802939B8, 0, 0, 0), Patch::Shine::extendFlagManagerSave);
  kWrite32(SME_PORT_REGION(0x802939BC, 0, 0, 0), 0x48000014);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x80297BD8, 0, 0, 0), Patch::Shine::thinkSetBootFlag);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x801BCD20, 0, 0, 0), Patch::Shine::loadAfterMaskState);
  kWrite32(SME_PORT_REGION(0x801BCD24, 0, 0, 0), 0x28030002);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x801BCEEC, 0, 0, 0), Patch::Shine::setKillState);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x8029A590, 0, 0, 0), shine_thinkCloseCamera);
  kWrite32(SME_PORT_REGION(0x8029A594, 0, 0, 0), 0x28000000);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x802999D8, 0, 0, 0), shine_animationFreezeCheck);
  kWrite32(SME_PORT_REGION(0x802999DC, 0, 0, 0), 0x48000034);
  // Make Shines glow more
  kWrite32(SME_PORT_REGION(0x803C9190, 0, 0, 0), 0x3F19999A);

  // stage.cpp
  KURIBO_PATCH_BL(SME_PORT_REGION(0x80299230, 0, 0, 0), Patch::Stage::setStageOnExit);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x80175F58, 0, 0, 0), Patch::Stage::startEpisodeSelect);

  // yoshi.cpp
  kWrite32(SME_PORT_REGION(0x8026E068, 0, 0, 0), 0x2C000001); // Turn green when out of juice
  kWrite32(SME_PORT_REGION(0x8026E0A0, 0, 0, 0), 0x60000000); // No flickering
  kWrite32(SME_PORT_REGION(0x8026EE14, 0, 0, 0), 0x4800020C); // Yoshi can't starve
  KURIBO_PATCH_BL(SME_PORT_REGION(0x8026EB00, 0, 0, 0), Patch::Yoshi::isYoshiDie);
  kWrite32(SME_PORT_REGION(0x8026EB04, 0, 0, 0), 0x2C030000);
  kWrite32(SME_PORT_REGION(0x8026EB08, 0, 0, 0), 0x41820518);

  KURIBO_PATCH_BL(SME_PORT_REGION(0x8026EBFC, 0, 0, 0), Patch::Yoshi::isYoshiDie);
  kWrite32(SME_PORT_REGION(0x8026EC00, 0, 0, 0), 0x2C030000);
  kWrite32(SME_PORT_REGION(0x8026EC04, 0, 0, 0), 0x4182041C);

  KURIBO_PATCH_BL(SME_PORT_REGION(0x8026F218, 0, 0, 0), Patch::Yoshi::isYoshiDie);
  kWrite32(SME_PORT_REGION(0x8026F21C, 0, 0, 0), 0x2C030000);
  kWrite32(SME_PORT_REGION(0x8026F220, 0, 0, 0), 0x41820164);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x801BC868, 0, 0, 0), Patch::Yoshi::isYoshiEggNeedFruit);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x801BC8B4, 0, 0, 0), Patch::Yoshi::isYoshiEggFree);
  kWrite32(SME_PORT_REGION(0x801BC8B8, 0, 0, 0), 0xA01E00FC);
  kWrite32(SME_PORT_REGION(0x801BC8BC, 0, 0, 0), 0x2C00000B);
  kWrite32(SME_PORT_REGION(0x801BC8C0, 0, 0, 0), 0x418200E4);
  kWrite32(SME_PORT_REGION(0x801BC8C4, 0, 0, 0), 0x2C030001);
  kWrite32(SME_PORT_REGION(0x801BC8C8, 0, 0, 0), 0x4182003C);
  kWrite32(SME_PORT_REGION(0x801BC8CC, 0, 0, 0), 0x418100D8);
  kWrite32(SME_PORT_REGION(0x801BC8D0, 0, 0, 0), 0x48000134);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x8024D68C, 0, 0, 0), Patch::Yoshi::isYoshiMaintainFluddModel);
  kWrite32(SME_PORT_REGION(0x8024D690, 0, 0, 0), 0x2C030000);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x8024F240, 0, 0, 0), Patch::Yoshi::maybeYoshiDrown);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x802810F8, 0, 0, 0), Patch::Yoshi::canMountYoshi);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x80281148, 0, 0, 0), Patch::Yoshi::getYoshiYPos_);
  kWrite32(SME_PORT_REGION(0x802810FC, 0, 0, 0), 0x2C030000);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x8026E810, 0, 0, 0), Patch::Yoshi::fixYoshiJuiceDecrement);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x8024E58C, 0, 0, 0), Patch::Yoshi::canYoshiSpray);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x80273198, 0, 0, 0), Patch::Yoshi::calcYoshiSwimVelocity);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x80270078, 0, 0, 0), Patch::Yoshi::isYoshiWaterFlutter);
  kWrite32(SME_PORT_REGION(0x8027007C, 0, 0, 0), 0x7C7E1B78);
  kWrite32(SME_PORT_REGION(0x80270080, 0, 0, 0), 0x60000000);
  kWrite32(SME_PORT_REGION(0x80270084, 0, 0, 0), 0x60000000);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x8024E788, 0, 0, 0), Patch::Yoshi::isYoshiValidDrip);
  KURIBO_PATCH_B(SME_PORT_REGION(0x801BC128, 0, 0, 0), Patch::Yoshi::initFreeEggCard);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x801BC380, 0, 0, 0), Patch::Yoshi::checkFreeEggCard);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x8028121C, 0, 0, 0), Patch::Yoshi::saveNozzles);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x8024EC18, 0, 0, 0), Patch::Yoshi::restoreNozzles);
  kWrite32(SME_PORT_REGION(0x8024EC2C, 0, 0, 0), 0x60000000);

  // utilities/card.cpp
  KURIBO_PATCH_BL(SME_PORT_REGION(0x802B20F8, 0, 0, 0), Patch::Card::mountCard);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x80163C40, 0, 0, 0), Patch::Card::probeCard);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x8015B2C0, 0, 0, 0), Patch::Card::formatCardMessage);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x8015B2F8, 0, 0, 0), Patch::Card::formatCardMessage);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x8015B5BC, 0, 0, 0), Patch::Card::formatCardMessage);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x8015B5E4, 0, 0, 0), Patch::Card::formatCardMessage);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x8015B638, 0, 0, 0), Patch::Card::formatCardMessage);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x8015B660, 0, 0, 0), Patch::Card::formatCardMessage);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x8015BCB8, 0, 0, 0), Patch::Card::formatCardMessage);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x8015BCE0, 0, 0, 0), Patch::Card::formatCardMessage);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x8015BE24, 0, 0, 0), Patch::Card::formatCardMessage);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x8015BE4C, 0, 0, 0), Patch::Card::formatCardMessage);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x8015BE9C, 0, 0, 0), Patch::Card::formatCardMessage);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x8015BEC4, 0, 0, 0), Patch::Card::formatCardMessage);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x8015C508, 0, 0, 0), Patch::Card::formatCardMessage);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x8015D194, 0, 0, 0), Patch::Card::formatCardMessage);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x8015D1CC, 0, 0, 0), Patch::Card::formatCardMessage);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x8015DEFC, 0, 0, 0), Patch::Card::formatCardMessage);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x8015DF34, 0, 0, 0), Patch::Card::formatCardMessage);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x8015E34C, 0, 0, 0), Patch::Card::formatCardMessage);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x8015E374, 0, 0, 0), Patch::Card::formatCardMessage);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x8015E3C8, 0, 0, 0), Patch::Card::formatCardMessage);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x8015E3F0, 0, 0, 0), Patch::Card::formatCardMessage);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x8015F970, 0, 0, 0), Patch::Card::formatCardMessage);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x8015F9A8, 0, 0, 0), Patch::Card::formatCardMessage);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x80161320, 0, 0, 0), Patch::Card::formatCardMessage);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x80161358, 0, 0, 0), Patch::Card::formatCardMessage);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x8016889C, 0, 0, 0), Patch::Card::formatCardMessage);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x801688D4, 0, 0, 0), Patch::Card::formatCardMessage);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x80169224, 0, 0, 0), Patch::Card::formatCardMessage);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x8016925C, 0, 0, 0), Patch::Card::formatCardMessage);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x8016950C, 0, 0, 0), Patch::Card::formatCardMessage);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x80169534, 0, 0, 0), Patch::Card::formatCardMessage);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x8016958C, 0, 0, 0), Patch::Card::formatCardMessage);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x801695B4, 0, 0, 0), Patch::Card::formatCardMessage);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x80169ACC, 0, 0, 0), Patch::Card::formatCardMessage);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x80169B04, 0, 0, 0), Patch::Card::formatCardMessage);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x8016A02C, 0, 0, 0), Patch::Card::formatCardMessage);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x8016A064, 0, 0, 0), Patch::Card::formatCardMessage);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x8016A3D0, 0, 0, 0), Patch::Card::formatCardMessage);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x8016A3F8, 0, 0, 0), Patch::Card::formatCardMessage);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x8016A44C, 0, 0, 0), Patch::Card::formatCardMessage);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x8016A474, 0, 0, 0), Patch::Card::formatCardMessage);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x8016B19C, 0, 0, 0), Patch::Card::formatCardMessage);
  KURIBO_PATCH_BL(SME_PORT_REGION(0x8016B1D4, 0, 0, 0), Patch::Card::formatCardMessage);

  /* -- PATCHES -- */

  // Restore Chao Seed
  kWrite32(SME_PORT_REGION(0x802FD1A0, 0, 0, 0), 0x808D8C70);

  // Show Exception Handler
  kWrite32(SME_PORT_REGION(0x8029D0BC, 0, 0, 0), 0x60000000);

#ifdef SME_DEBUG
  // Skip FMVs
  kWrite32(SME_PORT_REGION(0x802B5E8C, 0, 0, 0), 0x38600001);
  kWrite32(SME_PORT_REGION(0x802B5EF4, 0, 0, 0), 0x38600001);
#endif

#ifdef SME_DEBUG
  // Level Select
  kWrite32(SME_PORT_REGION(0x802A6788, 0, 0, 0), 0x3BC00009);
#endif

  // Fix Infinte Flutter
  kWrite32(SME_PORT_REGION(0x8028113C, 0, 0, 0), 0xC002F824);

  // Remove Dive While Wall Sliding
  kWrite32(SME_PORT_REGION(0x8024BC10, 0, 0, 0), 0x48000068);

  // Flood Till Corona Beat
  kWrite32(SME_PORT_REGION(0x8029961C, 0, 0, 0), 0x38840077);

  // Map on D Pad down
  kWrite32(SME_PORT_REGION(0x8017A830, 0, 0, 0), 0x5400077B);
  kWrite32(SME_PORT_REGION(0x80297A60, 0, 0, 0), 0x5400077B);

  // Hover move on roofs
  kWrite32(SME_PORT_REGION(0x802569BC, 0, 0, 0), 0xC02300B0);

  // Global surfing bloopies
  kWrite32(SME_PORT_REGION(0x801B74F8, 0, 0, 0), 0x60000000);
  kWrite32(SME_PORT_REGION(0x801B74FC, 0, 0, 0), 0x60000000);
  kWrite32(SME_PORT_REGION(0x801B7500, 0, 0, 0), 0x60000000);
  kWrite32(SME_PORT_REGION(0x801B7504, 0, 0, 0), 0x60000000);
  kWrite32(SME_PORT_REGION(0x801B7508, 0, 0, 0), 0x60000000);
  kWrite32(SME_PORT_REGION(0x801B750C, 0, 0, 0), 0x60000000);
  kWrite32(SME_PORT_REGION(0x801B7510, 0, 0, 0), 0x387E0780);
  kWrite32(SME_PORT_REGION(0x801B7514, 0, 0, 0), 0x4810BA9D);
  kWrite32(SME_PORT_REGION(0x801B7518, 0, 0, 0), 0x28030000);
  kWrite32(SME_PORT_REGION(0x801B751C, 0, 0, 0), 0x418200A4);
}
KURIBO_MODULE_END();