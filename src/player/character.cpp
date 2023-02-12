#include <Dolphin/types.h>
#include <Dolphin/printf.h>
#include <Dolphin/DVD.h>

#include <JSystem/JGadget/Vector.hxx>
#include <JSystem/JKernel/JKRDecomp.hxx>
#include <JSystem/JKernel/JKRDvdRipper.hxx>
#include <JSystem/JKernel/JKRHeap.hxx>
#include <JSystem/JKernel/JKRMemArchive.hxx>
#include <SMS/Camera/PolarSubCamera.hxx>
#include <SMS/System/Application.hxx>
#include <SMS/assert.h>
#include <SMS/macros.h>

#include <BetterSMS/module.hxx>
#include <BetterSMS/player.hxx>
#include <BetterSMS/stage.hxx>
#include <BetterSMS/libs/global_vector.hxx>

#include "globals.hxx"
#include "player.hxx"

static u8 gJ3DBuffer[0x2000];
static OSThread gCharacterSwapThread;
static u8 gCharacterSwapStack[0x4000];
static bool gSwapSuccessful = false;
static u8 gHeapAllocState   = 0;

static TGlobalVector<void *> sCharacterArcs;

bool SME::loadParams() {
    JKRMemArchive *marioVolumeData =
        static_cast<JKRMemArchive *>(JKRFileLoader::getVolume("mario"));

    u8 *params = static_cast<u8 *>(marioVolumeData->getResource("/params.szs"));

    void *allocation;
    if (params) {
        u32 filesize                     = marioVolumeData->getResSize(params);
        CompressionType compressionState = JKRDecomp::checkCompressed(params);

        switch (compressionState) {
        case CompressionType::SZS: {
            *(u32 *)0x8040E260 = params[1];
            allocation         = JKRHeap::sCurrentHeap->alloc(params[1], 32, JKRHeap::sCurrentHeap);
            decompSZS_subroutine(params, static_cast<u8 *>(allocation));
            TGlobals::sPRMFile = allocation;
        }
        case CompressionType::SZP:
            TGlobals::sPRMFile = nullptr;
        case CompressionType::NONE: {
            allocation = JKRHeap::sCurrentHeap->alloc(filesize, 32);
            memcpy(allocation, params, filesize);
            TGlobals::sPRMFile = allocation;
        }
        }

        JKRMemArchive *oldParams = static_cast<JKRMemArchive *>(JKRFileLoader::getVolume("params"));

        oldParams->unmountFixed();
        oldParams->mountFixed(TGlobals::sPRMFile, JKRMemBreakFlag::UNK_0);

        return true;
    }
    return false;
}

bool SME::swapBinary(SME::CharacterID id) {
    char buffer[32];
    snprintf(buffer, 32, "/data/chr%hhu.szs", id);

    if (DVDConvertPathToEntrynum(buffer) < 0) {
        return false;
    }

    JKRMemArchive *mario_volume =
        static_cast<JKRMemArchive *>(JKRFileLoader::getVolume("mario"));

    JKRHeap::sCurrentHeap->free(arcBufMario);

    void *marioData = SMSLoadArchive(buffer, nullptr, 0, nullptr);

    // Character failed to load / doesn't exist
    SMS_ASSERT(marioData, "Tried to swap the player with data that doesn't exist!");

    gHeapAllocState ^= 1;
    arcBufMario = marioData;

    mario_volume->unmountFixed();
    mario_volume->mountFixed(arcBufMario, JKRMemBreakFlag::UNK_0);

    return true;
}

