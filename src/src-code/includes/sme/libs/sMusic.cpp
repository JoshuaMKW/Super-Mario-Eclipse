#include "sMusic.hxx"
#include "SME.hxx"
#include "printf.h"
#include "sms/sound/MSBGM.hxx"

using namespace SME::Util;

static bool startStreamedBGM(MSStageInfo musicID, bool loopMusic) {
  char buffer[0x20];
  DVDFileInfo *handle = (DVDFileInfo *)0x803FDB7C;

  sprintf(buffer, "/AudioRes/Streams/Music/%lu.adp", (u32)(musicID & 0x3FF));

  if (!DVDOpen(buffer, handle))
    return false;

  MSBgm::stopBGM(gStageBGM, 32);
  DVDPrepareStreamAsync(handle, 0, 0, (DVDCallback)0x803184E4);

  if (!loopMusic)
    DVDStopStreamAtEndAsync(&handle->mCmdBlock, 0);
  return true;
}

static bool startStreamedSFX(u32 sfxID) {
  char buffer[0x20];
  DVDFileInfo *handle = (DVDFileInfo *)0x803FDB7C;

  sprintf(buffer, "/AudioRes/Streams/SFX/%lu.adp", (sfxID & 0x3FF));

  if (!DVDOpen(buffer, handle))
    return false;

  DVDPrepareStreamAsync(handle, 0, 0, (DVDCallback)0x803184E4);
  DVDStopStreamAtEndAsync(&handle->mCmdBlock, 0);

  return true;
}

bool Music::isValidBGM(MSStageInfo musicID) {
  switch (musicID) {
  case BGM_AIRPORT:
  case BGM_BIANCO:
  case BGM_CASINO:
  case BGM_CORONA:
  case BGM_DELFINO:
  case BGM_DOLPIC:
  case BGM_EVENT:
  case BGM_EXTRA:
  case BGM_MAMMA:
  case BGM_MARE_SEA:
  case BGM_MAREVILLAGE:
  case BGM_MERRY_GO_ROUND:
  case BGM_MONTE_NIGHT:
  case BGM_MONTE_ONSEN:
  case BGM_MONTE_RESCUE:
  case BGM_MONTEVILLAGE:
  case BGM_PINNAPACO:
  case BGM_PINNAPACO_SEA:
  case BGM_RICCO:
  case BGM_SHILENA:
  case BGM_SKY_AND_SEA:
    return true;
  default:
    return false;
  }
}

bool Music::streamAudio(MSStageInfo ID, u8 type, bool loopAudio) {
  if (ID & 0x400) {
    if (type == 0)
      return startStreamedBGM(ID, loopAudio);
    else if (type == 1)
      return startStreamedSFX(ID);
  }
  return false;
}