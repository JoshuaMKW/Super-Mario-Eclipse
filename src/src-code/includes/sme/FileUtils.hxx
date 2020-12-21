#ifndef SME_FILEUTILS
#define SME_FILEUTILS

#include "types.h"
#include "sms/game/Application.hxx"

#include "funcs.hxx"

inline char *getStageName(TApplication *gpApplication)
{
    AreaEpisodeArray *AreaPathArray = gpApplication->mStringPaths;

    if (!AreaPathArray)
        return NULL;

    u32 *AreaArrayStart = AreaPathArray->startArray;

    if (!AreaArrayStart || (((u32)AreaPathArray->endArray - (u32)AreaArrayStart) >> 2) < (u8)gpApplication->mCurrentScene.mAreaID)
        return NULL;

    AreaEpisodeArray *StagePathArray = (AreaEpisodeArray *)AreaArrayStart[(u8)gpApplication->mCurrentScene.mAreaID];
    u32 *StageArrayStart = (u32 *)StagePathArray->startArray;

    if (!StageArrayStart || (((u32)StagePathArray->endArray - (u32)StageArrayStart) >> 4) < gpApplication->mCurrentScene.mEpisodeID)
        return NULL;

    return (char *)(StageArrayStart[(gpApplication->mCurrentScene.mEpisodeID << 2) + (0xC / 4)]);
}

inline u32 *loadArchive(char *path, JKRHeap *heap)
{
    OSReport("Loading %s into memory...\n", path);
    if (DVDConvertPathToEntrynum(path) >= 0)
    {
        u32 *file = (u32 *)loadToMainRAM__12JKRDvdRipperFPCcPUc15JKRExpandSwitchUlP7JKRHeapQ212JKRDvdRipper15EAllocDirectionUlPi(path, 0, 1, 0, heap, 1, 0, 0);

        if (file)
        {
            OSReport("Success! Location = %X, Size = %X\n", file, file[1]);
        }
        else
        {
            OSReport("Failed! Could not allocate memory\n");
        }
        return file;
    }
    else
    {
        return NULL;
    }
}

#endif