//static void *t_swapCharacter(void *param) {
//    TMario *player  = reinterpret_cast<TMario *>(param);
//    gSwapSuccessful = false;
//
//    char buffer[32];
//    snprintf(buffer, 32, "/data/chr%hhu.szs", gTargetCharacterID);
//
//    // Player doesn't exist
//    if (DVDConvertPathToEntrynum(buffer) < 0) {
//        return &gSwapSuccessful;
//    }
//
//    JKRExpHeap *CharacterHeap = TGlobals::sCharacterHeap;
//    JKRMemArchive *marioVolumeData =
//        static_cast<JKRMemArchive *>(JKRFileLoader::getVolume("mario"));
//
//    // -- Start screen wipe and async load the new character binary -- //
//    u32 *marioData;
//    if (gFadeInOut) {
//        gpApplication.mFader->startWipe(TSMSFader::WipeRequest::FADE_CIRCLE_OUT, 1.0f, 0.0f);
//        CharacterHeap->freeAll();
//        marioData = reinterpret_cast<u32 *>(Util::loadArchive(
//            buffer, CharacterHeap, static_cast<JKRDvdRipper::EAllocDirection>(gHeapAllocState)));
//
//        // Threaded loop to wait on fade to complete
//        while (gpApplication.mFader->mFadeStatus != TSMSFader::FADE_ON) {
//            SMS_ASM_BLOCK("");
//        }
//    } else {
//        CharacterHeap->freeAll();
//        marioData = reinterpret_cast<u32 *>(Util::loadArchive(
//            buffer, CharacterHeap, static_cast<JKRDvdRipper::EAllocDirection>(gHeapAllocState)));
//    }
//
//    // Character failed to load / doesn't exist
//    SMS_ASSERT(marioData, "Tried to swap the player with data that doesn't exist!");
//
//    // Toggle heap direction to keep heap from fragmenting
//    gHeapAllocState ^= 1;
//
//    // Make the player inactive
//    player->mAttributes.mIsPerforming = true;
//
//    // Free buffer and swap pointers
//    arcBufMario = marioData;
//
//    // Refresh mounted handle
//    marioVolumeData->unmountFixed();
//    marioVolumeData->mountFixed(arcBufMario, JKRMemBreakFlag::UNK_0);
//
//    // -- Update player data -- //
//    {
//        const s16 health = player->mHealth;
//        Vec position;
//        Vec rotation;
//
//        player->JSGGetTranslation(&position);
//        player->JSGGetRotation(&rotation);
//
//        // Prevent other threads from interrupting
//
//        // clang-format off
//        SMS_ATOMIC_CODE(
//            JKRHeap *currentHeap = gJ3DHeap.becomeCurrentHeap();
//            gJ3DHeap.freeAll();
//            player->initValues();
//            player->loadAfter();
//            currentHeap->becomeCurrentHeap();
//        )
//        // clang-format on
//
//        player->mAttributes.mIsPerforming = false;
//        player->JSGSetTranslation(position);
//        player->JSGSetRotation(rotation);
//    }
//
//    if (gFadeInOut) {
//        gpApplication.mFader->startWipe(TSMSFader::WipeRequest::FADE_CIRCLE_IN, 1.0f, 0.0f);
//
//        while (gpApplication.mFader->mFadeStatus != TSMSFader::FADE_OFF) {
//            SMS_ASM_BLOCK("");
//        }
//    }
//
//    gSwapSuccessful = true;
//    return &gSwapSuccessful;
//}
//
//void SME::switchCharacter(TMario *player, CharacterID id, bool fadeInOut) {
//    gTargetCharacterID = id;
//    gFadeInOut         = fadeInOut;
//
//    if (OSIsThreadTerminated(&gCharacterSwapThread)) {
//        // fix stack bug, gets overwritten due to obnoxious pointer
//        OSCreateThread(&gCharacterSwapThread, &t_swapCharacter, player,
//                       (u8 *)(&gCharacterSwapStack) + sizeof(gCharacterSwapStack),
//                       sizeof(gCharacterSwapStack), 18, 0);
//        OSResumeThread(&gCharacterSwapThread);
//    }
//}

void SME::switchCharacter(TMario *player, CharacterID id, bool is_in_stage) {
    JKRMemArchive *archive = reinterpret_cast<JKRMemArchive *>(JKRFileLoader::getVolume("mario"));
    archive->unmountFixed();
    arcBufMario = sCharacterArcs.at(static_cast<int>(id));
    archive->mountFixed(arcBufMario, UNK_0);

    if (player) {
        player->initValues();
        player->loadAfter();
    }
}

SMS_WRITE_32(SMS_PORT_REGION(0x802A6C4C, 0, 0, 0), 0x60000000);  // Prevent early archive init
SMS_WRITE_32(SMS_PORT_REGION(0x802A7148, 0, 0, 0), 0x48000058);
SMS_WRITE_32(SMS_PORT_REGION(0x802A71A8, 0, 0, 0), 0x60000000);

void initCharacterArchives(TMarDirector *director) {
    sCharacterArcs.clear();
    sCharacterArcs.reserve(4);

    for (int i = 0; i < 2; ++i) {
        char buffer[32];
        snprintf(buffer, 32, "/data/chr%hhu.szs", SME::TGlobals::sCharacterIDList[i]);

        OSReport("Auuugghh\n");
        sCharacterArcs.push_back(SMSLoadArchive(buffer, nullptr, 0, nullptr));
    }

    arcBufMario = sCharacterArcs.at(0);

    JKRMemArchive *archive = reinterpret_cast<JKRMemArchive *>(JKRFileLoader::getVolume("mario"));
    if (!archive)
        archive = new JKRMemArchive(arcBufMario, 0, UNK_0);
}


