#pragma once

#include <Dolphin/types.h>
#include <SMS/MapObj/MapObjBall.hxx>

#include <BetterSMS/module.hxx>

class TPizzaBox : public TMapObjBall {
public:
    BETTER_SMS_FOR_CALLBACK static JDrama::TNameRef *instantiate() {
        return new TPizzaBox("TPizzaBox");
    }

    TPizzaBox(const char *name)
        : TMapObjBall(name) {}
    ~TPizzaBox() override = default;

    void loadAfter() override {
        TMapObjBall::loadAfter();
        mStateFlags.asU32 |= 0x100000;  // Enable grabbing
        mShadowRadius = 100.0f;
    }

    void control() override;

    void calcRootMatrix() override;
    void touchWaterSurface() override;
    void thrown() override;

    s32 getLivingTime() const override { return 0x7FFFFFFF; }
    s32 getFlushTime() const override { return 0; }
    void kicked() override {}

private:
    f32 mRotationalVelocity;
};

extern ObjData pizzaBoxData;