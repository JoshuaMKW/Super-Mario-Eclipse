#include "CheatHandler.hxx"

using namespace SME::Class;

TCheatHandler::TCheatHandler()
    : mInputList(nullptr), mSuccessCallback(nullptr),
      mFailureCallback(nullptr), mGamePad(nullptr), mActive(false),
      mInputIndex(0) {}

TCheatHandler::TCheatHandler(u16 *inputList, TMarioGamePad *gamepad,
                             void (*successCallback)(TCheatHandler *),
                             void (*failureCallback)(TCheatHandler *))
    : mInputList(inputList), mSuccessCallback(successCallback),
      mFailureCallback(failureCallback), mGamePad(gamepad), mActive(false),
      mInputIndex(0) {}

void TCheatHandler::advanceInput() {
  if (!mGamePad || !mInputList)
    return;
  else if (mGamePad->mButtons.mFrameInput == 0 || mInputIndex < 0)
    return;

  if (mGamePad->mButtons.mFrameInput !=
      mInputList[static_cast<u8>(mInputIndex)]) {
    fail();
    return;
  }

  if (mInputList[static_cast<u8>(mInputIndex) + 1] == 0) {
    succeed();
    return;
  }

  mInputIndex += 1;
}

void TCheatHandler::succeed() {
  mInputIndex = -1;
  mActive = true;

  if (mSuccessCallback)
    mSuccessCallback(this);
}

void TCheatHandler::fail() {
  mInputIndex = -1;
  mActive = false;

  if (mFailureCallback)
    mFailureCallback(this);
}

void TCheatHandler::reset() {
  mInputIndex = 0;
  mActive = false;
}