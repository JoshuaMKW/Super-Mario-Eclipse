#include "CARD.h"
#include "string.h"
#include "printf.h"
#include "sms/JUT.hxx"
#include "sms/J2D.hxx"
#include "sms/talk/Talk2D2.hxx"

#include "SME.hxx"

extern String *formatBMG(String *msg);
extern void drawCheatText();
extern J2DTextBox *gDebugTextBox;

bool inXYZMode;

static void xyzModifierMario()
{   
    #ifndef SME_DEBUG
        if (!gDebugModeHandler.isActive())
            return;
    #endif

    if (!gpMarioAddress)
        return;

    if ((void *)gpMarioAddress->mController < (void *)0x80000000 || (u32 *)gpMarioAddress->mController >= (void *)0x81800000)
        return;

    if (gpMarioAddress->mController->mButtons.mFrameInput & TMarioGamePad::Buttons::DPAD_RIGHT && !inXYZMode)
    {
        inXYZMode = true;
    }
    else if (gpMarioAddress->mController->mButtons.mFrameInput & TMarioGamePad::Buttons::DPAD_RIGHT && inXYZMode)
    {
        gpMarioAddress->mState = TMario::State::IDLE;
        inXYZMode = false;
    }

    if (inXYZMode)
    {
        gpMarioAddress->mState = TMario::State::IDLE | TMario::State::CUTSCENE;
        f32 speedMultiplier = lerp<f32>(1, 2, gpMarioAddress->mController->mButtons.mAnalogR);
        gpMarioAddress->mPosition.x += ((gpMarioAddress->mController->mControlStick.mStickX * 83) * speedMultiplier);
        gpMarioAddress->mPosition.z -= ((gpMarioAddress->mController->mControlStick.mStickY * 83) * speedMultiplier);

        if (gpMarioAddress->mController->mButtons.mInput & TMarioGamePad::Buttons::DPAD_DOWN)
        {
            gpMarioAddress->mPosition.y -= (83 * speedMultiplier);
        }
        else if (gpMarioAddress->mController->mButtons.mInput & TMarioGamePad::Buttons::DPAD_UP)
        {
            gpMarioAddress->mPosition.y += (83 * speedMultiplier);
        }
    }
    return;
}

// this is ran once
// extern -> SME.cpp
void onSetup(TMarDirector* director)
{
    gDebugTextBox = new (gInfo.mGlobalsHeap, 4) J2DTextBox(gpSystemFont->mFont, "Debug Mode");

	// run replaced call
	director->setupObjects();
}

// this is ran every frame
// extern -> SME.cpp
s32 onUpdate(TMarDirector* director)
{
    xyzModifierMario();
    //drawCheatText(); //currently bugged

    // run replaced call
	return director->direct();
}

// this is ran when drawing is needed
// extern -> SME.cpp
void onDraw2D(J2DOrthoGraph *graph)
{
	// run replaced call
	graph->setup2D();
}

// 0x802A8B58
// extern -> SME.cpp
bool SMS_IsExMap()
{
    if (SMEFile::mStageConfig.FileHeader.mMAGIC == SMEFile::MAGIC)
    {
        return SMEFile::mStageConfig.GlobalFlags.StageType.mIsExMap;
    }
    else
    {
        return (gpApplication.mCurrentScene.mAreaID >= TGameSequence::DOLPICEX0 &&
                gpApplication.mCurrentScene.mAreaID <= TGameSequence::COROEX6);
    }
}

// 0x802A8B30
// extern -> SME.cpp
bool SMS_IsMultiplayerMap()
{
    if (SMEFile::mStageConfig.FileHeader.mMAGIC == SMEFile::MAGIC)
    {
        return SMEFile::mStageConfig.GlobalFlags.StageType.mIsMultiPlayerMap;
    }
    else
    {
        return (gpMarDirector->mAreaID == TGameSequence::TEST10 && gpMarDirector->mEpisodeID == 0);
    }
}

// 0x802A8AFC
// extern -> SME.cpp
bool SMS_IsDivingMap()
{
    if (SMEFile::mStageConfig.FileHeader.mMAGIC == SMEFile::MAGIC)
    {
        return SMEFile::mStageConfig.GlobalFlags.StageType.mIsDivingMap;
    }
    else
    {
        return (gpMarDirector->mAreaID == TGameSequence::MAREBOSS ||
                gpMarDirector->mAreaID == TGameSequence::MAREEX0 ||
                gpMarDirector->mAreaID == TGameSequence::MAREUNDERSEA);
    }
}

// 0x802A8AE0
// extern -> SME.cpp
bool SMS_IsOptionMap()
{
    if (SMEFile::mStageConfig.FileHeader.mMAGIC == SMEFile::MAGIC)
    {
        return SMEFile::mStageConfig.GlobalFlags.StageType.mIsOptionMap;
    }
    else
    {
        return (gpMarDirector->mAreaID == 15);
    }
}

