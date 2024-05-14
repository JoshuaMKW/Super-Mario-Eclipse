#include "raw_fn.hxx"
#include "Enemy/BossPakkun.hxx"
#include "JParticle/JPAResourceManager.hxx"

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

TDarkZhine::TDarkZhine(const char *name)
    : TSpineEnemy(name), mBoundingPoint(0, 0, 0), mTarget(nullptr),
      mParams("/enemy/bossgesso.prm") {}

TDarkZhine::~TDarkZhine() {}

f32 TDarkZhine::getAngleToTarget() const {
    TVec3f zhineCoordinates = this->mTranslation;

    return atan2f(this->mTarget->mTranslation.x - zhineCoordinates.x,
                  this->mTarget->mTranslation.z - zhineCoordinates.z);
}

bool TDarkZhine::isTargetInRangeToHome(f32 r) const {
    return PSVECDistance(mBoundingPoint, mTarget->mTranslation) < r;
}

void TDarkZhine::advanceRollMovement(TPollutionManager *gpPollution) {

    // if (this->rollingTimer() < this->rollingTimerMax() / 2)
    //     this->mGroundSpeed =
    //         sigmoidCurve(this->rollingTimer(), 0.0f, this->maxSpeed() * this->speedMultiplier(),
    //                      300.0f, 0.04f * this->accelerationRate());
    // else
    //     this->mGroundSpeed = sigmoidCurve(this->rollingTimer() - this->rollingTimerMax() / 2, 0,
    //                                       this->maxSpeed() * this->speedMultiplier(), 300.0f,
    //                                       -0.04f * this->accelerationRate());
    //
    // if (this->mGroundSpeed > this->maxSpeed())
    //     this->mGroundSpeed = this->maxSpeed();
    // else if (this->mGroundSpeed < 0.0f)
    //     this->mGroundSpeed = 0.0f;
    //
    // this->mTranslation.x += (this->mGroundSpeed * sinf(angleToRadians(this->mRotation.y)));
    // this->mTranslation.z += (this->mGroundSpeed * cosf(angleToRadians(this->mRotation.y)));
    // this->mSpeed.x = (this->mGroundSpeed * sinf(angleToRadians(this->mRotation.y)));
    // this->mSpeed.z = (this->mGroundSpeed * cosf(angleToRadians(this->mRotation.y)));
    //
    // stamp__17TPollutionManagerFUsffff(gpPollution, 1, this->mTranslation.x, this->mTranslation.y,
    //                                   this->mTranslation.z, 400.0f);
}

