#include <JSystem/JParticle/JPAResourceManager.hxx>

#include <SMS/Camera/CameraShake.hxx>
#include <SMS/Enemy/Conductor.hxx>
#include <SMS/MSound/MSBGM.hxx>
#include <SMS/MSound/MSModBgm.hxx>
#include <SMS/MSound/MSoundSESystem.hxx>
#include <SMS/Manager/FlagManager.hxx>
#include <SMS/Manager/MarioParticleManager.hxx>
#include <SMS/Manager/ModelWaterManager.hxx>
#include <SMS/Manager/RumbleManager.hxx>
#include <SMS/Map/MapCollisionData.hxx>
#include <SMS/MarioUtil/DrawUtil.hxx>
#include <SMS/MarioUtil/MathUtil.hxx>
#include <SMS/MarioUtil/ShadowUtil.hxx>
#include <SMS/MarioUtil/TexUtil.hxx>
#include <SMS/Strategic/ObjHitCheck.hxx>
#include <SMS/Strategic/Strategy.hxx>
#include <SMS/System/TargetArrow.hxx>
#include <SMS/rand.h>

#include <BetterSMS/libs/constmath.hxx>
#include <BetterSMS/music.hxx>
#include <BetterSMS/player.hxx>

#include "enemy/bowser_car.hxx"
#include "message.hxx"

extern TVec3f *gpMarioPos;

static TNerveBowserIdle s_bowser_nerve_idle;
static TNerveBowserTalk s_bowser_nerve_talk;
static TNerveBowserTaunt s_bowser_nerve_taunt;
static TNerveBowserThrow s_bowser_nerve_throw;
static TNerveBowserFire s_bowser_nerve_fire;
static TNerveBowserShoot s_bowser_nerve_shoot;
static TNerveBowserWet s_bowser_nerve_wet;
static TNerveBowserStun s_bowser_nerve_stun;
static TNerveBowserShot s_bowser_nerve_shot;
static TNerveBowserRecover s_bowser_nerve_recover;
static TNerveBowserRise s_bowser_nerve_rise;
static TNerveBowserShock s_bowser_nerve_shock;
static TNerveBowserPreKill s_bowser_nerve_prekill;
static TNerveBowserKill s_bowser_nerve_kill;

static TVec3f s_fly_away_point = {45000.0f, 0.0f, -36000.0f};

static bool s_has_bgm_started = false;
static bool s_first_pound     = true;

#define BOWSER_ANM_IDX_FLY   0
#define BOWSER_ANM_IDX_RISE  1
#define BOWSER_ANM_IDX_ROLL  2
#define BOWSER_ANM_IDX_SLEEP 3
#define BOWSER_ANM_IDX_STUN  4
#define BOWSER_ANM_IDX_WAKE  5

#define BOWSER_FLY_HEIGHT         700.0f
#define BOWSER_FLY_WAVE_AMPLITUDE 80.0f
#define BOWSER_FLY_WAVE_FREQUENCY (1.0f / 30.0f);
#define BOWSER_GRAVITY            0.98f
#define BOWSER_BRAKE_FACTOR       0.97f
#define BOWSER_SPEED_FACTOR       0.003f
#define BOWSER_TURN_POWER         0.03f
#define BOWSER_MAX_FRAMERATE      5.0f
#define BOWSER_WATCH_MARIO_RADIUS 5000.0f

#define BOWSER_MODEL_TRANS_Y 50.0f

static const char *s_explosion_name = "\x83\x47\x83\x74\x83\x46\x83\x4E\x83\x67\x94\x9A\x94\xAD\x83"
                                      "\x7D\x83\x6C\x81\x5B\x83\x57\x83\x83\x81\x5B";

static const char *s_killer_name =
    "\x83\x4C\x83\x89\x81\x5B\x83\x7D\x83\x6C\x81\x5B\x83\x57\x83\x83\x81\x5B";

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

namespace J3DSys {
    extern Mtx mCurrentMtx;
}

TBossBowserCar *gpBowserCar = nullptr;

static int propellorCallback(J3DNode *node, int joint_index) {
    Mtx tmtx;
    PSMTXCopy(J3DSys::mCurrentMtx, tmtx);

    f32 x = tmtx[0][3];
    f32 y = tmtx[1][3];
    f32 z = tmtx[2][3];

    // tmtx[0][3] = 0.0f;
    // tmtx[1][3] = 0.0f;
    // tmtx[2][3] = 0.0f;

    Mtx rotmtx;
    if (gpBowserCar->mSpineBase->getLatestNerve() != &s_bowser_nerve_shock) {
        PSMTXRotRad(rotmtx, 'y',
                    (gpBowserCar->m_wave_point * 5.0f + angleToRadians(gpBowserCar->mRotation.y)));
    } else {
        f32 amplitude = (cosf(gpBowserCar->m_wave_point) * sinf(gpBowserCar->m_wave_point)) -
                        (sin(gpBowserCar->m_wave_point * 3.0f) * 0.5f);
        PSMTXRotRad(rotmtx, 'y',
                    gpBowserCar->m_wave_point * amplitude +
                        angleToRadians(gpBowserCar->mRotation.y));
    }

    PSMTXConcat(tmtx, rotmtx, tmtx);
    // tmtx[0][3] = x;
    // tmtx[1][3] = y;
    // tmtx[2][3] = z;

    PSMTXCopy(tmtx, J3DSys::mCurrentMtx);
    return 1;
}

#if 0
static int gunLCallback(J3DNode *node, int joint_index) {
    Mtx carmtx;
    //PSMTXTrans(tmtx, gpBowserCar->mTranslation.x, gpBowserCar->mTranslation.y, gpBowserCar);

    PSMTXCopy(gpBowserCar->mActorData->mModel->mJointArray[0], carmtx);

    f32 x = carmtx[0][3];
    f32 y = carmtx[1][3];
    f32 z = carmtx[2][3];

    carmtx[0][3] = 0.0f;
    carmtx[1][3] = 0.0f;
    carmtx[2][3] = 0.0f;

    TVec3f right = TVec3f::right();
    PSMTXMultVec(carmtx, right, right);
    right.scale(400.0f);

    Mtx tmtx;
    PSMTXTrans(tmtx, right.x, right.y, right.z);

    Mtx rotmtx;
    PSMTXRotRad(rotmtx, 'x',
                gpBowserCar->m_wave_point * 5.0f + angleToRadians(gpBowserCar->mRotation.y));

    PSMTXScaleApply(carmtx, carmtx, 3.0f, 3.0f, 3.0f);
    PSMTXConcat(rotmtx, carmtx, carmtx);
    //PSMTXConcat(carmtx, tmtx, carmtx);

    carmtx[0][3] = right.x + x;
    carmtx[1][3] = right.y + y;
    carmtx[2][3] = right.z + z;

    PSMTXCopy(carmtx, J3DSys::mCurrentMtx);
    return 1;
}

static int gunRCallback(J3DNode *node, int joint_index) {
    Mtx carmtx;
    // PSMTXTrans(tmtx, gpBowserCar->mTranslation.x, gpBowserCar->mTranslation.y, gpBowserCar);

    PSMTXCopy(gpBowserCar->mActorData->mModel->mJointArray[0], carmtx);

    f32 x = carmtx[0][3];
    f32 y = carmtx[1][3];
    f32 z = carmtx[2][3];

    carmtx[0][3] = 0.0f;
    carmtx[1][3] = 0.0f;
    carmtx[2][3] = 0.0f;

    TVec3f left = TVec3f::left();
    // PSMTXMultVec(carmtx, left, left);
    left.scale(400.0f);

    Mtx tmtx;
    PSMTXTrans(tmtx, left.x, left.y, left.z);

    Mtx rotmtx;
    PSMTXRotRad(rotmtx, 'y', gpBowserCar->m_current_gun_r_angle);

    PSMTXScaleApply(carmtx, carmtx, 3.0f, 3.0f, 3.0f);
    PSMTXConcat(carmtx, rotmtx, carmtx);
    PSMTXConcat(carmtx, tmtx, carmtx);

    carmtx[0][3] += x;
    carmtx[1][3] += y;
    carmtx[2][3] += z;

    PSMTXCopy(carmtx, J3DSys::mCurrentMtx);
    return 1;
}
#else

static int gunLCallback(J3DNode *node, int joint_index) { return 1; }

static int gunRCallback(J3DNode *node, int joint_index) { return 1; }
#endif

static int neckCallback(J3DNode *node, int joint_index) {
#if 0
    Mtx tmtx;
    PSMTXCopy(J3DSys::mCurrentMtx, tmtx);

    f32 x = tmtx[0][3];
    f32 y = tmtx[1][3];
    f32 z = tmtx[2][3];

    tmtx[0][3] = 0.0f;
    tmtx[1][3] = 0.0f;
    tmtx[2][3] = 0.0f;

    TVec3f player_head_pos = gpMarioAddress->mTranslation;
    player_head_pos.y += 85.0f;

    // Rotate the neck towards the target if close enough
    if (gpBowserCar->getDistanceToTargetXZ() < BOWSER_WATCH_MARIO_RADIUS) {
        TVec3f target_dir = {player_head_pos.x - x, player_head_pos.y - y,
                             player_head_pos.z - z};
        PSVECNormalize(target_dir, target_dir);

        TVec3f forward = {tmtx[2][0], tmtx[2][1], tmtx[2][2]};
        PSVECNormalize(forward, forward);

        TVec3f lerped = {
            lerp<f32>(forward.x, target_dir.x, 0.1f),
            lerp<f32>(forward.y, target_dir.y, 0.1f),
            lerp<f32>(forward.z, target_dir.z, 0.1f),
        };
        PSVECNormalize(lerped, lerped);

        TVec3f right;
        PSVECCrossProduct(TVec3f::up(), lerped, right);
        PSVECNormalize(right, right);

        TVec3f up;
        PSVECCrossProduct(lerped, right, up);
        PSVECNormalize(up, up);

        Mtx rotmtx;
        rotmtx[0][0] = -right.x;
        rotmtx[0][1] = right.y;
        rotmtx[0][2] = right.z;
        rotmtx[0][3] = 0.0f;
        rotmtx[1][0] = -up.x;
        rotmtx[1][1] = up.y;
        rotmtx[1][2] = up.z;
        rotmtx[1][3] = 0.0f;
        rotmtx[2][0] = -lerped.x;
        rotmtx[2][1] = lerped.y;
        rotmtx[2][2] = lerped.z;
        rotmtx[2][3] = 0.0f;

        PSMTXConcat(tmtx, rotmtx, tmtx);
    }

    tmtx[0][3] = x;
    tmtx[1][3] = y;
    tmtx[2][3] = z;

    PSMTXCopy(tmtx, J3DSys::mCurrentMtx);
#endif
    return 1;
}

