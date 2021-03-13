#pragma once

#include "types.h"
#include "JKRArchivePri.hxx"
#include "JKRDisposer.hxx"

class JKRFileLoader : public JKRDisposer
{
public:
	JKRFileLoader();
	virtual ~JKRFileLoader();
	
	virtual void unmount();

	static JKRFileLoader *getVolume(const char *);
	void changeDirectory(const char *dirName);
	u32* getGlbResource(const char *);
	u32* getGlbResource(const char *, JKRFileLoader *);
	u32 getResSize(void *resource, JKRFileLoader *);
	u32* findVolume(const char **);
	JKRArchive* findFirstFile(const char *);
	u8* fetchVolumeName(u8 *, u32, const char *);

	JSUPtrLink mPtrLink2; // _18
	u32 _28;
	u32 _2C;
	u8 _30;
	u8 _31; // padding?
	u8 _32; // ^^
	u8 _33; // ^^
	u32 _34;
};