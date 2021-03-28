#pragma once

#include "types.h"

class JKRArchive;

class JKRFileFinder
{
public:
	virtual ~JKRFileFinder();

	u32 _4;
	u16 _8;
	u16 _A;
};

class JKRArcFinder : public JKRFileFinder
{
public:
	JKRArcFinder(JKRArchive *, u32, u32);
	virtual ~JKRArcFinder();

	virtual bool findNextFile();

	u32 _0C;
	u8 _10;
	u8 _11;
	u8 _12;				  // padding?
	u8 _13;				  // ^^
	JKRArchive *mArchive; // _14
	u32 _18;
	u32 _1C;
	u32 _20;
};