static JPABaseParticle *particleSingle;

class TFireParticleCallback : public JPACallBackBase<> {
public:
    void execute(JPABaseEmitter *emitter) {
        emitter->setGlobalRTMatrix(
            gpBowserCar->mActorData->mModel->mJointArray[gpBowserCar->m_fire_index]);

        JSUPtrLink *link = emitter->mParticleList.mFirst;
        while (link) {
            JPABaseParticle *particle = static_cast<JPABaseParticle *>(link->mItemPtr);

            TVec3f pos;
            particle->getCurrentPosition(pos);

            f32 distance = PSVECDistance(pos, gpMarioAddress->mTranslation);

            f32 threshold = 200.0f * (particle->mTimerLerp) + 400.0f;

            if (PSVECDistance(pos, gpMarioAddress->mTranslation) < threshold &&
                !gpMarioAddress->isInvincible()) {
                gpMarioAddress->changePlayerDropping(0x20464, 0);
                gpMarioAddress->decHP(1);
                gpMarioAddress->dropObject();
                gpMarioAddress->changePlayerStatus(0x208B7, 1, false);
                gpMarioAddress->mInvincibilityFrames = 256;
                gpMarioAddress->mSpeed.y += 20.0f;

                gpMarioParticleManager->emitAndBindToPosPtr(6, &gpMarioAddress->mTranslation, 0,
                                                            nullptr);
                if (gpMSound->gateCheck(0x1813)) {
                    MSoundSE::startSoundActor(0x1813, gpMarioAddress->mTranslation, 0, nullptr, 0,
                                              4);
                }
            }

            link = link->mNextLink;
        }
    }
};

static TFireParticleCallback s_fire_particle_callback;

static TSpineEnemy *s_rocket_a = nullptr;
static TSpineEnemy *s_rocket_b = nullptr;
static TSpineEnemy *s_rocket_c = nullptr;

static bool isRocketInUse() {
    JDrama::TNameRef *nameRef = TMarNameRefGen::getInstance()->getRootNameRef();

    if (!s_rocket_a) {
        u16 keyCode = JDrama::TNameRef::calcKeyCode("RocketA");
        s_rocket_a  = static_cast<TSpineEnemy *>(nameRef->searchF(keyCode, "RocketA"));
    }

    if (!s_rocket_b) {
        u16 keyCode = JDrama::TNameRef::calcKeyCode("RocketB");
        s_rocket_b  = static_cast<TSpineEnemy *>(nameRef->searchF(keyCode, "RocketB"));
    }

    if (!s_rocket_c) {
        u16 keyCode = JDrama::TNameRef::calcKeyCode("RocketC");
        s_rocket_c  = static_cast<TSpineEnemy *>(nameRef->searchF(keyCode, "RocketC"));
    }

    bool any_attacking = isAttack__7TRocketFv(s_rocket_a) || isAttack__7TRocketFv(s_rocket_b) ||
                         isAttack__7TRocketFv(s_rocket_c);
    bool any_possessed =
        s_rocket_a->mSpineBase->getLatestNerve() == (TNerveBase<TLiveActor> *)0x8040dab4 ||
        s_rocket_b->mSpineBase->getLatestNerve() == (TNerveBase<TLiveActor> *)0x8040dab4 ||
        s_rocket_c->mSpineBase->getLatestNerve() == (TNerveBase<TLiveActor> *)0x8040dab4;

    return any_attacking || any_possessed;
}

AtomBalloonMessageViewer s_bomb_message(145, 200);

TBossBowserCar::TBossBowserCar(const char *name)
    : TSpineEnemy(name), m_ground_point(), m_stunned(false), m_invincible(false), m_hostile(true),
      m_anm_index(0), m_bounding_point(), m_target(nullptr), m_health_points(5),
      m_invincible_timer(0), m_rocket_angle(0.0f), m_damage_timer(0), m_forward_speed(0.0f),
      m_model_ofs_y(0.0f) {
    gpBowserCar = this;
}

void TBossBowserCar::init(TLiveManager *manager) {
    mLiveManager = manager;
    mLiveManager->manageActor(this);

    // Initialize the model
    mActorKeeper              = new TMActorKeeper(mLiveManager, 15);
    mActorKeeper->mModelFlags = 0x102F0000;
    mActorData                = mActorKeeper->createMActor("bowser.bmd", 0);
    mActorData->setLightType(1);
    mActorData->calc();
    mActorData->viewCalc();

    initHitActor(0x8000011, 10, 0x81000000, 370.0f, 800.0f, 370.0f, 800.0f);
    initAnmSound();

    mShadowRadius = 500.0f;
    mStateFlags.asU32 &= ~0x100;
    mStateFlags.asU32 |= 0x8;

    mSpineBase->setNerve(&s_bowser_nerve_talk);
    m_health_points = 5;
    m_model_ofs_y   = 50.0f;

    m_propeller_index = ((JUTNameTab *)mActorData->mModel->mModelData->_B0)->getIndex("prop") - 1;
    if (m_propeller_index != -1) {
        mActorData->setJointCallback(m_propeller_index, propellorCallback);
    }

    m_gun_l_index = ((JUTNameTab *)mActorData->mModel->mModelData->_B0)->getIndex("bullet_l") - 1;
    if (m_gun_l_index != -1) {
        mActorData->setJointCallback(m_gun_l_index, gunLCallback);
    }

    m_gun_r_index = ((JUTNameTab *)mActorData->mModel->mModelData->_B0)->getIndex("bullet_r") - 1;
    if (m_gun_r_index != -1) {
        mActorData->setJointCallback(m_gun_r_index, gunRCallback);
    }

    m_head_index = ((JUTNameTab *)mActorData->mModel->mModelData->_B0)->getIndex("neck") - 1;
    if (m_head_index != -1) {
        mActorData->setJointCallback(m_head_index, neckCallback);
    }

    m_hand_index = ((JUTNameTab *)mActorData->mModel->mModelData->_B0)->getIndex("Lyubi1_jnt1");
    m_fire_index = ((JUTNameTab *)mActorData->mModel->mModelData->_B0)->getIndex("head");

    mActorData->mBckInfo->initSimpleMotionBlend(20);
    m_intro_ended = false;

    s_has_bgm_started = false;
    s_bomb_message.reset();
}

void TBossBowserCar::load(JSUMemoryInputStream &in) { TSpineEnemy::load(in); }

void TBossBowserCar::perform(u32 flags, JDrama::TGraphics *graphics) {
    TSpineEnemy::perform(flags, graphics);

    TFlagManager::smInstance->setFlag(0x6001A, m_health_points);

#if 0
    for (size_t i = 0; i < ZHINE_POL_DROP_COUNT; ++i) {
        m_pol_drops[i]->perform(flags, graphics);
    }
#endif

    if ((flags & 1)) {
        if (m_health_points < 3) {
            if (JPABaseEmitter *emitter = gpMarioParticleManager->emitAndBindToMtxPtr(
                    0x1b6, mActorData->mModel->mJointArray[1], 1, nullptr)) {
                emitter->mSize1 = {1.0f, 1.0f, 1.0f};
            }

            if (m_health_points < 2) {
                if (JPABaseEmitter *emitter = gpMarioParticleManager->emitAndBindToMtxPtr(
                        0x1b5, mActorData->mModel->mJointArray[1], 1, nullptr)) {
                    emitter->mSize1 = {1.0f, 1.0f, 1.0f};
                }

                if (JPABaseEmitter *emitter = gpMarioParticleManager->emitAndBindToMtxPtr(
                        0x1b7, mActorData->mModel->mJointArray[1], 1, nullptr)) {
                    emitter->mSize1 = {1.0f, 1.0f, 1.0f};
                }
            }
        }
    }

    if ((flags & 2)) {
        if (isStunned() || isRocketInUse()) {
            gpTargetArrow->mIsVisible = true;
            gpTargetArrow->setPos({mTranslation.x, mTranslation.y + 1000.0f, mTranslation.z});
        } else {
            gpTargetArrow->mIsVisible = false;
        }
    }

#if 1
    if ((flags & 4)) {
        requestShadow();
    }
#else
    requestShadow();
#endif

    if ((flags & 0x200)) {
        if (m_damage_timer > 0) {
            SMS_AddDamageFogEffect(mActorData->mModel->mModelData, mTranslation, graphics);
            m_damage_timer -= 1;
        } else {
            SMS_ResetDamageFogEffect(mActorData->mModel->mModelData);
        }
    }
}

