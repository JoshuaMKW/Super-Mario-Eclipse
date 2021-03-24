#include "stdlib.h"
#include "printf.h"
#include "sms/JSystem/JDrama.hxx"
#include "sms/JSystem/JKR/JKRDecomp.hxx"
#include "sms/JSystem/JKR/JKRHeap.hxx"
#include "sms/JSystem/JKR/JKRMemArchive.hxx"

#include "SME.hxx"

extern OSAlarm gctAlarm;
extern void setUserCodes(OSAlarm *alarm, OSContext *context);

#ifdef SME_DEBUG
    extern OSStopwatch gctStopwatch;
#endif

GlobalSMEDataStruct gInfo;
Memory::Protection::MemoryMap gCodeProtector;

namespace Init
{

}
// 0x80005328
// extern -> SME.cpp
void initCodeProtection()
{
    OSInit();
    gCodeProtector.setIndex(2);
    gCodeProtector.setStart(0x80003800);
    gCodeProtector.setEnd(0x80373000);
    gCodeProtector.setPermission(Memory::Protection::READ);
    gCodeProtector.activate();
}

// 0x802A750C
// extern -> SME.cpp
JKRExpHeap *createGlobalHeaps(void *newHeap, size_t size, JKRHeap *rootHeap, bool unk_1)
{
    gInfo.mCharacterHeap = JKRExpHeap::create(0x200000, JKRHeap::sRootHeap, false);
    gInfo.mGlobalsHeap = JKRExpHeap::create(0x10000, JKRHeap::sRootHeap, false);
    auto *gameHeap = JKRExpHeap::create(newHeap, size - 0x200000, rootHeap, unk_1);
    return gameHeap;
}

// 0x802A7140
// extern -> SME.cpp
u32 setupMarioDatas(char *filepath)
{
    TMarioGamePad *gpGamePad = gpApplication.mGamePad1;
    u32 id;

    switch (gpGamePad->mButtons.mInput)
    {
    case TMarioGamePad::Buttons::DPAD_UP:
        id = 1;
        break;
    case TMarioGamePad::Buttons::DPAD_DOWN:
        id = 2;
        break;
    case TMarioGamePad::Buttons::DPAD_LEFT:
        id = 3;
        break;
    case TMarioGamePad::Buttons::DPAD_RIGHT:
        id = 4;
        break;
    default:
        id = 0;
    }

    sprintf(filepath, "/data/chr%d.szs", id);
    return DVDConvertPathToEntrynum(filepath);
}

// 0x802A716C
// extern -> SME.cpp
u32 *initFirstModel(char *path, u32 unk_1, u32 unk_2, u32 unk_3, JKRHeap *heap, u32 unk_4, u32 unk_5, u32 unk_6)
{
    if (gInfo.mCharacterHeap)
    {
        heap = gInfo.mCharacterHeap;
        freeAll__10JKRExpHeapFv(gInfo.mCharacterHeap);
    }
    u32 *file = reinterpret_cast<u32 *>(SME::loadArchive(path, heap));

    if (file)
        Memory::hmalloc(gInfo.mCharacterHeap, (0x1F0000 - file[1]) & -32, 32);
    else
        Memory::hmalloc(gInfo.mCharacterHeap, 0x1F0000 & -32, 32);
        
    return file;
}

void resetGlobalValues()
{
    gModelWaterManagerWaterColor.set(0x3C, 0x46, 0x78, 0x14); //Water rgba
    gYoshiJuiceColor[0].set(0xFE, 0xA8, 0x02, 0x6E); //Yoshi Juice rgba
    gYoshiJuiceColor[1].set(0x9B, 0x01, 0xFD, 0x6E);
    gYoshiJuiceColor[2].set(0xFD, 0x62, 0xA7, 0x6E);
    gYoshiBodyColor[0].set(0x40, 0xA1, 0x24, 0xFF); //Yoshi rgba
    gYoshiBodyColor[1].set(0xFF, 0x8C, 0x1C, 0xFF);
    gYoshiBodyColor[2].set(0xAA, 0x4C, 0xFF, 0xFF);
    gYoshiBodyColor[3].set(0xFF, 0xA0, 0xBE, 0xFF);
    *(f32 *)0x80415CA8 = 0.25f; //Mario overall speed
    gAudioVolume = 0.75f;
    gAudioPitch = 1.0f;
    gAudioSpeed = 1.0f;
}

