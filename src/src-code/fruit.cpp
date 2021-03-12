#include "sms/object/ResetFruit.hxx"

#include "SME.hxx"

bool canFruitDieWater(TResetFruit *fruit)
{
    u32 *vTable;
    asm volatile ( "mr %0, r12" : "=r"(vTable));

    void (*virtualFunc)(TResetFruit *) = reinterpret_cast<void (*)(TResetFruit *)>(vTable[0x1E0 / 4]);

    if (fruit->mObjectID == TResetFruit::DURIAN)
    {
        virtualFunc(fruit);
        return true;
    }
    else
    {
        fruit->mFlags.mHasPhysics = true;
        if (gateCheck__6MSoundFUl(*gpMSound, 14453))
        {
            emitColumnWater__11TMapObjBaseFv(fruit);
            startSoundActor__Q214MSoundSESystem8MSoundSEFUlPC3VecUlPP8JAISoundUlUc(14453, &fruit->mPosition, 0, 0, 0, 4);
        }
    }

    return false;
}
kmCall(0x801E542C, &canFruitDieWater);
kmWrite32(0x801E5430, 0x2C030000);
kmWrite32(0x801E5434, 0x41820140);

f32 chooseGrabDistancing(M3UModelMario *mModel)
{
    TMario *player;
    asm volatile ( "mr %0, r31" : "=r"(player));

    if (player->mPrevState & TMario::State::WATERBORN)
    {
        asm volatile ( "mr 3, %0" : : "r"(mModel));
        return 0.0f;
    }
    else
    {
        asm volatile ( "mr 3, %0" : : "r"(mModel));
        return 11.0f;
    }
}
kmCall(0x8023F964, &chooseGrabDistancing);

bool isGrabWaitOver(TMario *player)
{
    return isLast1AnimeFrame__6TMarioFv(player) | (player->mPrevState & TMario::State::WATERBORN);
}
kmCall(0x8023F9DC, &isGrabWaitOver);