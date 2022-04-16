#pragma once

#include "sms/event/MapEventSink.hxx"

class TDolpicEventBiancoGate : public TMapEventSink
{
    public:
    TDolpicEventBiancoGate(const char *);
    virtual ~TDolpicEventBiancoGate();

    virtual void loadAfter();
    virtual bool isFinishedAll() const;
    virtual s32 watch();
    virtual s32 control();

    u32* _20;
    f32 _24;
};