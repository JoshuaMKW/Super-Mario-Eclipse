#include "enemy/dark_zhine.hxx"
#include <SMS/Camera/CameraShake.hxx>
#include <SMS/MSound/MSBGM.hxx>
#include <SMS/MSound/MSModBgm.hxx>
#include <SMS/MSound/MSoundSESystem.hxx>
#include <SMS/Manager/MarioParticleManager.hxx>
#include <SMS/Manager/ModelWaterManager.hxx>
#include <SMS/Manager/RumbleManager.hxx>
#include <SMS/Map/MapCollisionData.hxx>
#include <SMS/MarioUtil/DrawUtil.hxx>
#include <SMS/MarioUtil/MathUtil.hxx>
#include <SMS/MarioUtil/TexUtil.hxx>
#include <SMS/System/TargetArrow.hxx>

#include <BetterSMS/libs/constmath.hxx>
#include <BetterSMS/music.hxx>
#include <BetterSMS/player.hxx>

#include "enemy/dark_zhine.hxx"

extern TVec3f *gpMarioPos;

static TNerveZhineSleep s_zhine_nerve_sleep;
static TNerveZhineWake s_zhine_nerve_wake;
static TNerveZhineFly s_zhine_nerve_fly;
static TNerveZhineTeleport s_zhine_nerve_teleport;
static TNerveZhineKnockBack s_zhine_nerve_knockback;
static TNerveZhineCrash s_zhine_nerve_crash;
static TNerveZhineStun s_zhine_nerve_stun;
static TNerveZhinePound s_zhine_nerve_pound;
static TNerveZhineIdle s_zhine_nerve_idle;
static TNerveZhineRoll s_zhine_nerve_roll;
static TNerveZhineRise s_zhine_nerve_rise;
static TNerveZhineRecover s_zhine_nerve_recover;
static TNerveZhinePreKill s_zhine_nerve_prekill;
static TNerveZhineKill s_zhine_nerve_kill;
static TNerveZhineAngry s_zhine_nerve_angry;

static bool s_has_bgm_started = false;
static bool s_first_pound     = true;

#define ZHINE_ANM_IDX_FLY   0
#define ZHINE_ANM_IDX_RISE  1
#define ZHINE_ANM_IDX_ROLL  2
#define ZHINE_ANM_IDX_SLEEP 3
#define ZHINE_ANM_IDX_STUN  4
#define ZHINE_ANM_IDX_WAKE  5

#define ZHINE_FLY_HEIGHT    750.0f
#define ZHINE_GRAVITY       0.98f
#define ZHINE_TURN_POWER    0.02f
#define ZHINE_MAX_FRAMERATE 5.0f

#define ZHINE_MODEL_TRANS_Y 50.0f

static f32 normalizeAngle(f32 angle) {
    angle = fmodf__3stdFff(angleToRadians(angle) + M_PI, 2 * M_PI);
    if (angle < 0)
        angle += 2 * M_PI;
    return radiansToAngle(angle - M_PI);
}

static f32 normalizeRadians(f32 angle) {
    angle = fmodf__3stdFff(angle + M_PI, 2 * M_PI);
    if (angle < 0)
        angle += 2 * M_PI;
    return angle - M_PI;
}

static f32 getAngleDelta(f32 a, f32 b) {
    f32 delta = normalizeAngle(a) - normalizeAngle(b);
    if (delta > 180.0f) {
        delta -= 360.0f;
    } else if (delta < -180.0f) {
        delta += 360.0f;
    }
    return delta;
}

static f32 getRadiansDelta(f32 a, f32 b) {
    f32 delta = normalizeRadians(a) - normalizeRadians(b);
    if (delta > M_PI) {
        delta -= 2 * M_PI;
    } else if (delta < -M_PI) {
        delta += 2 * M_PI;
    }
    return delta;
}

static bool isMarioVulnerableToShock(TMario *player) {
    return player->mState != TMario::STATE_KNCK_GND && player->mState != 0x4045C &&
           SMS_IsMarioTouchGround4cm__Fv(player);
}

class AtomMessageViewer;

static AtomMessageViewer *s_message_queue[10] = {0};
static size_t s_message_queue_head            = 0;
static size_t s_message_queue_size            = 0;
static s32 s_message_duration                 = -1;

static void pushMessage(AtomMessageViewer *message) {
    if (s_message_queue_size < 10) {
        s_message_queue[(s_message_queue_head + s_message_queue_size) % 10] = message;
        s_message_queue_size++;
    }
}

static AtomMessageViewer *peekMessage() {
    if (s_message_queue_size > 0) {
        return s_message_queue[s_message_queue_head];
    }
    return nullptr;
}

static void popMessage() {
    if (s_message_queue_size > 0) {
        s_message_queue_head = (s_message_queue_head + 1) % 10;
        s_message_queue_size--;
    }
}

class AtomMessageViewer {
public:
    AtomMessageViewer() = delete;
    AtomMessageViewer(u32 messageID, s32 duration)
        : m_message_id(messageID), m_duration(duration), m_is_used(false) {}

    u32 id() const { return m_message_id; }
    s32 duration() const { return m_duration; }

    void reset() { m_is_used = false; }

    void show() {
        if (m_is_used) {
            return;
        }
        pushMessage(this);
        m_is_used = true;
    }

private:
    u32 m_message_id;
    s32 m_duration;
    bool m_is_used;
};

static AtomMessageViewer s_msg_sleep(104, 460);
static AtomMessageViewer s_msg_pound_a(105, 500);
static AtomMessageViewer s_msg_pound_b(106, 560);
static AtomMessageViewer s_msg_pound_c(107, 560);
static AtomMessageViewer s_msg_attack(108, 520);
static AtomMessageViewer s_msg_last_point(109, 560);
static AtomMessageViewer s_msg_finish(110, 580);

