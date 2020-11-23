#include "includes/eclipse.hxx"

#ifdef SME_DEBUG
//0x802A615C
TMarDirector *xyzModifierMario(TMarDirector *gpMarDirector)
{
    TMario *gpMario = (TMario *)*(u32 *)TMarioInstance;
    if (!gpMario)
    {
        return gpMarDirector;
    }

    if ((u32 *)gpMario->mController < MEM1_LO || (u32 *)gpMario->mController >= MEM1_HI)
    {
        return gpMarDirector;
    }

    if (gpMario->mController->isPressed(TMarioGamePad::BUTTONS::DPAD_RIGHT) && *(bool *)0x80003ED0 == false)
    {
        *(bool *)0x80003ED0 = true;
    }
    else if (gpMario->mController->isPressed(TMarioGamePad::BUTTONS::DPAD_RIGHT) && *(bool *)0x80003ED0 == true)
    {
        if (gpMario->mPrevState != 0x133F)
        {
            changePlayerStatus__6TMarioFUlUlb(gpMario, gpMario->mPrevState, 0, 0);
        }
        else
        {
            gpMario->mState = TMario::STATE::IDLE;
        }
        *(bool *)0x80003ED0 = false;
    }

    if (*(bool *)0x80003ED0 == true)
    {
        gpMario->mState = TMario::STATE::IDLE | TMario::STATE::CUTSCENE;
        float speedMultiplier = lerp<float>(1, 2, gpMario->mController->getRAnalogf());
        gpMario->mCoordinates.x += ((gpMario->mController->getMainStickAnalogX() * 83) * speedMultiplier);
        gpMario->mCoordinates.z -= ((gpMario->mController->getMainStickAnalogY() * 83) * speedMultiplier);

        if (gpMario->mController->isPressed(TMarioGamePad::BUTTONS::DPAD_DOWN))
        {
            gpMario->mCoordinates.y -= (83 * speedMultiplier);
        }
        else if (gpMario->mController->isPressed(TMarioGamePad::BUTTONS::DPAD_UP))
        {
            gpMario->mCoordinates.y += (83 * speedMultiplier);
        }
    }
    return gpMarDirector;
}

//0x802A8064
/*
u32 set_debugger()
{
    TMario *gpMario = (TMario *)*(u32 *)TMarioInstance;
    TFlagManager *gpFlagManager = (TFlagManager *)*(u32 *)TFlagManagerInstance;
    TWaterManager *gpWaterManager = (TWaterManager *)*(u32 *)TWaterManagerInstance;

    if (!gpWaterManager || !gpMario || ((u32 *)gpMario->mController < MEM1_LO || (u32 *)gpMario->mController >= MEM1_HI))
    {
        return 0;
    }

    if (gpMario->mController->Buttons.mBButton && gpMario->mController->Buttons.mDPadUp)
    {
        gpWaterManager->mLayerCount += 1;
    }
    else if (gpMario->mController->Buttons.mBButton && gpMario->mController->Buttons.mDPadDown)
    {
        gpWaterManager->mLayerCount -= 1;
    }
    else if (gpMario->mController->Buttons.mBButton && gpMario->mController->Buttons.mDPadRight)
    {
        gpFlagManager->Type4Flag.mShineCount += 1;
    }
    else if (gpMario->mController->Buttons.mBButton && gpMario->mController->Buttons.mDPadLeft)
    {
        gpFlagManager->Type4Flag.mShineCount -= 1;
    }
    else if (gpMario->mController->Buttons.mXButton && gpMario->mController->Buttons.mDPadUp)
    {
        *(s16 *)0x8027C90E += 1;
        if (*(s16 *)0x8027C90E > 7)
        {
            *(s16 *)0x8027C90E = 7;
        }
        flushAddr((void *)0x8027C90C);
    }
    else if (gpMario->mController->Buttons.mXButton && gpMario->mController->Buttons.mDPadDown)
    {
        *(s16 *)0x8027C90E -= 1;
        if (*(s16 *)0x8027C90E < 0)
        {
            *(s16 *)0x8027C90E = 0;
        }
        flushAddr((void *)0x8027C90C);
    }
    return 0;
}
*/

