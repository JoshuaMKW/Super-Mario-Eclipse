#include "printf.h"
#include "sms/JKR.hxx"
#include "sms/JUT.hxx"

#include "SME.hxx"

extern void initializeMemProtection();

extern OSAlarm gctAlarm;
extern void setUserCodes(OSAlarm *alarm, OSContext *context);

#ifdef SME_DEBUG
    extern OSStopwatch gctStopwatch;
#endif

SMEInfoManager gInfo;
TCheatHandler gDebugModeHandler;

u16 gCheatCode[] = { TMarioGamePad::DPAD_UP,
                     TMarioGamePad::DPAD_UP, 
                     TMarioGamePad::DPAD_DOWN,
                     TMarioGamePad::DPAD_DOWN,
                     TMarioGamePad::DPAD_LEFT,
                     TMarioGamePad::DPAD_RIGHT,
                     TMarioGamePad::DPAD_LEFT,
                     TMarioGamePad::DPAD_RIGHT,
                     TMarioGamePad::B,
                     TMarioGamePad::A,
                     TMarioGamePad::START };

s8 gCodeInputIndex = 0;

//0x8000561C
void printInfo(TApplication * gpApplication)
{
    #ifdef SME_DEBUG
        OSReport("Super Mario Eclipse %s [DEBUG] - %d Shines; Build Date: %s %s\n", __VERSION__, MAX_SHINES, __DATE__, __TIME__);
        OSReport("Codeblocker - Creating OSAlarm at %p; Calls %p every %0.4f seconds\n", &gctAlarm, &setUserCodes, 0.001f);
        OSInitStopwatch(&gctStopwatch, "Codeblocker");
    #else
        OSReport("Super Mario Eclipse %s [RELEASE] - %d Shines; Build Date: %s %s\n", __VERSION__, MAX_SHINES, __DATE__, __TIME__);
    #endif

    OSCreateAlarm(&gctAlarm);
    OSSetPeriodicAlarm(&gctAlarm, OSGetTime(), OSMillisecondsToTicks(1), &setUserCodes);

    initializeMemProtection();
    initialize__12TApplicationFv(gpApplication);
}
kmCall(0x8000561C, &printInfo);

void debugModeNotify()
{
    
}

void *handleDebugCheat(void *GCLogoDir)
{
    if (!gDebugModeHandler.isInitialized())
    {
        gDebugModeHandler.setGamePad(gpApplication.mGamePad1);
        gDebugModeHandler.setInputList(gCheatCode);
        gDebugModeHandler.setSuccessCallBack(&debugModeNotify);
    }
    gDebugModeHandler.advanceInput();
    return GCLogoDir;
}
kmBranch(0x80295B6C, &handleDebugCheat);

//0x802A750C
u32 *createCharacterHeap(u32 *newHeap, u32 size, u32 *rootHeap, u32 unk_1)
{
    gInfo.mCharacterHeap = (JKRHeap *)create__10JKRExpHeapFUlP7JKRHeapb(0x200000, *sRootHeap, false);
    //strcpy((char *)0x803A4284, "/data/chr%d.arc");
    u32 *gameHeap = (u32 *)create__10JKRExpHeapFPvUlP7JKRHeapb(newHeap, size - 0x200000, rootHeap, unk_1);
    return (u32 *)gameHeap;
}
kmCall(0x802A750C, &createCharacterHeap);

