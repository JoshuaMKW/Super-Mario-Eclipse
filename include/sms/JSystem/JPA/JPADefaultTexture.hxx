#pragma once

#include "types.h"
#include "GX.h"

class JPADefaultTexture {
public:
    void initialize();

    void *mData;
    GXTexObj mTexObj;
};