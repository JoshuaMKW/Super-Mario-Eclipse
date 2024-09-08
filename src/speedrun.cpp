#include <SMS/Manager/FlagManager.hxx>
#include <SMS/raw_fn.hxx>

#include <BetterSMS/libs/lock.hxx>
#include <BetterSMS/module.hxx>

#include "settings.hxx"

using namespace BetterSMS;

#if 0
static OSThread sOverclockThread;
static char sOverclockThreadStack[2000] = {0};
#endif

static bool sIsCPUOverclocked = false;

static J2DTextBox *sOverclockTextbox = nullptr;
static char sOverclockBuffer[64]     = {0};

static J2DTextBox *sTimerTextboxF = nullptr;
static J2DTextBox *sTimerTextboxB = nullptr;
static char sTimerBuffer[64]      = {0};

static bool sTimerRunning   = false;
static bool sTimerStopped   = false;
static OSTime sTimerStart   = 0;
static OSTime sTimerCurrent = 0;
static bool sIsInitialized  = false;

extern SpeedrunSetting gSpeedrunSetting;

#if 0
BETTER_SMS_FOR_CALLBACK static void *checkForCPUOverclock(void *app) {
    while (true) {
        s32 diff = 0;
        {
            TAtomicGuard guard;
            OSTime start, end;

            start = OSGetTime();
            for (u32 i = 0; i < 100000; i++) {
                SMS_ASM_BLOCK("nop");
            }
            end  = OSGetTime();
            diff = (s32)(end - start);
        }

        OSReport("CPU Overclock: %li\n", diff);

        sIsCPUOverclocked = diff < 16600;
        diff = 100 + (s32)((f32)(16681 - diff) / (16681 + diff) / 2.0f);

        snprintf(sOverclockBuffer, 64, "CPU is overclocked: %li%c", diff, '%');

        OSSleepThread(&sOverclockQueue);
    }
    return app;
}

BETTER_SMS_FOR_CALLBACK void initCPUOverclock(TApplication *app) {
    sOverclockTextbox                  = new J2DTextBox(gpSystemFont->mFont, "");
    sOverclockTextbox->mStrPtr         = sOverclockBuffer;
    sOverclockTextbox->mNewlineSize    = 15;
    sOverclockTextbox->mCharSizeX      = 16;
    sOverclockTextbox->mCharSizeY      = 15;
    sOverclockTextbox->mGradientTop    = {255, 255, 255, 100};
    sOverclockTextbox->mGradientBottom = {255, 255, 255, 100};

    OSCreateThread(&sOverclockThread, checkForCPUOverclock, app, sOverclockThreadStack, sizeof(sOverclockThreadStack),
                   (OSPriority)28,
                   OS_THREAD_ATTR_DETACH);
    OSResumeThread(&sOverclockThread);
}
#else
BETTER_SMS_FOR_CALLBACK void checkForCPUOverclock(TMarDirector *app) {
    if (!sIsInitialized || gSpeedrunSetting.getInt() == 0) {
        return;
    }

    s32 diff = 0;
    {
        TAtomicGuard guard;
        OSTime start, end;

        start = OSGetTime();
        for (u32 i = 0; i < 10000; i++) {
            SMS_ASM_BLOCK("nop");
        }
        end  = OSGetTime();
        diff = (s32)(end - start);
    }

    sIsCPUOverclocked = diff < 1660;
    diff              = (1668.1f / (f32)diff) * 100.0f;

    snprintf(sOverclockBuffer, 64, "Overclock: %li", diff);
}

BETTER_SMS_FOR_CALLBACK void initCPUOverclock(TApplication *app) {
    JKRHeap *old = JKRHeap::sRootHeap->becomeCurrentHeap();

    sOverclockTextbox                  = new J2DTextBox(gpSystemFont->mFont, "");
    sOverclockTextbox->mStrPtr         = sOverclockBuffer;
    sOverclockTextbox->mNewlineSize    = 13;
    sOverclockTextbox->mCharSizeX      = 13;
    sOverclockTextbox->mCharSizeY      = 13;
    sOverclockTextbox->mGradientTop    = {255, 255, 255, 100};
    sOverclockTextbox->mGradientBottom = {255, 255, 255, 100};

    sTimerTextboxF                  = new J2DTextBox(gpSystemFont->mFont, "");
    sTimerTextboxF->mStrPtr         = sTimerBuffer;
    sTimerTextboxF->mNewlineSize    = 14;
    sTimerTextboxF->mCharSizeX      = 14;
    sTimerTextboxF->mCharSizeY      = 14;
    sTimerTextboxF->mGradientTop    = {180, 230, 10, 255};
    sTimerTextboxF->mGradientBottom = {240, 170, 10, 255};

    sTimerTextboxB                  = new J2DTextBox(gpSystemFont->mFont, "");
    sTimerTextboxB->mStrPtr         = sTimerBuffer;
    sTimerTextboxB->mNewlineSize    = 14;
    sTimerTextboxB->mCharSizeX      = 14;
    sTimerTextboxB->mCharSizeY      = 14;
    sTimerTextboxB->mGradientTop    = {0, 0, 0, 255};
    sTimerTextboxB->mGradientBottom = {0, 0, 0, 255};

    old->becomeCurrentHeap();

    sIsInitialized = true;
}
#endif