//0x80243940
TMario *warpMario(TMario *gpMario)
{
    warpInEffect__6TMarioFv(gpMario);
    return gpMario;
}
#endif

//0x802A8B58
bool SMS_IsExMap()
{
    TApplication *gpApplication = (TApplication *)0x803E9700;
    if (gInfo.mFile)
    {
        return gInfo.mFile->FileHeader.StageType.mIsExMap;
    }
    else
    {
        return (gpApplication->mCurrentScene.mAreaID >= TGameSequence::AREA::DOLPICEX0 &&
                gpApplication->mCurrentScene.mAreaID <= TGameSequence::AREA::COROEX6);
    }
}

//0x802A8B30
bool SMS_IsMultiplayerMap()
{
    TMarDirector *gpMarDirector = (TMarDirector *)*(u32 *)TMarDirectorInstance;
    if (gInfo.mFile)
    {
        return gInfo.mFile->FileHeader.StageType.mIsMultiPlayerMap;
    }
    else
    {
        return (gpMarDirector->mAreaID == TGameSequence::AREA::TEST10 && gpMarDirector->mEpisodeID == 0);
    }
}

//0x802A8AFC
bool SMS_IsDivingMap()
{
    TMarDirector *gpMarDirector = (TMarDirector *)*(u32 *)TMarDirectorInstance;
    if (gInfo.mFile)
    {
        return gInfo.mFile->FileHeader.StageType.mIsDivingMap;
    }
    else
    {
        return (gpMarDirector->mAreaID == TGameSequence::AREA::MAREBOSS ||
                gpMarDirector->mAreaID == TGameSequence::AREA::MAREEX0 ||
                gpMarDirector->mAreaID == TGameSequence::AREA::MAREUNDERSEA);
    }
}

//0x802A8AE0
bool SMS_IsOptionMap()
{
    TMarDirector *gpMarDirector = (TMarDirector *)*(u32 *)TMarDirectorInstance;
    if (gInfo.mFile)
    {
        return gInfo.mFile->FileHeader.StageType.mIsOptionMap;
    }
    else
    {
        return (gpMarDirector->mAreaID == 15);
    }
}

//0x801BD664
void manageShineVanish(JGeometry::TVec3<float> *marioPos)
{
    TMarDirector *gpMarDirector = (TMarDirector *)*(u32 *)TMarDirectorInstance;
    TMario *gpMario = (TMario *)*(u32 *)TMarioInstance;
    TShine *gpShine = gpMarDirector->mCollectedShine;

    if (gpShine->mSize.x - 0.01055 <= 0)
    {
        gpShine->mSize = {1, 1, 1};
        gpShine->mGlowSize = {1, 1, 1};
        gpShine->mRotation.y = 0;
        makeObjDead__11TMapObjBaseFv(gpShine);
    }
    else if (gpMario->mState != TMario::STATE::SHINE_C)
    {
        gpShine->mCoordinates.y += 4;
        gpShine->mSize.x -= 0.01055;
        gpShine->mSize.y -= 0.01055;
        gpShine->mSize.z -= 0.01055;
        gpShine->mGlowSize.x -= 0.01055;
        gpShine->mGlowSize.y -= 0.01055;
        gpShine->mGlowSize.z -= 0.01055;
        gpShine->mRotation.y += 3;
    }
    else
    {
        gpShine->mCoordinates = *marioPos;
    }
}

//kWrite32(0x801BD668, 0x48000568);

//0x802413E0
void isKillEnemiesShine(TConductor *gpConductor, JGeometry::TVec3<float> *gpMarioCoordinates, float range)
{
    TMario *gpMario = (TMario *)*(u32 *)TMarioInstance;
    TShine *gpShine = (TShine *)gpMario->mGrabTarget;
    if ((gpShine->mType & 0x10) == false)
    {
        killEnemiesWithin__10TConductorFRCQ29JGeometry8TVec3_f(gpConductor, gpMarioCoordinates, range);
    }
}

