#pragma once

#include "eclipse.hxx"

bool TYoshi::isGreenYoshi() {
    return this->mType == TYoshi::TYPE::GREEN;
}

bool TYoshi::isGreenYoshiMounted() {
    return this->isGreenYoshi() && this->mState == TYoshi::STATE::MOUNTED;
}

bool TYoshi::isGreenYoshiAscendingWater(TMario* gpMario) {
    if (!(gpMario->mAttributes.mIsWater) ||
        !gpMario->mController->isPressed(TMarioGamePad::BUTTONS::A) ||
        !gpMario->mYoshi->isGreenYoshiMounted()) {
        return false;
    } else {
        return true;
    }
}