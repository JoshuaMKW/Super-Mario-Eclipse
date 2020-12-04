#include "includes/eclipse.hxx"

bool inXYZMode;

//0x802A615C
TMarDirector *xyzModifierMario()
{
    register TApplication *gpApplication;
    __asm { mr gpApplication, r31 };

    #ifndef SME_DEBUG
        if (!gIsCheatsEnabled)
            return gpApplication->mMarDirector;
    #endif

    TMario *gpMario = (TMario *)*(u32 *)TMarioInstance;
    if (!gpMario)
        return gpApplication->mMarDirector;

    if ((u32 *)gpMario->mController < MEM1_LO || (u32 *)gpMario->mController >= MEM1_HI)
        return gpApplication->mMarDirector;

    if (gpMario->mController->isFramePressed(TMarioGamePad::DPAD_RIGHT) && !inXYZMode)
    {
        inXYZMode = true;
    }
    else if (gpMario->mController->isFramePressed(TMarioGamePad::DPAD_RIGHT) && inXYZMode)
    {
        gpMario->mState = TMario::SA_IDLE;
        inXYZMode = false;
    }

    if (inXYZMode)
    {
        gpMario->mState = TMario::SA_IDLE | TMario::SA_CUTSCENE;
        float speedMultiplier = lerp<float>(1, 2, gpMario->mController->getRAnalogf());
        gpMario->mCoordinates.x += ((gpMario->mController->getMainStickAnalogX() * 83) * speedMultiplier);
        gpMario->mCoordinates.z -= ((gpMario->mController->getMainStickAnalogY() * 83) * speedMultiplier);

        if (gpMario->mController->isPressed(TMarioGamePad::DPAD_DOWN))
        {
            gpMario->mCoordinates.y -= (83 * speedMultiplier);
        }
        else if (gpMario->mController->isPressed(TMarioGamePad::DPAD_UP))
        {
            gpMario->mCoordinates.y += (83 * speedMultiplier);
        }
    }
    return gpApplication->mMarDirector;
}
kmCall(0x802A615C, &xyzModifierMario);

//0x802A8B58
bool SMS_IsExMap()
{
    TApplication *gpApplication = (TApplication *)0x803E9700;
    if (gInfo.mFile)
    {
        return gInfo.mFile->GlobalFlags.StageType.mIsExMap;
    }
    else
    {
        return (gpApplication->mCurrentScene.mAreaID >= TGameSequence::DOLPICEX0 &&
                gpApplication->mCurrentScene.mAreaID <= TGameSequence::COROEX6);
    }
}
kmBranch(0x802A8B58, &SMS_IsExMap);

//0x802A8B30
bool SMS_IsMultiplayerMap()
{
    TMarDirector *gpMarDirector = (TMarDirector *)*(u32 *)TMarDirectorInstance;
    if (gInfo.mFile)
    {
        return gInfo.mFile->GlobalFlags.StageType.mIsMultiPlayerMap;
    }
    else
    {
        return (gpMarDirector->mAreaID == TGameSequence::TEST10 && gpMarDirector->mEpisodeID == 0);
    }
}
kmBranch(0x802A8B30, &SMS_IsMultiplayerMap);

//0x802A8AFC
bool SMS_IsDivingMap()
{
    TMarDirector *gpMarDirector = (TMarDirector *)*(u32 *)TMarDirectorInstance;
    if (gInfo.mFile)
    {
        return gInfo.mFile->GlobalFlags.StageType.mIsDivingMap;
    }
    else
    {
        return (gpMarDirector->mAreaID == TGameSequence::MAREBOSS ||
                gpMarDirector->mAreaID == TGameSequence::MAREEX0 ||
                gpMarDirector->mAreaID == TGameSequence::MAREUNDERSEA);
    }
}
kmBranch(0x802A8AFC, &SMS_IsDivingMap);

//0x802A8AE0
bool SMS_IsOptionMap()
{
    TMarDirector *gpMarDirector = (TMarDirector *)*(u32 *)TMarDirectorInstance;
    if (gInfo.mFile)
    {
        return gInfo.mFile->GlobalFlags.StageType.mIsOptionMap;
    }
    else
    {
        return (gpMarDirector->mAreaID == 15);
    }
}
kmBranch(0x802A8AE0, &SMS_IsOptionMap);

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
    return gInfo.Light.mLightType != 0 && gpMarDirector->mAreaID != TGameSequence::OPTION;
}
kmCall(0x8027C6A4, &manageLightSize);
kmWrite32(0x8027C6A8, 0x28030001);

//0x802571F0
/*
float velocityCoordinatePatches(float floorCoordinateY)
{
    TMario *gpMario = (TMario *)*(u32 *)TMarioInstance;

    if (gpMario->mState != TMario::SA_IDLE)
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
*/

/*
//0x8028113C
bool infiniteFlutterPatch(float yVelocity)
{
    return (yVelocity < -1);
}
*/

/*0x8018987C
addi r3, r31, 0
lis r4, 0x8000
ori r4, r4, 0x4A6C
mtctr r4
bctrl
lwz r0, 0x000C (sp)
*/
//0x80004A6C
/*
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
*/