#define MESSAGE_TOUCH_WATER 15
bool TBossBowserCar::receiveMessage(THitActor *sender, u32 message) {
    if (m_invincible_timer > 0 || mSpineBase->getLatestNerve() == &s_bowser_nerve_shot ||
        mSpineBase->getLatestNerve() == &s_bowser_nerve_prekill ||
        mSpineBase->getLatestNerve() == &s_bowser_nerve_kill) {
        return false;
    }

    if (sender->mObjectID == 0x1000002B) {
        m_health_points -= 1;
        m_damage_timer = 120;

        setInvincibleTimer(120);

        TVec3f direction = {mTranslation.x - sender->mTranslation.x, 0.0f,
                            mTranslation.z - sender->mTranslation.z};
        m_rocket_angle   = radiansToAngle(atan2f(direction.y, direction.x));
        if (isnan(m_rocket_angle)) {
            m_rocket_angle = 0.0f;
        }

        TSpineEnemy *rocket   = (TSpineEnemy *)sender;
        rocket->mPerformFlags = 0xFFFF;  // Render it gone completely
        rocket->kill();
        rocket->mTranslation = {0.0f, -10000.0f, 0.0f};

        m_wet_count = 0;

        mSpineBase->setNerve(&s_bowser_nerve_shot);
        return true;
    }

    if (message == MESSAGE_TOUCH_WATER) {
        TVec3f *water_speed = (TVec3f *)TMapObjBase::getWaterSpeed(sender);
        f32 water_magnitude = water_speed->magnitude();
        if (water_magnitude > 26.0f) {
            J3DModel *model = getModel();

            if (!isHardMode()) {
                TVec3f head_pos = {model->mJointArray[m_head_index][0][3],
                                   model->mJointArray[m_head_index][1][3],
                                   model->mJointArray[m_head_index][2][3]};

                if (PSVECDistance(head_pos, sender->mTranslation) < mReceiveRadius) {
                    m_wet_count = Min(m_wet_count + 4, 255);
                }
            } else {
                m_wet_count = Min(m_wet_count + 4, 255);
            }

            s32 water_threshold_adjust = 0;

            if (mSpineBase->getLatestNerve() == &s_bowser_nerve_fire) {
                water_threshold_adjust = 20;
            }

            if (!isInvincible()) {
                if (isHardMode()) {
                    if (m_wet_count >= 36 + water_threshold_adjust) {
                        for (size_t i = 0; i < 3; ++i) {
                            if (JPABaseEmitter *emitter =
                                    gpMarioParticleManager->emitAndBindToPosPtr(
                                        0x1b3, &mTranslation, 1, this)) {
                                emitter->mSize1 = {0.3f, 0.3f, 0.3f};
                            }
                        }

                        if (mSpineBase->getLatestNerve() != &s_bowser_nerve_shock) {
                            m_nerve_status = 0;
                            mSpineBase->setNerve(&s_bowser_nerve_shock);
                        } else {
                            m_shock_wait_time = Min(m_shock_wait_time + 2, 600);
                        }
                    }
                } else {
                    if (m_wet_count >= 28 + water_threshold_adjust &&
                        mSpineBase->getLatestNerve() != &s_bowser_nerve_wet) {
                        m_nerve_status = 0;
                        mSpineBase->setNerve(&s_bowser_nerve_wet);
                    }
                }
            }

            return true;
        }
    }

    return TSpineEnemy::receiveMessage(sender, message);
}

void TBossBowserCar::loadAfter() {
    m_target           = gpMarioAddress;
    m_bounding_point   = mTranslation;
    m_base_translation = mTranslation;
    m_wave_point       = 0.0f;
    m_nerve_status     = 0;
    m_wet_count        = 0;
    m_last_taunt       = false;

    TGraphWeb *web = mGraphTracer->mGraph;
    if (web) {
        int index = web->findNearestNodeIndex(mTranslation, -1);
        mGraphTracer->setTo(index);
        TGraphNode &graphNode = web->mNodes[index];
        TRailNode *railNode   = graphNode.mRailNode;

        m_base_translation = mGraphTracer->mGraph->indexToPoint(index);

        if (railNode->mValues[1] != 0xFFFF) {
            m_travel_speed = static_cast<f32>(railNode->mValues[1]) / 100.0f;
        } else {
            m_travel_speed = 10.0f;
        }
    }

    m_propeller_index = 0;

    s_first_pound     = true;
    s_has_bgm_started = false;
}

void TBossBowserCar::control() {
    TSpineEnemy::control();

    updateSquareToMario();

    mRotation.x = normalizeAngle(mRotation.x);
    mRotation.y = normalizeAngle(mRotation.y);
    mRotation.z = normalizeAngle(mRotation.z);

    m_ground_point   = mTranslation;
    m_ground_point.y = mGroundY;

    checkForActorContact();

    m_wave_point += BOWSER_FLY_WAVE_FREQUENCY;
    if (m_wave_point > M_PI * 2.0f) {
        m_wave_point -= M_PI * 2.0f;
    }

    // Signal end of wet nerve
    if (m_wet_count == 1) {
        m_nerve_status = 2;
    }
    m_wet_count = Max(0, m_wet_count - 1);

    m_current_tilt       = lerp<f32>(m_current_tilt, m_target_tilt, 0.1f);
    m_current_tilt_speed = lerp<f32>(m_current_tilt_speed, m_target_tilt_speed, 0.1f);

    m_current_gun_l_angle = lerp<f32>(m_current_gun_l_angle, m_target_gun_l_angle, 0.1f);
    m_current_gun_r_angle = lerp<f32>(m_current_gun_r_angle, m_target_gun_r_angle, 0.1f);

    m_highest_ground_y = getHighestGround(m_base_translation, 500.0f, &m_highest_ground);

    if (m_invincible_timer == 0 && mSpineBase->getLatestNerve() != &s_bowser_nerve_shot &&
        mSpineBase->getLatestNerve() != &s_bowser_nerve_prekill &&
        mSpineBase->getLatestNerve() != &s_bowser_nerve_kill) {
        if (TSpineEnemy *bomb =
                isBombWithinAndAttack(mReceiveRadius * 1.3f, mReceiveHeight * 1.2f)) {
            m_health_points -= 1;
            m_rocket_hit_pos = bomb->mTranslation;
            m_rocket_angle   = radiansToAngle(
                atan2f(m_rocket_hit_pos.y - mTranslation.y, m_rocket_hit_pos.x - mTranslation.x));
            if (isnan(m_rocket_angle)) {
                m_rocket_angle = 0.0f;
            }
            m_wet_count = 0;
            mSpineBase->setNerve(&s_bowser_nerve_shot);
            bomb->kill();

            setInvincibleTimer(120);
        }
    }

    m_invincible_timer = Max(0, m_invincible_timer - 1);

    if (mSpineBase->getLatestNerve() != &s_bowser_nerve_throw) {
        if (TSpineEnemy *bombhei = static_cast<TSpineEnemy *>(mHeldObject)) {
            // Drop the object
            TVec3f throw_speed = {10.0f, 10.0f, 10.0f};

            TVec3f direction = {sinf(angleToRadians(mRotation.y)), 0.0f,
                                cosf(angleToRadians(mRotation.y))};
            direction.y      = 0.0f;

            PSVECNormalize(direction, direction);
            direction *= throw_speed;
            direction.scale(0.5f + (rand() % 100) * 0.005f);
            direction.y = 15.0f;

            bombhei->mSpeed  = direction;
            bombhei->mHolder = nullptr;
            bombhei->ensureTakeSituation();
            mHeldObject = nullptr;
        }
    }
}

void TBossBowserCar::bind() { TSpineEnemy::bind(); }

void TBossBowserCar::moveObject() {
    control();

    if (!isDefeated()) {
        mSpeed.scale(BOWSER_BRAKE_FACTOR);
        if (PSVECMag(mSpeed) < 0.01f) {
            mSpeed = TVec3f::zero();
        }
    }

    mSpeed.y = Max(mSpeed.y - mGravity, -30.0f);
    m_base_translation += mSpeed;
    mTranslation = m_base_translation;
    mTranslation.y += BOWSER_FLY_WAVE_AMPLITUDE * sinf(m_wave_point);
}

void TBossBowserCar::calcRootMatrix() {
    TSpineEnemy::calcRootMatrix();

    Mtx tmtx;
    PSMTXTrans(tmtx, 0.0f, m_model_ofs_y * mScale.y, 0.0f);

    if (mSpineBase->getLatestNerve() == &s_bowser_nerve_wet) {
        Mtx rxmtx;
        Mtx rymtx;
        Mtx rzmtx;

        PSMTXRotRad(rxmtx, 'x',
                    sinf(m_wave_point * m_current_tilt_speed * 1.0f) * m_current_tilt * 0.4f);
        PSMTXRotRad(rymtx, 'y',
                    sinf(m_wave_point * m_current_tilt_speed * 2.0f) * m_current_tilt * 0.7f);
        PSMTXRotRad(rzmtx, 'z',
                    cosf(m_wave_point * m_current_tilt_speed * 1.0f) * m_current_tilt * 0.4f);

        PSMTXConcat(rxmtx, rymtx, rxmtx);
        PSMTXConcat(rxmtx, rzmtx, rxmtx);
        PSMTXConcat(tmtx, rxmtx, tmtx);
    } else if (mSpineBase->getLatestNerve() == &s_bowser_nerve_stun) {
        Mtx rxmtx;
        Mtx rymtx;
        Mtx rzmtx;

        PSMTXRotRad(rxmtx, 'x',
                    sinf(m_wave_point * m_current_tilt_speed * 1.0f) * m_current_tilt * 0.5f);
        PSMTXRotRad(rzmtx, 'z',
                    cosf(m_wave_point * m_current_tilt_speed * 1.0f) * m_current_tilt * 0.5f);

        PSMTXConcat(rxmtx, rymtx, rxmtx);
        PSMTXConcat(rxmtx, rzmtx, rxmtx);
        PSMTXConcat(tmtx, rxmtx, tmtx);
    }

    J3DModel *model = getModel();
    PSMTXConcat(model->mBaseMtx, tmtx, model->mBaseMtx);
}

void TBossBowserCar::reset() {
    setStunned(false);
    setInvincibleTimer(0);
    mTranslation       = m_bounding_point;
    mRotation          = TVec3f::zero();
    m_base_translation = mTranslation;
    m_wave_point       = 0.0f;
}

void TBossBowserCar::kill() {
    TSpineEnemy::kill();
    mStateFlags.asU32 |= 0xD9;
    mObjectType |= 1;  // Disable collision
}

void TBossBowserCar::requestShadow() {
#if 0
    TCircleShadowRequest request;
    request.mTranslation   = mTranslation;
    request.mTranslation.y = mGroundY;
    request.mOffsetY       = mShadowRadius;
    request.mOffsetY2      = mShadowRadius;
    request.mRotationY     = mRotation.y;
    request.mShadowType    = getShadowType();
    gpBindShadowManager->forceRequest(request, OBJECT_ID_MARIO);
#else
    TSpineEnemy::requestShadow();
#endif
}

