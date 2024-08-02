#pragma once

#include <Dolphin/types.h>
#include <SMS/MapObj/MapObjBall.hxx>

#include <BetterSMS/module.hxx>

class TCannonBall : public TMapObjBall {
public:
    BETTER_SMS_FOR_CALLBACK static JDrama::TNameRef *instantiate() {
        return new TCannonBall("TCannonBall");
    }

    TCannonBall(const char *name)
        : TMapObjBall(name), m_water_surface(nullptr), m_water_surface_y(0.0f), m_was_in_air(false) {}
    ~TCannonBall() override = default;

    void loadAfter() override {
        TMapObjBall::loadAfter();
        mStateFlags.asU32 |= 0x100000;  // Enable grabbing
    }

    Mtx44 *getTakingMtx() override;

    void checkGroundCollision(TVec3f *out) override;

    void touchGround(TVec3f *out) override;
    void touchWall(TVec3f *out, TBGWallCheckRecord *record) override;
    void touchRoof(TVec3f *out) override;
    void touchWaterSurface() override;

    s32 getLivingTime() const override { return 0x7FFFFFFF; }
    s32 getFlushTime() const override { return 0; }
    void kicked() override {}

private:
    const TBGCheckData *m_water_surface;
    f32 m_water_surface_y;
    bool m_was_in_air;
};

extern ObjData cannonBallData;