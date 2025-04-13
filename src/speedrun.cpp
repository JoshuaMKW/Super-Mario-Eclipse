#include <SMS/Manager/FlagManager.hxx>
#include <SMS/raw_fn.hxx>

#include <BetterSMS/libs/lock.hxx>
#include <BetterSMS/module.hxx>

#include <SMS/Camera/PolarSubCamera.hxx>

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

static J2DTextBox *sLoadTimerTextboxF = nullptr;
static J2DTextBox *sLoadTimerTextboxB = nullptr;
static char sLoadTimerBuffer[64]      = {0};

static bool sTimerRunning     = false;
static bool sTimerStopped     = false;
static OSTime sTimerStart     = 0;
static OSTime sTimerLoadStart = 0;
static OSTime sTimerLoadLast  = 0;
static OSTime sTimerLoadDelta = 0;
static OSTime sTimerCurrent   = 0;
static bool sIsInitialized    = false;

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
    sOverclockTextbox->mHBinding       = J2DTextBoxHBinding::Center;
    sOverclockTextbox->mRect.mX1       = 0;
    sOverclockTextbox->mRect.mX2       = 600;

    sTimerTextboxF                  = new J2DTextBox(gpSystemFont->mFont, "");
    sTimerTextboxF->mStrPtr         = sTimerBuffer;
    sTimerTextboxF->mNewlineSize    = 15;
    sTimerTextboxF->mCharSizeX      = 15;
    sTimerTextboxF->mCharSizeY      = 15;
    sTimerTextboxF->mGradientTop    = {180, 230, 10, 255};
    sTimerTextboxF->mGradientBottom = {240, 170, 10, 255};

    sTimerTextboxB                  = new J2DTextBox(gpSystemFont->mFont, "");
    sTimerTextboxB->mStrPtr         = sTimerBuffer;
    sTimerTextboxB->mNewlineSize    = 15;
    sTimerTextboxB->mCharSizeX      = 15;
    sTimerTextboxB->mCharSizeY      = 15;
    sTimerTextboxB->mGradientTop    = {0, 0, 0, 255};
    sTimerTextboxB->mGradientBottom = {0, 0, 0, 255};

    sLoadTimerTextboxF                  = new J2DTextBox(gpSystemFont->mFont, "");
    sLoadTimerTextboxF->mStrPtr         = sLoadTimerBuffer;
    sLoadTimerTextboxF->mNewlineSize    = 12;
    sLoadTimerTextboxF->mCharSizeX      = 13;
    sLoadTimerTextboxF->mCharSizeY      = 12;
    sLoadTimerTextboxF->mGradientTop    = {190, 170, 10, 200};
    sLoadTimerTextboxF->mGradientBottom = {240, 130, 10, 200};

    sLoadTimerTextboxB                  = new J2DTextBox(gpSystemFont->mFont, "");
    sLoadTimerTextboxB->mStrPtr         = sLoadTimerBuffer;
    sLoadTimerTextboxB->mNewlineSize    = 12;
    sLoadTimerTextboxB->mCharSizeX      = 13;
    sLoadTimerTextboxB->mCharSizeY      = 12;
    sLoadTimerTextboxB->mGradientTop    = {0, 0, 0, 200};
    sLoadTimerTextboxB->mGradientBottom = {0, 0, 0, 200};

    old->becomeCurrentHeap();

    sTimerRunning   = false;
    sTimerStopped   = false;
    sTimerStart     = OSGetTime();
    sTimerCurrent   = sTimerStart;
    sTimerLoadStart = sTimerStart;
    sTimerLoadLast  = sTimerStart;
    sTimerLoadDelta = 0;

    sIsInitialized = true;
}
#endif

static bool sIsNoBootShineCollecting = false;
static bool sShineCollected          = false;
static bool sIsLastShineNoBoot       = false;

BETTER_SMS_FOR_CALLBACK void stopSpeedrunTimerOnStageExit(TApplication *app) {
    if (!sIsInitialized) {
        return;
    }

    if (gSpeedrunSetting.getInt() == SpeedrunSetting::ILS) {
        if (sShineCollected && !sIsLastShineNoBoot) {
            if (sTimerRunning) {
                sTimerCurrent = OSGetTime();
                sTimerRunning = false;
                sTimerStopped = true;
            }
            sShineCollected    = false;
            sIsLastShineNoBoot = false;
            return;
        }

        if (SMS_getShineStage__FUc(gpApplication.mNextScene.mAreaID) ==
            SMS_getShineStage__FUc(gpApplication.mCurrentScene.mAreaID)) {
            if (gpApplication.mNextScene.mEpisodeID != 0xFF) {
                sTimerLoadStart = OSGetTime();
                sTimerLoadLast  = sTimerLoadStart;
                return;
            }
        }

        if (sTimerRunning) {
            sTimerCurrent = OSGetTime();
            sTimerRunning = false;
            sTimerStopped = true;
        }
        sShineCollected    = false;
        sIsLastShineNoBoot = false;
        return;
    }
}

static bool sWasLoading = false;
static bool sWideLayout = false;

