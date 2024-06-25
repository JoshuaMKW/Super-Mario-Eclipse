#pragma once

#include <BetterSMS/module.hxx>
#include <SMS/MapObj/MapObjGeneral.hxx>

class TPearlCracked : public TMapObjGeneral {
protected:
    enum class State : u8 {
        STATE_NORMAL,
        STATE_BROKEN,
        STATE_RESTING,
    };

public:
    BETTER_SMS_FOR_CALLBACK static JDrama::TNameRef *instantiate() {
        return new TPearlCracked("TPearlCracked");
    }

    TPearlCracked(const char *name);
    ~TPearlCracked() override = default;

    void control() override;
    void initMapCollisionData() override;
    void initMapObj() override;
    void setGroundCollision() override;

protected:
    bool checkMarioRiding(TMario *player);
    bool checkMarioPounding(TMario *player);
    void swapToCracked();
    void playFractureAnim();
    void spawnShine();

private:
    State mState;
};

extern ObjData pearlData;