void restoreMario(TMarDirector *gpMarDirector, u32 curState)
{
    TShine *gpShine = (TShine *)gpMarDirector->mCollectedShine;
    TMario *gpMario = (TMario *)*(u32 *)TMarioInstance;
    CPolarSubCamera *gpCamera = (CPolarSubCamera *)*(u32 *)CPolarSubCameraInstance;

    if (!gpShine || !(gpShine->mType & 0x10) || gpMarDirector->sNextState == nullptr)
        return;

    u8 *curSaveCard = (u8 *)(gpMarDirector->sNextState[0x118 / 4]);

    if (curState != TMarDirector::STATUS::NORMAL ||
        gpMarDirector->mLastState != TMarDirector::STATUS::SAVE_CARD ||
        gpMario->mState != TMario::STATE::SHINE_C)
        return;

    if (curSaveCard[0x2E9] != 1)
    {
        if (SMS_isDivingMap__Fv() || (gpMario->mPrevState & 0x20D0) == 0x20D0)
        {
            gpMario->mState = gpMario->mPrevState;
        }
        else
        {
            gpMario->mState = TMario::STATE::IDLE;
        }
        startStageBGM__10MSMainProcFUcUc();
        endDemoCamera__15CPolarSubCameraFv(gpCamera);
    }
    else
    {
        gpMarDirector->mGameState |= TMarDirector::STATE::WARP_OUT;
    }
}

//0x802995BC
void checkBootOut(TMarDirector *gpMarDirector, u32 curState)
{
    restoreMario(gpMarDirector, curState);
    currentStateFinalize__12TMarDirectorFUc(gpMarDirector, curState);
}

//0x80293B10
void extendShineIDLogic(TFlagManager *gpFlagManager, u32 flagID)
{
    if ((flagID & 0xFFFF) > 0x77)
        flagID += (0x60040 - 0x78);
    if (flagID > 0x60334)
        flagID = 0;
    getFlag__12TFlagManagerCFUl(gpFlagManager, flagID);
}

/*Needs to be input as gecko code
u32 shineObjectStringMod() {
    u32 shineStatus = 1;
    if (callFunction(strcmp)("nbnorm", "shine") == 0) {
        shineStatus = 0x10;
    } else if (callFunction(strcmp)("nbquik", "shine") == 0) {
        shineStatus = 0x12;
    } else if (callFunction(strcmp)("nbdemo", "shine") == 0) {
        shineStatus = 0x11;
    }
    return shineStatus;
}*/

/*Shine casts, fix light
kWrite32(0x80412548, (u32)300.0f);
kWrite32(0x80293AF8, 0x3BFF03E7);
kWrite32(0x802946B8, 0x280003E7);
kWrite32(0x8017BE78, 0x5464037E);
kWrite32(0x8017BEF4, 0x5464037E);
kWrite32(0x8017BF34, 0x5464037E);
kWrite32(0x801BCE30, 0x5464037E);
kWrite32(0x801FF850, 0x5404037E);
kWrite32(0x802946B4, 0x5480043E);
kWrite32(0x80294730, 0x5480043E);
kWrite32(0x80294734, 0x280003E7);
kWrite32(0x80297BA0, 0x5404037E);
kWrite32(0x80294FCC, 0x418200C8);
kWrite32(0x8029519C, 0x418200C8);
*/

//0x80294334
void extendFlagManagerLoad(JSUInputStream &stream)
{
    read__14JSUInputStreamFPvl(&stream, (*(u32 *)TFlagManagerInstance + 0x1F4), 0x8C);
    stream.skip(0x120);
}

//kWrite32(0x802939BC, 0x48000014);

//0x802939B8
void extendFlagManagerSave(JSUOutputStream &stream)
{
    write__15JSUOutputStreamFPCvl(&stream, (*(u32 *)TFlagManagerInstance + 0x1F4), 0x8C);
    stream.skip(0x120);
}

//kWrite32(0x80294338 0x48000010);

