#pragma once

#include <BetterSMS/module.hxx>
#include <Dolphin/types.h>
#include <SMS/Manager/MarioParticleManager.hxx>
#include <SMS/MapObj/MapObjGeneral.hxx>
#include <SMS/MapObj/MapObjInit.hxx>

#define MESSAGE_KEY_OPEN_CHEST 100

class TFollowKey : public TMapObjGeneral {
public:
    enum class State {
        KEY_IDLE,
        KEY_FOLLOW,
        KEY_OPEN_CHEST,
        KEY_USED,
    };

public:
    BETTER_SMS_FOR_CALLBACK static JDrama::TNameRef *instantiate() {
        return new TFollowKey("TFollowKey");
    }

    TFollowKey(const char *name);
    ~TFollowKey() override = default;

    void loadAfter() override;
    void control() override;
    bool receiveMessage(THitActor *, u32) override;

protected:
    void playIdleAnim();

    void calcOpenChestSeq(THitActor *actor);
    void calcUsedSeq(THitActor *actor);
    void followActor(THitActor *actor);

private:
    State mKeyState;
    THitActor *mFollowActor;
    THitActor *mChestActor;
    TVec3f mPrevPos;
    size_t mUnlockSoundFrame;
};

extern ObjData followKeyData;
