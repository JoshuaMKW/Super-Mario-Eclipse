#include "sms/actor/Mario.hxx"

class TEnemyMario : public TMario
{
public:

    enum CONTEXTS
    {
        UNK0,
        CAUTIONWAIT,
        REPEATJUMP,
        DELAYJUMP,
        UNK5,
        IDLE,
        IDLENOCOL,
        WARPAWAY,
        RUNTONODE,
        RUNWAIT,
        STARTRUN,
        FALLING,
        LAYWAITING,
        RUNAWAY,
        MOVETONEWGRAPH, //?
        LAUGHATTARGET = 16,
        LEAVEMAP = 17,
        LAUGHATTARGET2 = 18,
    };

    s16 _00;
    s16 _02;
    s16 mEnemyHealth;
    s16 _06;
    u32 _08[0x38 / 4];
    f32 mWarpSpeed;
    u32 _44[0x38 / 4];
    void *mEnemyManager; //?
};