//0x8027C6A4
bool manageLightSize()
{
    SMEFile *file = gInfo.mFile;
    TMarDirector *gpMarDirector = (TMarDirector *)*(u32 *)TMarDirectorInstance;
    TWaterManager *gpWaterManager = (TWaterManager *)*(u32 *)TWaterManagerInstance;
    TFlagManager *gpFlagManager = (TFlagManager *)*(u32 *)TFlagManagerInstance;
    JGeometry::TVec3<float> *gpLightCoordinates = (JGeometry::TVec3<float> *)ShineShadowCoordinates;
    JGeometry::TVec3<float> *gpMarioCoordinates = (JGeometry::TVec3<float> *)*(u32 *)TMarioCoordinates;

    if (!file)
    {
        return (gpMarDirector->mAreaID == 1);
    }

    if (gInfo.Light.mLightType == 1)
    {
        if (file->Light.mDarkLevel != 255)
        {
            gpWaterManager->mDarkLevel = file->Light.mDarkLevel;
        }
        else if (gpFlagManager->Type4Flag.mShineCount < MAX_SHINES)
        {
            gpWaterManager->mDarkLevel = lerp<u8>(30, 190, (float)gpFlagManager->Type4Flag.mShineCount / (float)MAX_SHINES);
        }
        else
        {
            if (gpWaterManager->mDarkLevel < 255)
            {
                gpWaterManager->mDarkLevel += 1;
            }
            else
            {
                gInfo.Light.mLightType = 0;
            }
        }

        gpLightCoordinates->x = gInfo.Light.mShineShadowCoordinates.x;
        gpLightCoordinates->y = gInfo.Light.mShineShadowCoordinates.y;
        gpLightCoordinates->z = gInfo.Light.mShineShadowCoordinates.z;

        float sigOfs;
        float sigStrength;
        float next;
        float prev;
        float cur;

        if (gpFlagManager->Type4Flag.mShineCount > gInfo.Light.mPrevShineCount)
        {
            sigOfs = 300;
            sigStrength = 0.04;
        }
        else if (gpFlagManager->Type4Flag.mShineCount < gInfo.Light.mPrevShineCount)
        {
            sigOfs = 300;
            sigStrength = -0.04;
        }

        if (!gInfo.Light.mSizeMorphing && gpFlagManager->Type4Flag.mShineCount > gInfo.Light.mPrevShineCount)
        {
            gInfo.Light.mPrevSize = gpWaterManager->mSize;
            gInfo.Light.mNextSize = gpWaterManager->mSize;

            for (u32 i = 0; i < (gpFlagManager->Type4Flag.mShineCount - gInfo.Light.mPrevShineCount); ++i)
            {
                gInfo.Light.mNextSize += (10000 / MAX_SHINES) + (gInfo.Light.mPrevShineCount + i) * 2;
            }

            gInfo.Light.mSizeMorphing = true;
            gInfo.Light.mStepContext = 0;
        }
        else if (!gInfo.Light.mSizeMorphing && gpFlagManager->Type4Flag.mShineCount < gInfo.Light.mPrevShineCount)
        {
            gInfo.Light.mPrevSize = gpWaterManager->mSize;
            gInfo.Light.mNextSize = gpWaterManager->mSize;

            for (u32 i = 0; i < (gInfo.Light.mPrevShineCount - gpFlagManager->Type4Flag.mShineCount); ++i)
            {
                gInfo.Light.mPrevSize -= (10000 / MAX_SHINES) + (gInfo.Light.mPrevShineCount - i) * 2;
            }

            gInfo.Light.mSizeMorphing = true;
            gInfo.Light.mStepContext = 0;
        }

        if (gInfo.Light.mSizeMorphing)
        {
            next = gInfo.Light.mNextSize;
            prev = gInfo.Light.mPrevSize;
            cur = sigmoidCurve(gInfo.Light.mStepContext, prev, next, sigOfs, sigStrength);

            if (gpWaterManager->mSize > 70000)
            {
                gpWaterManager->mSize = 70000;
                gInfo.Light.mSizeMorphing = false;
            }
            else if (gpWaterManager->mSize < 0)
            {
                gpWaterManager->mSize = 0;
                gInfo.Light.mSizeMorphing = false;
            }
            else if (cur != gInfo.Light.mNextSize && cur != gInfo.Light.mPrevSize)
            {
                gpWaterManager->mSize = cur;
                gpWaterManager->mSphereStep = cur / 2;
                gInfo.Light.mStepContext += 1;
            }
            else
            {
                gpWaterManager->mSize = cur;
                gpWaterManager->mSphereStep = cur / 2;
                gInfo.Light.mPrevShineCount = gpFlagManager->Type4Flag.mShineCount;
                gInfo.Light.mSizeMorphing = false;
            }
        }
    }
    else if (gInfo.Light.mLightType == 2)
    {
        gpWaterManager->mDarkLevel = file->Light.mDarkLevel;
        gpLightCoordinates->x = gpMarioCoordinates->x + gInfo.Light.mShineShadowCoordinates.x;
        gpLightCoordinates->y = gpMarioCoordinates->y + gInfo.Light.mShineShadowCoordinates.y;
        gpLightCoordinates->z = gpMarioCoordinates->z + gInfo.Light.mShineShadowCoordinates.z;
    }
    return !(gInfo.Light.mLightType == 0 || gpMarDirector->mAreaID == TGameSequence::AREA::OPTION);
}

