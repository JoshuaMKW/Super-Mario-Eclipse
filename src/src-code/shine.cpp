#include "sms/JSystem/JGeometry.hxx"
#include "sms/actor/item/Shine.hxx"
#include "sms/game/Conductor.hxx"
#include "sms/sound/MSBGM.hxx"

#include "SME.hxx"

using namespace SME;

// 0x801BD664
// extern -> SME.cpp
void Patch::Shine::manageShineVanish(JGeometry::TVec3<f32> *marioPos)
{
    TShine *shine;
    SME_FROM_GPR(r30, shine);

    const JGeometry::TVec3<f32> step(1.0f, 1.0f, 1.0f);
    
    JGeometry::TVec3<f32> size;
    JGeometry::TVec3<f32> rotation;
    JGeometry::TVec3<f32> position;

    shine->JSGGetScaling(reinterpret_cast<Vec *>(&size));
    shine->JSGGetRotation(reinterpret_cast<Vec *>(&rotation));
    shine->JSGGetTranslation(reinterpret_cast<Vec *>(&position));

    if (size.x - 0.011f <= 0)
    {
        rotation.y = 1.0f;
        size.set(1.0f, 1.0f, 1.0f);
        shine->JSGSetScaling(reinterpret_cast<Vec &>(size));
        shine->JSGSetRotation(reinterpret_cast<Vec &>(size));
        shine->mGlowSize.set(1.0f, 1.0f, 1.0f);
        shine->mActorData->mBckInfo->setAnmFromIndex(-1, nullptr);
        shine->kill();
    }
    else if (gpMarioAddress->mState != static_cast<u32>(TMario::State::SHINE_C))
    {
        rotation.y += 3.0f;
        position.y += 4.0f;
        size.sub(step);
        shine->JSGSetScaling(reinterpret_cast<Vec &>(size));
        shine->JSGSetRotation(reinterpret_cast<Vec &>(rotation));
        shine->JSGSetTranslation(reinterpret_cast<Vec &>(position));
        shine->mGlowSize.sub(step);
    }
    else
        shine->JSGSetTranslation(reinterpret_cast<Vec &>(*marioPos));
}

//0x802413E0
void Patch::Shine::isKillEnemiesShine(TConductor *gpConductor, JGeometry::TVec3<f32> *playerCoordinates, f32 range)
{
    TMario *player;
    SME_FROM_GPR(r31, player);

    TShine *shine = (TShine *)player->mGrabTarget;
    if (!(shine->mType & 0x10))
        killEnemiesWithin__10TConductorFRCQ29JGeometry8TVec3_f(gpConductor, playerCoordinates, range);
}

static void restoreMario(TMarDirector *gpMarDirector, u32 curState)
{
    TShine *shine = (TShine *)gpMarDirector->mCollectedShine;

    if (!shine || !(shine->mType & 0x10) || !gpMarDirector->sNextState)
        return;

    u8 *curSaveCard = (u8 *)(gpMarDirector->sNextState[0x118 / 4]);

    if (curState != TMarDirector::Status::NORMAL ||
        gpMarDirector->mLastState != TMarDirector::Status::SAVE_CARD ||
        gpMarioAddress->mState != static_cast<u32>(TMario::State::SHINE_C))
        return;

    if (curSaveCard[0x2E9] != 1)
    {
        if (SMS_isDivingMap__Fv() || (gpMarioAddress->mPrevState & 0x20D0) == 0x20D0)
            gpMarioAddress->mState = gpMarioAddress->mPrevState;
        else
            gpMarioAddress->mState = static_cast<u32>(TMario::State::IDLE);

        MSBgm::stopBGM(BGM_GET_SHINE, 20);
        MSBgm::startBGM(gStageBGM);
        endDemoCamera__15CPolarSubCameraFv(gpCamera);
    }
    else
        gpMarDirector->mGameState |= TMarDirector::State::WARP_OUT;
}

//0x802995BC
void Patch::Shine::checkBootOut(TMarDirector *gpMarDirector, u32 curState)
{
    restoreMario(gpMarDirector, curState);
    currentStateFinalize__12TMarDirectorFUc(gpMarDirector, curState);
}

