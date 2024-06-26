#include <Dolphin/DVD.h>
#include <Dolphin/printf.h>
#include <Dolphin/types.h>

#include <JSystem/J2D/J2DOrthoGraph.hxx>
#include <JSystem/JGadget/Vector.hxx>
#include <JSystem/JKernel/JKRDecomp.hxx>
#include <JSystem/JKernel/JKRDvdRipper.hxx>
#include <JSystem/JKernel/JKRHeap.hxx>
#include <JSystem/JKernel/JKRMemArchive.hxx>
#include <SMS/Camera/PolarSubCamera.hxx>
#include <SMS/GC2D/GCConsole2.hxx>
#include <SMS/System/Application.hxx>
#include <SMS/assert.h>
#include <SMS/macros.h>

#include <BetterSMS/libs/global_vector.hxx>
#include <BetterSMS/module.hxx>
#include <BetterSMS/player.hxx>
#include <BetterSMS/stage.hxx>

#include "globals.hxx"
#include "player.hxx"

static TGlobalVector<void *> sCharacterArcs;

SMS_WRITE_32(SMS_PORT_REGION(0x802A6C4C, 0, 0, 0), 0x60000000);  // Prevent early archive init
SMS_WRITE_32(SMS_PORT_REGION(0x802A7148, 0, 0, 0), 0x48000058);
SMS_WRITE_32(SMS_PORT_REGION(0x802A71A8, 0, 0, 0), 0x60000000);

static const char *sCharacterPaths[] = {"/data/mario.szs", "/data/luigi.szs",
                                        "/data/piantissimo.szs", "/data/shadow_mario.szs"};

BETTER_SMS_FOR_CALLBACK void initCharacterArchives(TMarDirector *director) {
    /*auto *multiplayer_module = BetterSMS::getModuleInfo("Mario Sunshine Coop");
    if (multiplayer_module)
        return;*/

    sCharacterArcs.clear();

    for (int i = 0; i < 1; ++i) {
        char buffer[32];
        snprintf(buffer, 32, "%s", sCharacterPaths[(int)SME::TGlobals::sCharacterIDList[i]]);

        void *arc = JKRDvdRipper::loadToMainRAM(buffer, nullptr, EXPAND, 0, nullptr,
                                                JKRDvdRipper::HEAD, 0, nullptr);
        sCharacterArcs.push_back(arc);
    }

    arcBufMario = sCharacterArcs.at(0);

    JKRMemArchive *archive = reinterpret_cast<JKRMemArchive *>(JKRFileLoader::getVolume("mario"));
    if (!archive)
        archive = new JKRMemArchive(arcBufMario, 0, UNK_0);
}

#if 1
void initCharacterBuffer(TMario *player, JSUMemoryInputStream *input) {
    load__Q26JDrama6TActorFR20JSUMemoryInputStream(player, input);
    u8 player_index = 0;

    JKRMemArchive *archive = reinterpret_cast<JKRMemArchive *>(JKRFileLoader::getVolume("mario"));
    archive->unmountFixed();
    arcBufMario = sCharacterArcs.at(player_index);
    archive->mountFixed(arcBufMario, UNK_0);
}
SMS_PATCH_BL(SMS_PORT_REGION(0x80276BF0, 0, 0, 0), initCharacterBuffer);

static void getGlobalOrLocalResFmt(char *dst, size_t size, const char *local_path,
                                   const char *specifier, const char *global_path) {
    auto *first_file = JKRFileLoader::findFirstFile("/common/01_waterboost");
    if (!first_file) {
        snprintf(dst, size, local_path, specifier);
        return;
    }
    delete first_file;
    snprintf(dst, size, global_path, specifier);
}

static void *getGlobalOrLocalRes(const char *local_path, const char *global_path) {
    auto *first_file = JKRFileLoader::findFirstFile("/common/01_waterboost");
    if (!first_file) {
        return JKRFileLoader::getGlbResource(local_path);
    }
    delete first_file;
    return JKRFileLoader::getGlbResource(global_path);
}

