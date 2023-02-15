#pragma once

#include <Dolphin/GX.h>

#include <JSystem/JAudio/JAISound.hxx>
#include <JSystem/JDrama/JDRActor.hxx>
#include <JSystem/JUtility/JUTColor.hxx>

class TDarknessEffect : public JDrama::TViewObj {
public:
    static JDrama::TNameRef *instantiate() { return new TDarknessEffect("TDarknessEffect"); }

    TDarknessEffect(const char *name) : TViewObj(name) {}

    void load(JSUMemoryInputStream &in) override;
    void perform(u32 flags, JDrama::TGraphics *graphics) override {}
};