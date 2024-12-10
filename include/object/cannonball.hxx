#pragma once

#include <Dolphin/types.h>
#include <SMS/MapObj/MapObjBall.hxx>
#include <SMS/Manager/MarioParticleManager.hxx>

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
    }

    void perform(u32 flags, JDrama::TGraphics *graphics) override {
        TMapObjBall::perform(flags, graphics);

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
};

extern ObjData cannonBallData;