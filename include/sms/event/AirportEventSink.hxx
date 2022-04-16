#pragma once

#include "sms/event/MapEventSinkInPollutionReset.hxx"

class TAirportEventSink : public TMapEventSinkInPollutionReset
{
    public:
    TAirportEventSink(const char *);
    virtual ~TAirportEventSink();

    virtual void loadAfter();
    virtual s32 watch();
    virtual s32 control();
    virtual void finishControl();

    u32 _64;
    u32 _68;
};