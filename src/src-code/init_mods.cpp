#include "stdlib.h"
#include "printf.h"
#include "sms/JDrama.hxx"
#include "sms/JKR.hxx"
#include "sms/JUT.hxx"

#include "SME.hxx"

extern OSAlarm gctAlarm;
extern void setUserCodes(OSAlarm *alarm, OSContext *context);

#ifdef SME_DEBUG
    extern OSStopwatch gctStopwatch;
#endif

GlobalSMEDataStruct gInfo;
Memory::Protection::MemoryMap gCodeProtector;

void initCodeProtection()
{
    OSInit();
    gCodeProtector.setIndex(2);
    gCodeProtector.setStart(0x80003800);
    gCodeProtector.setEnd(0x80373000);
    gCodeProtector.setPermission(Memory::Protection::READ);
    gCodeProtector.activate();
}
kmCall(0x80005328, &initCodeProtection);

//0x8000561C
void printInfo(TApplication * gpApplication)
{
    #ifdef SME_DEBUG
        OSReport("============================\n"
                 "Super Mario Eclipse %s [DEBUG]\n"
                 "    Shines:     %d\n"
                 "    Build Date: %s %s\n"
                 "============================\n", __VERSION__, SME_MAX_SHINES, __DATE__, __TIME__);
        OSReport("Codeblocker - Creating OSAlarm at %p; Calls %p every %0.4f seconds\n", &gctAlarm, &setUserCodes, 0.001f);
        //OSReport("Actor flags offset = %X\n", offsetof(JDrama::TNameRef, mKeyCode));
        //OSReport("Actor position offset = %X\n", offsetof(JDrama::TPlacement, mPosition));
        OSReport("Actor size offset = %X\n", offsetof(JDrama::TActor, mSize));
        OSInitStopwatch(&gctStopwatch, "Codeblocker");
    #else
        OSReport("============================\n"
                 "Super Mario Eclipse %s [RELEASE]\n"
                 "    Shines:     %d\n"
                 "    Build Date: %s %s\n"
                 "============================\n", __VERSION__, SME_MAX_SHINES, __DATE__, __TIME__);
    #endif

    OSCreateAlarm(&gctAlarm);
    OSSetPeriodicAlarm(&gctAlarm, OSGetTime(), OSMillisecondsToTicks(1), &setUserCodes);

    gpApplication->initialize();
}
kmCall(0x8000561C, &printInfo);

//0x802A750C
static u32 *createGlobalHeaps(u32 *newHeap, u32 size, u32 *rootHeap, u32 unk_1)
{
    gInfo.mCharacterHeap = (JKRHeap *)create__10JKRExpHeapFUlP7JKRHeapb(0x200000, JKRHeap::sRootHeap, false);
    gInfo.mGlobalsHeap = (JKRHeap *)create__10JKRExpHeapFUlP7JKRHeapb(0x10000, JKRHeap::sRootHeap, false);
    u32 *gameHeap = (u32 *)create__10JKRExpHeapFPvUlP7JKRHeapb(newHeap, size - 0x200000, rootHeap, unk_1);
    return (u32 *)gameHeap;
}
kmCall(0x802A750C, &createGlobalHeaps);

