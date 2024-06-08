#include "raw_fn.hxx"
#include "BetterSMS/libs/constmath.hxx"
#include "Camera/CameraShake.hxx"
#include "Enemy/BossPakkun.hxx"

#include <BetterSMS/module.hxx>

#include "enemy/dark_zhine.hxx"

/*
VTABLE

.long 0x00000000, 0x00000000, 0x8007BC50, 0x80007D70
.long 0x8003C39C, 0x802FA6F4, 0x802FA6F8, 0x802FA6FC
.long 0x80075C44* 0x00000000, 0x00000000, 0x8007C2C0
.long 0x803370C0, 0x8033720C, 0x80037214, 0x8033721C
.long 0x80337220, 0x80337228, 0x8033722C, 0x80337230
.long 0x*/

static TNerveZhineSleep s_zhine_nerve_sleep;
static TNerveZhineWake s_zhine_nerve_wake;
static TNerveZhineFly s_zhine_nerve_fly;
static TNerveZhinePound s_zhine_nerve_pound;
static TNerveZhineRoll s_zhine_nerve_roll;
static TNerveZhineRise s_zhine_nerve_rise;


TDarkZhine::TDarkZhine(const char *name)
    : TSpineEnemy(name), mBoundingPoint(0, 0, 0), mTarget(nullptr),
      mParams("/enemy/bossgesso.prm") {
}

TDarkZhine::~TDarkZhine() {
}

f32 TDarkZhine::getAngleToTarget() const {
    
    return atan2f(gpMarioPos->x - mTranslation.x, gpMarioPos->z - mTranslation.z);
}

bool TDarkZhine::isTargetInRangeToHome(f32 r) const {
    return PSVECDistance(mBoundingPoint, mTarget->mTranslation) < r;
}

void TDarkZhine::advanceRollMovement(TPollutionManager *gpPollution) {

}

TDarkZhine::PoundingState TDarkZhine::advanceDropAttack(TPollutionManager *gpPollution,
                                                        TMario *player) {

    return INACTIVE;
}

void TDarkZhine::advanceGoopDroplet() {
    TBGPolDrop *pollutionDrop = this->mPollutionDrop;
    TVec3f launchVelocity(this->mSpeed.x, this->mSpeed.y / 2, this->mSpeed.z);

    if (pollutionDrop->mStatus == TBGPolDrop::DEAD) {
        launch__10TBGPolDropFRCQ29JGeometry8TVec3_f(this->mPollutionDrop, this->mTranslation,
                                                    launchVelocity);
    } else {
        move__10TBGPolDropFv(pollutionDrop);
    }
}

void TDarkZhine::perform(u32 flags, JDrama::TGraphics *graphics) {
    TSpineEnemy::perform(flags, graphics);
    // mPollutionDrop->perform(flags,graphics);
}

// 0x80079B44
void TDarkZhine::init(TLiveManager *manager) {
    mLiveManager = manager;
    mLiveManager->manageActor(this);
    mActorKeeper = new TMActorKeeper(mLiveManager, 0xf);
    mActorData   = mActorKeeper->createMActor("bosspaku_model.bmd", 0xf);
    initAnmSound();

    mSpineBase->setNerve(&s_zhine_nerve_sleep);

    return;
}


bool TNerveZhineSleep::execute(TSpineBase<TLiveActor> *spine) const {
    TDarkZhine *zhine = reinterpret_cast<TDarkZhine *>(spine->mTarget);

    if (SMS_IsMarioTouchGround4cm__Fv(gpMarioAddress) &&
        PSVECDistance(*gpMarioPos, zhine->mTranslation) < 1500.0f) {
        spine->pushNerve(&s_zhine_nerve_wake);
        return true;
    }

    return false;
}