BETTER_SMS_FOR_CALLBACK void renderCPUOverclock(TMarDirector *director,
                                                const J2DOrthoGraph *graph) {
    if (!sIsCPUOverclocked || !sIsInitialized ||
        gSpeedrunSetting.getInt() == SpeedrunSetting::NONE || BetterSMS::isDebugMode()) {
        return;
    }

    sOverclockTextbox->draw(226, 456);
}

BETTER_SMS_FOR_CALLBACK void updateSpeedrunTimer(TApplication *app) {
    if (!sIsInitialized || gSpeedrunSetting.getInt() == SpeedrunSetting::NONE) {
        return;
    }

    if (!sTimerRunning) {
        if (sTimerStopped) {
            sTimerTextboxF->mGradientTop    = {60, 240, 70, 255};
            sTimerTextboxF->mGradientBottom = {10, 170, 20, 255};
        } else {
            sTimerTextboxF->mGradientTop    = {180, 230, 10, 255};
            sTimerTextboxF->mGradientBottom = {240, 170, 10, 255};
            strncpy(sTimerBuffer, " 0:00.0", 9);
            return;
        }
    } else if (!sTimerStopped) {
        sTimerTextboxF->mGradientTop    = {180, 230, 10, 255};
        sTimerTextboxF->mGradientBottom = {240, 170, 10, 255};
        sTimerCurrent                   = OSGetTime();
    }

    f64 milliseconds = OSTicksToMilliseconds((f64)(sTimerCurrent - sTimerStart));
    f64 fsecs        = milliseconds / 1000.0;

    int secs  = (int)fsecs;
    int mins  = secs / 60;
    int hours = mins / 60;
    secs %= 60;
    mins %= 60;

    if (hours > 0) {
        snprintf(sTimerBuffer, 64, "%2d:%2d:%02d.%01d", hours, mins, secs,
                 (int)(milliseconds / 100) % 10);
    } else {
        snprintf(sTimerBuffer, 64, "%2d:%02d.%01d", mins, secs, (int)(milliseconds / 100) % 10);
    }
}

BETTER_SMS_FOR_CALLBACK void renderSpeedrunTimer(TApplication *app, const J2DOrthoGraph *graph) {
    if (!sIsInitialized || gSpeedrunSetting.getInt() == SpeedrunSetting::NONE) {
        return;
    }

    int x = 14 + -BetterSMS::getScreenRatioAdjustX();
    sTimerTextboxB->draw(x + 1, 456 + 1);
    sTimerTextboxF->draw(x, 456);
}

static void initializeTimerOnFirstStart(TFlagManager *manager) {
    manager->firstStart();
    sTimerRunning = true;
    sTimerStopped = false;
    sTimerStart   = OSGetTime();
    sTimerCurrent = sTimerStart;
}
SMS_PATCH_BL(0x80163F54, initializeTimerOnFirstStart);
SMS_PATCH_BL(0x801642E0, initializeTimerOnFirstStart);
SMS_PATCH_BL(0x802A2EB8, initializeTimerOnFirstStart);

void TBathtub_startDemo_override(void *bathtub) {
    if (!sIsInitialized || !sTimerRunning || gSpeedrunSetting.getInt() == SpeedrunSetting::NONE) {
        startDemo__8TBathtubFv(bathtub);
        return;
    }

    if (gSpeedrunSetting.getInt() == SpeedrunSetting::ANY) {
        sTimerCurrent = OSGetTime();
        sTimerRunning = false;
        sTimerStopped = true;
    }

    if (gSpeedrunSetting.getInt() == SpeedrunSetting::SHINE_121) {
        if (TFlagManager::smInstance->getFlag(0x40000) == 120 &&
            !TFlagManager::smInstance->getFlag(0x10077)) {
            sTimerCurrent = OSGetTime();
            sTimerRunning = false;
            sTimerStopped = true;
        }
    }

    startDemo__8TBathtubFv(bathtub);
}
SMS_PATCH_BL(SMS_PORT_REGION(0x801fc090, 0, 0, 0), TBathtub_startDemo_override);

void TFlagManager_setShineFlag_override(TFlagManager *manager, u32 shineID) {
    manager->setShineFlag(shineID);

    if (!sIsInitialized || !sTimerRunning || gSpeedrunSetting.getInt() == SpeedrunSetting::NONE) {
        return;
    }

    switch (gSpeedrunSetting.getInt()) {
    case SpeedrunSetting::SHINE_121: {
        if (manager->getFlag(0x40000) >= 121) {
            sTimerCurrent = OSGetTime();
            sTimerRunning = false;
            sTimerStopped = true;
        }
    }
    case SpeedrunSetting::SHINE_197: {
        if (manager->getFlag(0x40000) >= 197) {
            sTimerCurrent = OSGetTime();
            sTimerRunning = false;
            sTimerStopped = true;
        }
    }
    case SpeedrunSetting::SHINE_240: {
        if (manager->getFlag(0x40000) >= 240) {
            sTimerCurrent = OSGetTime();
            sTimerRunning = false;
            sTimerStopped = true;
        }
    }
    }
}
SMS_PATCH_BL(SMS_PORT_REGION(0x80297BA4, 0, 0, 0), TFlagManager_setShineFlag_override);
SMS_PATCH_BL(SMS_PORT_REGION(0x802A2ECC, 0, 0, 0), TFlagManager_setShineFlag_override);
SMS_PATCH_BL(SMS_PORT_REGION(0x802B64FC, 0, 0, 0), TFlagManager_setShineFlag_override);