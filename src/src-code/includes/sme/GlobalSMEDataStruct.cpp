#pragma once

#include "GlobalSMEDataStruct.hxx"

TMario *GlobalSMEDataStruct::getPlayerByID(u8 id) const
{
    if (id < this->PlayerData.mCurPlayerCount)
        return this->PlayerData.mPlayerArray[id];

    return nullptr;
}