Mtx44 *TBossBowserCar::getTakingMtx() {
    J3DModel *model = getModel();

    // Scale the model up to be accurate with game size
    Mtx smtx;
    PSMTXScale(smtx, 4.0f, 4.0f, 4.0f);

    // Translate the local coordinates so it fits in his hand
    Mtx tmtx;
    PSMTXCopy(model->mJointArray[m_hand_index], tmtx);

    f32 x = tmtx[0][3];
    f32 y = tmtx[1][3];
    f32 z = tmtx[2][3];

    tmtx[0][3] = 0.0f;
    tmtx[1][3] = 0.0f;
    tmtx[2][3] = 0.0f;

    TVec3f local_ofs = {0.5f, -0.7f, 0.1f};
    PSMTXMultVec(tmtx, local_ofs, local_ofs);
    PSVECNormalize(local_ofs, local_ofs);
    local_ofs.x *= 150.0f;
    local_ofs.y *= 150.0f;
    local_ofs.z *= 20.0f;

    PSMTXConcat(tmtx, smtx, tmtx);
    tmtx[0][3] = x + local_ofs.x;
    tmtx[1][3] = y + local_ofs.y;
    tmtx[2][3] = z + local_ofs.z;

    PSMTXCopy(tmtx, m_taking_mtx);
    return (Mtx44 *)m_taking_mtx;
}

J3DFrameCtrl *TBossBowserCar::playAnimation(const char *anm_name, f32 delta, f32 blend_ratio) {
    J3DFrameCtrl *frameCtrl = mActorData->getFrameCtrl(MActor::BCK);

    if (mActorData->mBckInfo) {
        mActorData->mBckInfo->setOldMotionBlendAnmPtr(
            *(J3DAnmTransform **)((u8 *)mActorData->mBckInfo + 0x24));
    }

    mActorData->setBck(anm_name);
    mActorData->setFrameRate(SMSGetAnmFrameRate() * delta * 0.5f, MActor::BCK);
    return frameCtrl;
}

f32 TBossBowserCar::getDistanceToTargetXZ() const {
    return getDistanceToTargetXZ(m_target->mTranslation);
}

f32 TBossBowserCar::getDistanceToTargetXZ(const TVec3f &target) const {
    TVec3f self_xz   = {mTranslation.x, 0.0f, m_base_translation.z};
    TVec3f target_xz = {target.x, 0.0f, target.z};
    return PSVECDistance(self_xz, target_xz);
}

f32 TBossBowserCar::getAngleToTarget() const {
    return atan2f((m_target->mTranslation.x - m_base_translation.x),
                  (m_target->mTranslation.z - m_base_translation.z));
}

f32 TBossBowserCar::getAngleToTarget(const TVec3f &target) const {
    return atan2f((target.x - m_base_translation.x), (target.z - m_base_translation.z));
}

bool TBossBowserCar::moveToNextNode(f32 speed) {
    if (!mGraphTracer->mGraph || mGraphTracer->mGraph->isDummy()) {
        return false;
    }

    if (mGraphTracer->mGraph->mSplineRail) {
        f32 spline_speed = mGraphTracer->calcSplineSpeed(speed);
        bool traced      = mGraphTracer->traceSpline(spline_speed);

        TVec3f pos, rot;
        mGraphTracer->mGraph->mSplineRail->getPosAndRot(mGraphTracer->_02, &pos, &rot);

        TVec3f direction;
        PSVECSubtract(pos, m_base_translation, direction);
        PSVECNormalize(direction, direction);
        direction.scale(spline_speed);

        mSpeed.x = (mSpeed.x + direction.x);
        mSpeed.y = 0.0f;
        mSpeed.z = (mSpeed.z + direction.z);

        m_base_translation.y = Max(pos.y, mGroundY + BOWSER_FLY_HEIGHT);

        if (traced) {
            readRailFlag();
        }

        return traced;
    } else {
        TVec3f point      = mGraphTracer->mGraph->indexToPoint(mGraphTracer->mCurrentNode);
        TVec3f last_point = mGraphTracer->mGraph->indexToPoint(mGraphTracer->mPreviousNode == -1
                                                                   ? mGraphTracer->mCurrentNode
                                                                   : mGraphTracer->mPreviousNode);

        TVec3f diff = point - m_base_translation;
        diff.y      = 0.0f;

        if (PSVECDotProduct(diff, diff) < 200.0f * (speed * speed)) {
            readRailFlag();
            // mGraphTracer->mGraph->mNodes[mGraphTracer->mCurrentNode].getPoint(m_base_translation);
            return true;
        } else {
        }

        TVec3f direction;
        PSVECNormalize(diff, direction);
        direction.scale(speed);

        f32 dist_full = PSVECDistance(last_point, point);
        f32 dist_cur  = PSVECDistance(point, m_base_translation);

        f32 targetY = Max(lerp<f32>(last_point.y, point.y, 1.0f - (dist_cur / dist_full)),
                          mGroundY + BOWSER_FLY_HEIGHT);

        mSpeed.x = (mSpeed.x + direction.x);
        mSpeed.y = (targetY - m_base_translation.y) * 0.02f;
        mSpeed.z = (mSpeed.z + direction.z);
        return false;
    }
}

void TBossBowserCar::readRailFlag() {
    auto currentIndex = mGraphTracer->mCurrentNode;
    auto prevIndex    = mGraphTracer->mPreviousNode;

    TGraphWeb *graph = mGraphTracer->mGraph;
    if (graph->isDummy())
        return;

    if (prevIndex == -1) {
    } else {
        TGraphNode &graphNode = graph->mNodes[prevIndex];
        TRailNode *railNode   = graphNode.mRailNode;

        /*if (railNode->mValues[1] != 0xFFFF) {
            mTravelSpeed = static_cast<f32>(railNode->mValues[1]) / 100.0f;
        } else {
            mTravelSpeed = 10.0f;
        }*/
    }

    if (currentIndex == -1) {
    } else {
        TGraphNode &graphNextNode = graph->mNodes[currentIndex];
        TRailNode *railNextNode   = graphNextNode.mRailNode;

        if (railNextNode->mValues[1] != 0xFFFF) {
            m_travel_speed = static_cast<f32>(railNextNode->mValues[1]) * BOWSER_SPEED_FACTOR;
        } else {
            m_travel_speed = 100.0f * BOWSER_SPEED_FACTOR;
        }
    }
}

bool TBossBowserCar::processIdle() {
    TVec3f direction = {m_target->mTranslation.x - m_base_translation.x, 0.0f,
                        m_target->mTranslation.z - m_base_translation.z};
    f32 distance     = PSVECDistance(direction, TVec3f::zero());
    mSpeed.x += direction.x * 0.00008f;
    mSpeed.z += direction.z * 0.00008f;

    // Look forward to adjust height in advance
    TVec3f forward = {mSpeed.x, 0.0f, mSpeed.z};
    forward.scale(800.0f);

    TVec3f target = {m_base_translation.x + forward.x,
                     m_base_translation.y + BOWSER_FLY_HEIGHT * 2.0f,
                     m_base_translation.z + forward.z};

    const TBGCheckData *ground;
    f32 ground_y = gpMapCollisionData->checkGround(target.x, target.y, target.z, 0, &ground);

    mSpeed.y += ((ground_y + BOWSER_FLY_HEIGHT) - m_base_translation.y) * 0.0002f;
    if (mSpeed.y < 0.0f) {
        mSpeed.y *= 0.9f;
    } else {
        mSpeed.y *= 0.98f;
    }

    return ground != &TMapCollisionData::mIllegalCheckData;
}

void TBossBowserCar::calcReflectingVelocity(TBGCheckData *data, f32 bounce, TVec3f &speed) {
    f32 b1 = 1.0f + bounce;

    f32 ns  = PSVECDotProduct(speed, data->mNormal);
    speed.x = -(b1 * ns * data->mNormal.x - speed.x);
    speed.y = -(b1 * ns * data->mNormal.y - speed.y);
    speed.z = -(b1 * ns * data->mNormal.z - speed.z);
}

void TBossBowserCar::calcReflectingAngle(TBGCheckData *data, f32 *angle) {
    // Assume the angle represents the direction the object is moving
    f32 radians = angleToRadians(*angle);

    f32 ix = sinf(radians);
    f32 iz = cosf(radians);

    f32 dp = data->mNormal.x * ix + data->mNormal.z * iz;
    f32 rx = ix - 2.0f * dp * data->mNormal.x;
    f32 rz = iz - 2.0f * dp * data->mNormal.z;

    *angle = radiansToAngle(atan2f(-rx, -rz));
}

void TBossBowserCar::moveToTargetXZ() {
    mTranslation.x = m_target->mTranslation.x;
    mTranslation.z = m_target->mTranslation.z;
}

void TBossBowserCar::checkForActorContact() {
    for (size_t i = 0; i < mNumObjs; ++i) {
        if (mCollidingObjs[i]->mObjectID == OBJECT_ID_MARIO) {
            TMario *player = static_cast<TMario *>(mCollidingObjs[i]);

            if (!isStunned()) {
                if (isHostile() && !isDefeated()) {
                    player->damageExec(this, 1, 2, 0, 16.0f, 0x19, 0.0f, 160);
                }
            }
        }
    }
}

bool TBossBowserCar::isPlayerAttacking(TMario *player) {
    if (player->mState != TMario::STATE_DIVE && player->mState != TMario::STATE_DIVESLIDE) {
        return false;
    }

    if (player->mForwardSpeed < 30.0f) {
        return false;
    }

    return true;
}

bool TBossBowserCar::isPlayerPounding(TMario *player) { return false; }

TSpineEnemy *TBossBowserCar::isBombWithinAndAttack(f32 radius, f32 height) {
    if (isInvincible()) {
        return nullptr;
    }

    // Avoid invincible states
    if (mSpineBase->getLatestNerve() == &s_bowser_nerve_shot ||
        mSpineBase->getLatestNerve() == &s_bowser_nerve_rise) {
        return nullptr;
    }

    for (void *obj : gpStrategy->mEnemyGroup->mViewObjList) {
        THitActor *actor = static_cast<THitActor *>(obj);
        if (actor->mObjectID == 0x1000001E) {
            TSpineEnemy *enemy = static_cast<TSpineEnemy *>(actor);
            if ((enemy->mStateFlags.asU32 & 0b11) || enemy->mHolder == this) {
                continue;
            }

            if (!isDamageToCannon__8TBombHeiFv(enemy)) {
                continue;
            }

            if (enemy->mTranslation.y < mTranslation.y) {
                continue;
            }

            // Check if the bomb is within the ecliptic radius
            TVec3f diff = {enemy->mTranslation.x - mTranslation.x,
                           enemy->mTranslation.y - mTranslation.y,
                           enemy->mTranslation.z - mTranslation.z};
            f32 factor  = (diff.x / radius) * (diff.x / radius) +
                         (diff.y / height) * (diff.y / height) +
                         (diff.z / radius) * (diff.z / radius);
            if (factor <= 1.0f) {
                return enemy;
            }
        }
    }
    return nullptr;
}

