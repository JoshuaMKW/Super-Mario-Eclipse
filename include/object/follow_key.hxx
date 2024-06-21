#pragma once

#include <Dolphin/types.h>
#include <SMS/MapObj/MapObjGeneral.hxx>
#include <SMS/MapObj/MapObjInit.hxx>
#include <BetterSMS/module.hxx>
#include <SMS/Manager/MarioParticleManager.hxx>
class TFollowKey : public TMapObjGeneral
{
public:
    BETTER_SMS_FOR_CALLBACK static JDrama::TNameRef *instantiate() {
        return new TFollowKey("TFollowKey");
    }

    TFollowKey(const char *name);
    virtual ~TFollowKey();

    virtual void kill() override;
   

    virtual void control() override;
   
  
     virtual void touchPlayer(THitActor *) override;
  
     virtual void playIdleAnim();

    bool mIsFollowing;
    TMario *mFollowActor;
    TVec3f prevPos;
};

extern ObjData followKeyData;

extern TMarioParticleManager *gpMarioParticleManager;