bool TNerveZhineSleep::execute(TSpineBase<TLiveActor> *spine) const {
    TDarkZhine *zhine  = static_cast<TDarkZhine *>(spine->mTarget);
    zhine->mGravity    = ZHINE_GRAVITY;
    zhine->mSpeed      = TVec3f::zero();
    zhine->mRotation.x = 0.0f;
    zhine->mRotation.z = 0.0f;
    zhine->setForwardSpeed(0.0f);
    zhine->setModelOffsetY(ZHINE_MODEL_TRANS_Y);
    zhine->setInvincible(true);

    J3DFrameCtrl *ctrl = zhine->mActorData->getFrameCtrl(MActor::BCK);
    zhine->mRotation.y = zhine->calcNextAngle(ZHINE_TURN_POWER, 0.0f);

    if (spine->mNerveTimer == 0) {
        zhine->mActorData->setBck("zhine_resting");
    }

    if (spine->mNerveTimer > 120) {
        s_msg_sleep.show();
    }

    if (gpMSound->gateCheck(MSD_SE_BS_TELESA_BREATHE) && (spine->mNerveTimer % 120) == 0) {
        MSoundSE::startSoundActor(MSD_SE_BS_TELESA_BREATHE, zhine->mTranslation, 0, nullptr, 0, 4);
    }

    if (PSVECDistance(*gpMarioPos, zhine->mTranslation) < 1500.0f) {
        spine->pushNerve(&s_zhine_nerve_rise);
        spine->pushNerve(&s_zhine_nerve_wake);
        return true;
    }

    return false;
}

bool TNerveZhineWake::execute(TSpineBase<TLiveActor> *spine) const {
    TDarkZhine *zhine  = static_cast<TDarkZhine *>(spine->mTarget);
    zhine->mGravity    = ZHINE_GRAVITY;
    zhine->mSpeed      = TVec3f::zero();
    zhine->mRotation.x = 0.0f;
    zhine->mRotation.z = 0.0f;
    zhine->setForwardSpeed(0.0f);
    zhine->setModelOffsetY(Min(zhine->getModelOffsetY() + 1.0f, ZHINE_MODEL_TRANS_Y));
    zhine->setInvincible(true);

    J3DFrameCtrl *ctrl = zhine->mActorData->getFrameCtrl(MActor::BCK);

    if (spine->mNerveTimer == 0) {
        zhine->mActorData->setBck("zhine_notice");
        zhine->setStunned(false);

        if (!s_has_bgm_started) {
            MSBgm::startBGM(BGM_BOSS);
            s_has_bgm_started = true;
        }

        if (gpMSound->gateCheck(MSD_SE_BS_TELESA_V_LAUGH2)) {
            MSoundSE::startSoundActor(MSD_SE_BS_TELESA_V_LAUGH2, zhine->mTranslation, 0, nullptr, 0,
                                      4);
        }
    }

    if (zhine->mActorData->isCurAnmAlreadyEnd(MActor::BCK)) {
        bool illegal_out;
        zhine->doGroundPound(false, &illegal_out);

        return true;
    }

    return false;
}

bool TNerveZhineFly::execute(TSpineBase<TLiveActor> *spine) const {
    TDarkZhine *zhine  = static_cast<TDarkZhine *>(spine->mTarget);
    zhine->mGravity    = 0.0f;
    zhine->mRotation.x = 0.0f;
    zhine->mRotation.z = 0.0f;

    J3DFrameCtrl *ctrl = zhine->mActorData->getFrameCtrl(MActor::BCK);

    if (spine->mNerveTimer == 0) {
        zhine->mActorData->setBck("zhine_fly");
    }

    if ((spine->mNerveTimer % 50) == 25) {
        zhine->launchPolDrop();
    }

    if (spine->mNerveTimer > 600) {
        spine->pushNerve(&s_zhine_nerve_pound);
        return true;
    }

    zhine->mRotation.y = zhine->calcNextAngle(ZHINE_TURN_POWER, 0.0f);
    zhine->mSpeed      = {sinf(angleToRadians(zhine->mRotation.y)), 0.0f,
                          cosf(angleToRadians(zhine->mRotation.y))};
    zhine->mSpeed.scale(zhine->isHardMode() ? 16.0f : 8.0f);
    zhine->setForwardSpeed(zhine->isHardMode() ? 16.0f : 8.0f);

    return false;
}

bool TNerveZhineTeleport::execute(TSpineBase<TLiveActor> *spine) const {
    TDarkZhine *zhine  = static_cast<TDarkZhine *>(spine->mTarget);
    J3DFrameCtrl *ctrl = zhine->mActorData->getFrameCtrl(MActor::BCK);

    zhine->mGravity    = 0.0f;
    zhine->mSpeed      = TVec3f::zero();
    zhine->mRotation.x = 0.0f;
    zhine->mRotation.z = 0.0f;
    zhine->setForwardSpeed(0.0f);

    if (spine->mNerveTimer == 0) {
        zhine->mActorData->setBck("zhine_fly");
    }

    zhine->mActorData->setFrameRate(
        Min(5.0f + (f32)(spine->mNerveTimer % 100) / 20.0f, ZHINE_MAX_FRAMERATE), MActor::BCK);

    if ((spine->mNerveTimer % 100) == 0) {
        gpMarioParticleManager->emitAndBindToPosPtr(296, &zhine->mTranslation, 1, nullptr);
        zhine->moveToTargetXZ();

        if (zhine->mFloorBelow != &TMapCollisionData::mIllegalCheckData) {
            zhine->mTranslation.y = zhine->mGroundY + ZHINE_FLY_HEIGHT;
        }

        zhine->launchPolDrop();
        gpMarioParticleManager->emitAndBindToPosPtr(297, &zhine->mTranslation, 1, nullptr);
    }

    if (spine->mNerveTimer > 500) {
        spine->pushNerve(&s_zhine_nerve_pound);
        return true;
    }

    return false;
}

bool TNerveZhinePound::execute(TSpineBase<TLiveActor> *spine) const {
    TDarkZhine *zhine  = static_cast<TDarkZhine *>(spine->mTarget);
    zhine->mGravity    = 0.0f;
    zhine->mSpeed.x    = 0.0f;
    zhine->mSpeed.z    = 0.0f;
    zhine->mRotation.x = 0.0f;
    zhine->mRotation.z = 0.0f;
    zhine->setForwardSpeed(0.0f);

    zhine->setModelOffsetY(Min(zhine->getModelOffsetY() + 1.0f, ZHINE_MODEL_TRANS_Y));

    if (spine->mNerveTimer == 0) {
        if (gpMSound->gateCheck(MSD_SE_BS_TELESA_V_LAUGH1)) {
            MSoundSE::startSoundActor(MSD_SE_BS_TELESA_V_LAUGH1, zhine->mTranslation, 0, nullptr, 0,
                                      4);
        }
    }

    zhine->mActorData->setFrameRate(
        Min(1.0f + (f32)(spine->mNerveTimer) / 12.0f, ZHINE_MAX_FRAMERATE), MActor::BCK);

    if (spine->mNerveTimer > 60) {
        zhine->mGravity = 2.0f;

        zhine->mActorData->getFrameCtrl(MActor::BCK)->mAnimState = J3DFrameCtrl::ONCE;

        bool illegal_out;
        if (zhine->doGroundPound(true, &illegal_out)) {
            spine->pushNerve(&s_zhine_nerve_idle);
            return true;
        }

        if (illegal_out) {
            zhine->reset();
            spine->pushNerve(&s_zhine_nerve_sleep);
            return true;
        }
    }

    return false;
}

