#include "includes/eclipse.hxx"

bool isValidBGM(u32 musicID)
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
u32 setIsValid(u32 musicID)
{
    gInfo.mIsAudioStreamAllowed = isValidBGM(musicID);
    return musicID & 0x3FF;
}

bool startStreamedBGM(u32 musicID, bool loopMusic)
{
    char buffer[0x20];
    u32 handle = 0x803FDB7C;

    sprintf(buffer, (char *)0x80004A41, (musicID & 0x3FF)); //"/AudioRes/Streams/Music/%d.adp"

    if (!DVDOpen(buffer, handle))
        return false;

    stopBGM__5MSBgmFUlUl(*(u32 *)StageBGM, 32);
    DVDPrepareStreamAsync(handle, 0, 0, 0x803184E4);

    if (!loopMusic)
        DVDStopStreamAtEndAsync(0x8058943C, 0);
    return true;
}

bool startStreamedSFX(u32 sfxID)
{
    char buffer[0x20];
    u32 handle = 0x803FDB7C;

    sprintf(buffer, (char *)0x80004A20, (sfxID & 0x3FF)); //"/AudioRes/Streams/SFX/%d.adp"

    if (!DVDOpen(buffer, handle))
        return false;

    DVDPrepareStreamAsync(handle, 0, 0, 0x803184E4);
    DVDStopStreamAtEndAsync(0x8058943C, 0);

    return true;
}

bool streamAudio(u32 ID, u8 type, bool loopAudio)
{
    if (ID & 0x400)
    {
        if (type == 0)
        {
            return startStreamedBGM(ID, loopAudio);
        }
        else if (type == 1)
        {
            return startStreamedSFX(ID);
        }
    }
    return false;
}

//0x80016ABC
void initMusic()
{

    if (!gInfo.mFile)
        return;

    if (!gInfo.mIsAudioStreaming && (gInfo.mFile->Music.mMusicID & 0x400))
    {
        startStreamedBGM(gInfo.mFile->Music.mMusicID, true);
    }
    if (gInfo.mIsAudioStreaming && !gInfo.mIsAudioStreamAllowed)
    {
        DVDCancelStreamAsync(0x8058943C, 0);
        gInfo.mIsAudioStreaming = false;
    }
}

//0x80016948
void stopMusicOnStop()
{
    if (gInfo.mIsAudioStreaming)
    {
        DVDCancelStreamAsync(0x8058943C, 0);
        gInfo.mIsAudioStreaming = false;
    }
}

//0x802A670C
void stopMusicOnStageExit(TMarioGamePad *gamepad)
{
    if (gInfo.mIsAudioStreaming)
    {
        DVDCancelStreamAsync(0x8058943C, 0);
        gInfo.mIsAudioStreaming = false;
    }
    reset__9RumbleMgrFv(gamepad);
}