#include "JDrama/JDRNameRef.hxx"
#include "JKR/JKRDecomp.hxx"
#include "JKR/JKRHeap.hxx"
#include "JKR/JKRMemArchive.hxx"
#include "printf.h"
#include "stdlib.h"

#include "SME.hxx"

using namespace SME;
using namespace Util;
using namespace Class;

// for future intro sound patch
static u8 gOldAreaID = 0;
static u8 gOldEpisodeID = 0;

// 0x802B7A4C
static void initSoundBank(u8 areaID, u8 episodeID) {
  /*TStageParams *config = TStageParams::sStageConfig;

  gOldAreaID = areaID;
  gOldEpisodeID = episodeID;
  if (config->mMusicSetCustom.get()) {
    areaID = config->mMusicAreaID.get();
    episodeID = config->mMusicEpisodeID.get();
  }*/
  SME_DEBUG_LOG("Initializing the sound bank...\n");
  setMSoundEnterStage__10MSMainProcFUcUc(areaID, episodeID);
  SME_DEBUG_LOG("Initializing the sound bank... DONE!\n");
}
SME_PATCH_BL(SME_PORT_REGION(0x802B7A4C, 0, 0, 0), initSoundBank);