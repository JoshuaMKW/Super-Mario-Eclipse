#include "includes/file_utils.hxx"

//0x803A428A
//.asciz "chr%d.arc"

//0x802A750C
u32 *createCharacterHeap(u32 *newHeap, u32 size, u32 *rootHeap, u32 unk_1)
{
    gInfo.mCharacterHeap = (u32 *)create__10JKRExpHeapFUlP7JKRHeapb(0x200000, (u32 *)*(u32 *)JKRRootHeap, false);
    //hmalloc(gInfo.mCharacterHeap, 0x1FFF00, 32);
    u32 *gameHeap = (u32 *)create__10JKRExpHeapFPvUlP7JKRHeapb(newHeap, size - 0x200000, rootHeap, unk_1);
    return (u32 *)gameHeap;
}
kmCall(0x802A750C, &createCharacterHeap);

//0x802A7140
u32 setupMarioDatas(char *filepath)
{
    TApplication *gpApplication = (TApplication *)TApplicationInstance;
    TMarioGamePad *gpGamePad = gpApplication->mGamePad1;
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

    sprintf(filepath, filepath, id);
    return DVDConvertPathToEntrynum(filepath);
}
kmCall(0x802A7140, &setupMarioDatas);

//0x802A716C
u32 *initFirstModel(char *path, u32 unk_1, u32 unk_2, u32 unk_3, void *JKRHeap, u32 unk_4, u32 unk_5, u32 unk_6)
{
    JKRHeap = gInfo.mCharacterHeap ? gInfo.mCharacterHeap : JKRHeap;
    freeAll__10JKRExpHeapFv(gInfo.mCharacterHeap);
    OSReport("Loading %s into memory...", path);
    u32 *file = (u32 *)loadToMainRAM__12JKRDvdRipperFPCcPUc15JKRExpandSwitchUlP7JKRHeapQ212JKRDvdRipper15EAllocDirectionUlPi(path, unk_1, unk_2, unk_3, JKRHeap, unk_4, unk_5, unk_6);

    if (file)
    {
        OSReport("Success! Location = %X, Size = %X", file, file[1]);
        hmalloc(gInfo.mCharacterHeap, (0x1F0000 - file[1]) & -32, 32);
    }
    else
    {
        OSReport("Failed! Could not allocate memory");
        hmalloc(gInfo.mCharacterHeap, 0x1F0000 & -32, 32);
    }
    return file;
}
kmCall(0x802A716C, &initFirstModel);

void resetGlobalValues()
{
    RGBA<u8> *waterColor = (RGBA<u8> *)WaterAddr;
    RGBA<u8> *juiceColor = (RGBA<u8> *)YoshiJuiceColor;
    RGBA<u8> *yoshiColor = (RGBA<u8> *)YoshiColor;

    waterColor[0] = RGBA<u8>(0x3C, 0x46, 0x78, 0x14); //Water rgba
    juiceColor[0] = RGBA<u8>(0xFE, 0xA8, 0x02, 0x6E); //Yoshi Juice rgba
    juiceColor[1] = RGBA<u8>(0x9B, 0x01, 0xFD, 0x6E);
    juiceColor[2] = RGBA<u8>(0xFD, 0x62, 0xA7, 0x6E);
    yoshiColor[0] = RGBA<u8>(0x40, 0xA1, 0x24, 0xFF); //Yoshi rgba
    yoshiColor[1] = RGBA<u8>(0xFF, 0x8C, 0x1C, 0xFF);
    yoshiColor[2] = RGBA<u8>(0xAA, 0x4C, 0xFF, 0xFF);
    yoshiColor[3] = RGBA<u8>(0xFF, 0xA0, 0xBE, 0xFF);
    *(float *)0x80415CA8 = 0.25f; //Mario overall speed
    *(float *)0x8040C1C0 = 0.75f;
    *(float *)0x80417248 = 1.0f;
    *(float *)0x8041731C = 1.0f;
}

