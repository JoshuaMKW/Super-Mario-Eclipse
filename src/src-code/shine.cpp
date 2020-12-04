#include "includes/eclipse.hxx"

//0x801BD664
void manageShineVanish(JGeometry::TVec3<float> *marioPos)
{
    register TShine *gpShine;
    __asm { mr gpShine, r30 };

    TMarDirector *gpMarDirector = (TMarDirector *)*(u32 *)TMarDirectorInstance;
    TMario *gpMario = (TMario *)*(u32 *)TMarioInstance;

    if (gpShine->mSize.x - 0.011 <= 0)
    {
        setAnmFromIndex__12MActorAnmBlkFiPUs(gpShine->mActorData->mBckInfo, -1);
        makeObjDefault__11TMapObjBaseFv(gpShine);
        kill__6TShineFv(gpShine);
    }
    else if (gpMario->mState != TMario::SA_SHINE_C)
    {
        gpShine->mCoordinates.y += 4;
        gpShine->mSize.x -= 0.011f;
        gpShine->mSize.y -= 0.011f;
        gpShine->mSize.z -= 0.011f;
        gpShine->mGlowSize.x -= 0.011f;
        gpShine->mGlowSize.y -= 0.011f;
        gpShine->mGlowSize.z -= 0.011f;
        gpShine->mRotation.y += 3.0f;
    }
    else
    {
        gpShine->mCoordinates = *marioPos;
    }
}
kmCall(0x801BD664, &manageShineVanish);
kmWrite32(0x801BD668, 0x48000568);

//0x802413E0
void isKillEnemiesShine(TConductor *gpConductor, JGeometry::TVec3<float> *gpMarioCoordinates, float range)
{
    register TMario *gpMario;
    __asm { mr gpMario, r31 };

    TShine *gpShine = (TShine *)gpMario->mGrabTarget;
    if ((gpShine->mType & 0x10) == false)
    {
        killEnemiesWithin__10TConductorFRCQ29JGeometry8TVec3_f(gpConductor, gpMarioCoordinates, range);
    }
}
kmCall(0x802413E0, &isKillEnemiesShine);

void restoreMario(TMarDirector *gpMarDirector, u32 curState)
{
    TShine *gpShine = (TShine *)gpMarDirector->mCollectedShine;
    TMario *gpMario = (TMario *)*(u32 *)TMarioInstance;
    CPolarSubCamera *gpCamera = (CPolarSubCamera *)*(u32 *)CPolarSubCameraInstance;

    if (!gpShine || !(gpShine->mType & 0x10) || !gpMarDirector->sNextState)
        return;

    u8 *curSaveCard = (u8 *)(gpMarDirector->sNextState[0x118 / 4]);

    if (curState != TMarDirector::NORMAL ||
        gpMarDirector->mLastState != TMarDirector::SAVE_CARD ||
        gpMario->mState != TMario::SA_SHINE_C)
        return;

    if (curSaveCard[0x2E9] != 1)
    {
        if (SMS_isDivingMap__Fv() || (gpMario->mPrevState & 0x20D0) == 0x20D0)
        {
            gpMario->mState = gpMario->mPrevState;
        }
        else
        {
            gpMario->mState = TMario::SA_IDLE;
        }
        stopBGM__5MSBgmFUlUl(BGM_GET_SHINE);
        startBGM__5MSBgmFUl(*(u32 *)StageBGM);
        endDemoCamera__15CPolarSubCameraFv(gpCamera);
    }
    else
    {
        gpMarDirector->mGameState |= TMarDirector::WARP_OUT;
    }
}

//0x802995BC
void checkBootOut(TMarDirector *gpMarDirector, u32 curState)
{
    restoreMario(gpMarDirector, curState);
    currentStateFinalize__12TMarDirectorFUc(gpMarDirector, curState);
}
kmCall(0x802995BC, &checkBootOut);
kmWrite32(0x80297BE8, 0x60848200);

//0x80293B10
void extendShineIDLogic(TFlagManager *gpFlagManager, u32 flagID)
{
    if ((flagID & 0xFFFF) > 0x77)
        flagID += (0x60040 - 0x78);
    if (flagID > 0x60334)
        flagID = 0;
    getFlag__12TFlagManagerCFUl(gpFlagManager, flagID);
}
kmCall(0x80293B10, &extendShineIDLogic);

//0x801BCC98
u32 shineObjectStringMod(JSUInputStream *stream, u8 *dst, u32 size)
{
    register TShine *gpShine;
    __asm { mr gpShine, r30 };

    if (gpShine->mType == 1)
    {
        if (strcmp("nbnorm", (const char *)(dst + 4)) == 0) {
            gpShine->mType = 0x10;
        } else if (strcmp("nbquik", (const char *)(dst + 4)) == 0) {
            gpShine->mType = 0x12;
        } else if (strcmp("nbdemo", (const char *)(dst + 4)) == 0) {
            gpShine->mType = 0x11;
        }
    }
    read__14JSUInputStreamFPvl(stream, dst, size);
}
kmCall(0x801BCC98, &shineObjectStringMod);

