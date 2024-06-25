#pragma once

#include <SMS/MapObj/MapObjGeneral.hxx>
#include <BetterSMS/module.hxx>

class TJizoStone : public TMapObjGeneral {
public:
    BETTER_SMS_FOR_CALLBACK static JDrama::TNameRef *instantiate() {
        return new TJizoStone("TJizoStone");
    }

    TJizoStone(const char *name);
    ~TJizoStone() override = default;

    void touchWater(THitActor *actor) override;
    void initMapCollisionData() override;
    void initMapObj() override;
    void setGroundCollision() override;

protected:
    void playBoingAnim();
};

extern ObjData jizoStoneData;