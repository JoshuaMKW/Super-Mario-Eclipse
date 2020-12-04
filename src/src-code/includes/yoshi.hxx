#pragma once

#include "eclipse.hxx"

bool TYoshi::isMounted()
{
    return this->mState == TYoshi::MOUNTED;
}

bool TYoshi::isMounted(TMario *gpMario)
{
    return gpMario->mYoshi->mState == TYoshi::MOUNTED;
}

bool TYoshi::isGreenYoshi()
{
    return this->mType == TYoshi::GREEN;
}

bool TYoshi::isGreenYoshi(TMario *gpMario)
{
    return gpMario->mYoshi->isGreenYoshi();
}

bool TYoshi::isGreenYoshiMounted()
{
    return this->isGreenYoshi() && this->mState == TYoshi::MOUNTED;
}

bool TYoshi::isGreenYoshiMounted(TMario *gpMario)
{
    return gpMario->mYoshi->isGreenYoshiMounted();
}

bool TYoshi::isGreenYoshiAscendingWater(TMario *gpMario)
{
    if (!(gpMario->mAttributes.mIsWater) ||
        !gpMario->mController->isPressed(TMarioGamePad::A) ||
        !gpMario->mYoshi->isGreenYoshiMounted())
    {
        return false;
    }
    else
    {
        return true;
    }
}