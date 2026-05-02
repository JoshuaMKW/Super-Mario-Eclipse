#pragma once

#include <Dolphin/math.h>

#include <JSystem/J3D/J3DMaterial.hxx>
#include <JSystem/J3D/J3DTexture.hxx>
#include <JSystem/JDrama/JDRGraphics.hxx>
#include <JSystem/JParticle/JPABaseEmitter.hxx>
#include <JSystem/JUtility/JUTColor.hxx>
#include <JSystem/JUtility/JUTTexture.hxx>
#include <SMS/MoveBG/Coin.hxx>

#include <BetterSMS/module.hxx>

class TStarBit final : public TCoin {
    static const JUtility::TColor sStarBitColorArray[6];

public:
    BETTER_SMS_FOR_CALLBACK static JDrama::TNameRef *instantiate() {
        return new TStarBit("StarBit");
    }

    TStarBit(const char *name);
    ~TStarBit() override;

    void appear() override;
    void bind() override;
    void load(JSUMemoryInputStream &in) override;
    void loadAfter() override;
    void makeMActors() override;
    void perform(u32, JDrama::TGraphics *) override;
    bool receiveMessage(THitActor *sender, u32 message) override;
    void moveObject() override;
    void touchGround(TVec3f *) override;
    void touchWater(THitActor *) override;
    void taken(THitActor *) override;

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
    bool mIsAttracting;
    TVec3f *mAttractionPoint;
    f32 mBounceSpeedY;
    JPABaseEmitter *mGlowEffect;
    f32 mGlowSize;
    f32 mGlowAnmSpeed;
    int mGlowTick;
    JUtility::TColor mColor;
};

extern ObjData starBitData;