#pragma once

#include <Dolphin/types.h>
#include <SMS/Camera/PolarSubCamera.hxx>
#include <SMS/MapObj/MapObjBall.hxx>
#include <SMS/Manager/MarioParticleManager.hxx>
#include <SMS/MSound/MSoundSESystem.hxx>
#include <SMS/System/TargetArrow.hxx>

#include <BetterSMS/module.hxx>

class TCannonBall : public TMapObjBall {
public:
    BETTER_SMS_FOR_CALLBACK static JDrama::TNameRef *instantiate() {
        return new TCannonBall("TCannonBall");
    }

    TCannonBall(const char *name)
        : TMapObjBall(name), m_water_surface(nullptr), m_water_surface_y(0.0f),
          m_was_in_air(false) {}
    ~TCannonBall() override = default;

    void load(JSUMemoryInputStream &in) override {
        JDrama::TActor::load(in);

        mRegisterName = "cannonball";
        mModelName    = in.readString();

        initMapObj();
        makeObjAppeared();
    }

    void loadAfter() override {
        TMapObjBall::loadAfter();
        mStateFlags.asU32 |= 0x100000;  // Enable grabbing

        m_target_arrow = new TTargetArrow("cannonball_target_arrow");
        m_target_arrow->loadAfter();
    }

    void perform(u32 flags, JDrama::TGraphics *graphics) override {
        TMapObjBall::perform(flags, graphics);

        if ((flags & PERFORM_ON_MOVEMENT)) {
            if (m_respawn_timer > 0) {
                if (--m_respawn_timer == 0) {
                    makeObjRecovered();
                    if (gpMSound->gateCheck(MSD_SE_IT_COMMON_APPEAR)) {
                        MSoundSE::startSoundActor(MSD_SE_IT_COMMON_APPEAR, mTranslation, 0, nullptr,
                                                  0, 4);
                    }
                }
            }
        }

        if ((flags & 2)) {
            if (m_was_dead && (mObjectType & 1) == 0) {
                if (JPABaseEmitter *emitter =
                        gpMarioParticleManager->emit(0xC, &mTranslation, 0, nullptr)) {
                    /*emitter->mSize1 = {3.0f, 3.0f, 3.0f};
                    emitter->mSize3 = {3.0f, 3.0f, 3.0f};*/
                }
            }

            m_was_dead = (mObjectType & 1);
        }

        if (!mStateFlags.asFlags.mIsObjDead && !mHolder && !m_was_in_air) {
            TVec3f target_arrow_pos = mTranslation;
            target_arrow_pos.y += 200.0f;

            f32 camera_distance       = PSVECDistance(gpCamera->mTranslation, target_arrow_pos);
            TVec3f target_arrow_scale = {0.4f, 0.4f, 0.4f};
            TVec3f additive_scale     = {0.000125f * camera_distance, 0.000125f * camera_distance,
                                         0.0000125f * camera_distance};

            target_arrow_pos.y += additive_scale.y * 220.0f;
            m_target_arrow->setPos(target_arrow_pos);
            m_target_arrow->mActor->mModel->mBaseScale = target_arrow_scale + additive_scale;
            m_target_arrow->mIsVisible                 = true;
        } else {
            m_target_arrow->mIsVisible = false;
        }

        m_target_arrow->perform(flags, graphics);
    }

    Mtx44 *getTakingMtx() override;
    void makeMActors() override;

    void checkGroundCollision(TVec3f *out) override;

    void touchGround(TVec3f *out) override;
    void touchWall(TVec3f *out, TBGWallCheckRecord *record) override;
    void touchRoof(TVec3f *out) override;
    void touchWaterSurface() override;

    s32 getLivingTime() const override { return 0x7FFFFFFF; }
    s32 getFlushTime() const override { return 0; }
    void kicked() override {}

private:
    const char *mModelName;
    const TBGCheckData *m_water_surface;
    f32 m_water_surface_y;
    bool m_was_in_air;
    bool m_was_dead;
    s16 m_respawn_timer;

    TTargetArrow *m_target_arrow;
};

extern ObjData cannonBallData;