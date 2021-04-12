#pragma once

#include "sms/event/MapEventSink.hxx"

class TMapEventSinkInPollution : public TMapEventSink
{
    public:
    virtual ~TMapEventSinkInPollution();

    virtual void loadAfter();
    virtual s32 watch();
};