//0x802571F0
float velocityCoordinatePatches(float floorCoordinateY)
{
    TMario *gpMario = (TMario *)*(u32 *)TMarioInstance;

    if (gpMario->mState != TMario::STATE::IDLE)
    { //Y storage
        gpMario->mSpeed.y = 0;
    }

    if (floorCoordinateY < gpMario->mCoordinates.y - 20)
    { //Downwarping
        floorCoordinateY = gpMario->mCoordinates.y;
    }
    asm("lfs 0, -0x0EC8 (2)");
    return floorCoordinateY;
}

//0x8028113C
bool infiniteFlutterPatch(float yVelocity)
{
    return (yVelocity < -1);
}

/*0x8018987C
addi r3, r31, 0
lis r4, 0x8000
ori r4, r4, 0x4A6C
mtctr r4
bctrl
lwz r0, 0x000C (sp)
*/
//0x80004A6C
float downWarpPatch(TMario *gpMario, float yVelocity)
{
    if (yVelocity < -100)
    {
        return gpMario->mSpeed.y;
    }
    else
    {
        return yVelocity;
    }
}

/*0x8018987C
addi r3, r31, 0
lis r4, 0x8000
ori r4, r4, 0x4A6C
mtctr r4
bctrl
lwz r0, 0x000C (sp)
*/
float upWarpPatch(TMario *gpMario, float yVelocity)
{
    if (yVelocity > 1000000)
    {
        return gpMario->mSpeed.y;
    }
    else
    {
        return yVelocity;
    }
}

//MESSAGE MODIFICATIONS

static inline u32 bmgVarLen(char *src)
{
    u32 len = 1;
    src++;
    while (*src++ != '%')
    {
        len += 1;
    }
    return len;
}

//NOTE: Must send to new buffer
void replaceFmtSpecifier(char *buffer, char *src, char *fmt, char *sample)
{
    strcpy(buffer, src);

    u32 samplelen = strlen(sample);
    u32 fmtlen = strlen(fmt);

    char *curFmtPos = strstr(buffer, fmt);
    char *endstr = buffer + strlen(buffer);
    while (curFmtPos != nullptr)
    {
        memcpy(curFmtPos + samplelen, curFmtPos + fmtlen, strlen(curFmtPos + fmtlen));

        strncpy(curFmtPos, sample, samplelen);

        curFmtPos = strstr(buffer, fmt);
        endstr = buffer + strlen(buffer);
    }
}