//0x802998B4
u32 *initFileMods()
{
    register u32 *objList;
    __asm { mr objList, r26 };

    TApplication *gpApplication = (TApplication *)TApplicationInstance;
    TMarioGamePad *gpGamePad = gpApplication->mGamePad1;
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

    u32 *marioVolumeData = (u32 *)getVolume__13JKRFileLoaderFPCc("mario");        //"mario"
    u32 *params = (u32 *)getResource__10JKRArchiveFPCc(marioVolumeData, "/params.szs"); //"/params.szs"

    u32 *marioDataField;
    u32 *marioData;
    u32 *allocation;

    char buffer[32];

    free(gInfo.mFile); //Free the file allocation
    resetGlobalValues();

    char folder[] = "/data/scene/sme/";
    const char *stage = getStageName((TApplication *)TApplicationInstance);

    if (!stage) return objList;

    SMEFile *file = SMEFile::loadFile(SMEFile::parseExtension(folder, stage, false));

    if (!file)
    {
        folder[16] = '\0';
        file = SMEFile::loadFile(SMEFile::parseExtension(folder, stage, true));
    }

    gInfo.mFile = file;
    if (gInfo.mFile)
    {
        characterID = characterID == 0xFF ? gInfo.mFile->FileHeader.mPlayerID : characterID;
    }

    if (characterID >= 0)
    {
        //Attempt to swap character data
        sprintf(buffer, "/data/chr%d.arc", characterID);               //"/data/chr%d.arc"
        strcpy(strstr(buffer, ".arc"), ".szs"); //".arc", ".szs"

        if (DVDConvertPathToEntrynum(buffer) >= 0)
        {
            freeAll__10JKRExpHeapFv(gInfo.mCharacterHeap);

            marioData = (u32 *)loadToMainRAM__12JKRDvdRipperFPCcPUc15JKRExpandSwitchUlP7JKRHeapQ212JKRDvdRipper15EAllocDirectionUlPi(buffer, 0, 1, 0, gInfo.mCharacterHeap, 1, 0, 0);
            *(u32 *)ARCBufferMario = (u32)marioData;

            hmalloc(gInfo.mCharacterHeap, marioData[1] & -32, 32);

            __dt__13JKRMemArchiveFv(marioVolumeData);
            __ct__13JKRMemArchiveFPvUl15JKRMemBreakFlag(marioVolumeData, *(u32 *)ARCBufferMario, 0, 0);
        }
    }

    if (params)
    {
        u32 filesize = getResSize__10JKRArchiveCFPCv(marioVolumeData, params);
        u32 compressionState = checkCompressed__9JKRDecompFPUc(params);

        if (compressionState == 2)
        {
            *(u32 *)0x8040E260 = params[0x4 / 4];
            allocation = (u32 *)alloc__7JKRHeapFUliP7JKRHeap(params[0x4 / 4], 32, *(u32 *)JKRCurrentHeap);
            decompSZS_subroutine__FPUcPUc(params, allocation);
            gInfo.mPRMFile = allocation;
        }
        else if (compressionState == 1)
        {
            gInfo.mPRMFile = NULL;
        }
        else
        {
            allocation = (u32 *)alloc__7JKRHeapFUliP7JKRHeap(filesize, 32, *(u32 *)JKRCurrentHeap);
            memcpy(allocation, params, filesize);
            gInfo.mPRMFile = allocation;
        }

        u32 *oldParams = (u32 *)getVolume__13JKRFileLoaderFPCc(0x8041676C); //params

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
    TWaterManager *gpWaterManager = (TWaterManager *)*(u32 *)TWaterManagerInstance;
    TFlagManager *gpFlagManager = (TFlagManager *)*(u32 *)TFlagManagerInstance;

    if (!file || file->FileHeader.mIsShineShadow == false)
        return;

    if (gpFlagManager->Type4Flag.mShineCount < MAX_SHINES)
    {
        gInfo.Light.mLightType = file->FileHeader.mShineShadowFlag;
        gInfo.Light.mShineShadowBase = file->Light.mSize;
        gInfo.Light.mPrevShineCount = gpFlagManager->Type4Flag.mShineCount;

        gpWaterManager->mColor = file->Light.mColor;
        gpWaterManager->mDarkLevel = file->Light.mDarkLevel;
        gpWaterManager->mLayerCount = file->Light.mLayerCount;
        gpWaterManager->mSphereStep = file->Light.mStep;
        gpWaterManager->mSize = file->Light.mSize;
        gpWaterManager->LightType.mMaskObjects = true;
        gpWaterManager->LightType.mShowShadow = true;

        if (file->FileHeader.mShineShadowFlag == 1)
        {
            gInfo.Light.mNextSize = file->Light.mSize;
            for (u32 i = 0; i < gpFlagManager->Type4Flag.mShineCount; ++i)
            {
                gInfo.Light.mNextSize += (10000 / MAX_SHINES) + i * 2;
            }
            gpWaterManager->mSphereStep = gpWaterManager->mSize / 2;
        }
        gInfo.Light.mShineShadowCoordinates = file->Light.mCoordinates;
    }
    else
    {
        gInfo.Light.mLightType = 0;
    }
}
kmCall(0x80280180, &initShineShadow);

//0x802B7A4C
void initSoundBank(u8 areaID, u8 episodeID)
{
    SMEFile *file = gInfo.mFile;

    if (!file || !gInfo.mFile->FileHeader.mIsMusic)
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

    if (!file || !gInfo.mFile->FileHeader.mIsMusic)
    {
        startStageBGM__10MSMainProcFUcUc();
    }
    else if (gInfo.mFile->Music.mPlayMusic == true)
    {
        *(u32 *)0x8040E1F0 = 0x80010000 | gInfo.mFile->Music.mMusicID;
        *(float *)0x8041731C = gInfo.mFile->Music.mSpeed;
        *(float *)0x80417248 = gInfo.mFile->Music.mPitch;

        if (gInfo.mFile->Music.mVolume > 1)
        {
            *(float *)0x8040C1C0 = 1;
        }
        else if (gInfo.mFile->Music.mVolume < 0)
        {
            *(float *)0x8040C1C0 = 0;
        }
        else
        {
            *(float *)0x8040C1C0 = gInfo.mFile->Music.mVolume;
        }
        startStageBGM__10MSMainProcFUcUc();
    }
}
kmCall(0x802983F0, &initMusicTrack);
kmCall(0x80298420, &initMusicTrack);

void initFludd(TMario *gpMario)
{
    SMEFile *stagefile = gInfo.mFile;
    MarioParamsFile *params = gpMario->mCustomInfo->mParams;

    if (params)
    {
        waterColor = params->Attributes.FluddAttrs.mWaterColor;
        if (!params->Attributes.FluddAttrs.mCanUseNozzle[(u8)gpMario->mFludd->mCurrentNozzle])
        {
            for (u8 i = 0; i < 8; ++i)
            {
                if (params->Attributes.FluddAttrs.mCanUseNozzle[i])
                {
                    gpMario->mFludd->mCurrentNozzle = (TWaterGun::NOZZLETYPE)i;
                    gpMario->mAttributes.mHasFludd = params->Attributes.mCanUseFludd;
                    gpMario->mFludd->mCurrentWater = gpMario->mFludd->mNozzleList[(u8)gpMario->mFludd->mCurrentNozzle]->mMaxWater;
                    break;
                }
                else if (i == 7)
                {
                    gpMario->mAttributes.mHasFludd = false;
                    params->Attributes.mCanUseFludd = false;
                }
            }
        }

        if (!params->Attributes.FluddAttrs.mCanUseNozzle[(u8)gpMario->mFludd->mSecondNozzle])
        {
            for (u8 i = 0; i < 8; ++i)
            {
                if (params->Attributes.FluddAttrs.mCanUseNozzle[i])
                {
                    gpMario->mFludd->mSecondNozzle = (TWaterGun::NOZZLETYPE)i;
                    gpMario->mAttributes.mHasFludd = params->Attributes.mCanUseFludd;
                    break;
                }
                gpMario->mFludd->mSecondNozzle = gpMario->mFludd->mCurrentNozzle;
            }
        }
    }

    if (stagefile && stagefile->FileHeader.mIsFludd)
    {
        if (stagefile->Fludd.mIsColorWater)
        {
            waterColor = stagefile->Fludd.mWaterColor;
        }
        gpMario->mFludd->mCurrentNozzle = stagefile->Fludd.mPrimaryNozzle;
        gpMario->mFludd->mSecondNozzle = stagefile->Fludd.mSecondaryNozzle;

        gpMario->mFludd->mCurrentWater = gpMario->mFludd->mNozzleList[(u8)gpMario->mFludd->mCurrentNozzle]->mMaxWater;
    }
}

//0x80276AC0 - Mario
/*
mr r3, r30
__set r4, TRUE
__call r12, 0x800050D0
lwz r0, 0x44 (sp)
*/

//0x800397FC - Shadow Mario
/*
lwz r3, 0x150 (r31)
__set r4, FALSE
__call r12, 0x800050D0
lwz r0, 0x1C (sp)
*/

void initMario(TMario *gpMario, bool isMario)
{
    SMEFile *file = gInfo.mFile;
    gpMario->mCustomInfo = (TMario::CustomInfo *)malloc(sizeof(TMario::CustomInfo), 32);
    gpMario->mCustomInfo->mParams = NULL;
    gpMario->mCustomInfo->_mBaseParams = NULL;

    if (file && file->FileHeader.mIsMario)
    {
        gpMario->mCustomInfo->mPlayerID = file->FileHeader.mPlayerID;
        gInfo.PlayerData.mCurPlayerID[0] = file->FileHeader.mPlayerID;

        gpMario->mGravity = file->Mario.mGravity;
        gpMario->mBaseBounceSpeed1 = file->Mario.mBaseBounce1;
        gpMario->mBaseBounceSpeed2 = file->Mario.mBaseBounce2;
        gpMario->mBaseBounceSpeed3 = file->Mario.mBaseBounce3;
        gpMario->mMaxFallNoDamage = file->Mario.mMaxFallNoDamage;
        gpMario->mHealth = file->Mario.mHealth;
        gpMario->mMaxHealth = file->Mario.mMaxHealth;
        gpMario->mOBStep = file->Mario.mOBStep;
        gpMario->mOBMax = file->Mario.mOBMax;

        gpMario->mAttributes.mGainHelmet = file->FileHeader.MarioStates.mMarioHasHelmet;
        gpMario->mAttributes.mHasFludd = file->FileHeader.MarioStates.mMarioHasFludd;
        gpMario->mAttributes.mIsShineShirt = file->FileHeader.MarioStates.mMarioHasShirt;

        if (file->FileHeader.MarioStates.mMarioHasGlasses)
        {
            wearGlass__6TMarioFv(gpMario);
        }
    }

    if (!isMario) return;

    gpMario->mCustomInfo->_mBaseParams = (MarioParamsFile *)getResource__10JKRArchiveFPCc(getVolume__13JKRFileLoaderFPCc(0x804165A0), //mario
                                                                                          0x800049F5);                                ///params.bin

    if (gpMario->mCustomInfo->_mBaseParams)
    {
        gpMario->setCustomAttributes();
    }
    initFludd(gpMario);
}


//0x80276AC0 - Mario
/*
mr r3, r30
__set r4, TRUE
__call r12, 0x800050D0
lwz r0, 0x44 (sp)
*/

//0x80276C94
TMario *fromMarioInit(TMario *gpMario)
{
    register void *virtualFunc;
    __asm { mr virtualFunc, r12 };

    ((TMario *(*)(TMario *gpMario))virtualFunc)(gpMario); // call vtable func
    initMario(gpMario, true);
    return gpMario;
}
kmWrite32(0x80276C90, 0x60000000);
kmCall(0x80276C94, &fromMarioInit);

//0x800397DC
bool fromShadowMarioInit()
{
    register TMario *gpMario;
    __asm { lwz gpMario, 0x150 (r31) };
    initMario(gpMario, false);
    return SMS_isMultiPlayerMap__Fv();
}
kmCall(0x800397DC, &fromShadowMarioInit);


//0x802715A0
void initYoshi()
{
    SMEFile *file = gInfo.mFile;
    TMario *gpMario = (TMario *)*(u32 *)TMarioInstance;
    TYoshi *gpYoshi = gpMario->mYoshi;

    if (!file || !file->FileHeader.mIsYoshi)
        return;

    RGBA<u8> *juiceColor = (RGBA<u8> *)YoshiJuiceColor;
    RGBA<u8> *yoshiColor = (RGBA<u8> *)YoshiColor;

    juiceColor[0] = file->Yoshi.mOrangeYoshi;
    juiceColor[1] = file->Yoshi.mPurpleYoshi;
    juiceColor[2] = file->Yoshi.mPinkYoshi;

    yoshiColor[0] = file->Yoshi.mGreenYoshi;
    yoshiColor[1] = file->Yoshi.mOrangeYoshi;
    yoshiColor[2] = file->Yoshi.mPurpleYoshi;
    yoshiColor[3] = file->Yoshi.mPinkYoshi;

    gpYoshi->mMaxJuice = file->Yoshi.mMaxJuice;
    gpYoshi->mMaxVSpdStartFlutter = file->Yoshi.mMaxVSpdStartFlutter;
    gpYoshi->mFlutterAcceleration = file->Yoshi.mFlutterAcceleration;
    gpYoshi->mMaxFlutterTimer = file->Yoshi.mMaxFlutterTimer;
}
kmBranch(0x802715A0, &initYoshi);

//0x8029CCB0
void initCardColors()
{
    SMEFile *file = gInfo.mFile;
    TMarDirector *gpMarDirector = (TMarDirector *)*(u32 *)TMarDirectorInstance;

    if (file && file->FileHeader.mIsYoshi == true)
    {
        gpMarDirector->mGCConsole->mJuiceCardOrange = file->Yoshi.mOrangeYoshi;
        gpMarDirector->mGCConsole->mJuiceCardPurple = file->Yoshi.mPurpleYoshi;
        gpMarDirector->mGCConsole->mJuiceCardPink = file->Yoshi.mPinkYoshi;
    }

    if (file && file->FileHeader.mIsFludd == true && file->Fludd.mIsColorWater == true)
    {
        gpMarDirector->mGCConsole->mWaterLeftPanel = file->Fludd.mWaterColor;
        gpMarDirector->mGCConsole->mWaterRightPanel.R = lerp<u8>(0, file->Fludd.mWaterColor.R, 0.8125);
        gpMarDirector->mGCConsole->mWaterRightPanel.G = lerp<u8>(0, file->Fludd.mWaterColor.G, 0.8125);
        gpMarDirector->mGCConsole->mWaterRightPanel.B = lerp<u8>(0, file->Fludd.mWaterColor.B, 0.8125);
        gpMarDirector->mGCConsole->mWaterRightPanel.A = lerp<u8>(0, file->Fludd.mWaterColor.A, 0.8125);
    }
}
kmCall(0x8029CCB0, &initCardColors);

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
        if (((col->mFloorData->mFloorTriangles[i].mCollisionType & 0x7FFF) - validID) <= idGroupSize ||
            ((col->mFloorData->mFloorTriangles[i].mCollisionType & 0x7FFF) - validID) <= idGroupSize + 1000)
        {

            links->mColList[curDataIndex] = CollisionLink((TBGCheckData *)&col->mFloorData->mFloorTriangles[i],
                                                          (u8)(col->mFloorData->mFloorTriangles[i].mValue4 >> 8),
                                                          (u8)col->mFloorData->mFloorTriangles[i].mValue4);
            if (curDataIndex >= 0xFF) break;
            ++curDataIndex;
        }
    }
    links->arrayLength = curDataIndex;
}