bool TNerveZhineWake::execute(TSpineBase<TLiveActor> *spine) const {
    TDarkZhine *zhine = reinterpret_cast<TDarkZhine *>(spine->mTarget);

    bool can_mario_fall = gpMarioAddress->mState != TMario::STATE_KNCK_GND &&
                          SMS_IsMarioTouchGround4cm__Fv(gpMarioAddress);
    if (spine->mNerveTimer < 20 && can_mario_fall) {
        gpMarioAddress->changePlayerStatus(TMario::STATE_KNCK_GND, 0, false);
    }

    if (zhine->mActorData->isCurAnmAlreadyEnd(MActor::BCK)) {
        spine->pushNerve(&s_zhine_nerve_rise);
        return true;
    }

    // zhine->setBckAnm(ZHINE_ANM_IDX_WAKE);
    return false;
}

// 0x80005000
// void control_(TDarkZhine *thisZhine) { thisZhine->perform_(gpMarioAddress); }
bool TNerveZhineRise::execute(TSpineBase<TLiveActor> *spine) const {
    TDarkZhine *zhine = reinterpret_cast<TDarkZhine *>(spine->mTarget);

    if (spine->mNerveTimer == 0) {
        // TODO: Use asFlags
        zhine->mStateFlags.asU32 = zhine->mStateFlags.asU32 | 0x10;
        zhine->mStateFlags.asU32 = zhine->mStateFlags.asU32 | 0x80;
        // target->changeBck(0xD);
    }

    zhine->mTranslation.y += 5.0f;
    if (3800.0f < zhine->mTranslation.y) {
        // zhine->mTranslation.y = zhine->_10C;
        if (zhine->mGraphTracer) {
            spine->pushNerve(&s_zhine_nerve_fly);
        }
        return true;
    }
    return false;
}

bool TNerveZhineFly::execute(TSpineBase<TLiveActor> *spine) const {
    TDarkZhine *zhine = reinterpret_cast<TDarkZhine *>(spine->mTarget);

    if ((spine->mNerveTimer % 50) == 25) {
        // zhine->launchPolDrop();
        OSReport("Attack\n");
    }

    if (spine->mNerveTimer == 300) {
        spine->pushNerve(&s_zhine_nerve_pound);
        return true;
    }

    // zhine->setBckAnm(ZHINE_ANM_IDX_FLY);
    return false;
}

bool TNerveZhinePound::execute(TSpineBase<TLiveActor> *spine) const {
    TDarkZhine *zhine = reinterpret_cast<TDarkZhine *>(spine->mTarget);
    if (spine->mNerveTimer == 0) {
        zhine->mStateFlags.asU32 = zhine->mStateFlags.asU32 & 0xffffffef;
        zhine->mStateFlags.asU32 = zhine->mStateFlags.asU32 | 0x80;
    }

    if ((zhine->mStateFlags.asU32 & 0x80U) == 0) {
        // zhine->changeBck(0x8);
        gpCameraShake->startShake(UnknownShakeF, 1.0f);
        // zhine->rumblePad(2, zhine->mTranslation);
        spine->pushNerve(&s_zhine_nerve_roll);
        return true;
    }
    return false;
}

bool TNerveZhineRoll::execute(TSpineBase<TLiveActor> *spine) const {
    TDarkZhine *zhine = reinterpret_cast<TDarkZhine *>(spine->mTarget);


    zhine->mRotationalVelocity.y = 0.2;
    // zhine->calcTurnSpeedToReach()


    // zhine->mSpeed.x = 10.0f * sinf(zhine->mRotation.y);
    // zhine->mSpeed.z = 10.0f * cosf(zhine->mRotation.y);
    return false;
}

TDarkZhineManager::TDarkZhineManager(const char *name)
    : TEnemyManager(name) {
}

void TDarkZhineManager::load(JSUMemoryInputStream &inputStream) {
    TEnemyManager::load(inputStream);
}

void TDarkZhineManager::createModelData() {
    createModelDataArrayBase(anim_data, "/scene/darkzhine");
}

void TDarkZhineManager::createAnmData() {
    MActorAnmData *data = new MActorAnmData();
    data->init("/scene/darkzhine", nullptr);
    this->_20 = (u32)data;
}
