#include "types.h"
#include "OS.h"

#include "sms/SMS.hxx"
#include "InfoManager.hxx"
#include "Cheats.hxx"
#include "FileUtils.hxx"
#include "funcs.hxx"

#include "libs/kamek.h"
#include "libs/sMath.hxx"
#include "libs/sMemory.hxx"

#include "params/MarioParams.hxx"
#include "params/StageParams.hxx"


#ifndef SME_SME_H
#define SME_SME_H

#ifndef MAX_SHINES
#define MAX_SHINES 120
#endif

#define __VERSION__ "ALPHA"

extern SMEInfoManager gInfo;
extern TCheatHandler gDebugModeHandler;

#endif