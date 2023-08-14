#pragma once

#include <Dolphin/math.h>

#include <JSystem/J3D/J3DMaterial.hxx>
#include <JSystem/J3D/J3DTexture.hxx>
#include <JSystem/JDrama/JDRGraphics.hxx>
#include <JSystem/JParticle/JPABaseEmitter.hxx>
#include <JSystem/JUtility/JUTColor.hxx>
#include <JSystem/JUtility/JUTTexture.hxx>
#include <SMS/MoveBG/Coin.hxx>

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