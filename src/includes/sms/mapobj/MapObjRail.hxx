#pragma once

#include "MapObjGeneral.hxx"
#include "JDrama/JDRGraphics.hxx"
#include "JSU/JSUMemoryStream.hxx"
#include "sms/graph/GraphWeb.hxx"
#include "sms/graph/GraphTracer.hxx"

class TRailMapObj : public TMapObjBase {
public:
    enum RailStatus {
        ACTIVE,
        UNK_1,
        RESETTING
    };

    TRailMapObj(const char *);
    virtual ~TRailMapObj();

    virtual void load(JSUMemoryInputStream &) override;
    virtual void perform(u32, JDrama::TGraphics *) override;
    virtual void control() override;
    virtual u32 getShadowType() override;
    virtual void initMapObj() override;
    virtual void setGroundCollision() override;

    virtual bool calcRecycle();
    virtual void resetPosition();
    virtual void readRailFlag();

    bool checkMarioRiding();
    void initGraphTracer(TGraphWeb *);
    void moveToNextNode(f32);
    void resetStep(f32);

    TGraphTracer *mGraphTracer;
    s32 mDistanceToNext; // ??
    u32 _140; // flags
    f32 _144;
    u8 mRailStatus;
    s16 mContextTimer; // default = 180
    u8 mLastRailStatus; // ?
};