#pragma once

#include "JKRFileFinder.hxx"
#include "JKRFileLoader.hxx"
#include "types.h"


enum EMountMode {};

class JKRArchive : public JKRFileLoader {
public:
  enum EMountDirection {};

  class SDIFileEntry {
  public:
    u32 _0;
    u32 _4;
    s32 _8; // offset to something
    u32 _C;
    u32 *mEntry; // _10
  };

  JKRArchive();
  JKRArchive(u32, EMountMode);
  virtual ~JKRArchive();

  virtual s32 becomeCurrent(const char *);
  virtual void *getResource(const char *);
  virtual void *getResource(u32, const char *);
  virtual u32 *readResource(void *, u32, const char *);
  virtual u32 *readResource(void *, u32, u32, const char *);
  virtual void removeResourceAll();
  virtual bool removeResource(void *);
  virtual void detachResource(void *);
  virtual s32 getResSize(const void *) const;
  virtual s16 countFile(const char *);
  virtual JKRArcFinder *getFirstFile(const char *) const;

  u32 *_38;
  u8 _3C;
  u8 _3D; // padding?
  u8 _3E; // ^^
  u8 _3F; // ^^
  u32 _40;
  u32 _44;
  u32 _48;
  u32 _4C;
  u32 _50;
  u32 _54;
  u32 _58;
  int mountDirection; // _5C

  s32 **sCurrentDirID;
};