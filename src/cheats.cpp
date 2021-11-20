#include "OS.h"
#include "SME.hxx"
#include "J2D/J2DTextBox.hxx"
#include "types.h"

using namespace SME;

static u16 gDebugModeCheatCode[] = {
    TMarioGamePad::DPAD_UP,   TMarioGamePad::DPAD_UP,
    TMarioGamePad::DPAD_DOWN, TMarioGamePad::DPAD_DOWN,
    TMarioGamePad::DPAD_LEFT, TMarioGamePad::DPAD_RIGHT,
    TMarioGamePad::DPAD_LEFT, TMarioGamePad::DPAD_RIGHT,
    TMarioGamePad::B,         TMarioGamePad::A,
    TMarioGamePad::START};

J2DTextBox gDebugTextBox;

static void debugModeNotify(Class::TCheatHandler *) {
  if (gpMSound->gateCheck(MSound::SE_SHINE_TOUCH))
    startSoundActor__Q214MSoundSESystem8MSoundSEFUlPC3VecUlPP8JAISoundUlUc(
        gpMSound, MSound::SE_SHINE_TOUCH, 0, 0, 0, 4);

  TGlobals::setDebugMode(true);
  SME_LOG("Debug mode activated!\n");

  if (!TGlobals::isDebugMode())
    gDebugTextBox.mIsVisible = true;
}

// extern runtime_mods.cpp
void Patch::Cheat::drawCheatText() {
  if (gDebugTextBox.getStringPtr()) {
#ifndef SME_DEBUG
    if (*gDebugTextBox.getStringPtr() != '\0' &&
        Class::TCheatHandler::sDebugHandler.isActive())
#else
    if (*gDebugTextBox.getStringPtr() != '\0')
#endif
    {
      gDebugTextBox.draw(250, 24);
    }
  }
}

// 0x80295B6C
// extern -> SME.cpp
void *Patch::Cheat::handleDebugCheat(void *GCLogoDir) {
  if (!Class::TCheatHandler::sDebugHandler.isInitialized()) {
    Class::TCheatHandler::sDebugHandler.setGamePad(
        gpApplication.mGamePad1);
    Class::TCheatHandler::sDebugHandler.setInputList(gDebugModeCheatCode);
    Class::TCheatHandler::sDebugHandler.setSuccessCallBack(
        &debugModeNotify);
  }
  Class::TCheatHandler::sDebugHandler.advanceInput();
  return GCLogoDir;
}

static void isLevelSelectAvailable() {
  u32 context;
  SME_FROM_GPR(30, context);

  if (context == 9 || context == 4)
    context = TGlobals::isDebugMode() ? 9 : 4;
  gpApplication.mContext = context;
}
SME_PATCH_BL(SME_PORT_REGION(0x802A6794, 0, 0, 0), isLevelSelectAvailable);