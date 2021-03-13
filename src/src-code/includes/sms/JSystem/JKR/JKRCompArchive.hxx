#pragma once

#include "types.h"
#include "JKRArchivePri.hxx"

class JKRCompArchive : public JKRArchive
{
public:
	JKRCompArchive(s32, u32); // EMountDirection
	virtual ~JKRCompArchive();

	virtual void removeResourceAll();
	virtual bool removeResource(void *);

	virtual void *fetchResource(JKRArchive::SDIFileEntry *, u32 *);
	virtual void *fetchResource(void *src, u32 len, JKRArchive::SDIFileEntry *, u32 *);

	void open(s32);
};