// 0x8027C6A4
// extern -> SME.cpp
bool manageLightSize()
{
    if (SMEFile::mStageConfig.FileHeader.mMAGIC != SMEFile::MAGIC ||
        !SMEFile::mStageConfig.GlobalFlags.mIsShineShadow)
        return (gpMarDirector->mAreaID == 1);

    s32 &CurrentShineCount = TFlagManager::smInstance->Type4Flag.mShineCount;
    s32 &PrevShineCount = gInfo.Light.mPrevShineCount;
    switch (gInfo.Light.mLightType)
    {
    case LightContext::STATIC: {
        if (SMEFile::mStageConfig.Light.mDarkLevel != 255)
            gpModelWaterManager->mDarkLevel = SMEFile::mStageConfig.Light.mDarkLevel;
        else if (CurrentShineCount < SME_MAX_SHINES)
            gpModelWaterManager->mDarkLevel = lerp<u8>(30, 190,
                                                       static_cast<f32>(CurrentShineCount) /
                                                           static_cast<f32>(SME_MAX_SHINES));
        else
        {
            if (gpModelWaterManager->mDarkLevel < 255)
                gpModelWaterManager->mDarkLevel += 1;
            else
                gInfo.Light.mLightType = LightContext::DISABLED;
        }

        gShineShadowPos = gInfo.Light.mShineShadowCoordinates;

        f32 sigOfs = 300.0f;
        f32 sigStrength = CurrentShineCount >= PrevShineCount ? 0.04f : -0.04f;

        if (!gInfo.Light.mSizeMorphing &&
            CurrentShineCount == PrevShineCount)
            break;

        if (CurrentShineCount > PrevShineCount)
        {
            gInfo.Light.mPrevSize = gpModelWaterManager->mSize;
            gInfo.Light.mNextSize = gpModelWaterManager->mSize;

            for (u32 i = 0; i < (CurrentShineCount - PrevShineCount); ++i)
                gInfo.Light.mNextSize += (10000.0f / SME_MAX_SHINES) + (PrevShineCount + i) * 2.0f;

            gInfo.Light.mSizeMorphing = true;
            gInfo.Light.mStepContext = 0.0f;
        }
        else if (CurrentShineCount < PrevShineCount)
        {
            gInfo.Light.mPrevSize = gpModelWaterManager->mSize;
            gInfo.Light.mNextSize = gpModelWaterManager->mSize;

            for (u32 i = 0; i < (PrevShineCount - CurrentShineCount); ++i)
                gInfo.Light.mNextSize -= (10000.0f / SME_MAX_SHINES) + (PrevShineCount - i) * 2.0f;

            gInfo.Light.mSizeMorphing = true;
            gInfo.Light.mStepContext = 0.0f;
        }

        f32 cur = sigmoidCurve(gInfo.Light.mStepContext, gInfo.Light.mPrevSize,
                               gInfo.Light.mNextSize, sigOfs, sigStrength);

        if (gpModelWaterManager->mSize > 70000.0f)
        {
            gpModelWaterManager->mSize = 70000.0f;
            gInfo.Light.mSizeMorphing = false;
        }
        else if (gpModelWaterManager->mSize < 0.0f)
        {
            gpModelWaterManager->mSize = 0.0f;
            gInfo.Light.mSizeMorphing = false;
        }
        else if (cur != gInfo.Light.mNextSize && cur != gInfo.Light.mPrevSize)
        {
            gpModelWaterManager->mSize = cur;
            gpModelWaterManager->mSphereStep = cur / 2.0f;
            gInfo.Light.mStepContext += 1.0f;
        }
        else
        {
            gpModelWaterManager->mSize = cur;
            gpModelWaterManager->mSphereStep = cur / 2.0f;
            PrevShineCount = CurrentShineCount;
            gInfo.Light.mSizeMorphing = false;
        }
        break;
    }
    case LightContext::FOLLOWPLAYER: {
        gpModelWaterManager->mDarkLevel = SMEFile::mStageConfig.Light.mDarkLevel;
        gShineShadowPos.x = gpMarioPos->x + gInfo.Light.mShineShadowCoordinates.x;
        gShineShadowPos.y = gpMarioPos->y + gInfo.Light.mShineShadowCoordinates.y;
        gShineShadowPos.z = gpMarioPos->z + gInfo.Light.mShineShadowCoordinates.z;
        break;
    }
    default:
        break;
    }
    return gInfo.Light.mLightType != LightContext::DISABLED && gpMarDirector->mAreaID != TGameSequence::OPTION;
}

//0x802571F0
/*
f32 velocityCoordinatePatches(f32 floorCoordinateY)
{
    TMario *gpMario = (TMario *)*(u32 *)TMarioInstance;

    if (gpMario->mState != TMario::State::IDLE)
    { //Y storage
        gpMario->mSpeed.y = 0;
    }

    if (floorCoordinateY < gpMario->mPosition.y - 20)
    { //Downwarping
        floorCoordinateY = gpMario->mPosition.y;
    }
    asm("lfs 0, -0x0EC8 (2)");
    return floorCoordinateY;
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
f32 downWarpPatch(TMario *gpMario, f32 yVelocity)
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
f32 upWarpPatch(TMario *gpMario, f32 yVelocity)
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

// MESSAGE MODIFICATIONS //

// 0x80153DE8, 0x80153E1C
// extern -> SME.cpp
void formatTalkMessage(Talk2D2 *talker, char *msgfield, u32 *entrydata)
{
    String *fmtMessage = new String(1024);

    const char *basemsg = msgfield + *entrydata + talker->curMsgIndex;
    const char *newmsg = fmtMessage->data() - (*entrydata + talker->curMsgIndex);

    fmtMessage->assign(basemsg);
    formatBMG(fmtMessage);

    setupTextBox__8TTalk2D2FPCvP12JMSMesgEntry(talker, newmsg, entrydata);

    delete fmtMessage;
}

static void maintainYoshi(TYoshi *yoshi)
{
    if (yoshi->isGreenYoshi())
    {
        *(f32 *)0x80415F4C = 480.0f; //tounge
        *(f32 *)0x80415F68 = 16384.0f;
    }
    else
    {
        *(f32 *)0x80415F4C = 300.0f;
        *(f32 *)0x80415F68 = 10000.0f;
    }
}

// 0x8024D3A8
// extern -> SME.cpp
void realTimeCustomAttrsHandler(TMario *player)
{
    if (player->mCustomInfo->isInitialized() && player->mCustomInfo->isMario())
        player->mCustomInfo->update();

    maintainYoshi(player->mYoshi);

    setPositions__6TMarioFv(player);
}