BETTER_SMS_FOR_CALLBACK void updateSpeedrunTimer(TApplication *app) {
    if (!sIsInitialized || gSpeedrunSetting.getInt() == SpeedrunSetting::NONE) {
        return;
    }

    if (app->mCurrentScene.mAreaID == TGameSequence::AREA_OPTION) {
        /*if (sTimerRunning) {
            sTimerCurrent = OSGetTime();
            sTimerRunning = false;
            sTimerStopped = true;
        }*/
    } else {
        if (app->mContext == TApplication::CONTEXT_DIRECT_STAGE && gpMarDirector) {
            bool isActiveState = gpMarDirector->mCurState == TMarDirector::STATE_INTRO_PLAYING ||
                                 gpMarDirector->mCurState == TMarDirector::STATE_GAME_STARTING ||
                                 gpMarDirector->mCurState == TMarDirector::STATE_NORMAL;

            if (gSpeedrunSetting.getInt() == SpeedrunSetting::ILS) {
                if ((!sTimerRunning || sTimerStopped) && isActiveState && sWasLoading) {
                    sTimerRunning   = true;
                    sTimerStopped   = false;
                    sTimerStart     = OSGetTime();
                    sTimerCurrent   = sTimerStart;
                    sTimerLoadStart = sTimerStart;
                    sTimerLoadLast  = sTimerStart;
                    sTimerLoadDelta = 0;
                }
            }

            // Begin accumulating load frames
            if (gpMarDirector->mCurState == TMarDirector::STATE_INTRO_INIT) {
                OSTime time = OSGetTime();
                if (!sWasLoading) {
                    if (sTimerRunning) {
                        sTimerLoadStart = time;
                        sTimerLoadLast  = sTimerLoadStart;
                    }
                } else {
                    if (sTimerRunning) {
                        sTimerLoadDelta += time - sTimerLoadLast;
                        sTimerLoadLast = time;
                    }
                }
                sWasLoading = true;
            }

            if (gSpeedrunSetting.getInt() == SpeedrunSetting::ILS) {
                // Stop accumulating load frames
                if (isActiveState && sWasLoading && sTimerRunning) {
                    OSTime time = OSGetTime();
                    sTimerLoadDelta += time - sTimerLoadLast;
                    sTimerCurrent = time;
                    sTimerRunning = true;
                    sTimerStopped = false;
                    sWasLoading   = false;
                }

                /*if ((!sTimerRunning || sTimerStopped) && isActiveState) {
                    if (!sShineCollected) {
                        sTimerStart     = OSGetTime();
                        sTimerCurrent   = sTimerStart;
                        sTimerLoadDelta = sTimerStart - sTimerLoadLast;
                        sTimerLoadLast  = sTimerStart;
                        sTimerRunning   = true;
                        sTimerStopped   = false;
                    } else {
                        OSTime time = OSGetTime();
                        sTimerCurrent = time;
                        sTimerRunning = true;
                        sTimerStopped = false;
                    }
                }*/

                if (sTimerStopped && sIsNoBootShineCollecting &&
                    gpCamera->getRestDemoFrames() == 0) {
                    u64 time                 = OSGetTime();
                    sTimerCurrent            = time;
                    sTimerRunning            = true;
                    sTimerStopped            = false;
                    sIsNoBootShineCollecting = false;
                }
            } else {
                // Stop accumulating load frames
                if (isActiveState && sWasLoading && sTimerRunning) {
                    OSTime time = OSGetTime();
                    sTimerLoadDelta += time - sTimerLoadLast;
                    sTimerCurrent = time;
                    sTimerRunning = true;
                    sWasLoading   = false;
                }
            }

            if (gSpeedrunSetting.getInt() == SpeedrunSetting::SHINE_240 ||
                gSpeedrunSetting.getInt() == SpeedrunSetting::ILS) {
                if (sTimerRunning && gpMarDirector->mAreaID == TGameSequence::AREA_DOLPIC &&
                    gpMarDirector->mEpisodeID == 4 && TFlagManager::smInstance->getFlag(0x6001C)) {
                    sTimerCurrent = OSGetTime();
                    sTimerRunning = false;
                    sTimerStopped = true;
                }
            }
        }
    }

    if (!sTimerRunning) {
        if (sTimerStopped) {
            sTimerTextboxF->mGradientTop    = {60, 240, 70, 255};
            sTimerTextboxF->mGradientBottom = {10, 170, 20, 255};
        } else {
            sTimerTextboxF->mGradientTop    = {180, 230, 10, 255};
            sTimerTextboxF->mGradientBottom = {240, 170, 10, 255};
            strncpy(sTimerBuffer, "0:00.000", 11);
            strncpy(sLoadTimerBuffer, "(0:00.000)", 13);
            return;
        }
    } else if (!sTimerStopped) {
        sTimerTextboxF->mGradientTop    = {180, 230, 10, 255};
        sTimerTextboxF->mGradientBottom = {240, 170, 10, 255};
        sTimerCurrent                   = OSGetTime();
    }

    {
        f64 milliseconds = OSTicksToMilliseconds(
            (f64)(Max(sTimerCurrent - sTimerStart, sTimerLoadDelta) - sTimerLoadDelta));
        f64 fsecs = milliseconds / 1000.0;

        int secs  = (int)fsecs;
        int mins  = secs / 60;
        int hours = mins / 60;
        secs %= 60;
        mins %= 60;

        sWideLayout = hours > 0;

        if (sWideLayout) {
            snprintf(sTimerBuffer, 64, "%d:%02d:%02d.%03d", hours, mins, secs,
                     (int)(milliseconds) % 1000);
        } else {
            snprintf(sTimerBuffer, 64, "%d:%02d.%03d", mins, secs, (int)(milliseconds) % 1000);
        }
    }

    {
        f64 milliseconds = OSTicksToMilliseconds((f64)(sTimerLoadDelta));
        f64 fsecs        = milliseconds / 1000.0;

        int secs  = (int)fsecs;
        int mins  = secs / 60;
        int hours = mins / 60;
        secs %= 60;
        mins %= 60;

        if (hours > 0) {
            snprintf(sLoadTimerBuffer, 64, "(%d:%02d:%02d.%03d)", hours, mins, secs,
                     (int)(milliseconds) % 1000);
        } else {
            snprintf(sLoadTimerBuffer, 64, "(%d:%02d.%03d)", mins, secs,
                     (int)(milliseconds) % 1000);
        }
    }
}

