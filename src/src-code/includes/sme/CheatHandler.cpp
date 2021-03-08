#include "CheatHandler.hxx"

TCheatHandler::TCheatHandler()
{
    this->mInputList = nullptr;
    this->mSuccessCallback = nullptr;
    this->mFailureCallback = nullptr;
    this->mGamePad = nullptr;
    this->mActive = false;
    this->mInputIndex = 0;
}

TCheatHandler::TCheatHandler(u16 *inputList, TMarioGamePad *gamepad, void *successCallback, void *failureCallback)
{
    this->mInputList = inputList;
    this->mSuccessCallback = successCallback;
    this->mFailureCallback = failureCallback;
    this->mGamePad = gamepad;
    this->mActive = false;
    this->mInputIndex = 0;
}

void TCheatHandler::advanceInput()
{
    if (!this->mGamePad || !this->mInputList)
    {
        return;
    }

    if (this->mGamePad->getFrameInput() == NULL || this->mInputIndex < 0)
    {
        return;
    }

    if (this->mGamePad->getFrameInput() != this->mInputList[this->mInputIndex])
    {
        this->mInputIndex = -1;
        this->mActive = false;

        if (this->mFailureCallback != nullptr)
            ((void (*)(TCheatHandler *))this->mFailureCallback)(this);

        return;
    }

    if (this->mInputList[this->mInputIndex + 1] == NULL)
    {
        this->mInputIndex = -1;
        this->mActive = true;

        if (this->mSuccessCallback != nullptr)
            ((void (*)(TCheatHandler *))this->mSuccessCallback)(this);

        return;
    }

    this->mInputIndex += 1;
}

void TCheatHandler::reset()
{
    this->mInputIndex = 0;
    this->mActive = false;
}