//0x802A7140
u32 setupMarioDatas(char *filepath)
{
    TMarioGamePad *gpGamePad = gpApplication.mGamePad1;
    u32 id;

    switch (gpGamePad->getInput())
    {
    case TMarioGamePad::DPAD_UP:
        id = 1;
        break;
    case TMarioGamePad::DPAD_DOWN:
        id = 2;
        break;
    case TMarioGamePad::DPAD_LEFT:
        id = 3;
        break;
    case TMarioGamePad::DPAD_RIGHT:
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
    gFluddWaterColor = JUtility::TColor(0x3C, 0x46, 0x78, 0x14); //Water rgba
    gYoshiJuiceColor[0] = JUtility::TColor(0xFE, 0xA8, 0x02, 0x6E); //Yoshi Juice rgba
    gYoshiJuiceColor[1] = JUtility::TColor(0x9B, 0x01, 0xFD, 0x6E);
    gYoshiJuiceColor[2] = JUtility::TColor(0xFD, 0x62, 0xA7, 0x6E);
    gYoshiBodyColor[0] = JUtility::TColor(0x40, 0xA1, 0x24, 0xFF); //Yoshi rgba
    gYoshiBodyColor[1] = JUtility::TColor(0xFF, 0x8C, 0x1C, 0xFF);
    gYoshiBodyColor[2] = JUtility::TColor(0xAA, 0x4C, 0xFF, 0xFF);
    gYoshiBodyColor[3] = JUtility::TColor(0xFF, 0xA0, 0xBE, 0xFF);
    *(f32 *)0x80415CA8 = 0.25f; //Mario overall speed
    gAudioVolume = 0.75f;
    gAudioPitch = 1.0f;
    gAudioSpeed = 1.0f;
}

SMEFile *loadSMEConfig(const char *stage)
{
    char path[32];

    if (!stage) return NULL;

    SMEFile::parseExtension(path, "/data/scene/sme/", stage, false);

    OSReport("Attempting to load local config %s...\n", path);
    SMEFile *file = SMEFile::loadFile(path);

    if (file) return file;
    
    SMEFile::parseExtension(path, "/data/scene/sme/", stage, true);

    OSReport("Failure: Now attempting to load global config %s...\n", path);
    file = SMEFile::loadFile(path);
    
    return file;
}

//0x802998B4
u32 *initFileMods()
{
    register u32 *objList;
    __asm { mr objList, r26 };

    TMarioGamePad *gpGamePad = gpApplication.mGamePad1;
    s8 characterID;

    switch (gpGamePad->getInput())
    {
    case TMarioGamePad::Z:
        characterID = 0;
        break;
    case TMarioGamePad::DPAD_UP:
        characterID = 1;
        break;
    case TMarioGamePad::DPAD_DOWN:
        characterID = 2;
        break;
    case TMarioGamePad::DPAD_LEFT:
        characterID = 3;
        break;
    case TMarioGamePad::DPAD_RIGHT:
        characterID = 4;
        break;
    default:
        characterID = -1;
    }

    JKRMemArchive *marioVolumeData = (JKRMemArchive *)getVolume__13JKRFileLoaderFPCc("mario");
    u32 *params = (u32 *)getResource__10JKRArchiveFPCc(marioVolumeData, "/params.szs");

    u32 *marioDataField;
    u32 *marioData;
    u32 *allocation;

    char buffer[32];

    Memory::free(gInfo.mFile); //Free the file allocation
    resetGlobalValues();

    gInfo.mFile = loadSMEConfig(getStageName(&gpApplication));
    if (gInfo.mFile)
    {
        OSReport("Success: SME config loaded at %X\n", gInfo.mFile);
        characterID = characterID < 0 ? gInfo.mFile->GlobalFlags.mPlayerID : characterID;
    }
    else
    {
        OSReport("Failure: No SME configuration could be loaded\n", gInfo.mFile);
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
            allocation = (u32 *)alloc__7JKRHeapFUliP7JKRHeap(params[0x4 / 4], 32, *sCurrentHeap);
            decompSZS_subroutine__FPUcPUc(params, allocation);
            gInfo.mPRMFile = allocation;
        }
        else if (compressionState == 1)
        {
            gInfo.mPRMFile = NULL;
        }
        else
        {
            allocation = (u32 *)alloc__7JKRHeapFUliP7JKRHeap(filesize, 32, *sCurrentHeap);
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
    SMEFile *file = gInfo.mFile;

    if (!file || !file->GlobalFlags.mIsShineShadow)
        return;

    if (gpFlagManager->Type4Flag.mShineCount < MAX_SHINES)
    {
        gInfo.Light.mLightType = file->GlobalFlags.mShineShadowFlag;
        gInfo.Light.mShineShadowBase = file->Light.mSize;
        gInfo.Light.mPrevShineCount = gpFlagManager->Type4Flag.mShineCount;

        gpModelWaterManager->mColor = file->Light.mColor;
        gpModelWaterManager->mDarkLevel = file->Light.mDarkLevel;
        gpModelWaterManager->mLayerCount = file->Light.mLayerCount;
        gpModelWaterManager->mSphereStep = file->Light.mStep;
        gpModelWaterManager->mSize = file->Light.mSize;
        gpModelWaterManager->LightType.mMaskObjects = true;
        gpModelWaterManager->LightType.mShowShadow = true;

        if (file->GlobalFlags.mShineShadowFlag == 1)
        {
            gInfo.Light.mNextSize = file->Light.mSize;
            for (u32 i = 0; i < gpFlagManager->Type4Flag.mShineCount; ++i)
            {
                gInfo.Light.mNextSize += (10000 / MAX_SHINES) + i * 16;
            }
            gpModelWaterManager->mSphereStep = gpModelWaterManager->mSize / 2;
        }
        gInfo.Light.mShineShadowCoordinates = file->Light.mCoordinates;
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
    SMEFile *file = gInfo.mFile;

    if (!file || !gInfo.mFile->GlobalFlags.mIsMusic)
    {
        setMSoundEnterStage__10MSMainProcFUcUc(areaID, episodeID);
    }
    else
    {
        setMSoundEnterStage__10MSMainProcFUcUc(gInfo.mFile->Music.mAreaID, gInfo.mFile->Music.mEpisodeID);
    }
}
kmCall(0x802B7A4C, &initSoundBank);

//0x802983F0
//0x80298420
void initMusicTrack()
{
    SMEFile *file = gInfo.mFile;

    if (!file || !gInfo.mFile->GlobalFlags.mIsMusic)
    {
        startStageBGM__10MSMainProcFUcUc();
    }
    else if (gInfo.mFile->Music.mPlayMusic)
    {
        gStageBGM = 0x80010000 | gInfo.mFile->Music.mMusicID;
        gAudioSpeed = gInfo.mFile->Music.mSpeed;
        gAudioPitch = gInfo.mFile->Music.mPitch;
        gAudioVolume = Max(Min(gInfo.mFile->Music.mVolume, 1), 0);

        startStageBGM__10MSMainProcFUcUc();
    }
}
kmCall(0x802983F0, &initMusicTrack);
kmCall(0x80298420, &initMusicTrack);

void initFludd(TMario *player)
{
    SMEFile *stagefile = gInfo.mFile;
    MarioParamsFile *params = player->mCustomInfo->mParams;

    if (params)
    {
        gFluddWaterColor = params->Attributes.FluddAttrs.mWaterColor;

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

    if (stagefile && stagefile->GlobalFlags.mIsFludd)
    {
        if (stagefile->Fludd.mIsColorWater)
        {
            gFluddWaterColor = stagefile->Fludd.mWaterColor;
        }
        player->mFludd->mCurrentNozzle = stagefile->Fludd.mPrimaryNozzle;
        player->mFludd->mSecondNozzle = stagefile->Fludd.mSecondaryNozzle;

        player->mFludd->mCurrentWater = player->mFludd->mNozzleList[(u8)player->mFludd->mCurrentNozzle]->mMaxWater;
    }
}

void initMario(TMario *player, bool isMario)
{
    SMEFile *file = gInfo.mFile;
    player->mCustomInfo = (MarioParams *)Memory::malloc(sizeof(MarioParams), 32);
    player->mCustomInfo->mParams = NULL;
    player->mCustomInfo->_mBaseParams = NULL;

    if (file && file->GlobalFlags.mIsMario)
    {
        player->mCustomInfo->mPlayerID = file->GlobalFlags.mPlayerID;
        gInfo.PlayerData.mCurPlayerID[0] = file->GlobalFlags.mPlayerID;

        player->mGravity = file->Mario.mGravity;
        player->mBaseBounceSpeed1 = file->Mario.mBaseBounce1;
        player->mBaseBounceSpeed2 = file->Mario.mBaseBounce2;
        player->mBaseBounceSpeed3 = file->Mario.mBaseBounce3;
        player->mMaxFallNoDamage = file->Mario.mMaxFallNoDamage;
        player->mHealth = file->Mario.mHealth;
        player->mMaxHealth = file->Mario.mMaxHealth;
        player->mOBStep = file->Mario.mOBStep;
        player->mOBMax = file->Mario.mOBMax;

        player->mAttributes.mGainHelmet = file->GlobalFlags.MarioStates.mMarioHasHelmet;
        player->mAttributes.mHasFludd = file->GlobalFlags.MarioStates.mMarioHasFludd;
        player->mAttributes.mIsShineShirt = file->GlobalFlags.MarioStates.mMarioHasShirt;

        if (file->GlobalFlags.MarioStates.mMarioHasGlasses)
        {
            wearGlass__6TMarioFv(player);
        }
    }

    if (!isMario) return;

    player->mCustomInfo->_mBaseParams = (MarioParamsFile *)getResource__10JKRArchiveFPCc(getVolume__13JKRFileLoaderFPCc("mario"), "/params.bin");

    if (player->mCustomInfo->_mBaseParams)
    {
        player->mCustomInfo->_mBaseParams->initializeMario(player);
    }
    initFludd(player);
}


//0x80276AC0 - Mario
/*
mr r3, r30
__set r4, TRUE
__call r12, 0x800050D0
lwz r0, 0x44 (sp)
*/

//0x80276C94
TMario *fromMarioInit(TMario *player)
{
    register void *virtualFunc;
    __asm { mr virtualFunc, r12 };

    ((TMario *(*)(TMario *player))virtualFunc)(player); // call vtable func
    initMario(player, true);
    return player;
}
kmWrite32(0x80276C90, 0x60000000);
kmCall(0x80276C94, &fromMarioInit);

//0x800397DC
bool fromShadowMarioInit()
{
    register TMario *player;
    __asm { lwz player, 0x150 (r31) };
    initMario(player, false);
    return SMS_isMultiPlayerMap__Fv();
}
kmCall(0x800397DC, &fromShadowMarioInit);


//0x80271580
void initYoshi(void *anmSound, void *r4, u32 r5, f32 f1)
{
    initAnmSound__9MAnmSoundFPvUlf(anmSound, r4, r5, f1);

    register TYoshi *yoshi;
    __asm { mr yoshi, r31 };

    SMEFile *file = gInfo.mFile;

    if (!file || !file->GlobalFlags.mIsYoshi)
        return;

    gYoshiJuiceColor[0] = file->Yoshi.mOrangeYoshi;
    gYoshiJuiceColor[1] = file->Yoshi.mPurpleYoshi;
    gYoshiJuiceColor[2] = file->Yoshi.mPinkYoshi;

    gYoshiBodyColor[0] = file->Yoshi.mGreenYoshi;
    gYoshiBodyColor[1] = file->Yoshi.mOrangeYoshi;
    gYoshiBodyColor[2] = file->Yoshi.mPurpleYoshi;
    gYoshiBodyColor[3] = file->Yoshi.mPinkYoshi;

    yoshi->mMaxJuice = file->Yoshi.mMaxJuice;
    yoshi->mMaxVSpdStartFlutter = file->Yoshi.mMaxVSpdStartFlutter;
    yoshi->mFlutterAcceleration = file->Yoshi.mFlutterAcceleration;
    yoshi->mMaxFlutterTimer = file->Yoshi.mMaxFlutterTimer;
}
kmCall(0x80271580, &initYoshi);

//0x8029CCB0
void initCardColors()
{
    SMEFile *file = gInfo.mFile;

    if (file && file->GlobalFlags.mIsYoshi)
    {
        gpMarDirector->mGCConsole->mJuiceCardOrange = file->Yoshi.mOrangeYoshi;
        gpMarDirector->mGCConsole->mJuiceCardPurple = file->Yoshi.mPurpleYoshi;
        gpMarDirector->mGCConsole->mJuiceCardPink = file->Yoshi.mPinkYoshi;
    }

    if (file && file->GlobalFlags.mIsFludd && file->Fludd.mIsColorWater)
    {
        gpMarDirector->mGCConsole->mWaterLeftPanel = file->Fludd.mWaterColor;
        gpMarDirector->mGCConsole->mWaterRightPanel.R = lerp<u8>(0, file->Fludd.mWaterColor.R, 0.8125);
        gpMarDirector->mGCConsole->mWaterRightPanel.G = lerp<u8>(0, file->Fludd.mWaterColor.G, 0.8125);
        gpMarDirector->mGCConsole->mWaterRightPanel.B = lerp<u8>(0, file->Fludd.mWaterColor.B, 0.8125);
        gpMarDirector->mGCConsole->mWaterRightPanel.A = lerp<u8>(0, file->Fludd.mWaterColor.A, 0.8125);
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
    gInfo.mGame6Heap = (JKRHeap *)__nw__FUlP7JKRHeapi(size, sSystemHeap, 32);
    gpMarDirector->mGame6Data = (u32 *)__nw__FUli(size, alignment);
}
kmCall(0x802B57E4, &createUIHeap);

//0x802A72A4
u32 initHUDElements(char *filepath)
{
    char buffer[32];
    s32 entrynum;

    sprintf(buffer, filepath, gpFlagManager->Type6Flag.CustomFlags.mHUDElement);
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

    if (gpApplication.mGamePad1->isPressed(TMarioGamePad::DPAD_UP))
        gpFlagManager->Type6Flag.CustomFlags.mHUDElement = 1;
    else if (gpApplication.mGamePad1->isPressed(TMarioGamePad::DPAD_DOWN))
        gpFlagManager->Type6Flag.CustomFlags.mHUDElement = 0;

    sprintf(buffer, "/data/game_%d.arc", gpFlagManager->Type6Flag.CustomFlags.mHUDElement); //"/data/game_%d.arc"
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

/*PATCHES*/

//Restore Chao Seed
kmWrite32(0x802FD1A0, 0x808D8C70);
//Show Exception Handler
kmWrite32(0x8029D0BC, 0x60000000);
//Skip FMVs
kmWrite32(0x802B5E8C, 0x38600001);
kmWrite32(0x802B5EF4, 0x38600001);
//Level Select
kmWrite32(0x802A6788, 0x3BC00009);
//Fix Infinte Flutter
kmWrite32(0x8028113C, 0xC002F824);
//Remove Dive While Wall Sliding
kmWrite32(0x8024BC10, 0x48000068);
//Flood Till Corona Beat
kmWrite32(0x8029961C, 0x38840077);

