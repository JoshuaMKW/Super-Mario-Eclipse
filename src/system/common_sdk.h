#pragma once

#if !defined(SME_BUILD_KURIBO) && !defined(SME_BUILD_KAMEK) &&                 \
    !defined(SME_BUILD_KAMEK_INLINE)
#define SME_BUILD_KURIBO
#endif

#if !defined(SME_BUILD_KURIBO)
#include "Kamek/sdk/kamek_sdk.h"
#else
#include "Kuribo/sdk/kuribo_sdk.h"
#endif