bool TNerveZhineIdle::execute(TSpineBase<TLiveActor> *spine) const {
    TDarkZhine *zhine  = static_cast<TDarkZhine *>(spine->mTarget);
    zhine->mGravity    = ZHINE_GRAVITY;
    zhine->mSpeed      = TVec3f::zero();
    zhine->mRotation.x = 0.0f;
    zhine->mRotation.z = 0.0f;
    zhine->setForwardSpeed(0.0f);

    zhine->setInvincible(false);

    if (spine->mNerveTimer == 0) {
        zhine->mActorData->setBck("zhine_resting");
        zhine->mActorData->setFrameRate(2.0f, MActor::BCK);
    }

    if (gpMSound->gateCheck(MSD_SE_BS_TELESA_BREATHE) && (spine->mNerveTimer % 60) == 0) {
        MSoundSE::startSoundActor(MSD_SE_BS_TELESA_BREATHE, zhine->mTranslation, 0, nullptr, 0, 4);
    }

    if (spine->mNerveTimer > 300) {
        if (s_first_pound) {
            s_msg_pound_a.show();
            s_msg_pound_b.show();
        }
        spine->pushNerve(&s_zhine_nerve_roll);
        return true;
    }

    return false;
}

bool TNerveZhineKnockBack::execute(TSpineBase<TLiveActor> *spine) const {
    TDarkZhine *zhine = static_cast<TDarkZhine *>(spine->mTarget);
    zhine->mGravity   = ZHINE_GRAVITY;

    if (spine->mNerveTimer == 0) {
        zhine->mRotation.y = angleToRadians(zhine->getAngleToTarget());
    }

    zhine->setModelOffsetY(Max(zhine->getModelOffsetY() - 5.0f, 0.0f));

    zhine->mRotation.x = normalizeAngle(zhine->mRotation.x + 2.0f);
    zhine->mRotation.z = normalizeAngle(zhine->mRotation.z + 4.0f);

    if (fabsf(zhine->mTranslation.y - zhine->mGroundY) < 10.0f && zhine->mSpeed.y <= 0.0f) {
        if (gpMSound->gateCheck(0x28E1)) {
            MSoundSE::startSoundActor(0x28E1, zhine->mTranslation, 0, nullptr, 0, 4);
        }
        zhine->doWorldShake(0.1f);
        spine->pushNerve(&s_zhine_nerve_stun);
        return true;
    }

    TBGWallCheckRecord record{};
    record.mCollideMax  = 1;
    record.mIgnoreFlags = 0;
    record.mPosition    = zhine->mTranslation;
    record.mRadius      = zhine->mReceiveRadius;
    if (gpMapCollisionData->checkWalls(&record) == 0) {
        return false;
    }

    f32 prev_angle = zhine->mRotation.y;

    zhine->calcReflectingVelocity(record.mWalls[0], 1.0f, zhine->mSpeed);
    zhine->calcReflectingAngle(record.mWalls[0], &zhine->mRotation.y);

    f32 forward_speed = TVec3f(zhine->mSpeed.x, 0.0f, zhine->mSpeed.z).magnitude();
    zhine->setForwardSpeed(forward_speed);

    f32 new_angle = zhine->mRotation.y;

    f32 contact_speed =
        fabsf(forward_speed * sinf(angleToRadians(getAngleDelta(prev_angle, new_angle)) * 0.5f));
    f32 contact_speed_blend = Min(contact_speed / 30.0f, 1.0f);
    if (contact_speed_blend > 0.40f) {
        if (gpMSound->gateCheck(0x28E1)) {
            MSoundSE::startSoundActor(0x28E1, zhine->mTranslation, 0, nullptr, 0, 4);
        }
        zhine->doWorldShake(0.1f * contact_speed_blend);
        gpMarioParticleManager->emitAndBindToPosPtr(11, &zhine->mTranslation, 0, nullptr);
    }

    return false;
}

bool TNerveZhineCrash::execute(TSpineBase<TLiveActor> *spine) const {
    TDarkZhine *zhine = static_cast<TDarkZhine *>(spine->mTarget);
    zhine->mGravity   = ZHINE_GRAVITY;

    zhine->setModelOffsetY(0.0f);

    if (spine->mNerveTimer == 0) {
        zhine->setStunned(false);

        if (zhine->isDefeated()) {
            MSBgm::stopBGM(BGM_BOSSHANA_2ND3RD, 10);
            Music::pauseSong(0.2f);
        } else if (zhine->isHardMode()) {
            MSBgm::stopBGM(BGM_BOSS, 10);
            Music::pauseSong(0.2f);
        }

        if (gpMSound->gateCheck(MSD_SE_BS_TELESA_SPICY_HIT)) {
            MSoundSE::startSoundActor(MSD_SE_BS_TELESA_SPICY_HIT, zhine->mTranslation, 0, nullptr,
                                      0, 4);
        }

        s_msg_attack.show();
    }

    if (fabsf(zhine->mTranslation.y - zhine->mGroundY) < 10.0f && zhine->mSpeed.y <= 0.0f) {
        if (gpMSound->gateCheck(0x28E1)) {
            MSoundSE::startSoundActor(0x28E1, zhine->mTranslation, 0, nullptr, 0, 4);
        }
        zhine->setHostile(true);
        zhine->doWorldShake(0.1f);
        if (zhine->isDefeated()) {
            spine->pushNerve(&s_zhine_nerve_prekill);
        } else if (zhine->isHardMode()) {
            spine->pushNerve(&s_zhine_nerve_angry);
            spine->pushNerve(&s_zhine_nerve_recover);
        } else {
            spine->pushNerve(&s_zhine_nerve_roll);
            spine->pushNerve(&s_zhine_nerve_recover);
        }
        return true;
    }

    TBGWallCheckRecord record{};
    record.mCollideMax  = 1;
    record.mIgnoreFlags = 0;
    record.mPosition    = zhine->mTranslation;
    record.mRadius      = zhine->mReceiveRadius;
    if (gpMapCollisionData->checkWalls(&record) == 0) {
        return false;
    }

    f32 prev_angle = zhine->mRotation.y;

    zhine->calcReflectingVelocity(record.mWalls[0], 0.8f, zhine->mSpeed);
    zhine->calcReflectingAngle(record.mWalls[0], &zhine->mRotation.y);

    f32 forward_speed = TVec3f(zhine->mSpeed.x, 0.0f, zhine->mSpeed.z).magnitude();
    zhine->setForwardSpeed(forward_speed);

    f32 new_angle = zhine->mRotation.y;

    f32 contact_speed =
        fabsf(forward_speed * sinf(angleToRadians(getAngleDelta(prev_angle, new_angle)) * 0.5f));
    f32 contact_speed_blend = Min(contact_speed / 30.0f, 1.0f);
    if (contact_speed_blend > 0.40f) {
        if (gpMSound->gateCheck(0x28E1)) {
            MSoundSE::startSoundActor(0x28E1, zhine->mTranslation, 0, nullptr, 0, 4);
        }
        zhine->doWorldShake(0.1f * contact_speed_blend);
        gpMarioParticleManager->emitAndBindToPosPtr(11, &zhine->mTranslation, 0, nullptr);
    }

    return false;
}

