#pragma once

#include "JDrama/JDRGraphics.hxx"
#include "JDrama/JDRViewObj.hxx"
#include "JUT/JUTColor.hxx"

class TSelectGrad : public JDrama::TViewObj {
public:
    TSelectGrad();
    virtual ~TSelectGrad();

    virtual void perform(u32, JDrama::TGraphics *) override;

    void setStageColor(u8 stageID);

    u32 _10;
    u32 _14;
    u32 _18;
    JUtility::TColor mColorX1;
    JUtility::TColor mColorX2;
};