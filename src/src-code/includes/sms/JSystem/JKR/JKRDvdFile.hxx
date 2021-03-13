#pragma once

#include "types.h"
#include "DVD.h"
#include "OS.h"
#include "JKRDisposer.hxx"
#include "../JSU/JSUList.hxx"

class JKRDvdFile : public JKRDisposer
{
public:
	JKRDvdFile(s32);

	virtual bool open(const char *fileName);
	virtual void close();
	virtual s32 writeData(void const *addr, s32 length, s32 offset);
	virtual s32 readData(void *addr, s32 length, s32 offset);
	virtual s32 getFileSize() const;
	virtual bool open(s32);

	void sync();

	bool mIsOpened;		   // _18
	OSMutex *mMutex;	   // _1C
	DVDFileInfo *mDVDInfo; // _5C
	s32 mFileSize;		   // _90
	OSMessage mMessage;    // _C0
	JSUPtrLink mDVDList;   // _E4
	OSThread *mCurThread;  // _F4
};