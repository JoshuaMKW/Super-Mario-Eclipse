#pragma once

#include "types.h"
#include "sms/JSystem/JDrama.hxx"
#include "sms/JSystem/J3D/J3DModel.hxx"

class M3UMtxCalcSetInfo
{
};

class M3UModel
{
public:
    virtual void changeMtxCalcAnmTransform(int, u8);
    virtual void changeAnmTexPattern(int, u8);
    virtual void setMtxCalc(const M3UMtxCalcSetInfo &);
    virtual void updateIn();
    virtual void updateOut();
    virtual void entryIn();
    virtual void entryOut();

    void perform(u32, JDrama::TGraphics *);
    void updateInMotion();
    void updateInTexPatternAnm();

    void *_00[2];      //0x0000
    J3DModel *mModel;  //0x0008
    u32 _01[0x30 / 4]; //0x000C
    u32 mLightID;      // _3C
    u8 _40;
    u8 _41;         // padding?
    u8 _42;         // ^^
    u8 _43;         // ^^
    u32 mLightType; // _44
};