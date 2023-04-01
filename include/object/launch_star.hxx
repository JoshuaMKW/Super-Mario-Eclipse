#pragma once

#include <Dolphin/types.h>
#include <JSystem/JDrama/JDRGraphics.hxx>
#include <JSystem/JSupport/JSUMemoryStream.hxx>
#include <SMS/MapObj/MapObjRail.hxx>
#include <SMS/Player/Mario.hxx>
#include <SMS/Strategic/LiveActor.hxx>

#include <BetterSMS/module.hxx>

constexpr u32 PlayerLaunchStarWait   = 0xF00021C0;
constexpr u32 PlayerLaunchStarLaunch = 0xF00021C1;

class TLaunchStarObj : public TMapObjGeneral {
public:
    BETTER_SMS_FOR_CALLBACK static JDrama::TNameRef *instantiate() {
        return new TLaunchStarObj("LaunchStarObj");
    }

    TLaunchStarObj(const char *name);
    virtual ~TLaunchStarObj();

    virtual void load(JSUMemoryInputStream &) override;
    virtual void perform(u32, JDrama::TGraphics *) override;
    virtual void control() override;
    virtual void loadAfter() override;
    virtual void calcRootMatrix() override;

private:
    bool attractPlayer(TMario *player);
    void launchPlayer(TMario *player);
    void releasePlayer(TMario *player);
    void playCameraDemo(const char *fp, CPolarSubCamera *camera);
    void assignPlayerToRail(TMario *player, TGraphWeb *rail);

    void playIdleAnim();
    void playSpinAnim();

    const char *mTravelRail;
    const char *mCameraDemo;
    f32 mTravelSpeed;

    u32 mInactiveTimer;
    u32 mLaunchTimer;
    TMario *mPlayer;
};

extern ObjData launchStarData;