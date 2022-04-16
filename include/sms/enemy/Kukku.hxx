#include "sms/actor/SpineBase.hxx"

class TKukkuBall : public THitActor
{
public:
    TKukkuBall(const char *name) : THitActor(name) { unk1 = 1, unk2 = 0; };
    virtual ~TKukkuBall();
    virtual void init();
    virtual void perform(u32, JDrama::TGraphics *) override;

    MActor *mActorData;              // 0x0068
    u32 unk1;                        // 0x006c
    JGeometry::TVec3<f32> mVelocity; // 0x0070
    u32 unk2;                        // 0x007c
};