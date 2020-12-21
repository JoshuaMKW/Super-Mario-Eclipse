#ifndef JKR_H
#define JKR_H

#include "types.h"
#include "DVD.h"
#include "OS.h"
#include "sms/JSUStream.hxx"

class JKRArchive;
class JKRArcFinder;

enum JKRExpandSwitch
{
	NON_COMPRESSED = 0,
	COMPRESSED = 1
};

class JKRDisposer
{
public:
	u32 *vTable; // _0
	u32 _4;
	JSUPtrLink mPtrLink; // _8
};

class JKRHeap : public JKRDisposer
{
public:
	u8 _18[0x30 - 0x18];
	u32 _30;
	u32 _34;
	u32 _38;
	JSUPtrList ptrList; // _3C
};

class JKRThread : public JKRDisposer
{
public:
	JSUPtrLink mThreadPtrs;		  // _18
	JKRHeap *mHeap;				  // _28
	OSThread *mThread;			  // _2C
	OSMessageQueue mMessageQueue; // _30
	OSMessage *mMessage;		  // _50
	int mMessageCount;			  // _54
	void *mStackPtr;			  // _58
	u32 mStackSize;				  // _5C
};

class JKRAram : public JKRThread
{
public:
	u32 _60;
	u32 _64;
	u32 _68;
	u32 _70;
	u32 _74;
	u32 _78;
	u32 _7C;
	u32 _80;
	u32 _84;
};

class JKRDvdFile : public JKRDisposer
{
public:
	u8 mIsOpened;		   // _18
	OSMutex *mMutex;	   // _1C
	DVDFileInfo *mDVDInfo; // _5C
	s32 mFileSize;		   // _90
	OSMessage *mMessage;   // _C0
	JSUPtrLink *mDVDList;  // _E4
	OSThread *mCurThread;  // _F4
};

class JKRDecompCommand
{
public:
	u32 _0;
	u8 *_4;
	u8 *_8;
	u32 _C;
	u32 _10;
	u32 _14;
	JKRDecompCommand *self; // _18
	u32 _1C;
	u32 _20;
	OSMessageQueue mMessageQueue; // _28
	OSMessage mMessage;			  // _48
};

enum CompressionType
{
	NONE,
	SZP_COMPRESSED,
	SZS_COMPRESSED
};

class JKRDecomp : public JKRThread
{
};

class JKRAramHeap : public JKRDisposer
{
public:
	enum EAllocMode
	{
		ALLOC_FROM_HEAD = 0,
		ALLOC_FROM_TAIL = 1
	};

	OSMutex mMutex; // _18
	s32 mSize;		// _30
	u32 _34;
	u32 _38;
	u32 _3C;
	u8 _40;
	u8 _41;
	u8 _42;
	u8 _43;
	u8 _44;
};

class JKRFileLoader : public JKRDisposer
{
public:
	JSUPtrLink mPtrLink2; // _18
	u32 _28;
	u32 _2C;
	u8 _30;
	u8 _31; // padding?
	u8 _32; // ^^
	u8 _33; // ^^
	u32 _34;
};

enum EMountMode
{
};

class JKRArchive : public JKRFileLoader
{
public:
	class SDIFileEntry
	{
	public:
		u32 _0;
		u32 _4;
		s32 _8; // offset to something
		u32 _C;
		u32 *entry; // _10
	};

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

class JKRMemArchive : public JKRArchive
{
public:
	u32 _60;
	u32 _64; // related to offsets
};

class JKRDvdArchive : public JKRArchive
{
};

class JKRCompArchive : public JKRArchive
{
};

/* 
 * ARAM
*/

class JKRAramArchive : public JKRFileLoader
{
};

class JKRAramBlock
{
public:
	u32 *vTable;		 // _0
	JSUPtrLink blockPtr; // _4
	u32 _14;
	u32 _18;
	u32 _1C;
	u8 _20;
	u8 _21;
	u8 _22; // padding?
	u8 _23; // ^^
};

class JKRFileFinder
{
public:
	u32 _0;
	u32 _4;
	u16 _8;
	u16 _A;
	u32 *vTable; // _C
};

class JKRArcFinder : public JKRFileFinder
{
public:
	u8 _10;
	u8 _11;
	u8 _12;				  // padding?
	u8 _13;				  // ^^
	JKRArchive *mArchive; // _14
	u32 _18;
	u32 _1C;
	u32 _20;
};

#endif