//0x802A7140
u32 setupMarioDatas(char *filepath)
{
    TMarioGamePad *gpGamePad = gpApplication.mGamePad1;
    u32 id;

    switch (gpGamePad->getInput())
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
kmCall(0x802A7140, &setupMarioDatas);

//0x802A716C
u32 *initFirstModel(char *path, u32 unk_1, u32 unk_2, u32 unk_3, JKRHeap *heap, u32 unk_4, u32 unk_5, u32 unk_6)
{
    if (gInfo.mCharacterHeap)
    {
        heap = gInfo.mCharacterHeap;
        freeAll__10JKRExpHeapFv(gInfo.mCharacterHeap);
    }
    u32 *file = loadArchive(path, heap);

    if (file)
        Memory::hmalloc(gInfo.mCharacterHeap, (0x1F0000 - file[1]) & -32, 32);
    else
        Memory::hmalloc(gInfo.mCharacterHeap, 0x1F0000 & -32, 32);
        
    return file;
}
kmCall(0x802A716C, &initFirstModel);

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
    asm volatile ( "mr %0, r26" : "=r"(objList));

    TMarioGamePad *gpGamePad = gpApplication.mGamePad1;

    #ifdef SME_DEBUG
        s8 characterID;

        switch (gpGamePad->getInput())
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

    JKRMemArchive *marioVolumeData = (JKRMemArchive *)getVolume__13JKRFileLoaderFPCc("mario");
    u32 *params = (u32 *)getResource__10JKRArchiveFPCc(marioVolumeData, "/params.szs");

    u32 *marioDataField;
    u32 *marioData;
    u32 *allocation;

    char buffer[32];
    
    resetGlobalValues();
    SMEFile::mStageConfig.reset();

    bool stageConfigLoaded = SMEFile::mStageConfig.load(getStageName(&gpApplication));
    if (stageConfigLoaded)
    {
        OSReport("Success: SME config loaded at %X\n", SMEFile::mStageConfig);
        if (characterID < 0)
            characterID = SMEFile::mStageConfig.GlobalFlags.mPlayerID;
    }

    if (characterID >= 0)
    {
        //Attempt to swap character data
        sprintf(buffer, "/data/chr%d.szs", characterID);
        
        freeAll__10JKRExpHeapFv(gInfo.mCharacterHeap);

        marioData = loadArchive(buffer, gInfo.mCharacterHeap);
        *(u32 *)gpArcBufferMario = (u32)marioData;

        Memory::hmalloc(gInfo.mCharacterHeap, (0x1F0000 - marioData[1]) & -32, 32);

        __dt__13JKRMemArchiveFv(marioVolumeData);
        __ct__13JKRMemArchiveFPvUl15JKRMemBreakFlag(marioVolumeData, *(u32 *)gpArcBufferMario, 0, 0);
    }

    if (params)
    {
        u32 filesize = getResSize__10JKRArchiveCFPCv(marioVolumeData, params);
        u32 compressionState = checkCompressed__9JKRDecompFPUc(params);

        if (compressionState == 2)
        {
            *(u32 *)0x8040E260 = params[0x4 / 4];
            allocation = (u32 *)alloc__7JKRHeapFUliP7JKRHeap(params[0x4 / 4], 32, *JKRHeap::sCurrentHeap);
            decompSZS_subroutine__FPUcPUc(params, allocation);
            gInfo.mPRMFile = allocation;
        }
        else if (compressionState == 1)
        {
            gInfo.mPRMFile = nullptr;
        }
        else
        {
            allocation = (u32 *)alloc__7JKRHeapFUliP7JKRHeap(filesize, 32, *JKRHeap::sCurrentHeap);
            memcpy(allocation, params, filesize);
            gInfo.mPRMFile = allocation;
        }

        JKRMemArchive *oldParams = (JKRMemArchive *)getVolume__13JKRFileLoaderFPCc("params");

        unmountFixed__13JKRMemArchiveFv(oldParams);
        mountFixed__13JKRMemArchiveFPv15JKRMemBreakFlag(oldParams, gInfo.mPRMFile, 0);
    }

    return objList;
}
kmCall(0x802998B4, &initFileMods);

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

        if (SMEFile::mStageConfig.GlobalFlags.mShineShadowFlag == 1)
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
        gInfo.Light.mLightType = 0;
    }
}
kmBranch(0x80280180, &initShineShadow);

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
kmCall(0x802B7A4C, &initSoundBank);

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
kmCall(0x802983F0, &initMusicTrack);
kmCall(0x80298420, &initMusicTrack);

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
        player->mCustomInfo->mPlayerID = SMEFile::mStageConfig.GlobalFlags.mPlayerID;
        gInfo.PlayerData.mCurPlayerID[0] = SMEFile::mStageConfig.GlobalFlags.mPlayerID;

        player->mGravity = SMEFile::mStageConfig.Mario.mGravity;
        player->mBaseBounceSpeed1 = SMEFile::mStageConfig.Mario.mBaseBounce1;
        player->mBaseBounceSpeed2 = SMEFile::mStageConfig.Mario.mBaseBounce2;
        player->mBaseBounceSpeed3 = SMEFile::mStageConfig.Mario.mBaseBounce3;
        player->mMaxFallNoDamage = SMEFile::mStageConfig.Mario.mMaxFallNoDamage;
        player->mHealth = SMEFile::mStageConfig.Mario.mHealth;
        player->mMaxHealth = SMEFile::mStageConfig.Mario.mMaxHealth;
        player->mOBStep = SMEFile::mStageConfig.Mario.mOBStep;
        player->mOBMax = SMEFile::mStageConfig.Mario.mOBMax;

        player->mAttributes.mGainHelmet = SMEFile::mStageConfig.GlobalFlags.MarioStates.mMarioHasHelmet;
        player->mAttributes.mHasFludd = SMEFile::mStageConfig.GlobalFlags.MarioStates.mMarioHasFludd;
        player->mAttributes.mIsShineShirt = SMEFile::mStageConfig.GlobalFlags.MarioStates.mMarioHasShirt;

        if (SMEFile::mStageConfig.GlobalFlags.MarioStates.mMarioHasGlasses)
        {
            wearGlass__6TMarioFv(player);
        }
    }


    player->mCustomInfo->init(player, isMario);

    if (!isMario) return;
    
    initFludd(player);
}