bool TNerveZhineRecover::execute(TSpineBase<TLiveActor> *spine) const {
    TDarkZhine *zhine = static_cast<TDarkZhine *>(spine->mTarget);
    zhine->setHostile(true);
    zhine->setInvincible(true);

    MActor *actor = zhine->mActorData;

    if (spine->mNerveTimer == 0) {
        actor->setFrameRate(0.8f, MActor::BCK);
    }

    if (spine->mNerveTimer == 120) {
        zhine->mActorData->setBck("zhine_recover_a");

        if (gpMSound->gateCheck(MSD_SE_BS_TELESA_V_LAUGH2)) {
            MSoundSE::startSoundActor(MSD_SE_BS_TELESA_V_LAUGH2, zhine->mTranslation, 0, nullptr, 0,
                                      4);
        }
    }

    if (spine->mNerveTimer >= 120) {
        bool is_first_anim = zhine->mActorData->checkCurAnm("zhine_recover_a", MActor::BCK);
        bool is_anim_done  = zhine->mActorData->isCurAnmAlreadyEnd(MActor::BCK);

        if (is_first_anim && is_anim_done) {
            zhine->mActorData->setBck("zhine_recover_b");
        }

        if (!is_first_anim) {
            if (is_anim_done) {
                return true;
            }

            f32 cur_frame  = actor->getFrameCtrl(MActor::BCK)->mCurFrame;
            f32 num_frames = actor->getFrameCtrl(MActor::BCK)->mNumFrames;
            f32 f          = num_frames != 0 ? cur_frame / num_frames : 1.0f;

            f32 model_y = lerp<f32>(zhine->getModelOffsetY(), ZHINE_MODEL_TRANS_Y, f);
            zhine->setModelOffsetY(model_y);
        }
    }

    return false;
}

bool TNerveZhineStun::execute(TSpineBase<TLiveActor> *spine) const {
    TDarkZhine *zhine  = static_cast<TDarkZhine *>(spine->mTarget);
    zhine->mGravity    = ZHINE_GRAVITY;
    zhine->mSpeed.y    = 0.0f;
    zhine->mRotation.x = 0.0f;
    zhine->mRotation.z = 0.0f;
    zhine->setHostile(false);
    zhine->setModelOffsetY(Min(zhine->getModelOffsetY() + 1.0f, 20.0f));

    f32 forward_speed = zhine->getForwardSpeed() * 0.98f;
    forward_speed     = Max(forward_speed - 0.1f, 0.0f);
    zhine->setForwardSpeed(forward_speed);

    zhine->mSpeed.normalize();
    zhine->mSpeed.scale(forward_speed);

    J3DFrameCtrl *ctrl = zhine->mActorData->getFrameCtrl(MActor::BCK);

    if (spine->mNerveTimer == 0) {
        zhine->mActorData->setBck("zhine_stun");
        zhine->setStunned(true);
        s_msg_pound_c.show();
    }

    if (spine->mNerveTimer > 700) {
        zhine->setStunned(false);
        zhine->setHostile(true);
        spine->pushNerve(&s_zhine_nerve_roll);
        spine->pushNerve(&s_zhine_nerve_recover);
        return true;
    }

    if (gpMSound->gateCheck(MSD_SE_EN_DENNOKO_JITABATA) && (spine->mNerveTimer % 20) == 0) {
        MSoundSE::startSoundActor(MSD_SE_EN_DENNOKO_JITABATA, zhine->mTranslation, 0, nullptr, 0,
                                  4);
    }

    return false;
}