f32 TBossBowserCar::getHighestGround(const TVec3f &point, f32 radius, const TBGCheckData **out) {
    // Sample the ground at 8 points around the circle and once in the center
    f32 highest = -32768.0f;
    for (size_t i = 0; i < 8; ++i) {
        f32 angle = i * M_PI / 4.0f;
        f32 x     = point.x + radius * sinf(angle);
        f32 z     = point.z + radius * cosf(angle);

        const TBGCheckData *data;
        f32 y = gpMapCollisionData->checkGround(x, point.y, z, 0, &data);
        if (y > highest) {
            highest = y;
            *out    = data;
        }
    }

    const TBGCheckData *center;
    f32 y = gpMapCollisionData->checkGround(point.x, point.y, point.z, 0, &center);
    if (y > highest) {
        highest = y;
        *out    = center;
    }

    return highest;
}

void TBossBowserCar::doWorldShake(f32 strength) {
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

f32 TBossBowserCar::calcNextAngle(const TVec3f &target, f32 turn_power, f32 offset,
                                  bool indiscriminate) {
    if (getDistanceToTargetXZ(target) > 400.0f || indiscriminate) {
        f32 zhine_angle  = normalizeRadians(angleToRadians(mRotation.y));
        f32 radian_angle = normalizeRadians(getAngleToTarget(target) + angleToRadians(offset));

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

void TBossBowserCar::spawnShine() {
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

TBossBowserCarManager::TBossBowserCarManager(const char *name) : TEnemyManager(name) {}

void TBossBowserCarManager::load(JSUMemoryInputStream &in) {
    TEnemyManager::load(in);
    gpResourceManager->load("/scene/bowsercar/jpa/ms_kp_fire_a.jpa", 0x1a3);
    gpResourceManager->load("/scene/bowsercar/jpa/ms_kp_fire_b.jpa", 0x1a4);
    gpResourceManager->load("/scene/bowsercar/jpa/ms_kp_fire_c.jpa", 0x1a5);
    gpResourceManager->load("/scene/bowsercar/jpa/ms_kp_fire_d.jpa", 0x1a6);
    gpResourceManager->load("/scene/bowsercar/jpa/ms_kp_fire_e.jpa", 0x1a7);
    gpResourceManager->load("/scene/bowsercar/jpa/ms_kp_hit1_a.jpa", 0x1b0);
    gpResourceManager->load("/scene/bowsercar/jpa/ms_kp_hit1_b.jpa", 0x1b1);
    gpResourceManager->load("/scene/bowsercar/jpa/ms_kp_shock_a.jpa", 0x1b2);
    gpResourceManager->load("/scene/bowsercar/jpa/ms_kp_shock_b.jpa", 0x1b3);
    gpResourceManager->load("/scene/bowsercar/jpa/ms_kp_shot_a.jpa", 0x1b4);
    gpResourceManager->load("/scene/bowsercar/jpa/ms_kp_smoke_a.jpa", 0x1b5);
    gpResourceManager->load("/scene/bowsercar/jpa/ms_kp_spark_a.jpa", 0x1b6);
    gpResourceManager->load("/scene/bowsercar/jpa/ms_kp_spark_b.jpa", 0x1b7);
    gpResourceManager->load("/scene/map/map/ms_kp_hipdrop.jpa", 0x1b8);
}

bool TNerveBowserIdle::execute(TSpineBase<TLiveActor> *spine) const {
    TBossBowserCar *bowser = static_cast<TBossBowserCar *>(spine->mTarget);
    TGraphWeb *web         = bowser->mGraphTracer->mGraph;

    bowser->mGravity             = 0.0f;
    bowser->m_target_gun_l_angle = 0.0f;
    bowser->m_target_gun_r_angle = 0.0f;

    if (spine->mNerveTimer == 0) {
        bowser->playAnimation("bowser_idle", 0.8f, 0.5f);
    }

    if (gpMarDirector->mNextStateA != 0 || !TFlagManager::smInstance->getFlag(0x6000B)) {
        if (bowser->moveToNextNode(bowser->m_travel_speed)) {
            int nextIndex = web->getRandomNextIndex(bowser->mGraphTracer->mCurrentNode,
                                                    bowser->mGraphTracer->mPreviousNode, -1);
            bowser->mGraphTracer->moveTo(nextIndex);

            if (!bowser->m_last_taunt) {
                bowser->mSpineBase->pushNerve(&s_bowser_nerve_taunt);
                bowser->m_last_taunt = true;
                return true;
            }

            bowser->m_last_taunt = false;
            return false;
        }
    }

    if (bowser->isHardMode() ||
        bowser->getDistanceToTargetXZ(bowser->m_target->mTranslation) < BOWSER_WATCH_MARIO_RADIUS) {
        // Face mario
        bowser->mRotation.y =
            bowser->calcNextAngle(bowser->m_target->mTranslation, BOWSER_TURN_POWER, 0.0f);
    } else {
        if (web && !web->isDummy()) {
            TVec3f target       = web->indexToPoint(bowser->mGraphTracer->getCurGraphIndex());
            bowser->mRotation.y = bowser->calcNextAngle(target, BOWSER_TURN_POWER, 0.0f);
        }
    }

    if (web) {
        if (bowser->isHardMode()) {
            if (bowser->moveToNextNode(bowser->m_travel_speed * 1.2f)) {
                bowser->m_last_fire = false;

                if ((rand() % 100) < 50) {
                    bowser->mSpineBase->pushNerve(&s_bowser_nerve_throw);
                } else {
                    bowser->mSpineBase->pushNerve(&s_bowser_nerve_shoot);
                }

                f32 angle        = bowser->getAngleToTarget();
                TVec3f direction = {sinf(angle), 0.0f, cosf(angle)};

                int nextIndex = web->getRandomButDirLimited(
                    bowser->mGraphTracer->mCurrentNode, bowser->mGraphTracer->mPreviousNode,
                    direction, bowser->m_base_translation, 90.0f, -1);
                bowser->mGraphTracer->moveTo(nextIndex);
                return true;
            }
        } else {
            if (bowser->moveToNextNode(bowser->m_travel_speed)) {
                bowser->m_last_fire = false;

                if ((rand() % 100) < 70 || bowser->m_last_taunt) {
                    if ((rand() % 100) < 20) {
                        bowser->mSpineBase->pushNerve(&s_bowser_nerve_shoot);
                    } else {
                        bowser->mSpineBase->pushNerve(&s_bowser_nerve_throw);
                    }
                    bowser->m_last_taunt = false;
                } else {
                    bowser->mSpineBase->pushNerve(&s_bowser_nerve_taunt);
                    bowser->m_last_taunt = true;
                }

                int nextIndex = web->getRandomNextIndex(bowser->mGraphTracer->mCurrentNode,
                                                        bowser->mGraphTracer->mPreviousNode, -1);
                bowser->mGraphTracer->moveTo(nextIndex);
                return true;
            }
        }
    }

    if (bowser->getDistanceToTargetXZ() < 3000.0f &&
        fabsf(bowser->mTranslation.y - bowser->m_target->mTranslation.y) < 800.0f &&
        spine->mNerveTimer > 160 && !bowser->m_last_fire) {
        spine->pushNerve(&s_bowser_nerve_fire);
        bowser->m_last_fire = true;
        return true;
    }

    return false;
}

bool TNerveBowserTaunt::execute(TSpineBase<TLiveActor> *spine) const {
    TBossBowserCar *bowser = static_cast<TBossBowserCar *>(spine->mTarget);

    bowser->mGravity = 0.0f;

    if (spine->mNerveTimer == 0) {
        bowser->playAnimation("bowser_taunt", 0.9f, 0.5f);
        if (gpMSound->gateCheck(MSD_SE_BS_KOOPA_VO_LAUGH)) {
            JAISound *sound = MSoundSE::startSoundActor(MSD_SE_BS_KOOPA_VO_LAUGH,
                                                        bowser->mTranslation, 0, nullptr, 0, 4);
            if (sound) {
                sound->setTempoProportion(0.9f, 5);
            }
        }
    }

    if (spine->mNerveTimer == 40) {
    }

    if (bowser->mActorData->isCurAnmAlreadyEnd(MActor::BCK)) {
        spine->pushNerve(&s_bowser_nerve_idle);
        return true;
    }

    return false;
}

const char *bomb_name = "\x83\x7B\x83\x80\x95\xBA\x83\x7D\x83\x6C\x81\x5B\x83\x57\x83\x83\x81\x5B";

bool TNerveBowserThrow::execute(TSpineBase<TLiveActor> *spine) const {
    TBossBowserCar *bowser = static_cast<TBossBowserCar *>(spine->mTarget);

    bowser->mGravity = 0.0f;

    if (spine->mNerveTimer == 0) {
        bowser->playAnimation("bowser_throw", 0.5f, 0.5f);
        if (gpMSound->gateCheck(MSD_SE_BS_KOOPA_VO_THREAT_0)) {
            MSoundSE::startSoundActor(MSD_SE_BS_KOOPA_VO_THREAT_0, bowser->mTranslation, 0, nullptr,
                                      0, 4);
        }

        for (size_t i = 0; i < 10; ++i) {
            bowser->m_damage_pos.x =
                bowser->mTranslation.x + (sinf((f32)(rand() % 101) * 0.02f * M_PI) * 800.0f);
            bowser->m_damage_pos.z =
                bowser->mTranslation.z + (cosf((f32)(rand() % 101) * 0.02f * M_PI) * 800.0f);

            bowser->m_damage_pos.y = gpMapCollisionData->checkGround(
                bowser->m_damage_pos.x, bowser->mTranslation.y + 800.0f, bowser->m_damage_pos.z, 0,
                &bowser->m_damage_ground);

            if (bowser->m_damage_ground != &TMapCollisionData::mIllegalCheckData) {
                break;
            }
        }

        bowser->mHeldObject =
            (TTakeActor *)gpConductor->makeOneEnemyAppear(bowser->mTranslation, bomb_name, 1);
        if (bowser->mHeldObject) {
            TSpineEnemy *bombhei = static_cast<TSpineEnemy *>(bowser->mHeldObject);
            bombhei->reset();
            bombhei->mActorData->setFrameRate(0.0f, MActor::BCK);
            bombhei->mHolder = bowser;
            bombhei->ensureTakeSituation();
        }
    }

    const TVec3f &attack_pos = bowser->m_damage_ground == &TMapCollisionData::mIllegalCheckData ||
                                       bowser->getDistanceToTargetXZ() < BOWSER_WATCH_MARIO_RADIUS
                                   ? bowser->m_target->mTranslation
                                   : bowser->m_damage_pos;

    if (spine->mNerveTimer < 150) {
        bowser->mRotation.y = bowser->calcNextAngle(attack_pos, BOWSER_TURN_POWER, 0.0f, true);
    }

    if (spine->mNerveTimer == 150) {
        if (gpMSound->gateCheck(MSD_SE_BS_KOOPA_VO_GROAN_1)) {
            MSoundSE::startSoundActor(MSD_SE_BS_KOOPA_VO_GROAN_1, bowser->mTranslation, 0, nullptr,
                                      0, 4);
        }

        TSpineEnemy *bombhei = static_cast<TSpineEnemy *>(bowser->mHeldObject);
        if (bombhei) {
            TVec3f throw_speed = {10.0f, 10.0f, 10.0f};

            TVec3f direction = attack_pos - bowser->mTranslation;
            direction.y      = 0.0f;

            PSVECNormalize(direction, direction);
            direction *= throw_speed;
            direction.scale(0.5f + (rand() % 100) * 0.005f);
            direction.y = 15.0f;

            bombhei->mSpeed  = direction;
            bombhei->mHolder = nullptr;
            bombhei->ensureTakeSituation();
            bowser->mHeldObject = nullptr;
        }
    }

    if (bowser->mActorData->isCurAnmAlreadyEnd(MActor::BCK)) {
        spine->pushNerve(&s_bowser_nerve_idle);
        return true;
    }

    return false;
}

bool TNerveBowserFire::execute(TSpineBase<TLiveActor> *spine) const {
    TBossBowserCar *bowser = static_cast<TBossBowserCar *>(spine->mTarget);

    bowser->mGravity = 0.0f;

    if (spine->mNerveTimer == 0) {
        bowser->m_nerve_status = 0;
        bowser->playAnimation("bowser_fire_begin", 1.2f, 0.5f);

        if (gpMSound->gateCheck(MSD_SE_BS_KOOPA_FIRE_START)) {
            JAISound *sound = MSoundSE::startSoundActor(MSD_SE_BS_KOOPA_FIRE_START,
                                                        bowser->mTranslation, 0, nullptr, 0, 4);
            if (sound) {
                /*f32 pitch = 0.9f + (rand() % 100) * 0.002f;
                f32 tempo = 0.9f + (rand() % 100) * 0.002f;
                sound->setPitch(pitch, 0, 0);
                sound->setTempoProportion(tempo, 0);*/
            }
        }
    }

    bowser->mRotation.y =
        bowser->calcNextAngle(bowser->m_target->mTranslation, BOWSER_TURN_POWER * 0.3f, 0.0f);

    if (bowser->m_nerve_status == 1) {
        if (gpMSound->gateCheck(MSD_SE_BS_KOOPA_FIRE_LOOP)) {
            JAISound *sound = MSoundSE::startSoundActor(MSD_SE_BS_KOOPA_FIRE_LOOP,
                                                        bowser->mTranslation, 0, nullptr, 0, 4);
            if (sound) {
                /*f32 pitch = 0.9f + (rand() % 100) * 0.002f;
                f32 tempo = 0.9f + (rand() % 100) * 0.002f;
                sound->setPitch(pitch, 0, 0);
                sound->setTempoProportion(tempo, 0);*/
            }
        }
        Mtx *fire_mtx = &bowser->mActorData->mModel->mJointArray[bowser->m_fire_index];

        bowser->m_fire_joint_pos = {(*fire_mtx)[0][3], (*fire_mtx)[1][3], (*fire_mtx)[2][3]};

        if (spine->mNerveTimer % 10 == 0) {
#if 1
            /*JPABaseEmitter *emitterFire = (JPABaseEmitter *)
                emitParticleCallBack__21TMarioParticleManagerFlPCQ29JGeometry8TVec3_f(
                    gpMarioParticleManager, 0x1a7, &bowser->m_fire_joint_pos, 1,
                    &s_fire_particle_callback, bowser);*/
#else
            JPABaseEmitter *emitterFire = gpMarioParticleManager->emitParticleCallback(
                0x1a7, &bowser->m_fire_joint_pos, 1, &s_fire_particle_callback, bowser);
#endif

            gpMarioParticleManager->emitAndBindToMtxPtr(0x1a7, *fire_mtx, 1, bowser);

            if (JPABaseEmitter *emitterFire =
                    gpMarioParticleManager->emitAndBindToMtxPtr(0x1a6, *fire_mtx, 1, bowser)) {
                *(JPACallBackBase<> **)((u8 *)emitterFire + 0x110) = &s_fire_particle_callback;
            }

            gpMarioParticleManager->emitAndBindToMtxPtr(0x1a5, *fire_mtx, 1, bowser);
            gpMarioParticleManager->emitAndBindToMtxPtr(0x1a4, *fire_mtx, 1, bowser);
            gpMarioParticleManager->emitAndBindToMtxPtr(0x1a3, *fire_mtx, 1, bowser);
        }
    }

    if (bowser->mActorData->isCurAnmAlreadyEnd(MActor::BCK)) {
        if (bowser->m_nerve_status == 0) {
            bowser->m_nerve_status = 1;
            bowser->playAnimation("bowser_fire_loop", 0.7f, 0.5f);
        } else if (bowser->m_nerve_status == 2) {
            bowser->m_nerve_status = 0;
            spine->pushNerve(&s_bowser_nerve_idle);
            return true;
        }
    }

    if (spine->mNerveTimer > 700) {
        if (bowser->m_nerve_status == 1) {
            bowser->m_nerve_status = 2;
            bowser->playAnimation("bowser_fire_end", 0.7f, 0.5f);
        }
    }

    return false;
}

bool TNerveBowserShoot::execute(TSpineBase<TLiveActor> *spine) const {
    TBossBowserCar *bowser = static_cast<TBossBowserCar *>(spine->mTarget);

    bowser->mGravity = 0.0f;
    bowser->mRotation.y =
        bowser->calcNextAngle(bowser->m_target->mTranslation, BOWSER_TURN_POWER, 0.0f);

    if (spine->mNerveTimer == 0) {
        bowser->m_nerve_status = 0;

        bowser->playAnimation("bowser_taunt", 0.9f, 0.5f);

        if (gpMSound->gateCheck(MSD_SE_BS_KOOPA_VO_LAUGH)) {
            MSoundSE::startSoundActor(MSD_SE_BS_KOOPA_VO_LAUGH, bowser->mTranslation, 0, nullptr, 0,
                                      4);
        }

        bowser->m_target_gun_l_angle = (((f32)(rand() % 100) * 0.01f) - 0.5f) * M_PI * 0.15;
        bowser->m_target_gun_r_angle = (((f32)(rand() % 100) * 0.01f) - 0.5f) * M_PI * 0.15;

        bowser->m_bullets_shot = 0;
    }

    if (bowser->m_nerve_status == 0 && bowser->mActorData->isCurAnmAlreadyEnd(MActor::BCK)) {
        bowser->m_nerve_status = 1;
        bowser->playAnimation("bowser_idle", 0.7f, 0.5f);
    }

    if (spine->mNerveTimer > 0 && (spine->mNerveTimer % 120) == 0) {
        if (bowser->m_bullets_shot > (bowser->isHardMode() ? 4 : 2)) {
            spine->pushNerve(&s_bowser_nerve_idle);
            return true;
        }

        // Make bullet bill l appear
        f32 angle      = angleToRadians(bowser->mRotation.y);
        TVec3f forward = {sinf(angle), 0.0f, cosf(angle)};
        TVec3f right;
        PSVECCrossProduct(forward, TVec3f::up(), right);

        TVec3f fire_pos = bowser->mTranslation;
        if ((bowser->m_bullets_shot % 2) == 0) {
            fire_pos.x -= right.x * 400.0f;
            fire_pos.y -= right.y * 400.0f;
            fire_pos.z -= right.z * 400.0f;
        } else {
            fire_pos.x += right.x * 400.0f;
            fire_pos.y += right.y * 400.0f;
            fire_pos.z += right.z * 400.0f;
        }

        if (TSpineEnemy *enemy =
                (TSpineEnemy *)gpConductor->makeOneEnemyAppear(fire_pos, s_killer_name, 1)) {
            enemy->mRotation.y = bowser->mRotation.y;
            enemy->mSpeed      = forward;
            enemy->mSpeed.scale(12.0f);
            enemy->mScale.scale(1.8f);
        }

        TVec3f p_pos = fire_pos;
        forward.scale(400.0f);
        p_pos.x += forward.x;
        p_pos.y += forward.y;
        p_pos.z += forward.z;

        if (JPABaseEmitter *emitter = gpMarioParticleManager->emitWithRotate(
                0xa1, &p_pos, 0, convertAngleFloatToS16(bowser->mRotation.y), 0, 1, bowser)) {
            emitter->mSize1 = {10.0f, 10.0f, 10.0f};
        }

        if (JPABaseEmitter *emitter = gpMarioParticleManager->emitWithRotate(
                0xa2, &p_pos, 0, convertAngleFloatToS16(bowser->mRotation.y), 0, 1, bowser)) {
            emitter->mSize1 = {10.0f, 10.0f, 10.0f};
        }

        bowser->m_bullets_shot++;
    }

    return false;
}

bool TNerveBowserWet::execute(TSpineBase<TLiveActor> *spine) const {
    TBossBowserCar *bowser = static_cast<TBossBowserCar *>(spine->mTarget);

    bowser->mGravity            = 0.0f;
    bowser->m_target_tilt       = (f32)(bowser->m_wet_count) / 255.0f;
    bowser->m_target_tilt_speed = 1.0f;

    if (spine->mNerveTimer == 0) {
        bowser->playAnimation("bowser_water_begin", 1.0f, 0.5f);

        TSpineEnemy *bombhei = static_cast<TSpineEnemy *>(bowser->mHeldObject);
        if (bombhei) {
            f32 angle          = ((f32)(rand() % 100) / 100.0f) * 2.0f * M_PI;
            TVec3f throw_speed = {sinf(angle), 0.0f, cosf(angle)};
            PSVECNormalize(throw_speed, throw_speed);
            throw_speed.scale(10.0f);
            throw_speed.y = 20.0f;

            bombhei->mSpeed  = throw_speed;
            bombhei->mHolder = nullptr;
            bombhei->ensureTakeSituation();
            bowser->mHeldObject = nullptr;
        }
    }

    if ((spine->mNerveTimer % 200) == 0) {
        if (gpMSound->gateCheck(MSD_SE_BS_KOOPA_VO_CRY)) {
            JAISound *sound = MSoundSE::startSoundActor(MSD_SE_BS_KOOPA_VO_CRY,
                                                        bowser->mTranslation, 0, nullptr, 0, 4);
            if (sound) {
                f32 pitch = 0.9f + (rand() % 100) * 0.002f;
                f32 tempo = 0.9f + (rand() % 100) * 0.002f;
                sound->setPitch(pitch, 0, 0);
                sound->setTempoProportion(tempo, 0);
            }
        }
    }

    if (bowser->mActorData->isCurAnmAlreadyEnd(MActor::BCK)) {
        if (bowser->m_nerve_status == 0) {
            bowser->m_nerve_status = 1;
            bowser->playAnimation("bowser_water_loop", 0.7f, 0.5f);
        } else if (bowser->m_nerve_status == 3) {
            bowser->m_nerve_status = 0;
            spine->pushNerve(&s_bowser_nerve_idle);
            return true;
        }
    }

    if (bowser->m_nerve_status == 2) {
        bowser->m_nerve_status = 3;
        bowser->playAnimation("bowser_water_end", 0.7f, 0.5f);
    }

    return false;
}

bool TNerveBowserStun::execute(TSpineBase<TLiveActor> *spine) const {
    TBossBowserCar *bowser = static_cast<TBossBowserCar *>(spine->mTarget);

    bowser->mGravity = 0.0f;
    bowser->setStunned(true);

    if (spine->mNerveTimer == 0) {
        bowser->playAnimation("bowser_stun_begin", 1.0f, 0.5f);
        bowser->m_target_tilt       = 0.5f;
        bowser->m_target_tilt_speed = 0.5f;
    }

    if (bowser->mActorData->isCurAnmAlreadyEnd(MActor::BCK)) {
        if (bowser->m_nerve_status == 0) {
            bowser->m_nerve_status = 1;
            bowser->playAnimation("bowser_stun_loop", 0.7f, 0.5f);
        } else if (bowser->m_nerve_status == 2) {
            bowser->m_nerve_status = 0;
            spine->pushNerve(&s_bowser_nerve_rise);
            return true;
        }
    }

    if (bowser->m_nerve_status == 1) {
        if (spine->mNerveTimer % 200 == 0) {
            if (gpMSound->gateCheck(MSD_SE_BS_KOOPA_VO_GROAN_0)) {
                JAISound *sound = MSoundSE::startSoundActor(MSD_SE_BS_KOOPA_VO_GROAN_0,
                                                            bowser->mTranslation, 0, nullptr, 0, 4);
                if (sound) {
                    f32 pitch = 0.9f + (rand() % 100) * 0.002f;
                    f32 tempo = 0.9f + (rand() % 100) * 0.002f;
                    sound->setPitch(pitch, 0, 0);
                    sound->setTempoProportion(tempo, 0);
                    sound->setVolume(0.3f, 0, 0);
                }
            }
        }

        if (spine->mNerveTimer > 1600) {
            bowser->m_nerve_status      = 2;
            bowser->m_target_tilt       = 0.0f;
            bowser->m_target_tilt_speed = 0.5f;
            bowser->playAnimation("bowser_stun_end", 0.7f, 0.5f);
        }
    }

    return false;
}

bool TNerveBowserShot::execute(TSpineBase<TLiveActor> *spine) const {
    TBossBowserCar *bowser = static_cast<TBossBowserCar *>(spine->mTarget);

    bowser->mGravity = 0.0f;

    if (bowser->isDefeated()) {
        // Rotate away from fly away point

        TVec3f forward      = bowser->mTranslation - s_fly_away_point;
        bowser->mRotation.y = bowser->calcNextAngle(forward, BOWSER_TURN_POWER, 0.0f);

        TSpineEnemy *bombhei = static_cast<TSpineEnemy *>(bowser->mHeldObject);
        if (bombhei) {
            f32 angle          = ((f32)(rand() % 100) / 100.0f) * 2.0f * M_PI;
            TVec3f throw_speed = {sinf(angle), 0.0f, cosf(angle)};
            throw_speed.scale(10.0f);
            throw_speed.y = 20.0f;

            bombhei->mSpeed  = throw_speed;
            bombhei->mHolder = nullptr;
            bombhei->ensureTakeSituation();
            bowser->mHeldObject = nullptr;
        }
    }

    if (spine->mNerveTimer == 0) {
        bowser->playAnimation("bowser_shot", 0.9f, 0.5f);
        if (gpMSound->gateCheck(MSD_SE_BS_KOOPA_VO_DAMAGE_0)) {
            MSoundSE::startSoundActor(MSD_SE_BS_KOOPA_VO_DAMAGE_0, bowser->mTranslation, 0, nullptr,
                                      0, 4);
        }

        if (gpMSound->gateCheck(MSD_SE_OBJ_SANDBOMB_BANG)) {
            MSoundSE::startSoundActor(MSD_SE_OBJ_SANDBOMB_BANG, bowser->m_rocket_hit_pos, 0,
                                      nullptr, 0, 4);
        }

        TVec3f scale = {2.0f, 2.0f, 2.0f};

        void *explosion =
            gpConductor->makeOneEnemyAppear(bowser->m_rocket_hit_pos, s_explosion_name, 1);
        if (explosion) {
            generate__16TEffectExplosionFRQ29JGeometry8TVec3_f(explosion, bowser->m_rocket_hit_pos,
                                                               scale);
        }

        bowser->doWorldShake(0.2f);
        if (JPABaseEmitter *emitter = gpMarioParticleManager->emitAndBindToPosPtr(
                11, &bowser->m_rocket_hit_pos, 0, nullptr)) {
            emitter->mSize1 = {5.0f, 5.0f, 5.0f};
        }

        TSpineEnemy *bombhei = static_cast<TSpineEnemy *>(bowser->mHeldObject);
        if (bombhei) {
            f32 angle          = ((f32)(rand() % 100) / 100.0f) * 2.0f * M_PI;
            TVec3f throw_speed = {sinf(angle), 0.0f, cosf(angle)};
            PSVECNormalize(throw_speed, throw_speed);
            throw_speed.scale(10.0f);
            throw_speed.y = 20.0f;

            bombhei->mSpeed  = throw_speed;
            bombhei->mHolder = nullptr;
            bombhei->ensureTakeSituation();
            bowser->mHeldObject = nullptr;
        }

        bowser->mSpeed.x = -sinf(angleToRadians(bowser->m_rocket_angle)) * 20.0f;
        bowser->mSpeed.y = 10.0f;
        bowser->mSpeed.z = -cosf(angleToRadians(bowser->m_rocket_angle)) * 20.0f;
    }

    if (bowser->mActorData->isCurAnmAlreadyEnd(MActor::BCK)) {
        if (bowser->isDefeated()) {
            spine->pushNerve(&s_bowser_nerve_prekill);
        } else {
            spine->pushNerve(&s_bowser_nerve_recover);
        }
        return true;
    }

    return false;
}

bool TNerveBowserRecover::execute(TSpineBase<TLiveActor> *spine) const {
    TBossBowserCar *bowser = static_cast<TBossBowserCar *>(spine->mTarget);

    bowser->mGravity = 0.0f;

    if (spine->mNerveTimer == 0) {
        bowser->playAnimation("bowser_recover", 0.8f, 0.5f);

        if (gpMSound->gateCheck(MSD_SE_BS_KOOPA_VO_THREAT_1)) {
            MSoundSE::startSoundActor(MSD_SE_BS_KOOPA_VO_THREAT_1, bowser->mTranslation, 0, nullptr,
                                      0, 4);
        }
    }

    // Face mario
    bowser->mRotation.y =
        bowser->calcNextAngle(bowser->m_target->mTranslation, BOWSER_TURN_POWER, 0.0f);

    if (bowser->mActorData->isCurAnmAlreadyEnd(MActor::BCK)) {
        spine->pushNerve(&s_bowser_nerve_idle);
        return true;
    }

    return false;
}

bool TNerveBowserRise::execute(TSpineBase<TLiveActor> *spine) const {
    TBossBowserCar *bowser = static_cast<TBossBowserCar *>(spine->mTarget);

    bowser->mGravity = 0.0f;
    bowser->mSpeed.y =
        ((bowser->m_highest_ground_y + BOWSER_FLY_HEIGHT) - bowser->mTranslation.y) * 0.015f;

    if (spine->mNerveTimer == 0) {
        bowser->playAnimation("bowser_recover", 0.8f, 0.5f);
        if (gpMSound->gateCheck(MSD_SE_BS_KOOPA_VO_THREAT_1)) {
            MSoundSE::startSoundActor(MSD_SE_BS_KOOPA_VO_THREAT_1, bowser->mTranslation, 0, nullptr,
                                      0, 4);
        }
    }

    if (bowser->mActorData->isCurAnmAlreadyEnd(MActor::BCK)) {
        spine->pushNerve(&s_bowser_nerve_idle);
        return true;
    }

    return false;
}

bool TNerveBowserShock::execute(TSpineBase<TLiveActor> *spine) const {
    TBossBowserCar *bowser = static_cast<TBossBowserCar *>(spine->mTarget);

    bowser->mGravity            = 0.0f;
    bowser->m_target_tilt       = 0.5f;
    bowser->m_target_tilt_speed = 2.0f;

    if (spine->mNerveTimer == 0) {
        bowser->playAnimation("bowser_shock_begin", 1.0f, 0.5f);

        TSpineEnemy *bombhei = static_cast<TSpineEnemy *>(bowser->mHeldObject);
        if (bombhei) {
            f32 angle = ((f32)(rand() % 100) / 100.0f) * 2.0f * M_PI;
            f32 power = (f32)(rand() % 100) / 100.0f;

            bombhei->mSpeed  = {sinf(angle) * 10.0f * power, 15.0f, cosf(angle) * 10.0f * power};
            bombhei->mHolder = nullptr;
            bombhei->ensureTakeSituation();
            bowser->mHeldObject = nullptr;
        }

        s_bomb_message.show();
    }

    if ((spine->mNerveTimer % 200) == 0) {
        if (gpMSound->gateCheck(MSD_SE_BS_KOOPA_VO_CRY)) {
            JAISound *sound = MSoundSE::startSoundActor(MSD_SE_BS_KOOPA_VO_CRY,
                                                        bowser->mTranslation, 0, nullptr, 0, 4);
            if (sound) {
                f32 pitch = 0.9f + (rand() % 100) * 0.002f;
                f32 tempo = 0.9f + (rand() % 100) * 0.002f;
                sound->setPitch(pitch, 0, 0);
                sound->setTempoProportion(tempo, 0);
            }
        }

        TSpineEnemy *bombhei = static_cast<TSpineEnemy *>(
            gpConductor->makeOneEnemyAppear(bowser->mTranslation, bomb_name, 1));
        if (bombhei) {
            f32 angle = ((f32)(rand() % 100) / 100.0f) * 2.0f * M_PI;
            f32 power = (f32)(rand() % 100) / 100.0f;

            TVec3f throw_speed = {sinf(angle), 0.0f, cosf(angle)};
            PSVECNormalize(throw_speed, throw_speed);
            throw_speed.scale(7.0f * power);
            throw_speed.y = 20.0f;

            bombhei->mSpeed = throw_speed;
        }
    }

    if (bowser->mActorData->isCurAnmAlreadyEnd(MActor::BCK)) {
        if (bowser->m_nerve_status == 0) {
            bowser->m_nerve_status = 1;
            bowser->playAnimation("bowser_shock_loop", 0.7f, 0.5f);
        } else if (bowser->m_nerve_status == 3) {
            bowser->m_nerve_status = 0;
            spine->pushNerve(&s_bowser_nerve_rise);
            return true;
        }
    }

    if (bowser->m_nerve_status == 1) {
        bowser->mSpeed.y = -5.0f;

        for (size_t i = 0; i < 3; ++i) {
            if (JPABaseEmitter *emitter = gpMarioParticleManager->emitAndBindToPosPtr(
                    0x1b2, &bowser->mTranslation, 1, bowser)) {
                emitter->mSize1 = {0.2f, 0.2f, 0.2f};
                emitter->mSize3 = {0.2f, 0.2f, 0.2f};
            }
        }

        if (bowser->mTranslation.y - bowser->m_highest_ground_y < (BOWSER_FLY_HEIGHT * 0.5f)) {
            bowser->m_nerve_status    = 2;
            bowser->m_shock_wait_time = 600;
        }
    } else if (bowser->m_nerve_status == 2) {
        bowser->mSpeed.y *= 0.9f;
        bowser->m_target_tilt       = 0.5f;
        bowser->m_target_tilt_speed = 0.5f;

        for (size_t i = 0; i < 3; ++i) {
            if (JPABaseEmitter *emitter = gpMarioParticleManager->emitAndBindToPosPtr(
                    0x1b2, &bowser->mTranslation, 1, bowser)) {
                emitter->mSize1 = {0.2f, 0.2f, 0.2f};
                emitter->mSize3 = {0.2f, 0.2f, 0.2f};
            }
        }

        if (bowser->m_wet_count < 40) {
            bowser->m_shock_wait_time -= 1;
        }

        if (bowser->m_shock_wait_time <= 0) {
            bowser->m_nerve_status = 3;
            bowser->playAnimation("bowser_shock_end", 0.7f, 0.5f);
        }
    }

    return false;
}

extern void ControlBowserPrekillCam_(CPolarSubCamera *camera);
extern void ControlBowserKillCam_(CPolarSubCamera *camera);

bool TNerveBowserPreKill::execute(TSpineBase<TLiveActor> *spine) const {
    TBossBowserCar *bowser = static_cast<TBossBowserCar *>(spine->mTarget);
    bowser->mGravity       = 0.0f;
    bowser->mSpeed         = TVec3f::up();

    ControlBowserPrekillCam_(gpCamera);

    if (spine->mNerveTimer == 0) {
        bowser->playAnimation("bowser_dead_start", 1.0f, 0.5f);

        if (gpMSound->gateCheck(MSD_SE_OBJ_SANDBOMB_BANG)) {
            JAISound *sound = MSoundSE::startSoundActor(MSD_SE_OBJ_SANDBOMB_BANG,
                                                        bowser->mTranslation, 0, nullptr, 0, 4);
            if (sound) {
                sound->setVolume(0.5f, 0, 0);
            }
        }

        TFlagManager::smInstance->setFlag(0x6001B, true);
        gpMarioAddress->mInvincibilityFrames = 600;

        if (gpMarioAddress->mHealth < 8) {
            gpMarioAddress->incHP(8 - gpMarioAddress->mHealth);
        }

        gpConductor->killEnemiesWithin(gpMarioAddress->mTranslation, 2000.0f);
        bowser->mStateFlags.asU32 &= ~0x41;
    }

    if (spine->mNerveTimer == 180) {
        if (gpMSound->gateCheck(MSD_SE_BS_KOOPA_VO_GROAN_0)) {
            MSoundSE::startSoundActor(MSD_SE_BS_KOOPA_VO_GROAN_0, bowser->mTranslation, 0, nullptr,
                                      0, 4);
        }
    }

    if (spine->mNerveTimer == 360) {
        if (gpMSound->gateCheck(MSD_SE_BS_KOOPA_VO_GROAN_1)) {
            MSoundSE::startSoundActor(MSD_SE_BS_KOOPA_VO_GROAN_1, bowser->mTranslation, 0, nullptr,
                                      0, 4);
        }
    }

    if (bowser->mActorData->isCurAnmAlreadyEnd(MActor::BCK)) {
        spine->pushNerve(&s_bowser_nerve_kill);
        return true;
    }

    return false;
}

bool TNerveBowserKill::execute(TSpineBase<TLiveActor> *spine) const {
    TBossBowserCar *bowser = static_cast<TBossBowserCar *>(spine->mTarget);

    bowser->mGravity = 0.08f;

    ControlBowserKillCam_(gpCamera);

    if (spine->mNerveTimer == 0) {
        bowser->playAnimation("bowser_dead_end", 0.5f, 1.0f);

        if (gpMSound->gateCheck(MSD_SE_BS_KOOPA_VO_DAMAGE_1)) {
            JAISound *sound = MSoundSE::startSoundActor(MSD_SE_BS_KOOPA_VO_DAMAGE_1,
                                                        bowser->mTranslation, 0, nullptr, 0, 4);
            if (sound) {
                sound->setTempoProportion(0.4f, 3);
            }
        }

        if (gpMSound->gateCheck(MSD_SE_OBJ_SANDBOMB_BANG)) {
            MSoundSE::startSoundSystemSE(MSD_SE_OBJ_SANDBOMB_BANG, 0, nullptr, 0);
        }

        if (gpMSound->gateCheck(MSD_SE_OBJ_GLASS_BREAK)) {
            MSoundSE::startSoundSystemSE(MSD_SE_OBJ_GLASS_BREAK, 0, nullptr, 0);
        }

        // Make speed be towards the fly away point
        TVec3f forward = s_fly_away_point - bowser->mTranslation;
        forward.y      = 0.0f;
        PSVECNormalize(forward, forward);
        forward.scale(90.0f);
        bowser->mSpeed   = forward;
        bowser->mSpeed.y = 30.0f;

        TVec3f bomb_pos = bowser->mTranslation;
        bomb_pos.y += 300.0f;

        forward.scale(0.1f);
        bomb_pos -= forward;

        if (THitActor *explosion =
                (THitActor *)gpConductor->makeOneEnemyAppear(bomb_pos, s_explosion_name, 1)) {
            explosion->mScale.scale(2.0f);
        }

        /*if (JPABaseEmitter *emitter =
                gpMarioParticleManager->emit(0xa1, &bowser->mTranslation, 0, bowser)) {
            emitter->mSize1 = {3.0f, 3.0f, 3.0f};
        }

        if (JPABaseEmitter *emitter =
                gpMarioParticleManager->emit(0xa2, &bowser->mTranslation, 0, bowser)) {
            emitter->mSize1 = {3.0f, 3.0f, 3.0f};
        }*/
    }

    return false;
}

bool TNerveBowserTalk::execute(TSpineBase<TLiveActor> *spine) const {
    TBossBowserCar *bowser = static_cast<TBossBowserCar *>(spine->mTarget);

    bowser->mGravity = 0.0f;
    bowser->mRotation.y =
        bowser->calcNextAngle(bowser->m_target->mTranslation, BOWSER_TURN_POWER, 0.0f, true);

    if (spine->mNerveTimer == 0) {
        bowser->m_nerve_status = 0;
        bowser->playAnimation("bowser_idle", 1.0f, 0.5f);
    }

    if (bowser->m_nerve_status == 0) {
        if (gpMarDirector->mCurState == TMarDirector::STATE_NORMAL &&
            gpMarDirector->mNextStateA == 2) {
            bowser->m_nerve_status = 1;
            bowser->playAnimation("bowser_taunt", 0.9f, 0.5f);
            bowser->mActorData->getFrameCtrl(MActor::BCK)->mAnimState = J3DFrameCtrl::LOOP;
        }
    } else if (bowser->m_nerve_status == 1) {
        if (gpMarDirector->mCurState == TMarDirector::STATE_NORMAL &&
            gpMarDirector->mNextStateA != 2) {
            spine->pushNerve(&s_bowser_nerve_idle);
            return true;
        }
    }

    return false;
}
