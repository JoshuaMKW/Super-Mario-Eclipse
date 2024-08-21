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
#include <SMS/System/TargetArrow.hxx>
#include <SMS/rand.h>

#include <BetterSMS/libs/constmath.hxx>
#include <BetterSMS/music.hxx>
#include <BetterSMS/player.hxx>

#include "enemy/bowser_car.hxx"
#include "message.hxx"

extern TVec3f *gpMarioPos;

static TNerveBowserIdle s_bowser_nerve_idle;
static TNerveBowserTaunt s_bowser_nerve_taunt;
static TNerveBowserThrow s_bowser_nerve_throw;
static TNerveBowserFire s_bowser_nerve_fire;
static TNerveBowserPreShoot s_bowser_nerve_preshoot;
static TNerveBowserShoot s_bowser_nerve_shoot;
static TNerveBowserWet s_bowser_nerve_wet;
static TNerveBowserStun s_bowser_nerve_stun;
static TNerveBowserShot s_bowser_nerve_shot;
static TNerveBowserRecover s_bowser_nerve_recover;
static TNerveBowserRise s_bowser_nerve_rise;
static TNerveBowserAngry s_bowser_nerve_angry;
static TNerveBowserPreKill s_bowser_nerve_prekill;
static TNerveBowserKill s_bowser_nerve_kill;

static bool s_has_bgm_started = false;
static bool s_first_pound     = true;

#define BOWSER_ANM_IDX_FLY   0
#define BOWSER_ANM_IDX_RISE  1
#define BOWSER_ANM_IDX_ROLL  2
#define BOWSER_ANM_IDX_SLEEP 3
#define BOWSER_ANM_IDX_STUN  4
#define BOWSER_ANM_IDX_WAKE  5

#define BOWSER_FLY_HEIGHT         750.0f
#define BOWSER_FLY_WAVE_AMPLITUDE 80.0f
#define BOWSER_FLY_WAVE_FREQUENCY (1.0f / 30.0f);
#define BOWSER_GRAVITY            0.98f
#define BOWSER_BRAKE_FACTOR       0.97f
#define BOWSER_SPEED_FACTOR       0.003f
#define BOWSER_TURN_POWER         0.03f
#define BOWSER_MAX_FRAMERATE      5.0f
#define BOWSER_WATCH_MARIO_RADIUS 3000.0f

#define BOWSER_MODEL_TRANS_Y 50.0f

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

static TBossBowserCar *gpBowserCar = nullptr;

static int propellorCallback(J3DNode *node, int joint_index) {
    Mtx tmtx;
    PSMTXCopy(J3DSys::mCurrentMtx, tmtx);

    f32 x = tmtx[0][3];
    f32 y = tmtx[1][3];
    f32 z = tmtx[2][3];

    tmtx[0][3] = 0.0f;
    tmtx[1][3] = 0.0f;
    tmtx[2][3] = 0.0f;

    Mtx rotmtx;
    PSMTXRotRad(rotmtx, 'y',
                gpBowserCar->m_wave_point * 5.0f + angleToRadians(gpBowserCar->mRotation.y));
    PSMTXConcat(tmtx, rotmtx, tmtx);
    tmtx[0][3] = x;
    tmtx[1][3] = y;
    tmtx[2][3] = z;

    PSMTXCopy(tmtx, J3DSys::mCurrentMtx);
    return 1;
}

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

static AtomBalloonMessageViewer s_msg_start(137, 120);
static AtomBalloonMessageViewer s_msg_inspect(138, 120);
static AtomBalloonMessageViewer s_msg_health[] = {
    {143, 130},
    {142, 130},
    {141, 130},
    {140, 130},
    {139, 130}
};
static AtomBalloonMessageViewer s_msg_finish(144, 160);

