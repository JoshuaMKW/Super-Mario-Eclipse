#pragma once

#include <BetterSMS/module.hxx>
#include <Dolphin/types.h>
#include <SMS/Manager/MarioParticleManager.hxx>
#include <SMS/MapObj/MapObjGeneral.hxx>
#include <SMS/MapObj/MapObjInit.hxx>

class TKeyChest : public TMapObjGeneral {
public:
    BETTER_SMS_FOR_CALLBACK static JDrama::TNameRef *instantiate() {
        return new TKeyChest("TKeyChest");
    }

    TKeyChest(const char *name);
    ~TKeyChest() override = default;

    void control() override;
    void touchPlayer(THitActor *) override;
    bool receiveMessage(THitActor *, u32) override;

    void playIdleAnim();
    void playOpenAnim();
    void spawnShine();

private:
    bool mOpening;
    bool mHasFinishedSounds;
};

extern ObjData keyChestData;
