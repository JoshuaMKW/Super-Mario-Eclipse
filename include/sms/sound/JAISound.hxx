#pragma once

#include "types.h"

struct JAIMoveParaSet
{
    f32 _0;
    f32 _4;
    f32 _8;
    u32 *_C;
};

class JAISound
{
public:
    JAISound();

    virtual void setSeDistanceParameters();
    virtual void setSeDistanceVolume(u8);
    virtual void setSeDistancePan(u8);
    virtual void setSeDistancePitch(u8);
    virtual void setSeDistanceFxmix(u8);
    virtual void setSeDistanceFir(u8);
    virtual void setSeDistanceDolby(u8);
    virtual void setSePositionDopplar();
    virtual void setPositionDopplarCommon(u32);
    virtual void setDistanceVolumeCommon(f32, u8);
    virtual void setDistancePanCommon();

    bool initMoveParameter(JAIMoveParaSet *, f32, u32);
    u32 getSeCategoryNumber();
    u32 getSwBit();
    u32 checkSwBit(u32);
    u8 getInfoPriority();
    void claerMainSoundPPointer();
    void release();
    void stop(u32);
    void setVolume(f32, u32, u8);
    void setPan(f32, u32, u8);
    void setPitch(f32, u32, u8);
    void setFxmix(f32, u32, u8);

    u8 _0;
    u8 _1;
    u8 _2;
    u8 _3;
    u32 _4;
    u32 mID; // _8
    u8 _C[0x18 - 0xC];
    u32 _18;
    u32 _1C;
    u32 _20;
    u8 _24[0x34 - 0x24];
    u32 *_34;          // sound pointer, maybe?
    u32 *mStreamParam; // _38
    void *_3C;
    u32 *vTable; // _40
};