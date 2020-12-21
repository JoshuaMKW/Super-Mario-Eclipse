#include "sms/actor/Mario.hxx"

class TCheatHandler
{
    void *mCallback;
    u16 *mInputList;
    TMarioGamePad *mGamePad;
    s8 mInputIndex;
    bool mActive;

    TCheatHandler()
    {
        this->mInputList = NULL;
        this->mCallback = NULL;
        this->mGamePad = NULL;
        this->mActive = false;
        this->mInputIndex = 0;
    }

    TCheatHandler(u16 *inputList, TMarioGamePad *gamepad, void *successCallback, void *failureCallback)
    {
        this->mInputList = inputList;
        this->mSuccessCallback = successCallback;
        this->mFailureCallback = failureCallback;
        this->mGamePad = gamepad;
        this->mActive = false;
        this->mInputIndex = 0;
    }

public:
    void setInputList(u16 *inputList) { this->mInputList = inputList; }
    void setSuccessCallBack(void *callback) { this->mSuccessCallback = callback; }
    void setFailureCallBack(void *callback) { this->mFailureCallback = callback; }
    void setGamePad(TMarioGamePad *gamePad) { this->mGamePad = gamePad; }

    void advanceInput()
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
            void (*callback)(TCheatHandler *) = this->mFailureCallback;
            callback(this);
            return;
        }

        if (this->mInputList[this->mInputIndex + 1] == NULL)
        {
            this->mInputIndex = -1;
            this->mActive = true;
            void (*callback)(TCheatHandler *) = this->mSuccessCallback;
            callback(this);
            return;
        }

        this->mInputIndex += 1;
    }

    void reset()
    {
        this->mInputIndex = 0;
        this->mActive = false;
    }

    bool isActive()
    {
        return (this->mActive && this->mInputIndex == -1);
    }

    bool isDead()
    {
        return (!this->mActive && this->mInputIndex == -1);
    }

    bool isInitialized()
    {
        return (this->mGamePad != NULL);
    }
};