#include "sms/object/ResetFruit.hxx"

#include "SME.hxx"

bool canFruitDieWater(u32 sound, u32 unk_1, u32 unk_2, u32 unk_3)
{
    register TResetFruit *gpFruit;
    __asm { mr gpFruit, r31 };

    startSoundActor__Q214MSoundSESystem8MSoundSEFUlPC3VecUlPP8JAISoundUlUc(sound, unk_1, unk_2, unk_3, 0, 4);

    return gpFruit->mObjectID == TResetFruit::DURIAN;
}
kmCall(0x801E330C, &canFruitDieWater);
kmWrite32(0x801E3310, 0x2C030000);
kmWrite32(0x801E3314, 0x418200AC);
kmWrite32(0x801E5438, 0x4800013C);

f32 chooseGrabDistancing(register M3UModelMario *mModel)
{
    register TMario *gpMario;
    __asm { mr gpMario, r31 };

    if (gpMario->mState & TMario::SA_WATERBORN)
    {
        __asm { mr r3, mModel };
        return 0.0f;
    }
    else
    {
        __asm { mr r3, mModel };
        return 11.0f;
    }
}
kmCall(0x8023F964, &chooseGrabDistancing);

bool isGrabWaitOver(TMario *gpMario)
{
    return isLast1AnimeFrame__6TMarioFv(gpMario) | (gpMario->mState & TMario::SA_WATERBORN);
}
kmCall(0x8023F9DC, &isGrabWaitOver);