bool TNerveZhineRoll::execute(TSpineBase<TLiveActor> *spine) const {
    TDarkZhine *zhine = static_cast<TDarkZhine *>(spine->mTarget);

    if (spine->mNerveTimer == 0) {
        zhine->mActorData->setBck("zhine_roll");
    }

    zhine->setModelOffsetY(Min(zhine->getModelOffsetY() + 5.0f, ZHINE_MODEL_TRANS_Y));

    gpPollution->stamp(1, zhine->mTranslation.x, zhine->mTranslation.y, zhine->mTranslation.z,
                       400.0f * ((f32)zhine->getPollutionAlpha() / 255.0f));

    f32 max_speed     = zhine->isHardMode() ? 45.0f : 30.0f;
    f32 max_framerate = ZHINE_MAX_FRAMERATE;
    if (spine->mNerveTimer < 80) {
        zhine->mActorData->setFrameRate(
            Min((max_speed / 30.0f) + (f32)spine->mNerveTimer / 16.0f, max_framerate), MActor::BCK);
        zhine->mRotation.y = zhine->calcNextAngle(ZHINE_TURN_POWER * 2.0f, 90.0f, true);
    } else if (spine->mNerveTimer <= 600) {
        if (zhine->isHardMode()) {
            zhine->mRotation.y = zhine->calcNextAngle(ZHINE_TURN_POWER, 90.0f, true);
        }
        zhine->mActorData->setFrameRate(max_framerate, MActor::BCK);
        zhine->setForwardSpeed(Min(max_speed, zhine->getForwardSpeed() + 1.0f));
    }

    s32 sound_repeat_rate = (s32)(15.0f / zhine->mActorData->getFrameCtrl(MActor::BCK)->mFrameRate);
    if (gpMSound->gateCheck(MSD_SE_BS_HANA_WALK_RUN) &&
        (spine->mNerveTimer % sound_repeat_rate) == 0) {
        MSoundSE::startSoundActor(MSD_SE_BS_HANA_WALK_RUN, zhine->mTranslation, 0, nullptr, 0, 4);

        gpMarioParticleManager->emitWithRotate(23, &zhine->mTranslation, 0,
                                               convertAngleFloatToS16(zhine->mRotation.y - 90.0f),
                                               0, 0, nullptr);
        gpMarioParticleManager->emitWithRotate(22, &zhine->mTranslation, 0,
                                               convertAngleFloatToS16(zhine->mRotation.y - 90.0f),
                                               0, 0, nullptr);
    }

    if (spine->mNerveTimer > 600) {
        f32 forward_speed = zhine->getForwardSpeed();
        forward_speed *= 0.98f;
        forward_speed = Max(forward_speed - 0.1f, 0.0f);
        zhine->setForwardSpeed(forward_speed);

        if (forward_speed <= 0.0f) {
            spine->pushNerve(&s_zhine_nerve_rise);
            return true;
        }

        f32 frame_rate = max_framerate * (forward_speed / max_speed);
        zhine->mActorData->setFrameRate(frame_rate, MActor::BCK);
    }

    f32 forward_speed = zhine->getForwardSpeed();
    zhine->mSpeed.x   = sinf(angleToRadians(zhine->mRotation.y - 90.0f)) * forward_speed;
    zhine->mSpeed.z   = cosf(angleToRadians(zhine->mRotation.y - 90.0f)) * forward_speed;

    TBGWallCheckRecord record{};
    record.mCollideMax  = 1;
    record.mIgnoreFlags = 0;
    record.mPosition    = zhine->mTranslation;
    record.mPosition.y += ZHINE_MODEL_TRANS_Y;
    record.mRadius = zhine->mReceiveRadius;
    if (gpMapCollisionData->checkWalls(&record) == 0) {
        return false;
    }

    f32 prev_angle = zhine->mRotation.y;

    zhine->calcReflectingVelocity(record.mWalls[0], 1.0f, zhine->mSpeed);
    zhine->calcReflectingAngle(record.mWalls[0], &zhine->mRotation.y);

    f32 new_angle = zhine->mRotation.y;

    f32 contact_speed =
        fabsf(forward_speed * sinf(angleToRadians(getAngleDelta(prev_angle, new_angle)) * 0.5f));
    f32 contact_speed_blend = contact_speed / max_speed;

    if (contact_speed_blend > 0.40f) {
        if (gpMSound->gateCheck(MSD_SE_EN_WANWAN_REFLECT)) {
            MSoundSE::startSoundActor(MSD_SE_EN_WANWAN_REFLECT, zhine->mTranslation, 0, nullptr, 0,
                                      4);
        }
        zhine->doWorldShake(0.2f * contact_speed_blend);
        if (isMarioVulnerableToShock(gpMarioAddress) &&
            zhine->getDistanceToTargetXZ() < 2000.0f * contact_speed_blend) {
            gpMarioAddress->changePlayerStatus(TMario::STATE_KNCK_GND, 0, false);
        }
        gpMarioParticleManager->emitAndBindToPosPtr(11, &zhine->mTranslation, 0, nullptr);
    }

    return false;
}

bool TNerveZhineRise::execute(TSpineBase<TLiveActor> *spine) const {
    TDarkZhine *zhine = static_cast<TDarkZhine *>(spine->mTarget);
    MActor *actor     = zhine->mActorData;

    zhine->setInvincible(true);
    zhine->setModelOffsetY(Max(zhine->getModelOffsetY() - 0.5f, 0.0f));

    if (spine->mNerveTimer == 0) {
        actor->setBck("zhine_rise");
    }

    f32 cur_frame  = actor->getFrameCtrl(MActor::BCK)->mCurFrame;
    f32 num_frames = actor->getFrameCtrl(MActor::BCK)->mNumFrames;
    f32 f          = num_frames != 0 ? cur_frame / num_frames : 1.0f;

    zhine->mTranslation.y = lerp<f32>(zhine->mGroundY, zhine->mGroundY + ZHINE_FLY_HEIGHT, f);
    zhine->mRotation.x    = lerp<f32>(zhine->mRotation.x, 0.0f, f);
    zhine->mRotation.y    = zhine->calcNextAngle(ZHINE_TURN_POWER, 0.0f, true);
    zhine->mRotation.z    = lerp<f32>(zhine->mRotation.z, 0.0f, f);

    if (actor->isCurAnmAlreadyEnd(MActor::BCK)) {
        spine->pushNerve(&s_zhine_nerve_fly);
        return true;
    }

    return false;
}

bool TNerveZhinePreKill::execute(TSpineBase<TLiveActor> *spine) const {
    TDarkZhine *zhine = static_cast<TDarkZhine *>(spine->mTarget);
    MActor *actor     = zhine->mActorData;

    zhine->mGravity = 0.0f;
    zhine->mSpeed   = TVec3f::zero();

    zhine->setInvincible(true);
    zhine->setModelOffsetY(Min(zhine->getModelOffsetY() + 1.0f, ZHINE_MODEL_TRANS_Y + 5.0f));

    if (spine->mNerveTimer == 0) {
        actor->setBck("zhine_dying");
        actor->setFrameRate(1.1f, MActor::BCK);

        if (gpMSound->gateCheck(MSD_SE_BS_TELESA_DOWN)) {
            MSoundSE::startSoundActor(MSD_SE_BS_TELESA_DOWN, zhine->mTranslation, 0, nullptr, 0, 4);
        }

        zhine->mRotation.x = 0.0f;
        zhine->mRotation.z = 0.0f;
    }

    f32 cur_frame  = actor->getFrameCtrl(MActor::BCK)->mCurFrame;
    f32 num_frames = actor->getFrameCtrl(MActor::BCK)->mNumFrames;
    f32 f          = num_frames != 0 ? cur_frame / num_frames : 1.0f;

    f32 base_scale        = zhine->mScale.magnitude();
    f32 particle_scale    = lerp<f32>(0.5f * base_scale, 1.5f * base_scale, f);
    f32 current_height    = lerp<f32>(zhine->mGroundY, zhine->mGroundY + 300.0f, f);
    zhine->mTranslation.y = current_height;
    zhine->mRotation.y    = zhine->calcNextAngle(ZHINE_TURN_POWER, 0.0f);

    if (actor->isCurAnmAlreadyEnd(MActor::BCK)) {
        spine->pushNerve(&s_zhine_nerve_kill);
        return true;
    }

    return false;
}

