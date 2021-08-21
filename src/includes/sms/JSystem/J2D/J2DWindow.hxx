#pragma once

#include "J2DPane.hxx"
#include "../JUT/JUTRect.hxx"

class J2DWindow : public J2DPane {
public:
    JUTRect mFillRect;
    u32 _FC[0x40 / 4];
};