#pragma once

#include <Dolphin/types.h>

#include <JSystem/JGeometry/JGMVec.hxx>
#include <SMS/Enemy/BossGesso.hxx>
#include <SMS/Manager/EnemyManager.hxx>

#include <BetterSMS/module.hxx>
#include <Manager/PollutionManager.hxx>

class TNerveBowserTalk : public TNerveBase<TLiveActor> {
public:
    TNerveBowserTalk() {}
    virtual ~TNerveBowserTalk() = default;
    virtual bool execute(TSpineBase<TLiveActor> *) const;
};

class TNerveBowserIdle : public TNerveBase<TLiveActor> {
    friend class TDarkZhine;

public:
    TNerveBowserIdle(){};
    virtual ~TNerveBowserIdle() = default;
    virtual bool execute(TSpineBase<TLiveActor> *) const;
};

class TNerveBowserTaunt : public TNerveBase<TLiveActor> {
    friend class TDarkZhine;

public:
    TNerveBowserTaunt(){};
    virtual ~TNerveBowserTaunt() = default;
    virtual bool execute(TSpineBase<TLiveActor> *) const;
};

class TNerveBowserThrow : public TNerveBase<TLiveActor> {
    friend class TDarkZhine;

public:
    TNerveBowserThrow(){};
    virtual ~TNerveBowserThrow() = default;
    virtual bool execute(TSpineBase<TLiveActor> *) const;
};

class TNerveBowserFire : public TNerveBase<TLiveActor> {
    friend class TDarkZhine;

public:
    TNerveBowserFire(){};
    virtual ~TNerveBowserFire() = default;
    virtual bool execute(TSpineBase<TLiveActor> *) const;
};

class TNerveBowserShoot : public TNerveBase<TLiveActor> {
    friend class TDarkZhine;

public:
    TNerveBowserShoot(){};
    virtual ~TNerveBowserShoot() = default;
    virtual bool execute(TSpineBase<TLiveActor> *) const;
};

class TNerveBowserWet : public TNerveBase<TLiveActor> {
    friend class TDarkZhine;

public:
    TNerveBowserWet(){};
    virtual ~TNerveBowserWet() = default;
    virtual bool execute(TSpineBase<TLiveActor> *) const;
};

class TNerveBowserStun : public TNerveBase<TLiveActor> {
    friend class TDarkZhine;

public:
    TNerveBowserStun(){};
    virtual ~TNerveBowserStun() = default;
    virtual bool execute(TSpineBase<TLiveActor> *) const;
};

class TNerveBowserShot : public TNerveBase<TLiveActor> {
    friend class TDarkZhine;

public:
    TNerveBowserShot(){};
    virtual ~TNerveBowserShot() = default;
    virtual bool execute(TSpineBase<TLiveActor> *) const;
};

class TNerveBowserRecover : public TNerveBase<TLiveActor> {
    friend class TDarkZhine;

public:
    TNerveBowserRecover(){};
    virtual ~TNerveBowserRecover() = default;
    virtual bool execute(TSpineBase<TLiveActor> *) const;
};

class TNerveBowserRise : public TNerveBase<TLiveActor> {
    friend class TDarkZhine;

public:
    TNerveBowserRise(){};
    virtual ~TNerveBowserRise() = default;
    virtual bool execute(TSpineBase<TLiveActor> *) const;
};

class TNerveBowserShock : public TNerveBase<TLiveActor> {
    friend class TDarkZhine;

public:
    TNerveBowserShock(){};
    virtual ~TNerveBowserShock() = default;
    virtual bool execute(TSpineBase<TLiveActor> *) const;
};

class TNerveBowserPreKill : public TNerveBase<TLiveActor> {
    friend class TDarkZhine;

public:
    TNerveBowserPreKill(){};
    virtual ~TNerveBowserPreKill() = default;
    virtual bool execute(TSpineBase<TLiveActor> *) const;
};

class TNerveBowserKill : public TNerveBase<TLiveActor> {
    friend class TDarkZhine;

public:
    TNerveBowserKill(){};
    virtual ~TNerveBowserKill() = default;
    virtual bool execute(TSpineBase<TLiveActor> *) const;
};

class TBossBowserCar : public TSpineEnemy {
public:
    BETTER_SMS_FOR_CALLBACK static JDrama::TNameRef *instantiate() {
        return new TBossBowserCar("TBossBowserCar");
    }

    TBossBowserCar(const char *);
    virtual ~TBossBowserCar() = default;

