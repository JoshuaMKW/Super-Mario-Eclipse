#include "printf.h"
#include "sms/sound/MSBGM.hxx"

#include "SME.hxx"

bool isValidBGM(MSStageInfo musicID)
{
    switch (musicID)
    {
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

//0x80016998
u32 setIsValid(MSStageInfo musicID)
{
    gInfo.Context.mIsAudioStreamAllowed = isValidBGM(musicID);
    return musicID & 0x3FF;
}

bool startStreamedBGM(MSStageInfo musicID, bool loopMusic)
{
    char buffer[0x20];
    DVDFileInfo *handle = (DVDFileInfo *)0x803FDB7C;

    sprintf(buffer, "/AudioRes/Streams/Music/%u.adp", (musicID & 0x3FF));

    if (!DVDOpen(buffer, handle))
        return false;

    stopBGM__5MSBgmFUlUl(gStageBGM, 32);
    DVDPrepareStreamAsync(handle, 0, 0, (DVDCallback)0x803184E4);

    if (!loopMusic)
        DVDStopStreamAtEndAsync(&handle->mCmdBlock, 0);
    return true;
}

bool startStreamedSFX(u32 sfxID)
{
    char buffer[0x20];
    DVDFileInfo *handle = (DVDFileInfo *)0x803FDB7C;

    sprintf(buffer, "/AudioRes/Streams/SFX/%lu.adp", (sfxID & 0x3FF));

    if (!DVDOpen(buffer, handle))
        return false;

    DVDPrepareStreamAsync(handle, 0, 0, (DVDCallback)0x803184E4);
    DVDStopStreamAtEndAsync(&handle->mCmdBlock, 0);

    return true;
}

bool streamAudio(MSStageInfo ID, u8 type, bool loopAudio)
{
    if (ID & 0x400)
    {
        if (type == 0)
            return startStreamedBGM(ID, loopAudio);
        else if (type == 1)
            return startStreamedSFX(ID);
    }
    return false;
}

//0x80016ABC
void initMusic()
{
    if (SMEFile::mStageConfig.FileHeader.mMAGIC != SMEFile::MAGIC)
        return;
        
    DVDFileInfo *handle = (DVDFileInfo *)0x803FDB7C;

    if (!gInfo.Context.mIsAudioStreaming && (SMEFile::mStageConfig.Music.mMusicID & 0x400))
        startStreamedBGM((MSStageInfo)SMEFile::mStageConfig.Music.mMusicID, true);

    if (gInfo.Context.mIsAudioStreaming && !gInfo.Context.mIsAudioStreamAllowed)
    {
        DVDCancelStreamAsync(&handle->mCmdBlock, 0);
        gInfo.Context.mIsAudioStreaming = false;
    }
}

//0x80016948
void stopMusicOnStop()
{
    DVDFileInfo *handle = (DVDFileInfo *)0x803FDB7C;

    if (gInfo.Context.mIsAudioStreaming)
    {
        DVDCancelStreamAsync(&handle->mCmdBlock, 0);
        gInfo.Context.mIsAudioStreaming = false;
    }
}

//0x802A670C
void stopMusicOnStageExit(TMarioGamePad *gamepad)
{
    DVDFileInfo *handle = (DVDFileInfo *)0x803FDB7C;

    if (gInfo.Context.mIsAudioStreaming)
    {
        DVDCancelStreamAsync(&handle->mCmdBlock, 0);
        gInfo.Context.mIsAudioStreaming = false;
    }
    reset__9RumbleMgrFv(gamepad);
}