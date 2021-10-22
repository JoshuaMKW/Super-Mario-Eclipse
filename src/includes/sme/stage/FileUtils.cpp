#include "FileUtils.hxx"
#include "SME.hxx"
#include "JKR/JKRDvdRipper.hxx"
#include "JKR/JKRHeap.hxx"
#include "JUT/JUTGamePad.hxx"
#include "sms/game/Application.hxx"


using namespace SME;

const char *Util::getStageName(TApplication *gpApplication) {
  AreaEpisodeArray *AreaPathArray = gpApplication->mStringPaths;

  if (!AreaPathArray)
    return nullptr;

  u32 *AreaArrayStart = AreaPathArray->startArray;

  if (!AreaArrayStart ||
      (((u32)AreaPathArray->endArray - (u32)AreaArrayStart) >> 2) <
          (u8)gpApplication->mCurrentScene.mAreaID)
    return nullptr;

  AreaEpisodeArray *StagePathArray = (AreaEpisodeArray *)
      AreaArrayStart[(u8)gpApplication->mCurrentScene.mAreaID];
  u32 *StageArrayStart = (u32 *)StagePathArray->startArray;

  if (!StageArrayStart ||
      (((u32)StagePathArray->endArray - (u32)StageArrayStart) >> 4) <
          gpApplication->mCurrentScene.mEpisodeID)
    return nullptr;

  return (
      char *)(StageArrayStart[(gpApplication->mCurrentScene.mEpisodeID << 2) +
                              (0xC / 4)]);
}

void *Util::loadArchive(const char *path, JKRHeap *heap,
                        JKRDvdRipper::EAllocDirection allocDirection) {
  SME_DEBUG_LOG("Loading \"%s\" into memory...\n", path);
  if (DVDConvertPathToEntrynum(path) >= 0) {
    u32 *file = static_cast<u32 *>(JKRDvdRipper::loadToMainRAM(
        path, nullptr, JKRExpandSwitch::EXPAND, 0, heap, allocDirection, 0, nullptr));

    if (file) {
      SME_DEBUG_LOG("Success! Location = %X, Size = %X\n", file, file[1]);
    } else {
      SME_DEBUG_LOG("Failed! Could not allocate memory\n");
    }
    return file;
  } else {
    SME_DEBUG_LOG("Failed! \"%s\" was not found\n");
    return nullptr;
  }
}