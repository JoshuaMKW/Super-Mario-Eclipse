#pragma once

#include "types.h"
#include "sms/JSystem/JKR/JKRHeap.hxx"
#include "sms/JSystem/JUT/JUTGamePad.hxx"
#include "sms/game/Application.hxx"

#include "funcs.hxx"

namespace SME::Util
{
    s8 getCharacterID(JUTGamePad *controller = nullptr);
    const char *getStageName(TApplication *gpApplication);
    void *loadArchive(char *path, JKRHeap *heap);
}