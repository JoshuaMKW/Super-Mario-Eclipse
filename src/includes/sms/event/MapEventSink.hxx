#pragma once

#include "sms/event/MapEvent.hxx"

class TMapEventSink : public TMapEvent
{
    public:
    TMapEventSink(char const *);
    virtual ~TMapEventSink();

    virtual void load(JSUMemoryInputStream &stream);
    virtual bool isFinishedAll() const;
    virtual s32 watch();
    virtual void startContorl();
    virtual s32 control();
    virtual void finishControl();
    virtual u32* getBuilding(int) const;
    virtual void initBuilding(int, JSUMemoryInputStream &);
    virtual void initWithBuildingNum(JSUMemoryInputStream &);
    virtual void rising();
    virtual f32 getSinkOffsetY() const;
    virtual void makeBuildingRecovered(int);

    u32 _20; // defined in map binary
    u32 _24;
    s32 _28;
    u32 _2C;
    u32* _30;
    f32 _34;
    f32 _38;
    f32 _3C;
    u32 _40;
    u32 _44;
    u32 _48;
    u32 _4C;
    JGeometry::TVec3<f32>* _50; // array of something, count is this + 0x20
    void* _54;
    void* _58;
    void* _5C;
    void* _60;
};