void shineFlagSetter(TFlagManager *gpFlagManager, u32 flag, s32 val)
{
    if (flag < 0x60400)
    {
        flag &= 0xFFFF;
        if (flag < 0x40)
        {
            ((u32 *)&gpFlagManager->Type6Flag)[flag] = val;
        }
        else
        {
            u32 shiftedFlag = flag >> 3;
            u32 mask = (flag & 7);
            u8 *flagField = &((u8 *)&gpFlagManager->Type6Flag)[shiftedFlag];

            *flagField &= 1 << mask;
            *flagField |= (val & 1) << mask;
        }
    }
}
kmWritePointer(0x803DEE50, &shineFlagSetter);

bool shineFlagGetter(TFlagManager *gpFlagManager, u32 flag)
{
    if (flag < 0x60400)
    {
        flag &= 0xFFFF;
        if (flag < 0x40)
        {
            return ((u32 *)&gpFlagManager->Type6Flag)[flag];
        }
        else
        {
            u32 shiftedFlag = flag >> 3;
            u32 mask = (flag & 7);
            u8 *flagField = &((u8 *)&gpFlagManager->Type6Flag)[shiftedFlag];

            return (*flagField >> mask) & 1;
        }
    }
    return false;
}
kmWritePointer(0x803DEE7C, &shineFlagGetter);

//0x802946D4
u32 shineSetClamper(TFlagManager *gpFlagManager, u32 flag)
{
    flag &= 0x7FFFF;
    if (flag >= 120)
    {
        flag -= 0x10000;
    }
    return flag;
}
kmCall(0x802946D4, &shineSetClamper);

//0x8029474C
u32 shineGetClamper(TFlagManager *gpFlagManager, u32 flag)
{
    flag &= 0x7FFFF;
    if (flag >= 120)
    {
        flag -= 0x10000;
    }
    return getFlag__12TFlagManagerCFUl(gpFlagManager, flag);
}
kmWrite32(0x80294744, 0x60000000);
kmCall(0x8029474C, &shineGetClamper);

/*Shine casts, fix light*/
kmWrite32(0x80412548, (float)MAX_SHINES);
kmWrite32(0x80293AF8, 0x3BFF03E7);
kmWrite32(0x802946B8, 0x280003E7);
kmWrite32(0x8017BE78, 0x5464037E);
kmWrite32(0x8017BEF4, 0x5464037E);
kmWrite32(0x8017BF34, 0x5464037E);
kmWrite32(0x801BCE30, 0x5464037E);
kmWrite32(0x801FF850, 0x5404037E);
kmWrite32(0x802946B4, 0x5480043E);
kmWrite32(0x80294730, 0x5480043E);
kmWrite32(0x80294734, 0x280003E7);
kmWrite32(0x80297BA0, 0x5404037E);

//0x80294334
void extendFlagManagerLoad(JSUInputStream &stream)
{
    read__14JSUInputStreamFPvl(&stream, (*(u32 *)TFlagManagerInstance + 0x1F4), 0x8C);
    stream.skip(0x120);
}
kmCall(0x80294334, &extendFlagManagerLoad);
kmWrite32(0x80294338, 0x48000010);


//0x802939B8
void extendFlagManagerSave(JSUOutputStream &stream)
{
    write__15JSUOutputStreamFPCvl(&stream, (*(u32 *)TFlagManagerInstance + 0x1F4), 0x8C);
    stream.skip(0x120);
}
kmCall(0x802939B8, &extendFlagManagerSave);
kmWrite32(0x802939BC, 0x48000014);

//0x80297BD8
void thinkSetBootFlag(void *shineFader, u32 unk_1, u32 unk_2)
{
    register TMarDirector *gpMarDirector;
    __asm { mr gpMarDirector, r31 };
    
    if ((gpMarDirector->mCollectedShine->mType & 0x10) == false)
    {
        registFadeout__11TShineFaderFUsUs(shineFader, unk_1, unk_2);
        gpMarDirector->mGameState |= TMarDirector::WARP_OUT;
    }
}
kmCall(0x80297BD8, &thinkSetBootFlag);

u32 loadAfterMaskState()
{
    register TShine *gpShine;
    __asm { mr gpShine, r31 };

    return gpShine->mType & 0xF;
}
kmCall(0x801BCD20, &loadAfterMaskState);
kmWrite32(0x801BCD24, 0x28030002);

//0x801BCEEC
void setKillState()
{
    register TShine *gpShine;
    __asm { mr gpShine, r31 };

    gpShine->mType = (gpShine->mType & 0x10) | 1;
}
kmCall(0x801BCEEC, &setKillState);

//0x8029A590
asm void thinkCloseCamera()
{
    lbz r0, 0x190 (r4)
    lwz r4, 0x154 (r4)
    rlwinm. r4, r4, 0, 27, 27
    bne label
    li r0, 0
    label:
}
kmCall(0x8029A590, &thinkCloseCamera);
kmWrite32(0x8029A594, 0x28000000);

extern "C" void animationFreezeCheck();
kmCall(0x802999D8, &animationFreezeCheck);
kmWrite32(0x802999DC, 0x48000034);

//Make Shines glow more
kmWrite32(0x803C9190, 0x3F19999A);