//0x80276C94
TMario *fromMarioInit(TMario *player)
{
    TMario *(*virtualFunc)(TMario *);
    asm volatile ( "mr %0, r12" : "=r"(virtualFunc));

    virtualFunc(player); // call vtable func
    initMario(player, true);
    return player;
}
kmWrite32(0x80276C90, 0x60000000);
kmCall(0x80276C94, &fromMarioInit);

//0x800397DC
bool fromShadowMarioInit()
{
    TMario *player;
    asm volatile ( "lwz %0, 0x150 (r31)" : "=r"(player));
    initMario(player, false);
    return SMS_isMultiPlayerMap__Fv();
}
kmCall(0x800397DC, &fromShadowMarioInit);


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
kmCall(0x80271580, &initYoshi);

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
kmBranch(0x8029CCB0, &initCardColors);

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
u32 initCollisionWarpLinks(char *name, u32 *dest)
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

    return calcKeyCode__Q26JDrama8TNameRefFPCc(name, dest);
}
kmCall(0x802B8B20, &initCollisionWarpLinks);

//0x802B57E4
void createUIHeap(u32 size, s32 alignment)
{
    gInfo.mGame6Heap = (JKRHeap *)__nw__FUlP7JKRHeapi(size, JKRHeap::sSystemHeap, 32);
    gpMarDirector->mGame6Data = (u32 *)__nw__FUli(size, alignment);
}
kmCall(0x802B57E4, &createUIHeap);

//0x802A72A4
u32 initHUDElements(char *filepath)
{
    char buffer[32];
    s32 entrynum;

    sprintf(buffer, filepath, TFlagManager::smInstance->Type6Flag.CustomFlags.mHUDElement);
    entrynum = DVDConvertPathToEntrynum(buffer);

    if (entrynum < 0)
    {
        sprintf(filepath, filepath, 0);
    }
    else
    {
        strcpy(filepath, buffer);
    }

    return DVDConvertPathToEntrynum(filepath);
}
//kmCall(0x802A72A4, &initHUDElements);

static u32 *switchArchive(char *curArchive, void *newArchive)
{
    u32 *volume = (u32 *)getVolume__13JKRFileLoaderFPCc(curArchive);
    unmountFixed__13JKRMemArchiveFv(volume);
    mountFixed__13JKRMemArchiveFPv15JKRMemBreakFlag(volume, newArchive);
    return volume;
}

//0x80172440
u32 *switchHUDOnStageLoad(char *curArchive, u32 *gameUI)
{
    char buffer[32];

    if (gpApplication.mGamePad1->isPressed(TMarioGamePad::Buttons::DPAD_UP))
        TFlagManager::smInstance->Type6Flag.CustomFlags.mHUDElement = 1;
    else if (gpApplication.mGamePad1->isPressed(TMarioGamePad::Buttons::DPAD_DOWN))
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


/* PATCHES */

//Restore Chao Seed
kmWrite32(0x802FD1A0, 0x808D8C70);

//Show Exception Handler
kmWrite32(0x8029D0BC, 0x60000000);

#ifdef SME_DEBUG
//Skip FMVs
kmWrite32(0x802B5E8C, 0x38600001);
kmWrite32(0x802B5EF4, 0x38600001);
#endif

#ifdef SME_DEBUG
//Level Select
kmWrite32(0x802A6788, 0x3BC00009);
#endif

//Fix Infinte Flutter
kmWrite32(0x8028113C, 0xC002F824);

//Remove Dive While Wall Sliding
kmWrite32(0x8024BC10, 0x48000068);

//Flood Till Corona Beat
kmWrite32(0x8029961C, 0x38840077);

//Map on D Pad down
kmWrite32(0x8017A830, 0x5400077B);
kmWrite32(0x80297A60, 0x5400077B);

//Hover move on roofs
kmWrite32(0x802569BC, 0xC02300B0);

//Global surfing bloopies
kmWrite32(0x801B74F8, 0x60000000);
kmWrite32(0x801B74FC, 0x60000000);
kmWrite32(0x801B7500, 0x60000000);
kmWrite32(0x801B7504, 0x60000000);
kmWrite32(0x801B7508, 0x60000000);
kmWrite32(0x801B750C, 0x60000000);
kmWrite32(0x801B7510, 0x387E0780);
kmWrite32(0x801B7514, 0x4810BA9D);
kmWrite32(0x801B7518, 0x28030000);
kmWrite32(0x801B751C, 0x418200A4);