TBossBowserCar::TBossBowserCar(const char *name)
    : TSpineEnemy(name), m_ground_point(), m_stunned(false), m_invincible(false), m_hostile(true),
      m_anm_index(0), m_bounding_point(), m_target(nullptr), m_health_points(3), m_damage_timer(0),
      m_forward_speed(0.0f), m_model_ofs_y(0.0f) {
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

    initHitActor(0x8000011, 10, 0x81000000, 350.0f, 750.0f, 350.0f, 750.0f);
    initAnmSound();

    mShadowRadius = 500.0f;
    mStateFlags.asU32 &= ~0x100;
    mStateFlags.asU32 |= 0x8;

    mSpineBase->setNerve(&s_bowser_nerve_idle);
    m_health_points = 5;
    m_model_ofs_y   = 50.0f;

    m_propeller_index = ((JUTNameTab *)mActorData->mModel->mModelData->_B0)->getIndex("prop") - 1;
    if (m_propeller_index != -1) {
        mActorData->setJointCallback(m_propeller_index, propellorCallback);
    }

    m_head_index = ((JUTNameTab *)mActorData->mModel->mModelData->_B0)->getIndex("neck") - 1;
    if (m_head_index != -1) {
        mActorData->setJointCallback(m_head_index, neckCallback);
    }

    m_hand_index = ((JUTNameTab *)mActorData->mModel->mModelData->_B0)->getIndex("Lyubi1_jnt1");
    m_fire_index = ((JUTNameTab *)mActorData->mModel->mModelData->_B0)->getIndex("sebone_eff");

    OSReport("Hand index: %d\n", m_hand_index);

    s_has_bgm_started = false;
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

    if ((flags & 2)) {
        if (isStunned()) {
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
    if (sender->mObjectID == 0x1000002B || (sender->mObjectID == 0x1000001E && message == 10)) {
        m_health_points -= 1;
        m_damage_timer = 120;
        mSpineBase->setNerve(&s_bowser_nerve_shot);

        return true;
    }

    if (message == MESSAGE_TOUCH_WATER) {
        /*TVec3f *water_speed = (TVec3f *)TMapObjBase::getWaterSpeed(sender);
        f32 water_magnitude = water_speed->magnitude();
        m_pol_alpha -= 1 + (u8)(water_magnitude / 40.0f);
        if (gpMSound->gateCheck(MSD_SE_BS_GESO_WATER_HIT)) {
            MSoundSE::startSoundActor(MSD_SE_BS_GESO_WATER_HIT, mTranslation, 0, nullptr, 0, 4);
        }*/

        J3DModel *model = getModel();
        TVec3f head_pos = {model->mJointArray[m_head_index][0][3],
                           model->mJointArray[m_head_index][1][3],
                           model->mJointArray[m_head_index][2][3]};

        if (PSVECDistance(head_pos, sender->mTranslation) < 400.0f) {
            m_wet_count = Min(m_wet_count + 4, 255);
        }

        if (!isInvincible() && m_wet_count >= 28) {
            if (mSpineBase->getLatestNerve() != &s_bowser_nerve_wet) {
                m_nerve_status = 0;
                mSpineBase->setNerve(&s_bowser_nerve_wet);
            }
        }

        return true;
    }

    TSpineEnemy::receiveMessage(sender, message);
    return false;
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

        if (railNode->mValues[1] != 0xFFFF) {
            mTravelSpeed = static_cast<f32>(railNode->mValues[1]) / 100.0f;
        } else {
            mTravelSpeed = 10.0f;
        }
    }

    m_propeller_index = 0;

    s_msg_start.reset();
    for (AtomBalloonMessageViewer &msg : s_msg_health) {
        msg.reset();
    }
    s_msg_finish.reset();

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
}

void TBossBowserCar::bind() { TSpineEnemy::bind(); }

void TBossBowserCar::moveObject() {
    control();

    if (mSpineBase->getLatestNerve() == &s_bowser_nerve_idle ||
        mSpineBase->getLatestNerve() == &s_bowser_nerve_fire) {
        if (moveToNextNode(mTravelSpeed)) {
            if ((rand() % 100) < 70 || m_last_taunt) {
                mSpineBase->setNerve(&s_bowser_nerve_throw);
                m_last_taunt = false;
            } else {
                mSpineBase->setNerve(&s_bowser_nerve_taunt);
                m_last_taunt = true;
            }

            int nextIndex = mGraphTracer->mGraph->getRandomNextIndex(
                mGraphTracer->mCurrentNode, mGraphTracer->mPreviousNode, -1);
            mGraphTracer->moveTo(nextIndex);
        }
    }

    mSpeed.scale(BOWSER_BRAKE_FACTOR);
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

        PSMTXRotRad(rxmtx, 'x', sinf(m_wave_point * 1.0f) * ((f32)m_wet_count / 255.0f) * 0.4f);
        PSMTXRotRad(rymtx, 'y', sinf(m_wave_point * 2.0f) * ((f32)m_wet_count / 255.0f) * 0.7f);
        PSMTXRotRad(rzmtx, 'z', cosf(m_wave_point * 1.0f) * ((f32)m_wet_count / 255.0f) * 0.4f);

        PSMTXConcat(rxmtx, rymtx, rxmtx);
        PSMTXConcat(rxmtx, rzmtx, rxmtx);
        PSMTXConcat(tmtx, rxmtx, tmtx);
    }

    J3DModel *model = getModel();
    PSMTXConcat(model->mBaseMtx, tmtx, model->mBaseMtx);
}

