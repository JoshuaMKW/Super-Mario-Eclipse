#include "OS.h"
#include "SME.hxx"
#include "sms/JSystem/J2D/J2DTextBox.hxx"
#include "sms/JSystem/JDrama.hxx"
#include "types.h"

using namespace SME;

static u16 gDebugModeCheatCode[] = {
    TMarioGamePad::Buttons::DPAD_UP,   TMarioGamePad::Buttons::DPAD_UP,
    TMarioGamePad::Buttons::DPAD_DOWN, TMarioGamePad::Buttons::DPAD_DOWN,
    TMarioGamePad::Buttons::DPAD_LEFT, TMarioGamePad::Buttons::DPAD_RIGHT,
    TMarioGamePad::Buttons::DPAD_LEFT, TMarioGamePad::Buttons::DPAD_RIGHT,
    TMarioGamePad::Buttons::B,         TMarioGamePad::Buttons::A,
    TMarioGamePad::Buttons::START};

J2DTextBox gDebugTextBox;

static void debugModeNotify(Class::TCheatHandler *) {
  if (gpMSound->gateCheck(MSound::SE_SHINE_TOUCH))
    startSoundActor__Q214MSoundSESystem8MSoundSEFUlPC3VecUlPP8JAISoundUlUc(
        gpMSound, MSound::SE_SHINE_TOUCH, 0, 0, 0, 4);

  SME::Util::Memory::PPC::write<u32>((u32 *)0x802A6788, 0x3BC00009);

#ifndef SME_DEBUG
  gDebugTextBox.mIsVisible = true;
#endif
}

// extern runtime_mods.cpp
void Patch::Cheat::drawCheatText() {
  if (gDebugTextBox.getStringPtr()) {
#ifndef SME_DEBUG
    if (*gDebugTextBox.getStringPtr() != '\0' &&
        SME::Class::TCheatHandler::sDebugHandler.isActive())
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
  if (!SME::Class::TCheatHandler::sDebugHandler.isInitialized()) {
    SME::Class::TCheatHandler::sDebugHandler.setGamePad(
        gpApplication.mGamePad1);
    SME::Class::TCheatHandler::sDebugHandler.setInputList(gDebugModeCheatCode);
    SME::Class::TCheatHandler::sDebugHandler.setSuccessCallBack(
        &debugModeNotify);
  }
  SME::Class::TCheatHandler::sDebugHandler.advanceInput();
  return GCLogoDir;
}