TDarkZhine::PoundingState TDarkZhine::advanceDropAttack(TPollutionManager *gpPollution,
                                                        TMario *player) {

    // if (this->poundingStatus() == TDarkZhine::DROPPING) {
    //     this->mGravity = 1.0f;
    //     this->mSpeed.x = 0.0f;
    //     this->mSpeed.z = 0.0f;
    //     this->mSpeed.y *= this->mSpeedMultiplier;
    //
    //     if (this->mTranslation.y - this->mFloorBelow->mMaxHeight < 10.0f &&
    //         this->mTranslation.y >= this->mFloorBelow->mMinHeight) {
    //
    //         startShake__12TCameraShakeF16EnumCamShakeModef(gpCameraShake, 0x27, 1.0f);
    //         startSoundActor__Q214MSoundSESystem8MSoundSEFUlPC3VecUlPP8JAISoundUlUc(
    //             6158, this->mTranslation, 0, 0, 0, 4);
    //         start__9RumbleMgrFiP3Vec(gpPad1RumbleMgr, 8, this->mTranslation);
    //         stamp__17TPollutionManagerFUsffff(gpPollution, 1, this->mTranslation.x,
    //                                           this->mFloorBelow->mMaxHeight, this->mTranslation.z,
    //                                           mStampRadius);
    //
    //         if (this->mGoopLevel > (0xFF - 0x30)) {
    //             this->mGoopLevel = 0xFF;
    //         } else {
    //             this->mGoopLevel += 0x20;
    //         }
    //
    //         this->setPoundingStatus(TDarkZhine::SHOCKING);
    //         this->setStatusTimer(this->shockingTimerMax());
    //     }
    // } else if (this->poundingStatus() == TDarkZhine::SHOCKING) {
    //     if (!(player->mState & static_cast<u32>(TMario::State::AIRBORN)) &&
    //         this->isTargetInRangeToHome(this->mTranslation, this->shockRadius()) &&
    //         (player->mState != static_cast<u32>(TMario::State::KNCK_LND) &&
    //          player->mState != 0x4045C)) {
    //         decHP__6TMarioFi(player, 1);
    //         changePlayerStatus__6TMarioFUlUlb(player, static_cast<u32>(TMario::State::KNCK_LND), 0,
    //                                           0);
    //     }
    //
    //     if (this->statusTimer() <= 0) {
    //         this->setPoundingStatus(TDarkZhine::GROUNDROLL);
    //         this->setStatusTimer(this->rollingTimerMax());
    //     }
    // } else if (this->poundingStatus() == TDarkZhine::GROUNDROLL) {
    //     if (this->mTranslation.y - this->mFloorBelow->mMaxHeight > 100) {
    //         this->setPoundingStatus(TDarkZhine::RISING);
    //         this->mGravity *= (-1 * this->risingRate() * this->speedMultiplier());
    //     } else if (this->rollingTimer() > 0) {
    //         this->advanceRollMovement(gpPollution);
    //         if (!(this->statusTimer() % 15)) {
    //             if (this->mGoopLevel == 0xFF) {
    //                 this->mGoopLevel = 0xFF;
    //             } else {
    //                 this->mGoopLevel += 1;
    //             }
    //         }
    //     } else {
    //         this->setPoundingStatus(TDarkZhine::RISING);
    //         this->mTranslation.y += 1;
    //         this->mGravity *= (-1 * this->risingRate() * this->speedMultiplier());
    //     }
    // } else if (this->poundingStatus() == TDarkZhine::RISING) {
    //     f32 averageFloorHeight =
    //         (this->mFloorBelow->mMaxHeight + this->mFloorBelow->mMinHeight) / 2;
    //     this->mSpeed.x = 0.0f;
    //     this->mSpeed.z = 0.0f;
    //
    //     if (this->mTranslation.y - averageFloorHeight > 500.0f &&
    //         this->mTranslation.y > this->boundingPoint().y + 100.0f) {
    //
    //         this->mGravity *= (-1 * this->risingRate() * this->speedMultiplier());
    //         this->setPoundingStatus(TDarkZhine::INACTIVE);
    //     }
    // } else {
    //     this->setPoundingStatus(TDarkZhine::INACTIVE);
    // }
    // this->setStatusTimer(this->statusTimer() - 1);
    // return this->poundingStatus();
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
void TDarkZhine::init(TLiveManager * manager) {
    // OSReport("%s\n", manager->mObjAry[0]->mKeyName);
    mLiveManager = manager;
    mLiveManager->manageActor(this);
    mActorKeeper = new TMActorKeeper(mLiveManager, 0xf);
    mActorData   = mActorKeeper->createMActor("bosspaku_model.bmd", 0xf);
    initAnmSound();
    return;
}

/*
TDarkZhine *TDarkZhine::destroy()
{
    //use r6 and up
    *(u32 *)0x803B2A94 = 0x80074E54; //restore eye damage vtable entry
    *(f32 *)0x8041014C = 800.0f;
    *(f32 *)0x80410150 = 3000.0f;
    return this;
}
*/

// 0x80005004
// u32 TDarkZhine::cleanFromSpineBase(u32 *gpNerveBGEye, TSpineBase<TLiveActor> *gpSpineBase) {
//     TDarkZhine *gpDarkZhine = (TDarkZhine *)gpSpineBase->mTarget;
//     if (gpDarkZhine->cleaningTimer() < gpDarkZhine->framesToCleanOnce()) {
//         gpDarkZhine->setCleaningTimer(gpDarkZhine->cleaningTimer() + 1);
//         return 0;
//     } else {
//         gpDarkZhine->setCleaningTimer(0);
//         return execute__17TNerveBGEyeDamageCFP24TSpineBase_1(gpNerveBGEye, gpSpineBase);
//     }
// }

// 0x800764CC
/*
void canUtilizeTentacles(u32 *TBGTentacle, u32 *unk1, u32 *JDramaGraphics)
{
    TDarkZhine *gesso;
    __asm { mr zhine, r31 };

    if (!gesso->isDarkZhine())
    {
        testPerform__Q26JDrama8TViewObjFUlPQ26JDrama9TGraphics(TBGTentacle,
unk1, JDramaGraphics);
    }
}
kmCall(0x800764CC, &canUtilizeTentacles);
*/

// 0x80005000
// void control_(TDarkZhine *thisZhine) { thisZhine->perform_(gpMarioAddress); }

// 0x800FFFAC = remove tentacles BLR, 0x800764CC perform tentacles?

// 0x802A9D54 = TBossGesso Class Size

TDarkZhineManager::TDarkZhineManager(const char *name) : TEnemyManager(name) {}

void TDarkZhineManager::load(JSUMemoryInputStream &inputStream) {
    TEnemyManager::load(inputStream);
}

void TDarkZhineManager::createModelData() {
    createModelDataArrayBase((TModelDataLoadEntry *)(0x8037befc), "/scene/darkzhine");
}

void TDarkZhineManager::createAnmData() {
    MActorAnmData* data = new MActorAnmData();
    data->init("/scene/darkzhine",nullptr);
    this->_20 = (u32)data;
}