void TBossBowserCar::reset() {
    setStunned(false);
    setInvincible(true);
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
            mGraphTracer->mGraph->mNodes[mGraphTracer->mCurrentNode].getPoint(m_base_translation);
            return true;
        } else {
        }

        TVec3f direction;
        PSVECNormalize(diff, direction);
        direction.scale(speed);

        f32 dist_full = PSVECDistance(last_point, point);
        f32 dist_cur  = PSVECDistance(point, m_base_translation);

        m_base_translation.y =
            Max(lerp<f32>(last_point.y, point.y, 1.0f - (dist_cur / dist_full)), mGroundY + 400.0f);

        mSpeed.x = (mSpeed.x + direction.x);
        mSpeed.y = 0.0f;
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
            mTravelSpeed = static_cast<f32>(railNextNode->mValues[1]) * BOWSER_SPEED_FACTOR;
        } else {
            mTravelSpeed = 100.0f * BOWSER_SPEED_FACTOR;
        }
    }
}

void TBossBowserCar::processIdle() {}

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
                    // mSpineBase->setNerve(&s_bowser_knockback);
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
                if (player->mTranslation.y - mTranslation.y > 50.0f) {
                    gpMarioParticleManager->emitAndBindToPosPtr(11, &mTranslation, 0, nullptr);

                    m_damage_timer = 120;
                    m_health_points -= 1;
                    s_msg_health[m_health_points].show();

                    // mSpineBase->setNerve(&s_zhine_nerve_crash);

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

bool TBossBowserCar::isPlayerAttacking(TMario *player) {
    if (player->mState != TMario::STATE_DIVE && player->mState != TMario::STATE_DIVESLIDE) {
        return false;
    }

    if (player->mForwardSpeed < 30.0f) {
        return false;
    }

    return true;
}

bool TBossBowserCar::isPlayerPounding(TMario *player) {
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

void TBossBowserCarManager::load(JSUMemoryInputStream &in) { TEnemyManager::load(in); }

bool TNerveBowserIdle::execute(TSpineBase<TLiveActor> *spine) const {
    TBossBowserCar *bowser = static_cast<TBossBowserCar *>(spine->mTarget);

    bowser->mGravity = 0.0f;

    if (spine->mNerveTimer == 0) {
        bowser->mActorData->setBck("bowser_idle");
        bowser->mActorData->setFrameRate(0.8f, MActor::BCK);
    }

    if (bowser->getDistanceToTargetXZ(bowser->m_target->mTranslation) < BOWSER_WATCH_MARIO_RADIUS) {
        // Face mario
        bowser->mRotation.y =
            bowser->calcNextAngle(bowser->m_target->mTranslation, BOWSER_TURN_POWER, 0.0f);
    } else {
        TGraphWeb *web = bowser->mGraphTracer->mGraph;

        if (web && !web->isDummy()) {
            TVec3f target       = web->indexToPoint(bowser->mGraphTracer->getCurGraphIndex());
            bowser->mRotation.y = bowser->calcNextAngle(target, BOWSER_TURN_POWER, 0.0f);
        }
    }

    /*if (spine->mNerveTimer > 300) {
        spine->pushNerve(&s_bowser_nerve_taunt);
        spine->pushNerve(&s_bowser_nerve_prethrow);
        return true;
    }*/

    bowser->processIdle();
    if (bowser->getDistanceToTargetXZ() < 1300.0f && spine->mNerveTimer > 240) {
        spine->pushNerve(&s_bowser_nerve_fire);
        return true;
    }

    return false;
}

bool TNerveBowserTaunt::execute(TSpineBase<TLiveActor> *spine) const {
    TBossBowserCar *bowser = static_cast<TBossBowserCar *>(spine->mTarget);

    bowser->mGravity = 0.0f;

    if (spine->mNerveTimer == 0) {
        bowser->mActorData->setBck("bowser_taunt");
        bowser->mActorData->setFrameRate(0.9f, MActor::BCK);
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
        bowser->mActorData->setBck("bowser_throw");
        bowser->mActorData->setFrameRate(0.5f, MActor::BCK);
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
            TVec3f throw_speed = {9.0f, 15.0f, 9.0f};

            TVec3f direction = attack_pos - bowser->mTranslation;
            direction.y      = 0.0f;

            PSVECNormalize(direction, direction);
            direction *= throw_speed;
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

    OSReport("Fire\n");

    if (spine->mNerveTimer == 0) {
        bowser->mActorData->setBck("bowser_fire");
        bowser->mActorData->setFrameRate(0.3f, MActor::BCK);
    }

    if (bowser->mActorData->isCurAnmAlreadyEnd(MActor::BCK)) {
        spine->pushNerve(&s_bowser_nerve_idle);
        return true;
    }

    spine->pushNerve(&s_bowser_nerve_idle);
    return true;
}

bool TNerveBowserPreShoot::execute(TSpineBase<TLiveActor> *spine) const { return false; }

bool TNerveBowserShoot::execute(TSpineBase<TLiveActor> *spine) const { return false; }

bool TNerveBowserWet::execute(TSpineBase<TLiveActor> *spine) const {
    TBossBowserCar *bowser = static_cast<TBossBowserCar *>(spine->mTarget);

    bowser->mGravity = 0.0f;

    if (spine->mNerveTimer == 0) {
        bowser->mActorData->setBck("bowser_water_begin");
        bowser->mActorData->setFrameRate(1.0f, MActor::BCK);

        TSpineEnemy *bombhei = static_cast<TSpineEnemy *>(bowser->mHeldObject);
        if (bombhei) {
            TVec3f throw_speed = {sinf(rand() % 10), 0.0f, sinf(rand() % 10)};
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
            bowser->mActorData->setBck("bowser_water_loop");
            bowser->mActorData->setFrameRate(0.7f, MActor::BCK);
        } else if (bowser->m_nerve_status == 3) {
            bowser->m_nerve_status = 0;
            spine->pushNerve(&s_bowser_nerve_idle);
            return true;
        }
    }

    if (bowser->m_nerve_status == 2) {
        bowser->m_nerve_status = 3;
        bowser->mActorData->setBck("bowser_water_end");
        bowser->mActorData->setFrameRate(0.7f, MActor::BCK);
    }

    return false;
}

bool TNerveBowserStun::execute(TSpineBase<TLiveActor> *spine) const { return false; }

bool TNerveBowserShot::execute(TSpineBase<TLiveActor> *spine) const {
    TBossBowserCar *bowser = static_cast<TBossBowserCar *>(spine->mTarget);

    bowser->mGravity = 0.0f;

    // Face mario
    bowser->mRotation.y =
        bowser->calcNextAngle(bowser->m_target->mTranslation, BOWSER_TURN_POWER, 0.0f);

    if (spine->mNerveTimer == 0) {
        bowser->mActorData->setBck("bowser_hurt");
        bowser->mActorData->setFrameRate(0.3f, MActor::BCK);
        if (gpMSound->gateCheck(MSD_SE_BS_KOOPA_VO_DAMAGE_0)) {
            MSoundSE::startSoundActor(MSD_SE_BS_KOOPA_VO_DAMAGE_0, bowser->mTranslation, 0, nullptr,
                                      0, 4);
        }

        TSpineEnemy *bombhei = static_cast<TSpineEnemy *>(bowser->mHeldObject);
        if (bombhei) {
            TVec3f throw_speed = {sinf(rand() % 10), 0.0f, sinf(rand() % 10)};
            PSVECNormalize(throw_speed, throw_speed);
            throw_speed.scale(10.0f);
            throw_speed.y = 20.0f;

            bombhei->mSpeed  = throw_speed;
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

bool TNerveBowserRecover::execute(TSpineBase<TLiveActor> *spine) const { return false; }

bool TNerveBowserRise::execute(TSpineBase<TLiveActor> *spine) const { return false; }

bool TNerveBowserAngry::execute(TSpineBase<TLiveActor> *spine) const { return false; }

bool TNerveBowserPreKill::execute(TSpineBase<TLiveActor> *spine) const { return false; }

bool TNerveBowserKill::execute(TSpineBase<TLiveActor> *spine) const { return false; }
