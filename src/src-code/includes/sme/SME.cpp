#include "OS.h"
#include "SME.hxx"

#include "kuribo_sdk.h"

// CheatCode.cpp
extern void *handleDebugCheat(void *GCLogoDir);

// camera.cpp
extern void modifyCameraRangeToSize(f32 *params, f32 *saveParams);

extern OSAlarm gctAlarm;
extern void setUserCodes(OSAlarm *alarm, OSContext *context);

#ifdef SME_DEBUG
    extern OSStopwatch gctStopwatch;
#endif

KURIBO_MODULE_BEGIN("Eclipse", "JoshuaMK", __VERSION__ "[" STRINGIZE(SME_MAX_SHINES) " Shines]")
{
    #ifdef SME_DEBUG
	    KURIBO_EXECUTE_ON_LOAD {
            OSReport("Codeblocker - Creating OSAlarm at %p; Calls %p every %0.4f seconds\n", &gctAlarm, &setUserCodes, 0.001f);

            OSInitStopwatch(&gctStopwatch, "Codeblocker");
            OSCreateAlarm(&gctAlarm);
            OSSetPeriodicAlarm(&gctAlarm, OSGetTime(), OSMillisecondsToTicks(1), &setUserCodes);
            OSReport("Actor flags offset = %X\n", offsetof(JDrama::TNameRef, mKeyCode));
            OSReport("Actor position offset = %X\n", offsetof(JDrama::TPlacement, mPosition));
            OSReport("Actor size offset = %X\n", offsetof(JDrama::TActor, mSize));
        }
	    KURIBO_EXECUTE_ON_UNLOAD {
            OSReport("-- Destroying Module --\n");
            OSStopStopwatch(&gctStopwatch);
            OSCancelAlarm(&gctAlarm);
        }
    #else
        KURIBO_EXECUTE_ON_LOAD {
            OSCreateAlarm(&gctAlarm);
            OSSetPeriodicAlarm(&gctAlarm, OSGetTime(), OSMillisecondsToTicks(1), &setUserCodes);
        }
        KURIBO_EXECUTE_ON_UNLOAD {
            OSReport("-- Destroying Module --\n");
            OSCancelAlarm(&gctAlarm);
        }
    #endif


    /* -- HOOKS -- */

    // CheatCode.cpp
    KURIBO_PATCH_B(0x80295B6C, handleDebugCheat);

    // camera.cpp
    KURIBO_PATCH_B(0x80027548, modifyCameraRangeToSize);


    /* -- PATCHES -- */

    //Restore Chao Seed
    kWrite32(0x802FD1A0, 0x808D8C70);

    //Show Exception Handler
    kWrite32(0x8029D0BC, 0x60000000);

    #ifdef SME_DEBUG
        //Skip FMVs
        kWrite32(0x802B5E8C, 0x38600001);
        kWrite32(0x802B5EF4, 0x38600001);
    #endif

    #ifdef SME_DEBUG
        //Level Select
        kWrite32(0x802A6788, 0x3BC00009);
    #endif

    //Fix Infinte Flutter
    kWrite32(0x8028113C, 0xC002F824);

    //Remove Dive While Wall Sliding
    kWrite32(0x8024BC10, 0x48000068);

    //Flood Till Corona Beat
    kWrite32(0x8029961C, 0x38840077);

    //Map on D Pad down
    kWrite32(0x8017A830, 0x5400077B);
    kWrite32(0x80297A60, 0x5400077B);

    //Hover move on roofs
    kWrite32(0x802569BC, 0xC02300B0);

    //Global surfing bloopies
    kWrite32(0x801B74F8, 0x60000000);
    kWrite32(0x801B74FC, 0x60000000);
    kWrite32(0x801B7500, 0x60000000);
    kWrite32(0x801B7504, 0x60000000);
    kWrite32(0x801B7508, 0x60000000);
    kWrite32(0x801B750C, 0x60000000);
    kWrite32(0x801B7510, 0x387E0780);
    kWrite32(0x801B7514, 0x4810BA9D);
    kWrite32(0x801B7518, 0x28030000);
    kWrite32(0x801B751C, 0x418200A4);
}
KURIBO_MODULE_END();