//0x80153DE8, 0x80153E1C
/*
/ 0x803E780B = "%"
/ 0x800048D0 = "name"
/ 0x80416688 = "Mario"
*/
void formatMessage(Talk2D2 *talker, char *msgfield, u32 *entrydata)
{
    TFlagManager *gpFlagManager = (TFlagManager *)*(u32 *)TFlagManagerInstance;

    char *msgbuffer = (char *)hcalloc(*(u32 *)JKRSystemHeap, 1024, 32);
    char buffer[64];
    char fmt[16];

    OSCalendarTime *calendarTime = (OSCalendarTime *)hmalloc(*(u32 *)JKRSystemHeap, sizeof(OSCalendarTime), 32);
    char date[16];
    char time[16];

    char *basemsg = msgfield + *entrydata + talker->curMsgIndex;
    char *newmsg = msgbuffer - (*entrydata + talker->curMsgIndex);

    OSTicksToCalendarTime(OSGetTime(), calendarTime);

    if (calendarTime->mHours == 0)
        sprintf(time, (char *)0x800048EA, calendarTime->mHours + 12, calendarTime->mMinutes);
    else if (calendarTime->mHours < 12)
        sprintf(time, (char *)0x800048EA, calendarTime->mHours % 13, calendarTime->mMinutes);
    else if (calendarTime->mHours == 12)
        sprintf(time, (char *)0x800048DF, calendarTime->mHours, calendarTime->mMinutes);
    else
        sprintf(time, (char *)0x800048DF, (calendarTime->mHours + 1) % 13, calendarTime->mMinutes);

    sprintf(date, (char *)0x800048F5, calendarTime->mMonth + 1, calendarTime->mDayOfMonth, calendarTime->mYear);

    memset(msgbuffer, 0, sizeof(msgbuffer));
    strcpy(msgbuffer, basemsg);

    //%
    memset(fmt, 0, sizeof(fmt));
    strcpy(fmt, (char *)0x803E780B);

    //name%
    strcat(fmt, (char *)0x800048D0);
    strcat(fmt, (char *)0x803E780B);

    if (gInfo.mCharacterFile)
        replaceFmtSpecifier(msgbuffer, msgbuffer, fmt, gInfo.mCharacterFile->mCharacterName);
    else
        replaceFmtSpecifier(msgbuffer, msgbuffer, fmt, (char *)0x80416688);

    //%
    memset(fmt, 0, sizeof(fmt));
    strcpy(fmt, (char *)0x803E780B);

    //shine%
    strcat(fmt, (char *)0x80412EB8);
    strcat(fmt, (char *)0x803E780B);

    sprintf(buffer, (char *)0x800048FE, gpFlagManager->Type4Flag.mShineCount);

    replaceFmtSpecifier(msgbuffer, msgbuffer, fmt, buffer);

    //%
    memset(fmt, 0, sizeof(fmt));
    strcpy(fmt, (char *)0x803E780B);

    //bcoin%
    strcat(fmt, (char *)0x803E7885);
    strcat(fmt, (char *)0x80412EB0);
    strcat(fmt, (char *)0x803E780B);

    sprintf(buffer, (char *)0x800048FE, gpFlagManager->Type4Flag.mBlueCoinCount);

    replaceFmtSpecifier(msgbuffer, msgbuffer, fmt, buffer);

    //%
    memset(fmt, 0, sizeof(fmt));
    strcpy(fmt, (char *)0x803E780B);

    //rcoin%
    strcat(fmt, (char *)0x803E78BB);
    strcat(fmt, (char *)0x80412EB0);
    strcat(fmt, (char *)0x803E780B);

    sprintf(buffer, (char *)0x800048FE, gpFlagManager->Type6Flag.mRedCoinCount);

    replaceFmtSpecifier(msgbuffer, msgbuffer, fmt, buffer);

    //%
    memset(fmt, 0, sizeof(fmt));
    strcpy(fmt, (char *)0x803E780B);

    //coin%
    strcat(fmt, (char *)0x80412EB0);
    strcat(fmt, (char *)0x803E780B);

    sprintf(buffer, (char *)0x800048FE, gpFlagManager->Type4Flag.mGoldCoinCount);

    replaceFmtSpecifier(msgbuffer, msgbuffer, fmt, buffer);

    //%
    memset(fmt, 0, sizeof(fmt));
    strcpy(fmt, (char *)0x803E780B);

    //time%
    strcat(fmt, (char *)0x800048D5);
    strcat(fmt, (char *)0x803E780B);

    replaceFmtSpecifier(msgbuffer, msgbuffer, fmt, time);

    //%
    memset(fmt, 0, sizeof(fmt));
    strcpy(fmt, (char *)0x803E780B);

    //date%
    strcat(fmt, (char *)0x800048DA);
    strcat(fmt, (char *)0x803E780B);

    replaceFmtSpecifier(msgbuffer, msgbuffer, fmt, date);

    setupTextBox__8TTalk2D2FPCvP12JMSMesgEntry(talker, newmsg, entrydata);

    free(msgbuffer);
    free(calendarTime);
}