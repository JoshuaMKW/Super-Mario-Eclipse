#pragma once

#include "sms/actor/Mario.hxx"

class TCheatHandler
{
    void (*mSuccessCallback)(TCheatHandler *);
    void (*mFailureCallback)(TCheatHandler *);
    u16 *mInputList;
    TMarioGamePad *mGamePad;
    s8 mInputIndex;
    bool mActive;

public:
    TCheatHandler();
    TCheatHandler(u16 *inputList, TMarioGamePad *gamepad, void (*successCallback)(TCheatHandler *), void (*failureCallback)(TCheatHandler *));
    
    void setInputList(u16 *inputList) { this->mInputList = inputList; }
    void setSuccessCallBack(void (*callback)(TCheatHandler *)) { this->mSuccessCallback = callback; }
    void setFailureCallBack(void (*callback)(TCheatHandler *)) { this->mFailureCallback = callback; }
    void setGamePad(TMarioGamePad *gamePad) { this->mGamePad = gamePad; }

    constexpr bool isActive() const { return (this->mActive && this->mInputIndex < 0); }
    constexpr bool isDead() const { return !(this->mActive && this->mInputIndex < 0); }
    constexpr bool isInitialized() const { return (this->mGamePad != nullptr); }

    void advanceInput();
    void reset();
};