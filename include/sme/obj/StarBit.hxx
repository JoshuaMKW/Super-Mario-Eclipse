#pragma once

#include "math.h"
#include "sms/actor/item/Coin.hxx"
#include "JDrama/JDRGraphics.hxx"
#include "JUT/JUTColor.hxx"
#include "J2D/J2DTexture.hxx"
#include "J3D/J3DTexture.hxx"
#include "J3D/J3DMaterial.hxx"
#include "JPA/JPABaseEmitter.hxx"

class TStarBit final : public TCoin {
    static const JUtility::TColor sStarBitColorArray[6];

public:
    TStarBit(const char *name);
    virtual ~TStarBit();

    virtual void perform(u32, JDrama::TGraphics *) override;
    virtual void control() override;
    virtual void touchWater(THitActor *) override;
    virtual void taken(THitActor *) override;

    JUtility::TColor getColor() const { return mColor; }
    void setColor(JUtility::TColor color) { mColor = color; }

    void setBouncing(bool active) { mIsBouncing = active; }
    void setMoving(bool active) { mIsMoving = active; }
    void setGlowAnmSpeed(f32 speed) { mGlowAnmSpeed = speed; }

    static JUtility::TColor getColorFromIndex(int index) {
        index = Clamp(index, 0, sizeof(sStarBitColorArray));
        return sStarBitColorArray[index];
    }

private:
    bool mIsBouncing;
    bool mIsMoving;
    f32 mBounceSpeedY;
    JPABaseEmitter *mGlowEffect;
    f32 mGlowSize;
    f32 mGlowAnmSpeed;
    JUtility::TColor mColor;
};