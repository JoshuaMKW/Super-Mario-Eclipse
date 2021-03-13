#pragma once

#include "types.h"
#include "JKRArchivePri.hxx"

class JKRDvdArchive : public JKRArchive
{
public:
	JKRDvdArchive(s32, u32); // EMountDirection
	virtual ~JKRDvdArchive();

	virtual u32* fetchResource(JKRArchive::SDIFileEntry *, u32 *);
	virtual u32* fetchResource(void *src, u32 len, JKRArchive::SDIFileEntry *, u32 *);

	void open(s32);
};