static void getGlobalPlayerBoneAnim(char *dst, size_t size, const char *local_path,
                                    const char *specifier) {
    getGlobalOrLocalResFmt(dst, size, local_path, specifier, "/common/bck/ma_%s.bck");
}
SMS_PATCH_BL(SMS_PORT_REGION(0x80246B2C, 0, 0, 0), getGlobalPlayerBoneAnim);

static void getGlobalPlayerSoundAnim(char *dst, size_t size, const char *local_path,
                                     const char *specifier) {
    getGlobalOrLocalResFmt(dst, size, local_path, specifier, "/common/bas/ma_%s.bas");
}
// SMS_PATCH_BL(SMS_PORT_REGION(0x80246B60, 0, 0, 0), getGlobalPlayerSoundAnim);

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
    auto *first_file = JKRFileLoader::findFirstFile("/common/01_waterboost");
    if (!first_file) {
        data->init(local_path, nullptr);
        return;
    }
    delete first_file;
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
    auto *first_file = JKRFileLoader::findFirstFile("/common/01_waterboost");
    if (!first_file) {
        data->init(local_path, nullptr);
        return;
    }

    delete first_file;

    char buffer[64];
    snprintf(buffer, 64, "/common/%s", local_path + 7);  // Replace /mario/ with /common/

    data->init(buffer, nullptr);
}
SMS_PATCH_BL(SMS_PORT_REGION(0x8026A6E8, 0, 0, 0), getGlobalPlayerWaterGunAnim);

static void *getGlobalPlayerWaterGunMdl(const char *local_path) {
    auto *first_file = JKRFileLoader::findFirstFile("/common/01_waterboost");
    if (!first_file) {
        return JKRFileLoader::getGlbResource(local_path);
    }

    delete first_file;

    char buffer[64];
    snprintf(buffer, 64, "/common/%s", local_path + 7);  // Replace /mario/ with /common/

    return JKRFileLoader::getGlbResource(buffer);
}
SMS_PATCH_BL(SMS_PORT_REGION(0x8026A71C, 0, 0, 0), getGlobalPlayerWaterGunMdl);

static void getGlobalPlayerWaterGunBodyAnim(MActorAnmData *data, const char *local_path) {
    auto *first_file = JKRFileLoader::findFirstFile("/common/01_waterboost");
    if (!first_file) {
        data->init(local_path, nullptr);
        return;
    }
    delete first_file;
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

#else
#endif
// SMS_PATCH_BL(SMS_PORT_REGION(0x802474C0, 0, 0, 0), applyShadowEffects);
const char *player_fnames[] = {"mario", "luigi", "piantissimo", "shadow_mario"};

void updatePlayerHUD(TMarDirector *director, const J2DOrthoGraph *graph) {
    char buffer[64];
    char namebuf[32];

    auto *console = director->mGCConsole;

    {
        J2DPicture *marioIcon =
            reinterpret_cast<J2DPicture *>(console->mMainScreen->search('m_ic'));

        snprintf(buffer, 64, "/game_6/timg/%s_icon.bti",
                 player_fnames[static_cast<int>(
                     SME::TGlobals::getCharacterIDFromPlayer(gpMarioAddress))]);

        auto *timg = reinterpret_cast<ResTIMG *>(JKRFileLoader::getGlbResource(buffer));
        if (timg)
            marioIcon->changeTexture(timg, 0);
    }

    {
        J2DPicture *marioName =
            reinterpret_cast<J2DPicture *>(console->mMainScreen->search('m_tx'));

        snprintf(buffer, 64, "/game_6/timg/%s_text.bti",
                 player_fnames[static_cast<int>(
                     SME::TGlobals::getCharacterIDFromPlayer(gpMarioAddress))]);

        auto *timg = reinterpret_cast<ResTIMG *>(JKRFileLoader::getGlbResource(buffer));
        if (timg)
            marioName->changeTexture(timg, 0);
    }
}