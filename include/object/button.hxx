#pragma once

#include <BetterSMS/module.hxx>
#include <SMS/MapObj/MapObjGeneral.hxx>

class TButtonSwitch : public TMapObjGeneral {
protected:
    enum class State : u8 {
        STATE_NORMAL,
        STATE_PRESSING,
        STATE_PRESSED,
        STATE_RISING
    };

public:
    BETTER_SMS_FOR_CALLBACK static JDrama::TNameRef *instantiate() {
        return new TButtonSwitch("TButtonSwitch");
    }

    TButtonSwitch(const char *name);
    ~TButtonSwitch() override = default;

    void load(JSUMemoryInputStream &in) override;
    void control() override;
    void initMapCollisionData() override;
    void initMapObj() override;
    void setGroundCollision() override;

protected:
    bool checkMarioRiding(TMario *player);
    bool checkMarioPounding(TMario *player);
    void swapToPressed();
    void swapToUnpressed();
    void playPressedAnim();
    void playUnpressedAnim();

private:
    State mState;
    u32 mFlagID;
    s32 mPressedSoundID;
    bool mNeedsPound;
};

extern ObjData buttonSwitchData;