//0x802B8B20
u32 initCollisionWarpLinks(char *name, u32 *dest)
{
    TMapCollisionData *collisionMap = (TMapCollisionData *)*(u32 *)TMapCollisionDataInstance;
    WarpCollisionList *warpDataArray = (WarpCollisionList *)malloc(sizeof(WarpCollisionList), 32);
    WarpCollisionList *warpDataPreserveArray = (WarpCollisionList *)malloc(sizeof(WarpCollisionList), 32);
    gInfo.mWarpColArray = warpDataArray;
    gInfo.mWarpColPreserveArray = warpDataPreserveArray;

    if (warpDataArray)
    {
        parseWarpLinks(collisionMap, warpDataArray, 16040);
        parseWarpLinks(collisionMap, warpDataPreserveArray, 16041, 1);
    }

    return calcKeyCode__Q26JDrama8TNameRefFPCc(name, dest);
}
kmCall(0x802B8B20, &initCollisionWarpLinks);

//0x802B57E4
void createUIHeap(u32 size, s32 alignment)
{
    TMarDirector *gpMarDirector = (TMarDirector *)*(u32 *)TMarDirectorInstance;
    gInfo.mGame6Heap = (u32 *)__nw__FUlP7JKRHeapi(size, (u32 *)*(u32 *)JKRSystemHeap, 32);
    gpMarDirector->mGame6Data = (u32 *)__nw__FUli(size, alignment);
}
kmCall(0x802B57E4, &createUIHeap);

