#include "FileUtils.hxx"
#include "sms/JSystem/JKR/JKRHeap.hxx"
#include "sms/JSystem/JUT/JUTGamePad.hxx"
#include "sms/game/Application.hxx"
#include "SME.hxx"

using namespace SME;

const char *Util::getStageName(TApplication *gpApplication)
{
    AreaEpisodeArray *AreaPathArray = gpApplication->mStringPaths;

    if (!AreaPathArray)
        return nullptr;

    u32 *AreaArrayStart = AreaPathArray->startArray;

    if (!AreaArrayStart || (((u32)AreaPathArray->endArray - (u32)AreaArrayStart) >> 2) < (u8)gpApplication->mCurrentScene.mAreaID)
        return nullptr;

    AreaEpisodeArray *StagePathArray = (AreaEpisodeArray *)AreaArrayStart[(u8)gpApplication->mCurrentScene.mAreaID];
    u32 *StageArrayStart = (u32 *)StagePathArray->startArray;

    if (!StageArrayStart || (((u32)StagePathArray->endArray - (u32)StageArrayStart) >> 4) < gpApplication->mCurrentScene.mEpisodeID)
        return nullptr;

    return (char *)(StageArrayStart[(gpApplication->mCurrentScene.mEpisodeID << 2) + (0xC / 4)]);
}

void *Util::loadArchive(char *path, JKRHeap *heap)
{
    SME_DEBUG_LOG("Loading \"%s\" into memory...\n", path);
    if (DVDConvertPathToEntrynum(path) >= 0)
    {
        u32 *file = (u32 *)loadToMainRAM__12JKRDvdRipperFPCcPUc15JKRExpandSwitchUlP7JKRHeapQ212JKRDvdRipper15EAllocDirectionUlPi(path, 0, 1, 0, heap, 1, 0, 0);

        if (file) {
            SME_DEBUG_LOG("Success! Location = %X, Size = %X\n", file, file[1]);
        }
        else {
            SME_DEBUG_LOG("Failed! Could not allocate memory\n");
        }
        return file;
    }
    else
    {
        SME_DEBUG_LOG("Failed! \"%s\" was not found\n");
        return nullptr;
    }
}