    void load(JSUMemoryInputStream &) override;
    void perform(u32, JDrama::TGraphics *) override;
    bool receiveMessage(THitActor *, u32) override;
    void loadAfter() override;
    void init(TLiveManager *) override;
    void control() override;
    void bind() override;
    void moveObject() override;
    void calcRootMatrix() override;
    void reset() override;
    void kill() override;
    void requestShadow() override;
    Mtx44 *getTakingMtx() override;
    J3DFrameCtrl *playAnimation(const char *anm_name, f32 blend_ratio);

    bool isDefeated() const { return m_health_points == 0; }
    bool isHardMode() const { return m_health_points < 3; }

    void setStunned(bool stunned) { m_stunned = stunned; }
    bool isStunned() const { return m_stunned; }

    void setInvincible(bool invincible) { m_invincible = invincible; }
    bool isInvincible() const { return m_invincible; }

    void setHostile(bool hostile) { m_hostile = hostile; }
    bool isHostile() const { return m_hostile; }

    f32 getDistanceToTargetXZ() const;
    f32 getDistanceToTargetXZ(const TVec3f &target) const;
    f32 getAngleToTarget() const;
    f32 getAngleToTarget(const TVec3f &target) const;

    f32 getForwardSpeed() const { return m_forward_speed; }
    void setForwardSpeed(f32 speed) { m_forward_speed = speed; }

    f32 getModelOffsetY() const { return m_model_ofs_y; }
    void setModelOffsetY(f32 offset) { m_model_ofs_y = offset; }

    void setBckAnm(int index) {
        m_anm_index = index;
        mActorData->setBckFromIndex(index);
        /*
        const char **bas_table = getBasNameTable();
        if (bas_table) {
            setAnmSound(bas_table[index]);
        }
        */
    }

    bool moveToNextNode(f32 speed);
    void readRailFlag();

    bool processIdle();

    void calcReflectingVelocity(TBGCheckData *data, f32 speed, TVec3f &out);
    void calcReflectingAngle(TBGCheckData *data, f32 *angle);
    void moveToTargetXZ();
    void checkForActorContact();
    bool isPlayerAttacking(TMario *);
    bool isPlayerPounding(TMario *);
    TSpineEnemy *isBombWithinAndAttack(f32 radius, f32 height);
    f32 getHighestGround(const TVec3f &point, f32 radius, const TBGCheckData **out);

    void doWorldShake(f32 strength);

    f32 calcNextAngle(const TVec3f &target, f32 turn_power, f32 offset,
                      bool indiscriminate = false);

    void spawnShine();

    JDrama::TActor *m_target;
    const TBGCheckData *m_damage_ground;
    TVec3f m_damage_pos;

    f32 m_highest_ground_y;
    const TBGCheckData *m_highest_ground;

    f32 m_wave_point;
    int m_nerve_status;
    int m_wet_count;
    bool m_last_fire;
    bool m_last_taunt;
    bool m_intro_ended;
    f32 m_rocket_angle;
    TVec3f m_rocket_hit_pos;
    s32 m_shock_wait_time;

    int m_propeller_index;
    int m_gun_l_index;
    int m_gun_r_index;
    int m_head_index;
    int m_hand_index;
    int m_fire_index;

    f32 m_target_gun_l_angle;
    f32 m_target_gun_r_angle;

    f32 m_current_gun_l_angle;
    f32 m_current_gun_r_angle;

    // Graph Tracer
    f32 m_travel_speed;
    f32 m_target_tilt;
    f32 m_target_tilt_speed;

    TVec3f m_base_translation;

    TVec3f m_fire_joint_pos;

    size_t m_bullets_shot;

private:
    bool m_stunned;
    bool m_invincible;
    bool m_hostile;

    TVec3f m_ground_point;

    int m_anm_index;

    TVec3f m_bounding_point;

    u8 m_health_points;
    u16 m_damage_timer;

    f32 m_forward_speed;
    f32 m_model_ofs_y;

    TVec3f m_target_point;

    Mtx m_taking_mtx;

    f32 m_current_tilt;
    f32 m_current_tilt_speed;
};

class TBossBowserCarManager : public TEnemyManager {
public:
    BETTER_SMS_FOR_CALLBACK static JDrama::TNameRef *instantiate() {
        return new TBossBowserCarManager("BossBowserCarManager");
    }

    TBossBowserCarManager(const char *);
    ~TBossBowserCarManager() override = default;

    void load(JSUMemoryInputStream &) override;
};