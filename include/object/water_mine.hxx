#pragma once

#include <BetterSMS/module.hxx>
#include <Dolphin/types.h>
#include <SMS/Manager/MarioParticleManager.hxx>
#include <SMS/MapObj/MapObjGeneral.hxx>
#include <SMS/MapObj/MapObjInit.hxx>

// #define SEAMINE_COLLISION

class TWaterMine : public TMapObjGeneral {
public:
    BETTER_SMS_FOR_CALLBACK static JDrama::TNameRef *instantiate() {
        return new TWaterMine("TWaterMine");
    }

    TWaterMine(const char *name);
    ~TWaterMine() override = default;

    //void loadAfter() override;
    void control() override;
    bool receiveMessage(THitActor *, u32) override;
    //void touchPlayer(THitActor *) override;

    void calc() override;

    #if SEAMINE_COLLISION
    void initMapCollisionData() override;
    void initMapObj() override;
    void setGroundCollision() override;
    #endif

    bool isFragmentAnimComplete() const;
    void playFragmentAnim();

private:
    bool m_is_hit;
    bool m_is_dead;
};

extern ObjData waterMineData;