bool TNerveZhineKill::execute(TSpineBase<TLiveActor> *spine) const {
    TDarkZhine *zhine = static_cast<TDarkZhine *>(spine->mTarget);
    MActor *actor     = zhine->mActorData;

    zhine->mGravity = 0.0f;
    zhine->mSpeed   = TVec3f::zero();

    zhine->setModelOffsetY(ZHINE_MODEL_TRANS_Y + 5.0f);

    if (spine->mNerveTimer == 0) {
        actor->setBck("zhine_explode");

        // Drops
        {
            auto *particle = gpMarioParticleManager->emit(132, &zhine->mTranslation, 0, nullptr);
            if (particle) {
                particle->mSize1 = {4.0f, 4.0f, 4.0f};
            }
        }

        zhine->mStateFlags.asU32 |= 0x100;
        zhine->mStateFlags.asU32 &= ~0x8;
        zhine->spawnShine();

        s_msg_finish.show();
    }

    f32 cur_frame  = actor->getFrameCtrl(MActor::BCK)->mCurFrame;
    f32 num_frames = actor->getFrameCtrl(MActor::BCK)->mNumFrames;
    f32 f          = num_frames != 0 ? cur_frame / num_frames : 1.0f;

    if (actor->isCurAnmAlreadyEnd(MActor::BCK)) {

        zhine->setStunned(false);
        zhine->kill();
        return true;
    }

    return false;
}

bool TNerveZhineAngry::execute(TSpineBase<TLiveActor> *spine) const {
    TDarkZhine *zhine = static_cast<TDarkZhine *>(spine->mTarget);
    MActor *actor     = zhine->mActorData;

    zhine->setModelOffsetY(Min(zhine->getModelOffsetY() + 1.0f, ZHINE_MODEL_TRANS_Y));

    if (spine->mNerveTimer == 0) {
        MSBgm::startBGM(BGM_BOSSHANA_2ND3RD);
        (*(MSModBgm **)((u8 *)gpMSound + 0x9C))->changeTempo(0, 1);
        actor->setBck("zhine_angry");
        // actor->setFrameRate(0.2f, MActor::BCK);
    }

    f32 cur_frame  = actor->getFrameCtrl(MActor::BCK)->mCurFrame;
    f32 num_frames = actor->getFrameCtrl(MActor::BCK)->mNumFrames;
    f32 f          = num_frames != 0 ? cur_frame / num_frames : 1.0f;

    zhine->mRotation.x = lerp<f32>(zhine->mRotation.x, 0.0f, f);
    zhine->mRotation.y = zhine->calcNextAngle(ZHINE_TURN_POWER, 0.0f, true);
    zhine->mRotation.z = lerp<f32>(zhine->mRotation.z, 0.0f, f);

    if (actor->isCurAnmAlreadyEnd(MActor::BCK)) {
        spine->pushNerve(&s_zhine_nerve_roll);
        return true;
    }

    return false;
}

TDarkZhine::TDarkZhine(const char *name)
    : TSpineEnemy(name), m_is_hard_mode(false), m_pol_alpha(255), m_ground_point(), m_pol_drops(),
      m_pol_drop_index(0), m_pol_collect_timer(0), m_stunned(false), m_invincible(false),
      m_hostile(true), m_anm_index(0), m_bounding_point(), m_target(nullptr), m_health_points(3),
      m_damage_timer(0), m_forward_speed(0.0f), m_model_ofs_y(0.0f) {}

void TDarkZhine::init(TLiveManager *manager) {
    mLiveManager = manager;
    mLiveManager->manageActor(this);

    // Initialize the model
    mActorKeeper              = new TMActorKeeper(mLiveManager, 15);
    mActorKeeper->mModelFlags = 0x102F0000;
    mActorData                = mActorKeeper->createMActor("zhine.bmd", 0);
    mActorData->setLightType(1);
    mActorData->calc();
    mActorData->viewCalc();

    // Initialize the pollution droplets
    for (size_t i = 0; i < ZHINE_POL_DROP_COUNT; ++i) {
        m_pol_drops[i]              = new TBGPolDrop("PolDrop");
        m_pol_drops[i]->mActor      = mActorKeeper->createMActor("bgeso_osenball.bmd", 0);
        m_pol_drops[i]->mActorWhite = mActorKeeper->createMActor("bgeso_osenball_white.bmd", 0);
    }

    f32 attack_radius  = 120.0f;
    f32 receive_radius = 120.0f;
    initHitActor(0x8000010, 5, 0x81000000, attack_radius, attack_radius, receive_radius,
                 receive_radius);
    initAnmSound();

    mShadowRadius = 120.0f;
    mStateFlags.asU32 &= ~0x100;
    mStateFlags.asU32 |= 0x8;

    // Set dummy texture as the stage goop
    ResTIMG *goop_texture_res =
        static_cast<ResTIMG *>(JKRFileLoader::getGlbResource("/scene/map/pollution/H_ma_rak.bti"));
    if (goop_texture_res) {
        SMS_ChangeTextureAll(mActorData->mModel->mModelData, "H_ma_rak_dummy", *goop_texture_res);
    }

    mSpineBase->setNerve(&s_zhine_nerve_sleep);
    m_health_points = 3;
    m_model_ofs_y   = 90.0f;

    s_has_bgm_started = false;
}

void TDarkZhine::load(JSUMemoryInputStream &in) {
    TSpineEnemy::load(in);
    in.read(&m_is_hard_mode, 1);
    in.read(&m_pol_alpha, 1);
}

void TDarkZhine::perform(u32 flags, JDrama::TGraphics *graphics) {
    TSpineEnemy::perform(flags, graphics);

    for (size_t i = 0; i < ZHINE_POL_DROP_COUNT; ++i) {
        m_pol_drops[i]->perform(flags, graphics);
    }

    if ((flags & 1)) {
        checkMessagePool();
    }

    if ((flags & 2)) {
        if (isStunned()) {
            gpTargetArrow->mIsVisible = true;
            gpTargetArrow->setPos({mTranslation.x, mTranslation.y + 250.0f, mTranslation.z});
        } else {
            gpTargetArrow->mIsVisible = false;
        }
    }

    if ((flags & 4)) {
        requestShadow();
    }

    if ((flags & 0x200)) {
        if (m_damage_timer > 0) {
            SMS_AddDamageFogEffect(mActorData->mModel->mModelData, mTranslation, graphics);
            m_damage_timer -= 1;
        } else {
            SMS_ResetDamageFogEffect(mActorData->mModel->mModelData);
        }
        J3DTevStage *tev_stage = mActorData->mModel->mModelData->mStages[0];
        J3DTevBlock *tev_block = *(J3DTevBlock **)((u8 *)tev_stage + 0x28);
        tev_block->setTevKColor(0, {0, 0, 0, m_pol_alpha});
    }
}

