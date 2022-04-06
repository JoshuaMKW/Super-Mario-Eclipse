#pragma once

#include "JGeometry.hxx"
#include "J3D/J3DJoint.hxx"
#include "sms/event/MapEvent.hxx"

class TDolpicEventRiccoMammaGate : public TMapEvent
{
    public:
    TDolpicEventRiccoMammaGate(const char *);
    virtual ~TDolpicEventRiccoMammaGate();

    virtual void loadAfter();
    virtual bool isFinished() const;
    virtual u32 watch();
    virtual s32 control();

    J3DJoint *mJoint; // _20
    u32 _24;
    u32 _28;
    u32 mSomeFlag; // seems to be a boolean flag in TFlagManager
    u8 _30;
    u8 _31;
    u8 _32;
    u8 _33;
    f32 _34;
    u32 _38;
    u32 _3C;
    u32 _40;
    u32 _44;
    TVec3f mEventPosition; // _48
    TVec3f _54;
    f32 _60;
};