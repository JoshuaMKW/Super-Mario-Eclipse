#pragma once

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
	JKRDisposer();
	virtual ~JKRDisposer();
	
	u32 _4;
	JSUPtrLink mPtrLink; // _8
};

class JKRHeap : public JKRDisposer
{
public:
	class TState {};

	JKRHeap(void *, size_t, JKRHeap *, bool);
	virtual ~JKRHeap();
	
	virtual void *alloc(size_t, int) = 0;
	virtual void free(void *) = 0;
	virtual void freeAll();
	virtual void freeTail() = 0;
	virtual u32 resize(void *, u32) = 0;
	virtual u32 getSize(void *) = 0;
	virtual u32 getFreeSize() = 0;
	virtual u32 getTotalFreeSize() = 0;
	virtual u32 getHeapType() = 0;
	virtual bool check() = 0;
	virtual u32 dump_sort();
	virtual bool dump() = 0;
	virtual u8 changeGroupID(u8);
	virtual u8 getCurrentGroupId();
	virtual u32 state_register(JKRHeap::TState *, size_t);
	virtual u32 state_compare(const JKRHeap::TState &, const JKRHeap::TState &);
	virtual void state_dump(const JKRHeap::TState &);

	void becomeSystemHeap();
	void becomeCurrentHeap();
	bool initArena(u8 **, size_t, int);
	void free(void *, JKRHeap *);
	static JKRHeap* findFromRoot(void *);
	JKRHeap *find(void *) const;
	void dispose_subroutine(size_t, size_t);
	void dispose(void *, size_t);
	void dispose(void *, void *);
	void dispose();

	static void *alloc(size_t, int, JKRHeap *);
	static void copyMemory(void *, void *, size_t);

	u32 _0C[3];
	OSMutex mMutex;
	u32 _2C;
	void *mStart;
	void *mEnd;
	size_t mSize;
	JSUPtrList mPtrList; // _3C
	u32 _48[4];
	JSUPtrList mPtrList2; // _58
	bool _64;

	static JKRHeap *sSystemHeap;
	static JKRHeap *sCurrentHeap;
	static JKRHeap *sRootHeap;
	static void *mErrorHandler;
	static u8 *mCodeStart;
	static u8 *mCodeEnd;
	static u8 *mUserRamStart;
	static u8 *mUserRamEnd;
	static size_t mMemorySize;
};

class JKRExpHeap : public JKRHeap
{
public:
	class CMemBlock
	{
	public:
		void *allocBack(size_t, u8, u8, u8, u8);
		void *allocFore(size_t, u8, u8, u8, u8);
		void free(JKRExpHeap *);
		void initiate(CMemBlock *, CMemBlock *, size_t, u8, u8);

		static CMemBlock *getHeapBlock(void *);
	};

	JKRExpHeap(void *, size_t, JKRHeap *, bool);
	virtual ~JKRExpHeap();

	virtual void *alloc(size_t, int) override;
	virtual void free(void *) override;
	virtual void freeAll() override;
	virtual void freeTail() override;
	virtual u32 resize(void *, size_t) override;
	virtual u32 getSize(void *) override;
	virtual u32 getFreeSize() override;
	virtual u32 getTotalFreeSize() override;
	virtual u32 getHeapType() override;
	virtual bool check() override;
	virtual u32 dump_sort() override;
	virtual bool dump() override;
	virtual u8 changeGroupID(u8) override;
	virtual u8 getCurrentGroupId() override;
	virtual u32 state_register(JKRHeap::TState *, size_t) override;
	virtual u32 state_compare(const JKRHeap::TState &, const JKRHeap::TState &) override;
	virtual void state_dump(const JKRHeap::TState &) override;

	void destroy();
	u32 getUsedSize(u8) const;
	void joinTwoBlocks(CMemBlock *);
	void recycleFreeBlock(CMemBlock *);
	void removeFreeBlock(CMemBlock *);
	void setFreeBlock(CMemBlock *, CMemBlock *, CMemBlock *);

