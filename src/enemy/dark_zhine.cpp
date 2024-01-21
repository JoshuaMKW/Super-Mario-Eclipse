#include <BetterSMS/player.hxx>
#include <SMS/Camera/CameraShake.hxx>
#include <SMS/Manager/MarioParticleManager.hxx>
#include <SMS/Manager/RumbleManager.hxx>
#include <SMS/Map/MapCollisionData.hxx>
#include <SMS/MarioUtil/MathUtil.hxx>
#include <SMS/MarioUtil/TexUtil.hxx>

#include "enemy/dark_zhine.hxx"

extern TVec3f *gpMarioPos;

static TNerveZhineSleep s_zhine_nerve_sleep;
static TNerveZhineWake s_zhine_nerve_wake;
static TNerveZhineFly s_zhine_nerve_fly;
static TNerveZhinePound s_zhine_nerve_pound;
static TNerveZhineRoll s_zhine_nerve_roll;
static TNerveZhineRise s_zhine_nerve_rise;

static s32 s_contact_time = -1;

#define ZHINE_ANM_IDX_FLY   0
#define ZHINE_ANM_IDX_RISE  1
#define ZHINE_ANM_IDX_ROLL  2
#define ZHINE_ANM_IDX_SLEEP 3
#define ZHINE_ANM_IDX_WAKE  4

bool TNerveZhineSleep::execute(TSpineBase<TLiveActor> *spine) const {
    TDarkZhine *zhine = reinterpret_cast<TDarkZhine *>(spine->mTarget);

    if (SMS_IsMarioTouchGround4cm__Fv(gpMarioAddress) &&
        PSVECDistance(*gpMarioPos, zhine->mTranslation) < 1500.0f) {
        spine->pushNerve(&s_zhine_nerve_wake);
        return true;
    }

    zhine->setBckAnm(ZHINE_ANM_IDX_SLEEP);
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

    zhine->setBckAnm(ZHINE_ANM_IDX_WAKE);
    return false;
}

bool TNerveZhineFly::execute(TSpineBase<TLiveActor> *spine) const {
    TDarkZhine *zhine = reinterpret_cast<TDarkZhine *>(spine->mTarget);

    if ((spine->mNerveTimer % 50) == 25) {
        zhine->launchPolDrop();
    }

    zhine->setBckAnm(ZHINE_ANM_IDX_FLY);
    return false;
}

bool TNerveZhinePound::execute(TSpineBase<TLiveActor> *spine) const {
    TDarkZhine *zhine = reinterpret_cast<TDarkZhine *>(spine->mTarget);

    // Closer than 10cm
    if (fabsf(zhine->mTranslation.y - zhine->mGroundY) < 10.0f) {
        if (zhine->doGroundPound()) {
            spine->pushNerve(&s_zhine_nerve_roll);
            return true;
        }
    }

    return false;
}

bool TNerveZhineRoll::execute(TSpineBase<TLiveActor> *spine) const {
    if (spine->mNerveTimer > 1000) {
        spine->pushNerve(&s_zhine_nerve_rise);
        return true;
    }

    return false;
}

bool TNerveZhineRise::execute(TSpineBase<TLiveActor> *spine) const { return false; }

void TDarkZhine::init(TLiveManager *manager) {
    mLiveManager = manager;
    mLiveManager->manageActor(this);

    // Initialize the model
    mActorKeeper = new TMActorKeeper(mLiveManager, 15);
    mActorData   = mActorKeeper->createMActor("zhine.bmd", 0);
    mActorData->calc();
    mActorData->setLightType(1);

    // Initialize the pollution droplets
    for (size_t i = 0; i < ZHINE_POL_DROP_COUNT; ++i) {
        mPolDrops[i].mActor      = mActorKeeper->createMActor("zhine_osenball.bmd", 0);
        mPolDrops[i].mActorWhite = mActorKeeper->createMActor("zhine_osenball_white.bmd", 0);
    }

    initAnmSound();

    // Set dummy texture as the stage goop
    ResTIMG *goop_texture_res = reinterpret_cast<ResTIMG *>(
        JKRFileLoader::getGlbResource("/scene/map/pollution/H_ma_rak.bti"));
    if (goop_texture_res) {
        SMS_ChangeTextureAll(mActorData->mModel->mModelData, "H_ma_rak_dummy", *goop_texture_res);
    }
}

void TDarkZhine::load(JSUMemoryInputStream &in) {}

void TDarkZhine::perform(u32 flags, JDrama::TGraphics *graphics) {
    TSpineEnemy::perform(flags, graphics);

    if ((flags & 4)) {
        // Request a shadow
    }
}

bool TDarkZhine::receiveMessage(THitActor *sender, u32 message) {}

void TDarkZhine::control() {}

void TDarkZhine::bind() {}

void TDarkZhine::moveObject() {}

static const char *s_bas_name_table[] = {
    "/scene/zhine/bas/zhine_hit.bas",   "/scene/zhine/bas/zhine_osen.bas",
    "/scene/zhine/bas/zhine_roll.bas",  "/scene/zhine/bas/zhine_sleep.bas",
    "/scene/zhine/bas/zhine_sleep.bas",
};

const char **TDarkZhine::getBasNameTable() const { return s_bas_name_table; }

void TDarkZhine::launchPolDrop() {
    TBGPolDrop &drop = mPolDrops[mPolDropCurrent];
    mPolDropCurrent  = (mPolDropCurrent + 1) % ZHINE_POL_DROP_COUNT;

    TVec3f pol_velocity;
    SMSCalcJumpVelocityXZ(*gpMarioPos, mTranslation, 32.0f, 0.2f, &pol_velocity);

    drop.launch(mTranslation, pol_velocity);
}

bool TDarkZhine::doGroundPound() {
    // TODO: Determine wait time for UX
    if (true) {
        // Emit ground pound effects
        gpMarioParticleManager->emit(0x14, &mTranslation, 0, nullptr);
        gpMarioParticleManager->emit(0x13, &mTranslation, 0, nullptr);
        gpMarioParticleManager->emit(0x12, &mTranslation, 0, nullptr);

        // Rumble the controller
        SMSRumbleMgr->start(0x15, 0x1E, (f32 *)nullptr);

        // Shake the camera
        gpCameraShake->startShake(UnknownShake27, 1.0f);
    }

    return true;
}