#pragma once

#include <BetterSMS/module.hxx>
#include <SMS/MapObj/MapObjGeneral.hxx>

class TElevatorObject : public TMapObjGeneral {


public:
    BETTER_SMS_FOR_CALLBACK static JDrama::TNameRef *instantiate() {
        return new TElevatorObject("TElevatorObject");
    }

    TElevatorObject(const char *name);
    ~TElevatorObject() override = default;

    void load(JSUMemoryInputStream &in) override;
    void control() override;
    void initMapCollisionData() override;
    void initMapObj() override;
    void setGroundCollision() override;

protected:
 
    void playAnim();
   
private:
    int flagId;
    bool hasPlayed;
 
};

extern ObjData elevatorObjectData;