/*0x8018987C
addi r3, r31, 0
lis r4, 0x8000
ori r4, r4, 0x4A6C
mtctr r4
bctrl
lwz r0, 0x000C (sp)
*/
/*
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
*/

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
void replaceFmtSpecifier(char *buffer, char *src, const char *fmt, const char *sample)
{
    strcpy(buffer, src);

    u32 samplelen = strlen(sample);
    u32 fmtlen = strlen(fmt);

    char *curFmtPos = strstr(buffer, fmt);
    char *endstr = buffer + strlen(buffer);
    while (curFmtPos)
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
    TMario *gpMario = (TMario *)*(u32 *)TMarioInstance;

    char *msgbuffer = (char *)hcalloc(*(u32 *)JKRSystemHeap, 1024, 32);
    char buffer[64];
    char fmt[16];

    OSCalendarTime *calendarTime = (OSCalendarTime *)hmalloc(*(u32 *)JKRSystemHeap, sizeof(OSCalendarTime), 32);
    char date[16];
    char time[16];

    char *basemsg = msgfield + *entrydata + talker->curMsgIndex;
    char *newmsg = msgbuffer - (*entrydata + talker->curMsgIndex);

    OSTicksToCalendarTime(OSGetTime(), calendarTime);

    if (calendarTime->hour == 0)
        sprintf(time, (char *)"%u:%02u AM", calendarTime->hour + 12, calendarTime->min);
    else if (calendarTime->hour < 12)
        sprintf(time, (char *)"%u:%02u AM", calendarTime->hour % 13, calendarTime->min);
    else if (calendarTime->hour == 12)
        sprintf(time, (char *)"%u:%02u PM", calendarTime->hour, calendarTime->min);
    else
        sprintf(time, (char *)"%u:%02u PM", (calendarTime->hour + 1) % 13, calendarTime->min);

    sprintf(date, (char *)"%u/%u/%u", calendarTime->mon + 1, calendarTime->mday, calendarTime->year);

    memset(msgbuffer, 0, sizeof(msgbuffer));
    strcpy(msgbuffer, basemsg);

    //%
    memset(fmt, 0, sizeof(fmt));

    //name%
    strcat(fmt, (char *)"%name%");

    if (gpMario->mCustomInfo->mParams)
        replaceFmtSpecifier(msgbuffer, msgbuffer, fmt, (char *)(&gpMario->mCustomInfo->mParams + gpMario->mCustomInfo->mParams->Attributes.mNameOffset));
    else
        replaceFmtSpecifier(msgbuffer, msgbuffer, fmt, "Mario");

    //%
    memset(fmt, 0, sizeof(fmt));
    strcpy(fmt, "%shine%");

    sprintf(buffer, "%u", gpFlagManager->Type4Flag.mShineCount);

    replaceFmtSpecifier(msgbuffer, msgbuffer, fmt, buffer);

    //%
    memset(fmt, 0, sizeof(fmt));
    strcpy(fmt, "%bcoin%");

    sprintf(buffer, "%u", gpFlagManager->Type4Flag.mBlueCoinCount);

    replaceFmtSpecifier(msgbuffer, msgbuffer, fmt, buffer);

    //%
    memset(fmt, 0, sizeof(fmt));
    strcpy(fmt, "%rcoin%");

    sprintf(buffer, "%u", gpFlagManager->Type6Flag.mRedCoinCount);

    replaceFmtSpecifier(msgbuffer, msgbuffer, fmt, buffer);

    //%
    memset(fmt, 0, sizeof(fmt));
    strcpy(fmt, "%coin%");

    sprintf(buffer, "%u", gpFlagManager->Type4Flag.mGoldCoinCount);

    replaceFmtSpecifier(msgbuffer, msgbuffer, fmt, buffer);

    //%
    memset(fmt, 0, sizeof(fmt));
    strcpy(fmt, "%time%");

    replaceFmtSpecifier(msgbuffer, msgbuffer, fmt, time);

    //%
    memset(fmt, 0, sizeof(fmt));
    strcpy(fmt, "%date%");

    replaceFmtSpecifier(msgbuffer, msgbuffer, fmt, date);

    setupTextBox__8TTalk2D2FPCvP12JMSMesgEntry(talker, newmsg, entrydata);

    free(msgbuffer);
    free(calendarTime);
}

void maintainYoshi(TYoshi *gpYoshi)
{
    TMario *gpMario = (TMario *)*(u32 *)TMarioInstance;
    if (gpMario->mYoshi->isGreenYoshi())
    {
        *(float *)0x80415F4C = 480.0f; //tounge
        *(float *)0x80415F68 = 16384.0f;
    }
    else
    {
        *(float *)0x80415F4C = 300.0f;
        *(float *)0x80415F68 = 10000.0f;
    }
}

void realTimeCustomAttrsHandler(TMario *gpMario)
{
    gpMario->setCustomAttributes();
    maintainYoshi(gpMario->mYoshi);
}