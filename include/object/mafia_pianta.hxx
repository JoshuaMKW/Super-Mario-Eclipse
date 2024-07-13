#include <BetterSMS/module.hxx>
#include <Dolphin/types.h>
#include <SMS/NPC/NpcBase.hxx>

class TMafiaPianta : public TBaseNPC {

public:
    BETTER_SMS_FOR_CALLBACK static JDrama::TNameRef *instantiate() {
        return new TMafiaPianta("MafiaPianta");
    }

    TMafiaPianta(const char *name);
    ~TMafiaPianta() override;

    void control() override;
    void loadAfter() override;
    void perform(u32 flags, JDrama::TGraphics *graphics) override;
    bool receiveMessage(THitActor *sender, u32 message) override;
    void load(JSUMemoryInputStream &in) override;
    void playIdleAnim();
    void playSpottedAnim();
    void rotatePianta();

private:
    float mMaxAngle;
    float mDistance;
    float mTimeForSwitch;

    float mXTeleport;
    float mYTeleport;
    float mZTeleport;

    int frameCounter;
    int currentFrameCount;
    TVec3f mTeleportLoc;
};

extern ObjData mafiaPiantaData;