//0x802998B4
u32 *initFileMods()
{
    u32 *objList;
    SME_FROM_GPR(r26, objList);

    TMarioGamePad *gpGamePad = gpApplication.mGamePad1;

    #ifdef SME_DEBUG
        s8 characterID;

        switch (gpGamePad->mButtons.mInput)
        {
        case TMarioGamePad::Buttons::Z:
            characterID = 0;
            break;
        case TMarioGamePad::Buttons::DPAD_UP:
            characterID = 1;
            break;
        case TMarioGamePad::Buttons::DPAD_DOWN:
            characterID = 2;
            break;
        case TMarioGamePad::Buttons::DPAD_LEFT:
            characterID = 3;
            break;
        case TMarioGamePad::Buttons::DPAD_RIGHT:
            characterID = 4;
            break;
        default:
            characterID = -1;
        }
    #else
        s8 characterID = -1;
    #endif

    JKRMemArchive *marioVolumeData = reinterpret_cast<JKRMemArchive *>(JKRFileLoader::getVolume("mario"));
    u8 *params = marioVolumeData->getResource("/params.szs");


    char buffer[32];
    
    resetGlobalValues();
    SMEFile::mStageConfig.reset();

    if (SMEFile::mStageConfig.load(SME::getStageName(&gpApplication)))
    {
        SME_DEBUG_LOG("Success: SME config loaded at %X\n", SMEFile::mStageConfig);
        if (characterID < 0)
            characterID = SMEFile::mStageConfig.GlobalFlags.mPlayerID;
    }

    if (characterID >= 0)
    {
        //Attempt to swap character data
        sprintf(buffer, "/data/chr%d.szs", characterID);
        
        gInfo.mCharacterHeap->freeAll();

        u32 *marioData = reinterpret_cast<u32 *>(SME::loadArchive(buffer, gInfo.mCharacterHeap));
        *(u32 *)gpArcBufferMario = (u32)marioData;

        gInfo.mCharacterHeap->alloc((0x1F0000 - marioData[1]) & -32, 32);

        marioVolumeData->unmountFixed();
        marioVolumeData->mountFixed((void *)*(u32 *)gpArcBufferMario, JKRMemBreakFlag::UNK_0);
        //__dt__13JKRMemArchiveFv(marioVolumeData);
        //__ct__13JKRMemArchiveFPvUl15JKRMemBreakFlag(marioVolumeData, *(u32 *)gpArcBufferMario, 0, 0);
    }

    void *allocation;
    if (params)
    {
        u32 filesize = marioVolumeData->getResSize(params);
        CompressionType compressionState = JKRDecomp::checkCompressed(params);

        switch (compressionState)
        {
        case CompressionType::SZS:
        {
            *(u32 *)0x8040E260 = params[1];
            allocation = JKRHeap::alloc(params[1], 32, JKRHeap::sCurrentHeap);
            decompSZS_subroutine(params, reinterpret_cast<u8 *>(allocation));
            gInfo.mPRMFile = allocation;
        }
        case CompressionType::SZP:
            gInfo.mPRMFile = nullptr;
        case CompressionType::NONE:
        {
            allocation = JKRHeap::alloc(filesize, 32, JKRHeap::sCurrentHeap);
            memcpy(allocation, params, filesize);
            gInfo.mPRMFile = allocation;
        }
        }

        JKRMemArchive *oldParams = reinterpret_cast<JKRMemArchive *>(JKRFileLoader::getVolume("params"));

        oldParams->unmountFixed();
        oldParams->mountFixed(gInfo.mPRMFile, JKRMemBreakFlag::UNK_0);
    }

    return objList;
}

