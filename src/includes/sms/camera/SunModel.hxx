#pragma once

#include "sms/JSystem/JDrama.hxx"
#include "sms/JSystem/J3D/J3DAnimation.hxx"

class TSunModel : public JDrama::TActor {
public:
    TSunModel(bool, const char *name);
    virtual ~TSunModel();
    
    virtual void load(JSUMemoryInputStream &stream) override;
    virtual void perform(u32, JDrama::TGraphics *) override;

    void calcDispRatioAndScreenPos();
    void calcOtherFPosFromCenterAndRadius(JGeometry::TVec2<f32> *, const JGeometry::TVec2<f32> &, f32);
    void getZBufValue();

    u32 _44[3];
    J3DFrameCtrl mFrameCtrl; // 0x0050
    u32 _64[0x1C / 4];
    u8 mBlindingStrength;    // 0x0080
    u32 _84[0x20 / 4];
    JGeometry::TVec2<s16> _B4[11]; // 0x00B4
    JGeometry::TVec2<f32> _F8[11]; // 0x00F8
    u32 _150[0x5C / 4];
    u8 _1AC; //END
};