#pragma once

#include "eclipse.hxx"

const char *getStageName(TApplication *gpApplication)
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

    return (const char *)(StageArrayStart[(gpApplication->mCurrentScene.mEpisodeID << 2) + (0xC / 4)]);
}