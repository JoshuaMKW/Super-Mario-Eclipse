#include <SMS/Enemy/SpineEnemy.hxx>
#include <SMS/Manager/FlagManager.hxx>
#include <SMS/Map/Map.hxx>
#include <SMS/raw_fn.hxx>

#include "player.hxx"
#include <BetterSMS/module.hxx>

static bool removeRocketCollisionUnlessFly_(TMap *map, TBGWallCheckRecord *record,
                                            TSpineEnemy *rocket) {
    if (rocket->mSpineBase->mNerveCurrent != (TNerveBase<TLiveActor> *)0x8040dabc) {
        return false;
    }
    return map->isTouchedWallsAndMoveXZ(record);
}

static bool removeRocketCollisionUnlessFly(TMap *map, TBGWallCheckRecord *record) {
    TSpineEnemy *rocket;
    SMS_FROM_GPR(30, rocket);

    return removeRocketCollisionUnlessFly_(map, record, rocket);
}
SMS_PATCH_BL(0x8010B150, removeRocketCollisionUnlessFly);

static bool isRocketLaunchWithStabilize() {
    TMarioGamePad *controller = gpMarioAddress->mController;

    SME::Player::PlayerState *state = SME::Player::getEclipseData(gpMarioAddress);

    if (gpMarioAddress->mFluddUsageState == 0) {
        state->mRocketTimer += 1;
    } else {
        state->mRocketTimer = 0;
    }

    return (controller->mMeaning & 0x400) && state->mRocketTimer > 12;
}
SMS_PATCH_BL(0x8010AB9C, isRocketLaunchWithStabilize);
SMS_WRITE_32(0x8010ABA0, 0x2C030000);
SMS_WRITE_32(0x8010ABA4, 0x60000000);
SMS_WRITE_32(0x8010ABA8, 0x60000000);
SMS_WRITE_32(0x8010ABAC, 0x60000000);