	static JKRExpHeap *create(void *, size_t, JKRHeap *, bool);
	static JKRExpHeap *create(size_t, JKRHeap *, bool);
	static JKRExpHeap *createRoot(int, bool);

	u8 _68;
	u8 mGroupID; //_69
	u16 _6A;
	u32 _6C;
	u32 _70;
	u32 _74;
	u32 _78;
	u32 _7C;
	u32 _80;
};

class JKRStdHeap : public JKRHeap
{
public:
	JKRStdHeap(void *, size_t, JKRHeap *, bool);
	virtual ~JKRStdHeap();

	virtual void *alloc(size_t, int) override;
	virtual void free(void *) override;
	virtual void freeAll() override;
	virtual void freeTail() override;
	virtual u32 resize(void *, size_t) override;
	virtual u32 getSize(void *) override;
	virtual u32 getFreeSize() override;
	virtual u32 getTotalFreeSize() override;
	virtual u32 getHeapType() override;
	virtual bool check() override;
	virtual bool dump() override;
	virtual u32 state_register(JKRHeap::TState *, size_t) override;
	virtual u32 state_compare(const JKRHeap::TState &, const JKRHeap::TState &) override;

	static JKRExpHeap *create(size_t, JKRHeap *, bool);

	u32 mHeapID;
};

class JKRSolidHeap : public JKRHeap
{
public:
	JKRSolidHeap(void *, size_t, JKRHeap *, bool);
	virtual ~JKRSolidHeap();

	virtual void *alloc(size_t, int) override;
	virtual void free(void *) override;
	virtual void freeAll() override;
	virtual void freeTail() override;
	virtual u32 resize(void *, size_t) override;
	virtual u32 getSize(void *) override;
	virtual u32 getFreeSize() override;
	virtual u32 getTotalFreeSize() override;
	virtual u32 getHeapType() override;
	virtual bool check() override;
	virtual u32 dump_sort() override;
	virtual bool dump() override;
	virtual u32 state_register(JKRHeap::TState *, size_t) override;
	virtual u32 state_compare(const JKRHeap::TState &, const JKRHeap::TState &) override;

	static JKRExpHeap *create(size_t, JKRHeap *, bool);

	u32 _68;
	u32 _6C;
	u32 _70;
	u32 _74;
};

class JKRThread : public JKRDisposer
{
public:
	JKRThread(u32 stackSize, int messageCount, int);
	virtual ~JKRThread();

	virtual void run();

	static void* start(void *src);

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
	JKRAram(u32, u32, s32);
	virtual ~JKRAram();

	virtual void run();

	static u32* create(u32, u32, s32, s32, s32);
	static s32 mainRamToAram(u8 *, u32, u32, JKRExpandSwitch, u32, JKRHeap *, int);
	static s32 aramToMainRam(u32, u8 *, u32, JKRExpandSwitch, u32, JKRHeap *, int, u32 *);

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
	OSMessage *mMessage;   // _C0
	JSUPtrLink *mDVDList;  // _E4
	OSThread *mCurThread;  // _F4
};

class JKRDecompCommand
{
public:
	JKRDecompCommand();
	~JKRDecompCommand();

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
	SZP,
	SZS
};

class JKRDecomp : public JKRThread
{
public:
	JKRDecomp(s32);
	virtual ~JKRDecomp();

	virtual void run();

	static void create(s32);
	static void sendCommand(JKRDecompCommand *);
	void orderSync(u8 *, u8 *, u32, u32);
	void decode(u8 *, u8 *, u32, u32);
	static void decodeSZP(u8 *src, u8 *dest, u32, u32);
	static void decodeSZS(u8 *src, u8 *dest, u32, u32);
	static CompressionType checkCompressed(u8 *);
};

class JKRAramHeap : public JKRDisposer
{
public:
	enum EAllocMode
	{
		HEAD = 0,
		TAIL = 1
	};

	JKRAramHeap(u32, u32);
	virtual ~JKRAramHeap();