//0x80280180
void initShineShadow()
{
    if (SMEFile::mStageConfig.FileHeader.mMAGIC != SMEFile::MAGIC || !SMEFile::mStageConfig.GlobalFlags.mIsShineShadow)
        return;

    if (TFlagManager::smInstance->Type4Flag.mShineCount < SME_MAX_SHINES)
    {
        gInfo.Light.mLightType = SMEFile::mStageConfig.GlobalFlags.mShineShadowFlag;
        gInfo.Light.mShineShadowBase = SMEFile::mStageConfig.Light.mSize;
        gInfo.Light.mPrevShineCount = TFlagManager::smInstance->Type4Flag.mShineCount;

        gpModelWaterManager->mColor = SMEFile::mStageConfig.Light.mColor;
        gpModelWaterManager->mDarkLevel = SMEFile::mStageConfig.Light.mDarkLevel;
        gpModelWaterManager->mLayerCount = SMEFile::mStageConfig.Light.mLayerCount;
        gpModelWaterManager->mSphereStep = SMEFile::mStageConfig.Light.mStep;
        gpModelWaterManager->mSize = SMEFile::mStageConfig.Light.mSize;
        gpModelWaterManager->LightType.mMaskObjects = true;
        gpModelWaterManager->LightType.mShowShadow = true;

        if (SMEFile::mStageConfig.GlobalFlags.mShineShadowFlag == LightContext::FOLLOWPLAYER)
        {
            gInfo.Light.mNextSize = SMEFile::mStageConfig.Light.mSize;
            for (u32 i = 0; i < TFlagManager::smInstance->Type4Flag.mShineCount; ++i)
            {
                gInfo.Light.mNextSize += (10000 / SME_MAX_SHINES) + i * 2.0f;
            }
            gpModelWaterManager->mSize = gInfo.Light.mNextSize;
            gpModelWaterManager->mSphereStep = gpModelWaterManager->mSize / 2.0f;
        }
        gInfo.Light.mShineShadowCoordinates = SMEFile::mStageConfig.Light.mCoordinates;
    }
    else
    {
        gInfo.Light.mLightType = LightContext::DISABLED;
    }
}

//0x802B7A4C
void initSoundBank(u8 areaID, u8 episodeID)
{
    if (SMEFile::mStageConfig.FileHeader.mMAGIC != SMEFile::MAGIC || !SMEFile::mStageConfig.GlobalFlags.mIsMusic)
    {
        setMSoundEnterStage__10MSMainProcFUcUc(areaID, episodeID);
    }
    else
    {
        setMSoundEnterStage__10MSMainProcFUcUc(SMEFile::mStageConfig.Music.mAreaID, SMEFile::mStageConfig.Music.mEpisodeID);
    }
}

//0x802983F0
//0x80298420
void initMusicTrack()
{
    if (SMEFile::mStageConfig.Music.mPlayMusic)
    {
        gStageBGM = 0x80010000 | SMEFile::mStageConfig.Music.mMusicID;
        gAudioSpeed = SMEFile::mStageConfig.Music.mSpeed;
        gAudioPitch = SMEFile::mStageConfig.Music.mPitch;
        gAudioVolume = Max(Min(SMEFile::mStageConfig.Music.mVolume, 1), 0);
    }
    startStageBGM__10MSMainProcFUcUc();
}

