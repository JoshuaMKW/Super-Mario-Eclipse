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

    if (gpMarioAddress->mController->isFramePressed(TMarioGamePad::DPAD_RIGHT) && !inXYZMode)
    {
        inXYZMode = true;
    }
    else if (gpMarioAddress->mController->isFramePressed(TMarioGamePad::DPAD_RIGHT) && inXYZMode)
    {
        gpMarioAddress->mState = TMario::SA_IDLE;
        inXYZMode = false;
    }

    if (inXYZMode)
    {
        gpMarioAddress->mState = TMario::SA_IDLE | TMario::SA_CUTSCENE;
        f32 speedMultiplier = lerp<f32>(1, 2, gpMarioAddress->mController->mButtons.mAnalogR);
        gpMarioAddress->mPosition.x += ((gpMarioAddress->mController->mControlStick.mStickX * 83) * speedMultiplier);
        gpMarioAddress->mPosition.z -= ((gpMarioAddress->mController->mControlStick.mStickY * 83) * speedMultiplier);

        if (gpMarioAddress->mController->isPressed(TMarioGamePad::DPAD_DOWN))
        {
            gpMarioAddress->mPosition.y -= (83 * speedMultiplier);
        }
        else if (gpMarioAddress->mController->isPressed(TMarioGamePad::DPAD_UP))
        {
            gpMarioAddress->mPosition.y += (83 * speedMultiplier);
        }
    }
    return;
}

// this is ran once
static void onSetup(TMarDirector* director)
{
    gDebugTextBox = new (gInfo.mGlobalsHeap, 4) J2DTextBox(gpSystemFont->mFont, "Debug Mode");

	// run replaced call
	director->setupObjects();
}
//kmCall(0x802998B8, &onSetup);

// this is ran every frame
static s32 onUpdate(TMarDirector* director)
{
    xyzModifierMario();
    //drawCheatText(); //currently bugged

    // run replaced call
	return director->direct();
}
//kmCall(0x802A616C, &onUpdate);

// this is ran when drawing is needed
static void draw2D(J2DOrthoGraph *graph)
{
	// run replaced call
	graph->setup2D();
}
kmCall(0x80143F14, &draw2D);

//0x802A8B58
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
kmBranch(0x802A8B58, &SMS_IsExMap);

//0x802A8B30
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
kmBranch(0x802A8B30, &SMS_IsMultiplayerMap);

//0x802A8AFC
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
kmBranch(0x802A8AFC, &SMS_IsDivingMap);

//0x802A8AE0
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
kmBranch(0x802A8AE0, &SMS_IsOptionMap);

