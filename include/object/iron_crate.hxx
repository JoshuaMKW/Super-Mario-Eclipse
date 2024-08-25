#pragma once

#include <BetterSMS/module.hxx>
#include <Dolphin/types.h>
#include <SMS/Manager/MarioParticleManager.hxx>
#include <SMS/MapObj/MapObjGeneral.hxx>
#include <SMS/MapObj/MapObjInit.hxx>

class TIronCrate : public TMapObjGeneral {
public:
    BETTER_SMS_FOR_CALLBACK static JDrama::TNameRef *instantiate() {
        return new TIronCrate("TIronCrate");
    }

    TIronCrate(const char *name);
    ~TIronCrate() override = default;

    void makeMActors() override;
    void initMapCollisionData() override;
    void initMapObj() override;
    void setGroundCollision() override;

    void control() override;
    bool receiveMessage(THitActor *, u32) override;

    void calc() override;

    bool isFragmentAnimComplete() const;
    void playFragmentAnim();

private:
    MActor *m_model_datas[2];
    bool m_is_hit;
    bool m_is_dead;
};

extern ObjData ironCrateData;