void initCharacterBuffer(TMario *player, JSUMemoryInputStream *input) {
    load__Q26JDrama6TActorFR20JSUMemoryInputStream(player, input);
    u8 player_index        = 0;

    JKRMemArchive *archive = reinterpret_cast<JKRMemArchive *>(JKRFileLoader::getVolume("mario"));
    archive->unmountFixed();
    arcBufMario = sCharacterArcs.at(player_index);
    archive->mountFixed(arcBufMario, UNK_0);
}
SMS_PATCH_BL(SMS_PORT_REGION(0x80276BF0, 0, 0, 0), initCharacterBuffer);

static void getGlobalOrLocalResFmt(char *dst, size_t size, const char *local_path,
                                const char *specifier, const char *global_path) {
    if (!JKRFileLoader::findFirstFile("/common/01_waterboost")) {
        snprintf(dst, size, local_path, specifier);
        return;
    }
    snprintf(dst, size, global_path, specifier);
}

static void *getGlobalOrLocalRes(const char *local_path, const char *global_path) {
    if (!JKRFileLoader::findFirstFile("/common/01_waterboost")) {
        return JKRFileLoader::getGlbResource(local_path);
    }
    return JKRFileLoader::getGlbResource(global_path);
}

static void getGlobalPlayerBoneAnim(char *dst, size_t size, const char *local_path, const char *specifier) {
    getGlobalOrLocalResFmt(dst, size, local_path, specifier, "/common/bck/ma_%s.bck");
}
SMS_PATCH_BL(SMS_PORT_REGION(0x80246B2C, 0, 0, 0), getGlobalPlayerBoneAnim);

static void getGlobalPlayerSoundAnim(char *dst, size_t size, const char *local_path,
                                     const char *specifier) {
    getGlobalOrLocalResFmt(dst, size, local_path, specifier, "/common/bas/ma_%s.bas");
}
//SMS_PATCH_BL(SMS_PORT_REGION(0x80246B60, 0, 0, 0), getGlobalPlayerSoundAnim);

static void getGlobalPlayerTobiKomiAnim(MActorAnmData *data, const char *local_path) {
    if (!JKRFileLoader::findFirstFile("/common/04_tobikomi")) {
        data->init(local_path, nullptr);
        return;
    }
    data->init("common/04_tobikomi", nullptr);
}
SMS_PATCH_BL(SMS_PORT_REGION(0x8027211C, 0, 0, 0), getGlobalPlayerTobiKomiAnim);

static void *getGlobalPlayerTobiKomiMdl(const char *local_path) {
    return getGlobalOrLocalRes(local_path, "/common/04_tobikomi/04_tobikomi.bmd");
}
SMS_PATCH_BL(SMS_PORT_REGION(0x80272160, 0, 0, 0), getGlobalPlayerTobiKomiMdl);

static void getGlobalPlayerWaterAnim(MActorAnmData *data, const char *local_path) {
    if (!JKRFileLoader::findFirstFile("/common/01_waterboost")) {
        data->init(local_path, nullptr);
        return;
    }
    data->init("common/01_waterboost", nullptr);
}
SMS_PATCH_BL(SMS_PORT_REGION(0x802721F0, 0, 0, 0), getGlobalPlayerWaterAnim);

static void *getGlobalPlayerWaterBoostMdl(const char *local_path) {
    return getGlobalOrLocalRes(local_path, "/common/01_waterboost/01_waterboost.bmd");
}
SMS_PATCH_BL(SMS_PORT_REGION(0x80272220, 0, 0, 0), getGlobalPlayerWaterBoostMdl);

static void *getGlobalPlayerWaterRefTex(const char *local_path) {
    return getGlobalOrLocalRes(local_path, "/common/timg/waterref.bti");
}
SMS_PATCH_BL(SMS_PORT_REGION(0x80280330, 0, 0, 0), getGlobalPlayerWaterRefTex);

static void *getGlobalPlayerWaterMaskTex(const char *local_path) {
    return getGlobalOrLocalRes(local_path, "/common/timg/waterMask.bti");
}
SMS_PATCH_BL(SMS_PORT_REGION(0x80280364, 0, 0, 0), getGlobalPlayerWaterMaskTex);

static void *getGlobalPlayerWaterSpecTex(const char *local_path) {
    return getGlobalOrLocalRes(local_path, "/common/timg/waterSpec.bti");
}
SMS_PATCH_BL(SMS_PORT_REGION(0x8028039c, 0, 0, 0), getGlobalPlayerWaterSpecTex);