#define MESSAGE_TOUCH_WATER 15
bool TDarkZhine::receiveMessage(THitActor *sender, u32 message) {
    if (message == MESSAGE_TOUCH_WATER) {
        TVec3f *water_speed = (TVec3f *)TMapObjBase::getWaterSpeed(sender);
        f32 water_magnitude = water_speed->magnitude();
        m_pol_alpha -= 1 + (u8)(water_magnitude / 40.0f);
        if (gpMSound->gateCheck(MSD_SE_BS_GESO_WATER_HIT)) {
            MSoundSE::startSoundActor(MSD_SE_BS_GESO_WATER_HIT, mTranslation, 0, nullptr, 0, 4);
        }
        return true;
    }
    TSpineEnemy::receiveMessage(sender, message);
    return false;
}

void TDarkZhine::loadAfter() {
    m_target         = gpMarioAddress;
    m_bounding_point = mTranslation;

    s_msg_sleep.reset();
    s_msg_pound_a.reset();
    s_msg_pound_b.reset();
    s_msg_pound_c.reset();
    s_msg_attack.reset();
    s_msg_last_point.reset();
    s_msg_finish.reset();

    s_first_pound     = true;
    s_has_bgm_started = false;
}

void TDarkZhine::control() {
    TSpineEnemy::control();

    mRotation.x = normalizeAngle(mRotation.x);
    mRotation.y = normalizeAngle(mRotation.y);
    mRotation.z = normalizeAngle(mRotation.z);

    if (gpPollution->isPolluted(mTranslation.x, mTranslation.y, mTranslation.z)) {
        m_pol_alpha = Max(m_pol_alpha + 1, 255);
    }

    if (m_pol_collect_timer++ >= 10) {
        m_pol_collect_timer = 0;
        m_pol_alpha         = Max(m_pol_alpha + 1, 255);
    }

    m_ground_point   = mTranslation;
    m_ground_point.y = mGroundY;

    checkForActorContact();

    if (isHardMode()) {
        gpMarioParticleManager->emitAndBindToMtxPtr(296, getModel()->mBaseMtx, 1, this);
        gpMarioParticleManager->emitAndBindToMtxPtr(297, getModel()->mBaseMtx, 1, this);
    }
}

void TDarkZhine::bind() { TSpineEnemy::bind(); }

void TDarkZhine::moveObject() { TSpineEnemy::moveObject(); }

void TDarkZhine::calcRootMatrix() {
    TSpineEnemy::calcRootMatrix();

    Mtx tmtx;
    PSMTXTrans(tmtx, 0.0f, m_model_ofs_y * mScale.y, 0.0f);

    J3DModel *model = getModel();
    PSMTXConcat(model->mBaseMtx, tmtx, model->mBaseMtx);
}

void TDarkZhine::reset() {
    setStunned(false);
    setInvincible(true);
    mTranslation = m_bounding_point;
    mRotation    = TVec3f::zero();
}

void TDarkZhine::kill() {
    TSpineEnemy::kill();
    mStateFlags.asU32 |= 0xD9;
    mObjectType |= 1;  // Disable collision
}

f32 TDarkZhine::getDistanceToTargetXZ() const {
    TVec3f self_xz   = {mTranslation.x, 0.0f, mTranslation.z};
    TVec3f target_xz = {m_target->mTranslation.x, 0.0f, m_target->mTranslation.z};
    return PSVECDistance(self_xz, target_xz);
}

f32 TDarkZhine::getAngleToTarget() const {
    return atan2f((m_target->mTranslation.x - mTranslation.x),
                  (m_target->mTranslation.z - mTranslation.z));
}

void TDarkZhine::launchPolDrop() {
    TBGPolDrop *drop = m_pol_drops[m_pol_drop_index];
    m_pol_drop_index = (m_pol_drop_index + 1) % ZHINE_POL_DROP_COUNT;

    TVec3f pol_velocity;
    SMSCalcJumpVelocityXZ(m_target->mTranslation, mTranslation, 15.0f, 1.0f, &pol_velocity);

    drop->launch(mTranslation, pol_velocity);
}

void TDarkZhine::calcReflectingVelocity(TBGCheckData *data, f32 bounce, TVec3f &speed) {
    f32 b1 = 1.0f + bounce;

    f32 ns  = PSVECDotProduct(speed, data->mNormal);
    speed.x = -(b1 * ns * data->mNormal.x - speed.x);
    speed.y = -(b1 * ns * data->mNormal.y - speed.y);
    speed.z = -(b1 * ns * data->mNormal.z - speed.z);
}

void TDarkZhine::calcReflectingAngle(TBGCheckData *data, f32 *angle) {
    // Assume the angle represents the direction the object is moving
    f32 radians = angleToRadians(*angle);

    f32 ix = sinf(radians);
    f32 iz = cosf(radians);

    f32 dp = data->mNormal.x * ix + data->mNormal.z * iz;
    f32 rx = ix - 2.0f * dp * data->mNormal.x;
    f32 rz = iz - 2.0f * dp * data->mNormal.z;

    *angle = radiansToAngle(atan2f(-rx, -rz));
}

void TDarkZhine::moveToTargetXZ() {
    mTranslation.x = m_target->mTranslation.x;
    mTranslation.z = m_target->mTranslation.z;
}

void TDarkZhine::checkForActorContact() {
    for (size_t i = 0; i < mNumObjs; ++i) {
        if (mCollidingObjs[i]->mObjectID == OBJECT_ID_MARIO) {
            TMario *player = static_cast<TMario *>(mCollidingObjs[i]);

            if (!isStunned()) {
                if (isPlayerAttacking(player) && !isInvincible()) {
                    gpMarioParticleManager->emitAndBindToPosPtr(11, &mTranslation, 0, nullptr);

                    f32 player_angle = convertAngleS16ToFloat(player->mAngle.y);
                    mSpeed.x         = 42.0f * sinf(angleToRadians(player_angle));
                    mSpeed.y         = 38.0f;
                    mSpeed.z         = 42.0f * cosf(angleToRadians(player_angle));

                    if (gpMSound->gateCheck(0x28E1)) {
                        MSoundSE::startSoundActor(0x28E1, mTranslation, 0, nullptr, 0, 4);
                    }

                    // Block FLUDD analysis.
                    s_first_pound = false;
                    mSpineBase->setNerve(&s_zhine_nerve_knockback);
                    break;
                }

                if (isHostile() && !isDefeated()) {
                    if (getForwardSpeed() > 20.0f) {
                        player->damageExec(this, 1, 2, 0, 16.0f, 0x19, 0.0f, 160);
                    } else {
                        player->damageExec(this, 1, 1, 0, 16.0f, 0x19, 0.0f, 160);
                    }
                }
            } else {
                if (isPlayerPounding(player)) {
                    gpMarioParticleManager->emitAndBindToPosPtr(11, &mTranslation, 0, nullptr);

                    m_damage_timer = 120;
                    m_health_points -= 1;
                    if (m_health_points == 1) {
                        m_is_hard_mode = true;
                        s_msg_last_point.show();
                    }

                    mSpineBase->setNerve(&s_zhine_nerve_crash);

                    TVec3f pos_delta = mTranslation - m_target->mTranslation;

                    // Scale the magnitude of delta between 10 and 30
                    f32 new_magnitude =
                        lerp<f32>(10.0f, 30.0f, Min(pos_delta.magnitude() / 100.0f, 1.0f));

                    pos_delta.normalize();
                    pos_delta.scale(new_magnitude);

                    mSpeed.x = pos_delta.x;
                    mSpeed.y = 28.0f;
                    mSpeed.z = pos_delta.z;

                    // gpMarioParticleManager->emit(0x57, &mTranslation, 1, this);
                    if (gpMSound->gateCheck(0x28E1)) {
                        MSoundSE::startSoundActor(0x28E1, mTranslation, 0, nullptr, 0, 4);
                    }

                    break;
                }
            }
        }
    }
}