void initFludd(TMario *player)
{
    MarioParamsFile *params = player->mCustomInfo->mParams;

    if (params)
    {
        gModelWaterManagerWaterColor = params->Attributes.FluddAttrs.mWaterColor;

        if (!params->Attributes.FluddAttrs.mCanUseNozzle[(u8)player->mFludd->mCurrentNozzle])
        {
            for (u8 i = 0; i < 8; ++i)
            {
                if (params->Attributes.FluddAttrs.mCanUseNozzle[i])
                {
                    player->mFludd->mCurrentNozzle = i;
                    player->mAttributes.mHasFludd = params->Attributes.mCanUseFludd;
                    player->mFludd->mCurrentWater = player->mFludd->mNozzleList[(u8)player->mFludd->mCurrentNozzle]->mMaxWater;
                    break;
                }
                else if (i == 7)
                {
                    player->mAttributes.mHasFludd = false;
                    params->Attributes.mCanUseFludd = false;
                }
            }
        }

        if (!params->Attributes.FluddAttrs.mCanUseNozzle[(u8)player->mFludd->mSecondNozzle])
        {
            for (u8 i = 0; i < 8; ++i)
            {
                if (params->Attributes.FluddAttrs.mCanUseNozzle[i])
                {
                    player->mFludd->mSecondNozzle = i;
                    player->mAttributes.mHasFludd = params->Attributes.mCanUseFludd;
                    break;
                }
                player->mFludd->mSecondNozzle = player->mFludd->mCurrentNozzle;
            }
        }
    }

    if (SMEFile::mStageConfig.FileHeader.mMAGIC == SMEFile::MAGIC && SMEFile::mStageConfig.GlobalFlags.mIsFludd)
    {
        if (SMEFile::mStageConfig.Fludd.mIsColorWater)
        {
            gModelWaterManagerWaterColor = SMEFile::mStageConfig.Fludd.mWaterColor;
        }
        player->mFludd->mCurrentNozzle = SMEFile::mStageConfig.Fludd.mPrimaryNozzle;
        player->mFludd->mSecondNozzle = SMEFile::mStageConfig.Fludd.mSecondaryNozzle;

        player->mFludd->mCurrentWater = player->mFludd->mNozzleList[(u8)player->mFludd->mCurrentNozzle]->mMaxWater;
    }
}