//0x80293B10
u32 Patch::Shine::extendShineIDLogic(TFlagManager *flagManager, u32 flagID)
{
    if ((flagID & 0xFFFF) > 0x77)
        flagID += (0x60040 - 0x78);
    if (flagID > 0x60334)
        flagID = 0;
    return flagManager->getFlag(flagID);
}

//0x801BCC98
void Patch::Shine::shineObjectStringMod(JSUInputStream *stream, u8 *dst, u32 size)
{
    TShine *shine;
    SME_FROM_GPR(r30, shine);

    if (shine->mType == 1)
    {
        if (strcmp("nbnorm", (const char *)(dst + 4)) == 0)
            shine->mType = 0x10;
        else if (strcmp("nbquik", (const char *)(dst + 4)) == 0)
            shine->mType = 0x12;
        else if (strcmp("nbdemo", (const char *)(dst + 4)) == 0)
            shine->mType = 0x11;
    }
    read__14JSUInputStreamFPvl(stream, dst, size);
}

void Patch::Shine::shineFlagSetter(TFlagManager *flagManager, u32 flag, s32 val)
{
    if (flag < 0x60400)
    {
        flag &= 0xFFFF;
        if (flag < 0x40)
            ((u32 *)&flagManager->Type6Flag)[flag] = val;
        else
        {
            u32 shiftedFlag = flag >> 3;
            u32 mask = (flag & 7);
            u8 *flagField = &((u8 *)&flagManager->Type6Flag)[shiftedFlag];

            *flagField &= 1 << mask;
            *flagField |= (val & 1) << mask;
        }
    }
}

u32 Patch::Shine::shineFlagGetter(TFlagManager *flagManager, u32 flag)
{
    if (flag < 0x60400)
    {
        flag &= 0xFFFF;
        if (flag < 0x40)
            return ((u32 *)&flagManager->Type6Flag)[flag];
        else
        {
            u32 shiftedFlag = flag >> 3;
            u32 mask = (flag & 7);
            u8 *flagField = &((u8 *)&flagManager->Type6Flag)[shiftedFlag];

            return (*flagField >> mask) & 1;
        }
    }
    return false;
}

//0x802946D4
u32 Patch::Shine::shineSetClamper(TFlagManager *flagManager, u32 flag)
{
    flag &= 0x7FFFF;
    if (flag >= 0x10078)
        flag -= 0x10000;
        
    return flag;
}

//0x8029474C
u32 Patch::Shine::shineGetClamper(TFlagManager *flagManager, u32 flag)
{
    flag &= 0x7FFFF;
    if (flag >= 0x10078)
        flag -= 0x10000;
        
    return flagManager->getFlag(flag);
}

//0x80294334
void Patch::Shine::extendFlagManagerLoad(JSUMemoryInputStream &stream)
{
    stream.read(((u8 *)TFlagManager::smInstance + 0x1F4), 0x8C);
    stream.skip(0x120);
}


//0x802939B8
void Patch::Shine::extendFlagManagerSave(JSUMemoryOutputStream &stream)
{
    stream.write(((u8 *)TFlagManager::smInstance + 0x1F4), 0x8C);
    stream.skip(0x120, 0);
}

//0x80297BD8
void Patch::Shine::thinkSetBootFlag(void *shineFader, u32 unk_1, u32 unk_2)
{
    TMarDirector *gpMarDirector;
    SME_FROM_GPR(r31, gpMarDirector);
    
    if ((gpMarDirector->mCollectedShine->mType & 0x10) == false)
    {
        registFadeout__11TShineFaderFUsUs(shineFader, unk_1, unk_2);
        gpMarDirector->mGameState |= TMarDirector::State::WARP_OUT;
    }
}

u32 Patch::Shine::loadAfterMaskState()
{
    TShine *shine;
    SME_FROM_GPR(r31, shine);

    return shine->mType & 0xF;
}

//0x801BCEEC
void Patch::Shine::setKillState()
{
    TShine *shine;
    SME_FROM_GPR(r31, shine);

    shine->mType = (shine->mType & 0x10) | 1;
}