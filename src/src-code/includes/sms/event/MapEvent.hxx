#pragma once

#include "sms/event/EventWatcher.hxx"

class TMapEvent : public TEventWatcher
{
    public:
    TMapEvent(char const *);
    virtual ~TMapEvent();

    virtual void load(JSUMemoryInputStream &stream);
    virtual void perform(u32, JDrama::TGraphics *);
    virtual bool isFinishedAll() const;
    virtual void startControl();
    virtual void finishControl();
    virtual u32* getBuilding(int) const;

    u32 mEventState; // _18
    u32* _1C;
};