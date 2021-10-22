#pragma once

#include "JSU/JSUList.hxx"
#include "JKRDisposer.hxx"
#include "types.h"


class JKRArchive;

class JKRFileLoader : public JKRDisposer {
public:
  JKRFileLoader();
  virtual ~JKRFileLoader();

  virtual void unmount();

  static JKRArchive *getVolume(const char *);
  static void changeDirectory(const char *dirName);
  static u32 *getGlbResource(const char *);
  static u32 *getGlbResource(const char *, JKRFileLoader *);
  static u32 getResSize(void *resource, JKRFileLoader *);
  static u32 *findVolume(const char **);
  static JKRArchive *findFirstFile(const char *);
  static u8 *fetchVolumeName(u8 *, u32, const char *);

  JSUPtrLink mPtrLink2; // _18
  u32 _28;
  u32 _2C;
  u8 _30;
  u8 _31; // padding?
  u8 _32; // ^^
  u8 _33; // ^^
  u32 _34;
};