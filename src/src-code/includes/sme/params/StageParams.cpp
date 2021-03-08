#include "DVD.h"

#include "params/StageParams.hxx"
#include "libs/sString.hxx"

bool SMEFile::openFile(const char *path)
{
    DVDFileInfo handle;
    SMEFile tmp __attribute__((aligned(32)));

    for (u32 i = 0; DVDGetDriveStatus() != DVD_STATE_END; ++i)
    {
        if (i > 100000)
        {
            return false;
        }
    }

    if (!DVDOpen(path, &handle))
        return false;

    if (DVDReadPrio(&handle, &tmp, 32, 0, 2) < DVD_ERROR_OK || tmp.FileHeader.mMAGIC != SMEFile::MAGIC)
    {
        DVDClose(&handle);
        return false;
    }

    if (DVDReadPrio(&handle, &tmp, tmp.FileHeader.mFileSize, 0, 2) < DVD_ERROR_OK)
    {
        DVDClose(&handle);
        return false;
    }
    DVDClose(&handle);

    memcpy(&mStageConfig, &tmp, sizeof(SMEFile));
    return true;
}

bool SMEFile::load(const char *stage)
{
    char configPath[32];

    SMEFile::withSMEExtension(configPath, stage);

    #ifdef SME_DEBUG
        OSReport("Attempting to load local config %s...\n", configPath);
    #endif

    if (SMEFile::openFile(configPath))
        return true;

    SMEFile::withSMEExtension(configPath, stage, true);

    #ifdef SME_DEBUG
        OSReport("Failure: Now attempting to load global config %s...\n", configPath);
    #endif
    
    if (SMEFile::openFile(configPath))
    {   
        #ifdef SME_DEBUG
            OSReport("Success: SME config loaded at %p\n", this);
        #endif

        return true;
    }
    else
    {
        #ifdef SME_DEBUG
            OSReport("Failure: No SME configuration could be loaded\n");
        #endif

        return false;
    }
}

char *SMEFile::withSMEExtension(char *dst, const char *stage, bool generalize)
{
    String *path = new String("/data/scene/sme/", 128);
    String *file = new String(stage, 32);

    size_t numIDPos = file->findAny(String::numbers);
    if (generalize && numIDPos != String::npos)
    {
        file->erase(numIDPos);
        file->append("+.sme");
    }
    else
    {
        file->erase(file->find('.'));
        file->append(".sme");
    }
    path->append(*file);
    strncpy(dst, path->data(), path->size() + 1);

    delete path;
    delete file;

    return dst;
}