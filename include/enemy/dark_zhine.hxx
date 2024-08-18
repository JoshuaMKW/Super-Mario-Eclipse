#pragma once

#include <Dolphin/types.h>

#include <JSystem/JGeometry/JGMVec.hxx>
#include <SMS/Enemy/BossGesso.hxx>
#include <SMS/Manager/EnemyManager.hxx>

#include <BetterSMS/module.hxx>
#include <Manager/PollutionManager.hxx>

#define ZHINE_POL_DROP_COUNT 4

class TNerveZhineSleep : public TNerveBase<TLiveActor> {
    friend class TDarkZhine;

public:
    TNerveZhineSleep(){};
    virtual ~TNerveZhineSleep() = default;
    virtual bool execute(TSpineBase<TLiveActor> *) const;
};

class TNerveZhineWake : public TNerveBase<TLiveActor> {
    friend class TDarkZhine;

public:
    TNerveZhineWake(){};
    virtual ~TNerveZhineWake() = default;
    virtual bool execute(TSpineBase<TLiveActor> *) const;
};

class TNerveZhineFly : public TNerveBase<TLiveActor> {
    friend class TDarkZhine;

public:
    TNerveZhineFly(){};
    virtual ~TNerveZhineFly() = default;
    virtual bool execute(TSpineBase<TLiveActor> *) const;
};

class TNerveZhineTeleport : public TNerveBase<TLiveActor> {
    friend class TDarkZhine;

public:
    TNerveZhineTeleport(){};
    virtual ~TNerveZhineTeleport() = default;
    virtual bool execute(TSpineBase<TLiveActor> *) const;
};

class TNerveZhinePound : public TNerveBase<TLiveActor> {
    friend class TDarkZhine;

public:
    TNerveZhinePound(){};
    virtual ~TNerveZhinePound() = default;
    virtual bool execute(TSpineBase<TLiveActor> *) const;
};

class TNerveZhineIdle : public TNerveBase<TLiveActor> {
    friend class TDarkZhine;

public:
    TNerveZhineIdle(){};
    virtual ~TNerveZhineIdle() = default;
    virtual bool execute(TSpineBase<TLiveActor> *) const;
};

class TNerveZhineKnockBack : public TNerveBase<TLiveActor> {
    friend class TDarkZhine;

public:
    TNerveZhineKnockBack(){};
    virtual ~TNerveZhineKnockBack() = default;
    virtual bool execute(TSpineBase<TLiveActor> *) const;
};

class TNerveZhineCrash : public TNerveBase<TLiveActor> {
    friend class TDarkZhine;

public:
    TNerveZhineCrash(){};
    virtual ~TNerveZhineCrash() = default;
    virtual bool execute(TSpineBase<TLiveActor> *) const;
};

class TNerveZhineRecover : public TNerveBase<TLiveActor> {
    friend class TDarkZhine;

public:
    TNerveZhineRecover(){};
    virtual ~TNerveZhineRecover() = default;
    virtual bool execute(TSpineBase<TLiveActor> *) const;
};

class TNerveZhineStun : public TNerveBase<TLiveActor> {
    friend class TDarkZhine;

public:
    TNerveZhineStun(){};
    virtual ~TNerveZhineStun() = default;
    virtual bool execute(TSpineBase<TLiveActor> *) const;
};

class TNerveZhineRoll : public TNerveBase<TLiveActor> {
    friend class TDarkZhine;

public:
    TNerveZhineRoll(){};
    virtual ~TNerveZhineRoll() = default;
    virtual bool execute(TSpineBase<TLiveActor> *) const;
};

class TNerveZhineRise : public TNerveBase<TLiveActor> {
    friend class TDarkZhine;

public:
    TNerveZhineRise(){};
    virtual ~TNerveZhineRise() = default;
    virtual bool execute(TSpineBase<TLiveActor> *) const;
};

class TNerveZhinePreKill : public TNerveBase<TLiveActor> {
    friend class TDarkZhine;

public:
    TNerveZhinePreKill(){};
    virtual ~TNerveZhinePreKill() = default;
    virtual bool execute(TSpineBase<TLiveActor> *) const;
};

class TNerveZhineKill : public TNerveBase<TLiveActor> {
    friend class TDarkZhine;

public:
    TNerveZhineKill(){};
    virtual ~TNerveZhineKill() = default;
    virtual bool execute(TSpineBase<TLiveActor> *) const;
};

class TNerveZhineAngry : public TNerveBase<TLiveActor> {
    friend class TDarkZhine;

public:
    TNerveZhineAngry(){};
    virtual ~TNerveZhineAngry() = default;
    virtual bool execute(TSpineBase<TLiveActor> *) const;
};

class TDarkZhine : public TSpineEnemy {
    enum PoundingState { INACTIVE, DROPPING, SHOCKING, GROUNDROLL, RISING };

public:
    BETTER_SMS_FOR_CALLBACK static JDrama::TNameRef *instantiate() {
        return new TDarkZhine("TDarkZhine");
    }

    TDarkZhine(const char *);
    virtual ~TDarkZhine() = default;

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

    bool isDefeated() const { return m_health_points == 0; }

    bool isHardMode() const { return m_is_hard_mode; }
    bool isTargetInRangeToHome(f32 range) const;

    void setStunned(bool stunned) { m_stunned = stunned; }
    bool isStunned() const { return m_stunned; }

    void setInvincible(bool invincible) { m_invincible = invincible; }
    bool isInvincible() const { return m_invincible; }

    void setHostile(bool hostile) { m_hostile = hostile; }
    bool isHostile() const { return m_hostile; }

    f32 getDistanceToTargetXZ() const;
    f32 getAngleToTarget() const;

    f32 getForwardSpeed() const { return m_forward_speed; }
    void setForwardSpeed(f32 speed) { m_forward_speed = speed; }

    u8 getPollutionAlpha() const { return m_pol_alpha; }
    void setPollutionAlpha(u8 alpha) { m_pol_alpha = alpha; }

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

    void calcReflectingVelocity(TBGCheckData *data, f32 speed, TVec3f &out);
    void calcReflectingAngle(TBGCheckData *data, f32 *angle);
    void moveToTargetXZ();
    void checkForActorContact();
    bool isPlayerAttacking(TMario *);
    bool isPlayerPounding(TMario *);

    bool doGroundPound(bool attack, bool *illegal_out);
    void doWorldShake(f32 strength);
    void launchPolDrop();

    f32 calcNextAngle(f32 turn_power, f32 offset, bool indiscriminate = false);

    void spawnShine();

private:
    bool m_is_hard_mode;
    u8 m_pol_alpha;
    u8 m_pol_collect_timer;

    bool m_stunned;
    bool m_invincible;
    bool m_hostile;

    TVec3f m_ground_point;

    TBGPolDrop *m_pol_drops[ZHINE_POL_DROP_COUNT];
    u8 m_pol_drop_index;

    int m_anm_index;

    TVec3f m_bounding_point;
    PoundingState m_pounding_status;
    JDrama::TActor *m_target;

    u8 m_health_points;
    u16 m_damage_timer;

    f32 m_forward_speed;
    f32 m_model_ofs_y;
};

class TDarkZhineManager : public TEnemyManager {
public:
    BETTER_SMS_FOR_CALLBACK static JDrama::TNameRef *instantiate() {
        return new TDarkZhineManager("DarkZhineManager");
    }

    TDarkZhineManager(const char *);
    ~TDarkZhineManager() override = default;

    void load(JSUMemoryInputStream &) override;
};