void initMario(TMario *player, bool isMario)
{
    player->mCustomInfo = (MarioParams *)Memory::malloc(sizeof(MarioParams), 32);
    player->mCustomInfo->mParams = nullptr;
    player->mCustomInfo->_mBaseParams = nullptr;

    if (SMEFile::mStageConfig.FileHeader.mMAGIC == SMEFile::MAGIC && SMEFile::mStageConfig.GlobalFlags.mIsMario)
    {
        player->mCustomInfo->mPlayerID.set(SMEFile::mStageConfig.GlobalFlags.mPlayerID;
        gInfo.PlayerData.mCurPlayerID[0] = SMEFile::mStageConfig.GlobalFlags.mPlayerID;

        player->mHealth = SMEFile::mStageConfig.Mario.mHealth;
        player->mDeParams.mTramplePowStep1.set(SMEFile::mStageConfig.Mario.mBaseBounce1);
        player->mDeParams.mTramplePowStep2.set(SMEFile::mStageConfig.Mario.mBaseBounce2);
        player->mDeParams.mTramplePowStep3.set(SMEFile::mStageConfig.Mario.mBaseBounce3);
        player->mDeParams.mDamageFallHeight.set(SMEFile::mStageConfig.Mario.mMaxFallNoDamage);
        player->mDeParams.mHPMax.set(SMEFile::mStageConfig.Mario.mMaxHealth);
        player->mDeParams.mIllegalPlaneCtInc.set(SMEFile::mStageConfig.Mario.mOBStep);
        player->mDeParams.mIllegalPlaneTime.set(SMEFile::mStageConfig.Mario.mOBMax);
        player->mJumpParams.mGravity.set(SMEFile::mStageConfig.Mario.mGravity);

        player->mAttributes.mGainHelmet.set(SMEFile::mStageConfig.GlobalFlags.MarioStates.mMarioHasHelmet;
        player->mAttributes.mHasFludd.set(SMEFile::mStageConfig.GlobalFlags.MarioStates.mMarioHasFludd;
        player->mAttributes.mIsShineShirt.set(SMEFile::mStageConfig.GlobalFlags.MarioStates.mMarioHasShirt;

        if (SMEFile::mStageConfig.GlobalFlags.MarioStates.mMarioHasGlasses)
            wearGlass__6TMarioFv(player);
    }


    player->mCustomInfo->init(player, isMario);

    if (!isMario) return;
    
    initFludd(player);
}


//0x80276C94
TMario *fromMarioInit(TMario *player)
{
    TMario *(*virtualFunc)(TMario *);
    SME_FROM_GPR(r12, virtualFunc);

    virtualFunc(player); // call vtable func
    initMario(player, true);
    return player;
}

//0x800397DC
bool fromShadowMarioInit()
{
    TMario *player;
    asm volatile ( "lwz %0, 0x150 (r31)" : "=r"(player));
    initMario(player, false);
    return SMS_isMultiPlayerMap__Fv();
}


//0x80271580
void initYoshi(void *anmSound, void *r4, u32 r5, f32 f1)
{
    initAnmSound__9MAnmSoundFPvUlf(anmSound, r4, r5, f1);

    TYoshi *yoshi;
    SME_FROM_GPR(r31, yoshi);

    if (SMEFile::mStageConfig.FileHeader.mMAGIC != SMEFile::MAGIC || !SMEFile::mStageConfig.GlobalFlags.mIsYoshi)
        return;

    gYoshiJuiceColor[0] = SMEFile::mStageConfig.Yoshi.mOrangeYoshi;
    gYoshiJuiceColor[1] = SMEFile::mStageConfig.Yoshi.mPurpleYoshi;
    gYoshiJuiceColor[2] = SMEFile::mStageConfig.Yoshi.mPinkYoshi;

    gYoshiBodyColor[0] = SMEFile::mStageConfig.Yoshi.mGreenYoshi;
    gYoshiBodyColor[1] = SMEFile::mStageConfig.Yoshi.mOrangeYoshi;
    gYoshiBodyColor[2] = SMEFile::mStageConfig.Yoshi.mPurpleYoshi;
    gYoshiBodyColor[3] = SMEFile::mStageConfig.Yoshi.mPinkYoshi;

    yoshi->mMaxJuice = SMEFile::mStageConfig.Yoshi.mMaxJuice;
    yoshi->mMaxVSpdStartFlutter = SMEFile::mStageConfig.Yoshi.mMaxVSpdStartFlutter;
    yoshi->mFlutterAcceleration = SMEFile::mStageConfig.Yoshi.mFlutterAcceleration;
    yoshi->mMaxFlutterTimer = SMEFile::mStageConfig.Yoshi.mMaxFlutterTimer;
}

//0x8029CCB0
void initCardColors()
{
    if (SMEFile::mStageConfig.FileHeader.mMAGIC == SMEFile::MAGIC && SMEFile::mStageConfig.GlobalFlags.mIsYoshi)
    {
        gpMarDirector->mGCConsole->mJuiceCardOrange = SMEFile::mStageConfig.Yoshi.mOrangeYoshi;
        gpMarDirector->mGCConsole->mJuiceCardPurple = SMEFile::mStageConfig.Yoshi.mPurpleYoshi;
        gpMarDirector->mGCConsole->mJuiceCardPink = SMEFile::mStageConfig.Yoshi.mPinkYoshi;
    }

    if (SMEFile::mStageConfig.FileHeader.mMAGIC == SMEFile::MAGIC && SMEFile::mStageConfig.GlobalFlags.mIsFludd && SMEFile::mStageConfig.Fludd.mIsColorWater)
    {
        gpMarDirector->mGCConsole->mWaterLeftPanel = SMEFile::mStageConfig.Fludd.mWaterColor;
        gpMarDirector->mGCConsole->mWaterRightPanel.R = lerp<u8>(0, SMEFile::mStageConfig.Fludd.mWaterColor.R, 0.8125);
        gpMarDirector->mGCConsole->mWaterRightPanel.G = lerp<u8>(0, SMEFile::mStageConfig.Fludd.mWaterColor.G, 0.8125);
        gpMarDirector->mGCConsole->mWaterRightPanel.B = lerp<u8>(0, SMEFile::mStageConfig.Fludd.mWaterColor.B, 0.8125);
        gpMarDirector->mGCConsole->mWaterRightPanel.A = lerp<u8>(0, SMEFile::mStageConfig.Fludd.mWaterColor.A, 0.8125);
    }
}

/*This works by taking the target id and matching it to the
/ ID of the first entry to have the same home ID as the target.
/
/ This makes a half decent linking system for the collision
/ triangles for functionalities like linked warping!
*/

static void parseWarpLinks(TMapCollisionData *col, WarpCollisionList *links, u32 validID, u32 idGroupSize = 0)
{
    u32 curDataIndex = 0;

    for (u32 i = 0; i < col->mFloorArraySize; ++i)
    {
        if (((col->mColTable[i].mCollisionType & 0x7FFF) - validID) <= idGroupSize ||
            ((col->mColTable[i].mCollisionType & 0x7FFF) - validID) <= idGroupSize + 1000)
        {

            links->mColList[curDataIndex] = CollisionLink((TBGCheckData *)&col->mColTable[i],
                                                          (u8)(col->mColTable[i].mValue4 >> 8),
                                                          (u8)col->mColTable[i].mValue4);
            if (curDataIndex >= 0xFF) break;
            ++curDataIndex;
        }
    }
    links->arrayLength = curDataIndex;
}

//0x802B8B20
u32 initCollisionWarpLinks(const char *name)
{
    WarpCollisionList *warpDataArray = (WarpCollisionList *)Memory::malloc(sizeof(WarpCollisionList), 32);
    WarpCollisionList *warpDataPreserveArray = (WarpCollisionList *)Memory::malloc(sizeof(WarpCollisionList), 32);
    gInfo.mWarpColArray = warpDataArray;
    gInfo.mWarpColPreserveArray = warpDataPreserveArray;

    if (warpDataArray)
    {
        parseWarpLinks(gpMapCollisionData, warpDataArray, 16040);
        parseWarpLinks(gpMapCollisionData, warpDataPreserveArray, 16041, 1);
    }

    return JDrama::TNameRef::calcKeyCode(name);
}

//0x802B57E4
void createUIHeap(u32 size, s32 alignment)
{
    gInfo.mGame6Heap = JKRExpHeap::create(size, JKRHeap::sSystemHeap, false);
    gpMarDirector->mGame6Data = (u32 *)Memory::malloc(size, alignment);
}

//0x802A72A4
u32 initHUDElements(char *filepath)
{
    char buffer[32];
    s32 entrynum;

    sprintf(buffer, filepath, TFlagManager::smInstance->Type6Flag.CustomFlags.mHUDElement);
    entrynum = DVDConvertPathToEntrynum(buffer);

    if (entrynum < 0)
        sprintf(filepath, filepath, 0);
    else
        strcpy(filepath, buffer);

    return DVDConvertPathToEntrynum(filepath);
}
//kmCall(0x802A72A4, &initHUDElements);

static JKRMemArchive *switchArchive(char *curArchive, void *newArchive)
{
    auto *volume = reinterpret_cast<JKRMemArchive *>(JKRFileLoader::getVolume(curArchive));
    volume->unmountFixed();
    volume->mountFixed(newArchive, JKRMemBreakFlag::UNK_0);
    return volume;
}

//0x80172440
JKRMemArchive *switchHUDOnStageLoad(char *curArchive, u32 *gameUI)
{
    char buffer[32];

    if (gpApplication.mGamePad1->mButtons.mInput & TMarioGamePad::Buttons::DPAD_UP)
        TFlagManager::smInstance->Type6Flag.CustomFlags.mHUDElement = 1;
    else if (gpApplication.mGamePad1->mButtons.mInput & TMarioGamePad::Buttons::DPAD_DOWN)
        TFlagManager::smInstance->Type6Flag.CustomFlags.mHUDElement = 0;

    sprintf(buffer, "/data/game_%d.arc", TFlagManager::smInstance->Type6Flag.CustomFlags.mHUDElement); //"/data/game_%d.arc"
    strcpy(strstr(buffer, ".arc"), ".szs");                        //".arc", ".szs"

    if (DVDConvertPathToEntrynum(buffer) >= 0)
    {
        Memory::free(gpMarDirector->mGame6Data);
        *gameUI = loadToMainRAM__12JKRDvdRipperFPCcPUc15JKRExpandSwitchUlP7JKRHeapQ212JKRDvdRipper15EAllocDirectionUlPi(buffer, 0, 1, 0, gpMarDirector->mGame6Data, 1, 0, 0);
    }

    return switchArchive(curArchive, gameUI);
}
//kmCall(0x80172440, &switchHUDOnStageLoad);

//0x802B57E4
JKRHeap *useCustomHUDHeap(u32 size, s32 alignment)
{
    return gInfo.mGame6Heap;
}
//kmCall(0x802B57E4, &useCustomHUDHeap);