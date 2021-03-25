#include "DVD.h"

#include "StageParams.hxx"
#include "libs/sLogging.hxx"
#include "libs/sString.hxx"

using namespace SME;

bool Class::TSMEFile::openFile(const char *path) {
  DVDFileInfo handle;
  TSMEFile tmp __attribute__((aligned(32)));

  for (u32 i = 0; DVDGetDriveStatus() != DVD_STATE_END; ++i) {
    if (i > 100000) {
      return false;
    }
  }

  if (!DVDOpen(path, &handle))
    return false;

  if (DVDReadPrio(&handle, &tmp, 32, 0, 2) < DVD_ERROR_OK ||
      tmp.FileHeader.mMAGIC != TSMEFile::MAGIC) {
    DVDClose(&handle);
    return false;
  }

  if (DVDReadPrio(&handle, &tmp, tmp.FileHeader.mFileSize, 0, 2) <
      DVD_ERROR_OK) {
    DVDClose(&handle);
    return false;
  }
  DVDClose(&handle);

  memcpy(&mStageConfig, &tmp, sizeof(TSMEFile));
  return true;
}

bool Class::TSMEFile::load(const char *stage) {
  char configPath[32];

  TSMEFile::withSMEExtension(configPath, stage);

  SME_DEBUG_LOG("Attempting to load local config %s...\n", configPath);

  if (TSMEFile::openFile(configPath))
    return true;

  TSMEFile::withSMEExtension(configPath, stage, true);

  SME_DEBUG_LOG("Failure: Now attempting to load global config %s...\n",
                configPath);

  if (TSMEFile::openFile(configPath)) {
    SME_DEBUG_LOG("Success: SME config loaded at %p\n", &mStageConfig);
    return true;
  } else {
    SME_DEBUG_LOG("Failure: No SME configuration could be loaded\n");
    return false;
  }
}

char *Class::TSMEFile::withSMEExtension(char *dst, const char *stage,
                                        bool generalize) {
  String *path = new String("/data/scene/sme/", 128);
  String *file = new String(stage, 32);

  size_t numIDPos = file->findAny(String::numbers);
  if (generalize && numIDPos != String::npos) {
    file->erase(numIDPos);
    file->append("+.sme");
  } else {
    file->erase(file->find('.'));
    file->append(".sme");
  }
  path->append(*file);
  strncpy(dst, path->data(), path->size() + 1);

  delete path;
  delete file;

  return dst;
}