bool TDarkZhine::isPlayerAttacking(TMario *player) {
    if (player->mState != TMario::STATE_DIVE && player->mState != TMario::STATE_DIVESLIDE) {
        return false;
    }

    if (player->mForwardSpeed < 30.0f) {
        return false;
    }

    return true;
}

bool TDarkZhine::isPlayerPounding(TMario *player) {
    if (player->mSpeed.y >= 0.0f) {
        return false;
    }

    if (player->mState == TMario::STATE_G_POUND || player->mState == 0x24DF) {
        return true;
    }

    if (player->mPrevState == TMario::STATE_G_POUND || player->mPrevState == 0x24DF) {
        return true;
    }

    return false;
}

bool TDarkZhine::doGroundPound(bool attack, bool *illegal_out) {
    const TBGCheckData *ground;
    f32 groundY = gpMapCollisionData->checkGround(mTranslation.x, mTranslation.y, mTranslation.z,
                                                  0b101, &ground);

    *illegal_out = false;

    if (ground == &TMapCollisionData::mIllegalCheckData) {
        *illegal_out = true;
        return false;
    }

    if (fabsf(mTranslation.y - groundY) < 10.0f) {
        doWorldShake(0.8f);

        gpPollution->stamp(1, mTranslation.x, mTranslation.y, mTranslation.z,
                           600.0f * ((f32)m_pol_alpha / 255.0f));

        if (isMarioVulnerableToShock(gpMarioAddress)) {
            gpMarioAddress->changePlayerStatus(TMario::STATE_KNCK_GND, 0, false);
            if (attack) {
                gpMarioAddress->floorDamageExec(1, 1, 0, 120);
            }
        }

        gpMarioParticleManager->emit(20, &mTranslation, 0, nullptr);
        gpMarioParticleManager->emit(19, &mTranslation, 0, nullptr);
        gpMarioParticleManager->emit(18, &mTranslation, 0, nullptr);

        return true;
    }

    return false;
}

void TDarkZhine::doWorldShake(f32 strength) {
    if (strength > 0.5f) {
        SMSRumbleMgr->start(0x15, 0x1E, (f32 *)nullptr);
        gpCameraShake->startShake(UnknownShake27, strength);
    } else {
        SMSRumbleMgr->start(0x10, 0x17, (f32 *)nullptr);
        gpCameraShake->startShake(UnknownShake27, strength);
    }
    if (gpMSound->gateCheck(6158)) {
        MSoundSE::startSoundActor(6158, mTranslation, 0, nullptr, 0, 4);
    }
}

void TDarkZhine::checkMessagePool() {
    bool is_console_ready = *(u16 *)((u8 *)gpMarDirector->mGCConsole + 0x3F4) == 0;

    AtomMessageViewer *message = peekMessage();
    if (!message) {
        return;
    }

    if (s_message_duration == -1 && is_console_ready) {
        s_message_duration = 0;
        startAppearBalloon__11TGCConsole2FUlb(gpMarDirector->mGCConsole, message->id(), false);
    } else if (s_message_duration++ == message->duration()) {
        startDisappearBalloon__11TGCConsole2FUlb(gpMarDirector->mGCConsole, message->id(), true);
    } else if (is_console_ready) {
        popMessage();

        AtomMessageViewer *next_message = peekMessage();
        if (next_message) {
            s_message_duration = 0;
            startAppearBalloon__11TGCConsole2FUlb(gpMarDirector->mGCConsole, next_message->id(),
                                                  false);
        } else {
            s_message_duration = -1;
        }
    }
}

f32 TDarkZhine::calcNextAngle(f32 turn_power, f32 offset, bool indiscriminate) {
    if (getDistanceToTargetXZ() > 400.0f || indiscriminate) {
        f32 zhine_angle  = normalizeRadians(angleToRadians(mRotation.y));
        f32 radian_angle = normalizeRadians(getAngleToTarget() + angleToRadians(offset));

        double delta = radian_angle - zhine_angle;
        if (delta > M_PI)
            delta -= 2 * M_PI;
        if (delta < -M_PI)
            delta += 2 * M_PI;

        f32 angle = lerp<f32>(zhine_angle, zhine_angle + delta, turn_power);
        return radiansToAngle(normalizeRadians(angle));
    } else {
        return radiansToAngle(normalizeRadians(angleToRadians(mRotation.y)));
    }
}

void TDarkZhine::spawnShine() {
    auto *nameRef = TMarNameRefGen::getInstance()->getRootNameRef();
    u16 keyCode   = JDrama::TNameRef::calcKeyCode("ZhineShine");
    if (TNameRef *p = nameRef->searchF(keyCode, "ZhineShine")) {
        TShine *shine       = reinterpret_cast<TShine *>(p);
        shine->mTranslation = mTranslation;
        shine->mRotation.y  = mRotation.y;
        shine->appearSimple(300);

        MSBgm::startBGM(BGM_SHINE_APPEAR);

        /*shine->mTranslation = mTranslation;
        shine->mTranslation.y += 100.0f;
        shine->appearWithDemo("ZhineShineCamera");*/
    }
}

TDarkZhineManager::TDarkZhineManager(const char *name) : TEnemyManager(name) {}

void TDarkZhineManager::load(JSUMemoryInputStream &in) { TEnemyManager::load(in); }