//0x8027C6A4
bool manageLightSize()
{
    if (SMEFile::mStageConfig.FileHeader.mMAGIC != SMEFile::MAGIC || !SMEFile::mStageConfig.GlobalFlags.mIsShineShadow)
    {
        return (gpMarDirector->mAreaID == 1);
    }

    if (gInfo.Light.mLightType == LightContext::STATIC)
    {
        if (SMEFile::mStageConfig.Light.mDarkLevel != 255)
        {
            gpModelWaterManager->mDarkLevel = SMEFile::mStageConfig.Light.mDarkLevel;
        }
        else if (TFlagManager::smInstance->Type4Flag.mShineCount < SME_MAX_SHINES)
        {
            gpModelWaterManager->mDarkLevel = lerp<u8>(30, 190, (f32)TFlagManager::smInstance->Type4Flag.mShineCount / (f32)SME_MAX_SHINES);
        }
        else
        {
            if (gpModelWaterManager->mDarkLevel < 255)
            {
                gpModelWaterManager->mDarkLevel += 1;
            }
            else
            {
                gInfo.Light.mLightType = LightContext::DISABLED;
            }
        }

        gShineShadowPos = gInfo.Light.mShineShadowCoordinates;

        f32 sigOfs;
        f32 sigStrength;
        f32 next;
        f32 prev;
        f32 cur;

        sigOfs = 300.0f;
        if (TFlagManager::smInstance->Type4Flag.mShineCount > gInfo.Light.mPrevShineCount)
        {
            sigStrength = 0.04f;
        }
        else if (TFlagManager::smInstance->Type4Flag.mShineCount < gInfo.Light.mPrevShineCount)
        {
            sigStrength = -0.04f;
        }

        if (!gInfo.Light.mSizeMorphing && TFlagManager::smInstance->Type4Flag.mShineCount > gInfo.Light.mPrevShineCount)
        {
            gInfo.Light.mPrevSize = gpModelWaterManager->mSize;
            gInfo.Light.mNextSize = gpModelWaterManager->mSize;

            for (u32 i = 0; i < (TFlagManager::smInstance->Type4Flag.mShineCount - gInfo.Light.mPrevShineCount); ++i)
            {
                gInfo.Light.mNextSize += (10000.0f / SME_MAX_SHINES) + (gInfo.Light.mPrevShineCount + i) * 2.0f;
            }

            gInfo.Light.mSizeMorphing = true;
            gInfo.Light.mStepContext = 0.0f;
        }
        else if (!gInfo.Light.mSizeMorphing && TFlagManager::smInstance->Type4Flag.mShineCount < gInfo.Light.mPrevShineCount)
        {
            gInfo.Light.mPrevSize = gpModelWaterManager->mSize;
            gInfo.Light.mNextSize = gpModelWaterManager->mSize;

            for (u32 i = 0; i < (gInfo.Light.mPrevShineCount - TFlagManager::smInstance->Type4Flag.mShineCount); ++i)
            {
                gInfo.Light.mNextSize -= (10000.0f / SME_MAX_SHINES) + (gInfo.Light.mPrevShineCount - i) * 2.0f;
            }

            gInfo.Light.mSizeMorphing = true;
            gInfo.Light.mStepContext = 0.0f;
        }

        if (gInfo.Light.mSizeMorphing)
        {
            next = gInfo.Light.mNextSize;
            prev = gInfo.Light.mPrevSize;
            cur = sigmoidCurve(gInfo.Light.mStepContext, prev, next, sigOfs, sigStrength);

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
                gInfo.Light.mPrevShineCount = TFlagManager::smInstance->Type4Flag.mShineCount;
                gInfo.Light.mSizeMorphing = false;
            }
        }
    }
    else if (gInfo.Light.mLightType == LightContext::FOLLOWPLAYER)
    {
        gpModelWaterManager->mDarkLevel = SMEFile::mStageConfig.Light.mDarkLevel;
        gShineShadowPos.x = gpMarioPos->x + gInfo.Light.mShineShadowCoordinates.x;
        gShineShadowPos.y = gpMarioPos->y + gInfo.Light.mShineShadowCoordinates.y;
        gShineShadowPos.z = gpMarioPos->z + gInfo.Light.mShineShadowCoordinates.z;
    }
    return gInfo.Light.mLightType != LightContext::DISABLED && gpMarDirector->mAreaID != TGameSequence::OPTION;
}
kmCall(0x8027C6A4, &manageLightSize);
kmWrite32(0x8027C6A8, 0x28030001);

//0x802571F0
/*
f32 velocityCoordinatePatches(f32 floorCoordinateY)
{
    TMario *gpMario = (TMario *)*(u32 *)TMarioInstance;

    if (gpMario->mState != TMario::SA_IDLE)
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

//MESSAGE MODIFICATIONS

//0x80153DE8, 0x80153E1C
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
kmCall(0x80153DE8, &formatTalkMessage);
kmCall(0x80153E1C, &formatTalkMessage);

void maintainYoshi(TYoshi *yoshi)
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

void realTimeCustomAttrsHandler(TMario *player)
{
    if (player->mCustomInfo->isInitialized() && player->mCustomInfo->isMario())
        player->mCustomInfo->update();

    maintainYoshi(player->mYoshi);

    setPositions__6TMarioFv(player);
}
kmCall(0x8024D3A8, &realTimeCustomAttrsHandler);