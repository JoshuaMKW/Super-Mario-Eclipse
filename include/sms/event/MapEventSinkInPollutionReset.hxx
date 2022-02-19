#pragma once

#include "sms/event/MapEventSinkInPollution.hxx"

class TMapEventSinkInPollutionReset : public TMapEventSinkInPollution
{
    public:
    virtual ~TMapEventSinkInPollutionReset();

    virtual void loadAfter();
    virtual void makeBuildingRecovered(int);
};