static void *getGlobalPlayerWaterJumpingTex(const char *local_path) {
    return getGlobalOrLocalRes(local_path, "/common/timg/waterJumping.bti");
}
SMS_PATCH_BL(SMS_PORT_REGION(0x802803D4, 0, 0, 0), getGlobalPlayerWaterJumpingTex);

static void *getGlobalPlayerWaterShadowYukaMdl(const char *local_path) {
    return getGlobalOrLocalRes(local_path, "/common/water_shadow_yuka.bmd");
}
SMS_PATCH_BL(SMS_PORT_REGION(0x8028040C, 0, 0, 0), getGlobalPlayerWaterShadowYukaMdl);

static void *getGlobalPlayerWaterShadowKabeMdl(const char *local_path) {
    return getGlobalOrLocalRes(local_path, "/common/water_shadow_kabe.bmd");
}
SMS_PATCH_BL(SMS_PORT_REGION(0x80280420, 0, 0, 0), getGlobalPlayerWaterShadowKabeMdl);

static void *getGlobalPlayerWaterMaskMdl(const char *local_path) {
    return getGlobalOrLocalRes(local_path, "/common/watermask.bmd");
}
SMS_PATCH_BL(SMS_PORT_REGION(0x80280434, 0, 0, 0), getGlobalPlayerWaterMaskMdl);

static void *getGlobalPlayerWaterHideYukaSMdl(const char *local_path) {
    return getGlobalOrLocalRes(local_path, "/common/water_hide_yuka_s.bmd");
}
SMS_PATCH_BL(SMS_PORT_REGION(0x80280448, 0, 0, 0), getGlobalPlayerWaterHideYukaSMdl);

static void *getGlobalPlayerWaterHideKabeSMdl(const char *local_path) {
    return getGlobalOrLocalRes(local_path, "/common/water_hide_kabe_s.bmd");
}
SMS_PATCH_BL(SMS_PORT_REGION(0x8028045C, 0, 0, 0), getGlobalPlayerWaterHideKabeSMdl);

static void *getGlobalPlayerWaterDiverHelmMdl(const char *local_path) {
    return getGlobalOrLocalRes(local_path, "/common/watergun2/body/diver_helm.bmd");
}
SMS_PATCH_BL(SMS_PORT_REGION(0x802423B0, 0, 0, 0), getGlobalPlayerWaterDiverHelmMdl);

static void getGlobalPlayerWaterGunAnim(MActorAnmData *data, const char *local_path) {
    if (!JKRFileLoader::findFirstFile("/common/01_waterboost")) {
        data->init(local_path, nullptr);
        return;
    }

    char buffer[64];
    snprintf(buffer, 64, "/common/%s", local_path + 7);  // Replace /mario/ with /common/

    data->init(buffer, nullptr);
}
SMS_PATCH_BL(SMS_PORT_REGION(0x8026A6E8, 0, 0, 0), getGlobalPlayerWaterGunAnim);

static void *getGlobalPlayerWaterGunMdl(const char *local_path) {
    if (!JKRFileLoader::findFirstFile("/common/01_waterboost")) {
        return JKRFileLoader::getGlbResource(local_path);
    }

    char buffer[64];
    snprintf(buffer, 64, "/common/%s", local_path + 7);  // Replace /mario/ with /common/

    return JKRFileLoader::getGlbResource(buffer);
}
SMS_PATCH_BL(SMS_PORT_REGION(0x8026A71C, 0, 0, 0), getGlobalPlayerWaterGunMdl);

static void getGlobalPlayerWaterGunBodyAnim(MActorAnmData *data, const char *local_path) {
    if (!JKRFileLoader::findFirstFile("/common/01_waterboost")) {
        data->init(local_path, nullptr);
        return;
    }
    data->init("common/watergun2/body", nullptr);
}
SMS_PATCH_BL(SMS_PORT_REGION(0x8026A550, 0, 0, 0), getGlobalPlayerWaterGunBodyAnim);

static void *getGlobalPlayerWaterGunBodyMdl(const char *local_path) {
    return getGlobalOrLocalRes(local_path, "/common/watergun2/body/wg_mdl1.bmd");
}
SMS_PATCH_BL(SMS_PORT_REGION(0x8026A578, 0, 0, 0), getGlobalPlayerWaterGunBodyMdl);

static void *getGlobalPlayerSplashTex(const char *local_path) {
    return getGlobalOrLocalRes(local_path, "/common/timg/splash.bti");
}
SMS_PATCH_BL(SMS_PORT_REGION(0x8026707C, 0, 0, 0), getGlobalPlayerSplashTex);