	void* alloc(u32 size, EAllocMode allocMode);
	void* allocFromHead(u32 size);
	void* allocFromTail(u32 size);

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

enum EMountMode
{
};

class JKRArchive : public JKRFileLoader
{
public:
	enum EMountDirection
	{
	};

	class SDIFileEntry
	{
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
	virtual u8 *getResource(const char *);
	virtual u8 *getResource(u32, const char *);
	virtual u32* readResource(void *, u32, const char *);
	virtual u32* readResource(void *, u32, u32, const char *);
	virtual void removeResourceAll();
	virtual bool removeResource(void *);
	virtual void detachResource(void *);
	virtual s32 getResSize(const void *) const;
	virtual s16 countFile(const char *);
	virtual JKRArcFinder* getFirstFile(const char *) const;

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

enum JKRMemBreakFlag
{
	UNK_0,
	UNK_1
};

class JKRMemArchive : public JKRArchive
{
public:
	JKRMemArchive();
	JKRMemArchive(s32, JKRArchive::EMountDirection);
	JKRMemArchive(void *, size_t, JKRMemBreakFlag);
	virtual ~JKRMemArchive();
	
	virtual void removeResourceAll();
	virtual bool removeResource(void *);

	virtual void *fetchResource(JKRArchive::SDIFileEntry *, u32 *);
	virtual void *fetchResource(void *src, u32 len, JKRArchive::SDIFileEntry *, u32 *);
	virtual void *fetchResource_subroutine(u8 *, size_t, u8 *, size_t, int);

	bool mountFixed(void *, JKRMemBreakFlag);
	bool open(s32, JKRArchive::EMountDirection);
	bool open(void *, size_t, JKRMemBreakFlag);
	void unmountFixed();

	u32 _60;
	u32 _64; // related to offsets
};

class JKRDvdArchive : public JKRArchive
{
public:
	JKRDvdArchive(s32, u32); // EMountDirection
	virtual ~JKRDvdArchive();

	virtual u32* fetchResource(JKRArchive::SDIFileEntry *, u32 *);
	virtual u32* fetchResource(void *src, u32 len, JKRArchive::SDIFileEntry *, u32 *);

	void open(s32);
};

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

/* 
 * ARAM
*/

class JKRAramArchive : public JKRFileLoader
{
	JKRAramArchive(s32, int);
	virtual ~JKRAramArchive();
	
	virtual void *fetchResource(int *, u32 *);
	virtual void *fetchResource(void *, u32, int *, u32 *);

	u32 *fetchResource_subroutine(u32, u32, JKRHeap *, int, u8 **);
	u32 *fetchResource_subroutine(u32, u32, u8 *, u32, int);
};

class JKRAramBlock
{
public:
	JKRAramBlock(u32, u32, u32, u8, bool);
	virtual ~JKRAramBlock();

	JKRAramBlock *allocHead(u32, u8, JKRAramHeap *);
	JKRAramBlock *allocTail(u32, u8, JKRAramHeap *);
	
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
	virtual ~JKRFileFinder();

	u32 _0;
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

	u8 _10;
	u8 _11;
	u8 _12;				  // padding?
	u8 _13;				  // ^^
	JKRArchive *mArchive; // _14
	u32 _18;
	u32 _1C;
	u32 _20;
};

void JKRDefaultMemoryErrorRoutine(void *, u32, u32);
void JKRDecompressFromAramToMainRam(u32, void *, u32, u32, u32);
s32 decompSZS_subroutine(u8 *src, u8 *dest);
u32* firstSrcData();
u32* nextSrcData(u8 *);

void *operator new(size_t blocksize);
void *operator new(size_t blocksize, int align);
void *operator new(size_t blocksize, JKRHeap *heap, int align);
void *operator new[](size_t blocksize);
void *operator new[](size_t blocksize, int align);
void *operator new[](size_t blocksize, JKRHeap *heap, int align);
void operator delete(void *block);
void operator delete[](void *block);