BETTER_SMS_FOR_CALLBACK void renderSpeedrunTimer(TApplication *app, const J2DOrthoGraph *graph) {
    if (!sIsInitialized || gSpeedrunSetting.getInt() == SpeedrunSetting::NONE) {
        return;
    }

    int x = 16 + -BetterSMS::getScreenRatioAdjustX();
    sTimerTextboxB->draw(x + 1, 457 + 1);
    sTimerTextboxF->draw(x, 457);

    if (sWideLayout) {
        sLoadTimerTextboxB->draw(x + 107, 457 + 1);
        sLoadTimerTextboxF->draw(x + 106, 457);
    } else {
        sLoadTimerTextboxB->draw(x + 95, 457 + 1);
        sLoadTimerTextboxF->draw(x + 94, 457);
    }

    if (sIsCPUOverclocked) {
        sOverclockTextbox->draw(242, 456);
    }
}

static void initializeTimerOnFirstStart(TFlagManager *manager) {
    manager->firstStart();

    if (!sIsInitialized || gSpeedrunSetting.getInt() == SpeedrunSetting::NONE ||
        gSpeedrunSetting.getInt() == SpeedrunSetting::ILS) {
        return;
    }

    sTimerRunning   = true;
    sTimerStopped   = false;
    sTimerStart     = OSGetTime();
    sTimerCurrent   = sTimerStart;
    sTimerLoadStart = sTimerStart;
    sTimerLoadLast  = sTimerStart;
    sTimerLoadDelta = 0;
    sWasLoading = false;
}
SMS_PATCH_BL(0x80163F54, initializeTimerOnFirstStart);
SMS_PATCH_BL(0x801642E0, initializeTimerOnFirstStart);
SMS_PATCH_BL(0x802A2EB8, initializeTimerOnFirstStart);

void TBathtub_startDemo_override(void *bathtub) {
    if (!sIsInitialized || !sTimerRunning || gSpeedrunSetting.getInt() == SpeedrunSetting::NONE) {
        startDemo__8TBathtubFv(bathtub);
        return;
    }

    if (gSpeedrunSetting.getInt() == SpeedrunSetting::ANY ||
        gSpeedrunSetting.getInt() == SpeedrunSetting::ILS) {
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

    sIsNoBootShineCollecting = (gpMarDirector->mCollectedShine->mType & 0x10);
    sShineCollected          = true;
    sIsLastShineNoBoot       = sIsNoBootShineCollecting;

    switch (gSpeedrunSetting.getInt()) {
    case SpeedrunSetting::SHINE_121: {
        if (manager->getFlag(0x40000) >= 121) {
            sTimerCurrent = OSGetTime();
            sTimerRunning = false;
            sTimerStopped = true;
        }
        break;
    }
    case SpeedrunSetting::SHINE_197: {
        if (manager->getFlag(0x40000) >= 197) {
            sTimerCurrent = OSGetTime();
            sTimerRunning = false;
            sTimerStopped = true;
        }
        break;
    }
    case SpeedrunSetting::ILS: {
        sTimerCurrent = OSGetTime();
        sTimerRunning = false;
        sTimerStopped = true;
        break;
    }
    default:
        break;
    }
}
SMS_PATCH_BL(SMS_PORT_REGION(0x80297BA4, 0, 0, 0), TFlagManager_setShineFlag_override);
SMS_PATCH_BL(SMS_PORT_REGION(0x802A2ECC, 0, 0, 0), TFlagManager_setShineFlag_override);
SMS_PATCH_BL(SMS_PORT_REGION(0x802B64FC, 0, 0, 0), TFlagManager_setShineFlag_override);