//0x802A72A4
u32 initHUDElements(char *filepath)
{
    TFlagManager *gpFlagManager = (TFlagManager *)*(u32 *)TFlagManagerInstance;
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
    TMarDirector *gpMarDirector = (TMarDirector *)*(u32 *)TMarDirectorInstance;
    TFlagManager *gpFlagManager = (TFlagManager *)*(u32 *)TFlagManagerInstance;
    TApplication *gpApplication = (TApplication *)TApplicationInstance;

    char buffer[32];

    if (gpApplication->mGamePad1->isPressed(TMarioGamePad::DPAD_UP))
        gpFlagManager->Type6Flag.CustomFlags.mHUDElement = 1;
    else if (gpApplication->mGamePad1->isPressed(TMarioGamePad::DPAD_DOWN))
        gpFlagManager->Type6Flag.CustomFlags.mHUDElement = 0;

    sprintf(buffer, "/data/game_%d.arc", gpFlagManager->Type6Flag.CustomFlags.mHUDElement); //"/data/game_%d.arc"
    strcpy(strstr(buffer, ".arc"), ".szs");                        //".arc", ".szs"

    if (DVDConvertPathToEntrynum(buffer) >= 0)
    {
        free(gpMarDirector->mGame6Data);
        *gameUI = loadToMainRAM__12JKRDvdRipperFPCcPUc15JKRExpandSwitchUlP7JKRHeapQ212JKRDvdRipper15EAllocDirectionUlPi(buffer, 0, 1, 0, gpMarDirector->mGame6Data, 1, 0, 0);
    }

    return switchArchive(curArchive, gameUI);
}
//kmCall(0x80172440, &switchHUDOnStageLoad);

//0x802B57E4
u32 *useCustomHUDHeap(u32 size, s32 alignment)
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
